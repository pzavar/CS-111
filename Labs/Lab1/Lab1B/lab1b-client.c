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

struct termios OG_attr;
const char r = '\r';
const char n = '\n';
const char d[] = "^D";
const char c[] = "^C";
MCRYPT enc, dec;

void die(char *msg)
{
  perror(msg);
  exit(1);
}

void attrResetter() {
  tcsetattr(0, TCSANOW, &OG_attr);
}

void terminalSetup() {
  /* get attributes on start-up */
  tcgetattr(0, &OG_attr);
  /* and to reset attributes before shutdown */
  atexit(attrResetter);

  struct termios attr;

  if (tcgetattr(0, &attr) < 0) {
    fprintf(stderr, "tcgetattr() failed\n");
    exit(1);
  }

  tcgetattr(0, &attr);

  attr.c_iflag = ISTRIP;
  attr.c_oflag = 0;
  attr.c_lflag = 0;

  if (tcsetattr(0, TCSANOW, &attr) < 0) {
    fprintf(stderr, "tcsetattr() failed\n");
    exit(1);
  }
  tcsetattr(0, TCSANOW, &attr);
}



int main(int argc, char *argv[]) {
  if (!isatty)
    die("Not on the terminal");
  
  int c;
  int flagPort = -1;
  int flagLog = 0;
  int flagEnc = 0;
  int l = -1;
  char *logF;
  int lc;
  char lbuf[1024];

  char *keyName;
  int kfd;
  int keylen;
  int keybuf[128];
  char *IV;


  static struct option long_options[] = 
    {
      {"port", required_argument, 0, 'p'},
      {"encrypt", required_argument, 0, 'e'},
      {"log", required_argument, 0, 'l'},
      {0,0,0,0}
    };

  while(1) {
    c = getopt_long(argc, argv, "", long_options, NULL);

    if (c == -1) break;

    switch(c)
      {
      case 'p':
        flagPort = 1;
	break;

      case 'e':
	flagEnc = 1;
	keyName = optarg;
	break;

      case 'l':
        flagLog = 1;
	l = open(optarg, O_RDWR | O_CREAT, 0666);
	//l = creat(optarg, S_IRWXU);
	if (l < 0) {
	  die("ERROR on creat()\n");
	}
        break;

      default:
        fprintf(stderr, "Unrecognized argument. Terminating program\n");
        exit(1);
      }


    terminalSetup();

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
    
 
    if (flagPort == -1) {
      die("Need to provide an argument. Try --port=#\n");
    }

    if (flagPort == 1) {
      int sockfd, portno, n;
      struct sockaddr_in serv_addr;
      struct hostent *server;
      char buffer[256];

      portno = atoi(optarg);
      sockfd = socket(AF_INET, SOCK_STREAM, 0);

      if (sockfd < 0)
        die("ERROR opening socket\n");

      server = gethostbyname("localhost");

      if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n)");
        exit(0);
      }

      bzero((char *) &serv_addr, sizeof(serv_addr));
      serv_addr.sin_family = AF_INET;

      bcopy((char *)server->h_addr, 
	    (char *)&serv_addr.sin_addr.s_addr, 
	    server->h_length);

      serv_addr.sin_port = htons(portno);

      if (connect(sockfd, (struct sockaddr *)&serv_addr,
		  sizeof(serv_addr)) < 0)
	die("ERROR connecting\n");
  

      struct pollfd pfds[2];
      int count = 0;
      int x = 0;

      pfds[0].fd = 0;
      pfds[0].events = POLLIN | POLLHUP | POLLERR;
      pfds[1].fd = sockfd;
      pfds[1].events = POLLIN | POLLHUP | POLLERR;

      while (1) {
	x = poll(pfds, 2, -1);

	if (x < 0)
	  die("ERROR on pfds[0]\n");
	
	if (x > 0) {
	  if (pfds[0].revents & POLLIN) {

	    count = read(0, buffer, sizeof(buffer));
	    if (count < 0)
	      die("ERROR on read()");
	    
	    int i;
	    char ch;
	    
	    for (i = 0; i < count; i++) {
	      ch = buffer[i];
	      if (ch == '\n' || ch == '\r'){
		write(1, &n, 1);
		write(1, &r, 1);
	      }	      
	      if (ch == 0x04) {
		write(1, "^D", 1);
	      }
	      if (ch == 0x03) {
		write(1, "^C", 1);
	      }
	      else
		write(sockfd, &ch, 1);
	    }
	    
	    if (flagLog) {
	      lc = sprintf(lbuf, "SENT %d bytes: %s", 1, buffer);
	      write(l, lbuf, lc);
	    }
	  }

	  if (pfds[1].revents & POLLIN) {
	    count = read(sockfd, buffer, sizeof(buffer));
	 
	    if (flagEnc) {
	      mdecrypt_generic(dec, buffer, count);
	    }
	    
	    if (count < 0)
	      die("ERROR on pfds[1]\n");
	    
	    int i;
	    char ch;

	    for (i=0; i < count; i++) {
	      ch = buffer[i];
	      if (ch == '\n') {
		write(1, &r, 1);
		write(1, &n, 1);
	      }

	      else {
		write(1, &ch, 1);
	      }
	    }
	    if (flagLog) {
	      lc = sprintf(lbuf, "RECEIVED %d bytes: %s\n", 1, buffer);
	      write(l, lbuf, lc);
	    }
	  }

	  
	  if ((pfds[0].revents & POLLHUP) || (pfds[1].revents & POLLHUP)) {
	    exit (0);
	  }
	  
	  if ((pfds[0].revents & POLLERR) || (pfds[1].revents & POLLERR)) {
	    exit(1);
	  }
	  
	}
      } //while(1)               
    } //if (flagPort)    
  } //while (1)

  exit(0);
  
} //main
