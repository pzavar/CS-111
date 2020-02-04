#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <poll.h>
#include <mcrypt.h>

int pipe2shell[2];
int pipe2term[2];
const char r = '\r';
const char n = '\n';
pid_t pid;
int status;
MCRYPT enc, dec;

void die(char * msg) {
  perror(msg);
  exit(1);
}

void shutDown(){
  waitpid(pid, &status, 0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(status), WEXITSTATUS(status));

  mcrypt_generic_deinit(enc);
  mcrypt_module_close(enc);

  mcrypt_generic_deinit(dec);
  mcrypt_module_close(dec);
}

int main(int argc, char *argv[]) {
  int c;
  int flagPort = -1;
  int flagLog = 0;
  int flagEnc = 0;
  char *keyName;
  int kfd;
  int keylen;
  int keybuf[128];
  char *IV;

  static struct option long_options[] = 
    {
      {"port", required_argument, 0, 'p'},
      {"encrypt", required_argument, 0, 'e'},
      {0,0,0,0}
    };

  while(1) {
    c = getopt_long(argc, argv, "", long_options, NULL);

    if (c== -1) break;

    switch (c)
      {
      case 'p':
	flagPort = 1;
	break;
	
      case 'e':
	flagEnc = 1;
	keyName = optarg;
	break;

      default:
	fprintf(stderr, "Unrecognized argument(s). Terminating program\n");
	exit(1);
      }

    if (flagPort == 1) {
      int sockfd, newsockfd, portno, clilen;
      char buffer[256];
      struct sockaddr_in serv_addr, cli_addr;
      int n;

      if (argc < 2) {
	fprintf(stderr, "ERROR, no port provided\n");
	exit(1);
      }
      
      sockfd = socket(AF_INET, SOCK_STREAM, 0);

      if (sockfd < 0)
	die("ERROR opening socket\n");
     
      bzero((char *) &serv_addr, sizeof(serv_addr));
      portno = atoi(optarg);

      serv_addr.sin_family = AF_INET;
      serv_addr.sin_addr.s_addr = INADDR_ANY;
      serv_addr.sin_port = htons(portno);

      if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
	die("ERROR on binding");

      listen(sockfd, 5);
      clilen = sizeof(cli_addr);

      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

      if (newsockfd < 0)
	die("ERROR on accepting\n");

      close(sockfd);
      pid = fork();
      
      if (pid == -1)
        die("ERROR on child fork()");

      if (pid == 0) {
	//printf("Entered child process\n");
	
	close(pipe2shell[1]);
	close(pipe2term[0]);
	
	close(0); //closing in
	dup(pipe2shell[0]);
	close(pipe2shell[0]);

	close(1); //closing out
	dup(pipe2term[1]);
	close(2); //closing err

	dup(pipe2term[1]);
	close(pipe2term[1]);


	if (execvp(optarg, NULL) < 0)
	  die("Execvp() failed\n");
	else {
	  execvp(optarg, NULL);
	}

      } //child
    
      //starting parent
      if (pid > 0) {
	close(pipe2shell[0]);
	close(pipe2term[1]);
	atexit(shutDown);

	struct pollfd pfds[2];
	int count = 0;
	int x = 0;
	

	if (flagEnc) {
	  kfd = open(keyName, O_RDONLY);
	  
	  if (kfd == -1)
	    die("ERROR on opening key file descriptor\n");
	  
	  enc = mcrypt_module_open("twofish", NULL, "cfb", NULL);
	  dec = mcrypt_module_open("twofish", NULL, "cfb", NULL);

	  struct stat kstats;
	  keylen = kstats.st_size;
	  keyName = (char*)malloc(keylen*sizeof(char));

	  IV = (char *)malloc(mcrypt_enc_get_iv_size(enc));
	  int IVsize = mcrypt_enc_get_iv_size(enc);

	  int i = 0;
	  while (i < IVsize) {
	    IV[i] = '\0';
	    i++;
	  }

	  mcrypt_generic_init(enc, keyName, keylen, IV);
	  mcrypt_generic_init(dec, keyName, keylen, IV);

	}


	pfds[0].fd = newsockfd;
	pfds[0].events = POLLIN | POLLHUP | POLLERR;
	pfds[1].fd = pipe2term[0];
	pfds[1].events = POLLIN | POLLHUP | POLLERR;

	while (1) {
	  x = poll(pfds, 2, -1); //-1 is timeout
	  if (x < 0) 
	    die("ERROR on poll()\n");

	  if (x > 0) {
	    if (pfds[0].revents & POLLIN){
	      count = read(newsockfd, buffer, sizeof(buffer));
	      
	      if (count < 0)
		die("ERROR on read()\n");

	      int i;
	      char ch;

	      for (i=0; i<count; ++i) {
		ch = buffer[i];
		if (ch == '\n' || ch == '\r'){
		  write(pipe2shell[1], &n, 1);
		}

		if (ch == 0x04) { //^D
		  close(pipe2shell[1]);
		  exit(0);
		}

		if (ch == 0x03) { //^C
		  kill(pid, SIGINT);
		}

		else {
		  write(1, &ch, 1);
		}
	      }
	    } //pfds[0].revents & POLLIN

	    if (pfds[1].revents & POLLIN) {
	      count = read(pipe2term[0], buffer, sizeof(buffer));
	      if (count < 0)
		die("ERROR reading pipe to terminal\n");

	      int i;
	      char ch;

	      for (i = 0; i < count; i++) {
		ch = buffer[i];
		if (ch == '\n') {
		  write(1, &r, 1);
		  write(1, &n, 1);
		}

		if (ch == 0x04)
		  exit(0);

		else {
		  write(newsockfd, &ch, 1);
		}
	      }
	    }

	    if ((pfds[0].revents & POLLHUP) || (pfds[1].revents & POLLHUP)) {
	      kill(pid, SIGHUP);
	      exit(0);
	    }
            

	    if ((pfds[0].revents & POLLERR) || (pfds[1].revents & POLLERR)) {            
	      kill(pid, SIGINT);
	      exit(1);
	    }
	  } // if(x > 0)
	} //while(1)
      } //if (pid > 0)
    } //if (flagPort == 1)
  }//while (1)
  exit(0);
} //main


