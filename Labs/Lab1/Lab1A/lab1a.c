#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <poll.h>

extern int errno;
struct termios OG_attr;
int pipe2shell[2];
int pipe2term[2];
pid_t pid;
const char r = '\r';
const char n = '\n';

void exitStatus() {
  int status;
  waitpid(pid, &status, 0);
  fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d", WIFSIGNALED(status), WEXITSTATUS(status));
}

void sigpipe_handler() {
  fprintf(stderr, "\nSIGPIPE error. Terminating program\n", strerror(errno));
  exit(1);
}

void sigint_handler() {
  fprintf(stderr, "\nSIGINT error. Killing program\n", strerror(errno));
  kill(pid, SIGINT);
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

  tcgetattr(0, &attr);

  attr.c_iflag = ISTRIP;
  attr.c_oflag = 0;
  attr.c_lflag = 0;

  //tcsetattr(0, TCSANOW, &attr);
  if (tcsetattr(0, TCSANOW, &attr) < 0) {
    fprintf(stderr, "error setting terminal modes\n");
    exit(1);
  }
}

//MAIN
int main(int argc, char **argv)
{
  terminalSetup();
  int c;
  int flagShell = 0;
  
  static struct option long_options[] =
    {
      {"shell", required_argument, 0, 's'},
      {0,0,0,0}
    };

  while (1) {
    c = getopt_long(argc, argv, "", long_options, NULL);

    if (c == -1) break;

    switch (c)
      {
      case 's':
	flagShell = 1;
	break;
      default:
	fprintf(stderr, "Unrecognized argument. Terminating program\n", strerror(errno));
	exit(1);
      }
  }
  //Check if on terminal or not
  while (!isatty) {
    fprintf(stderr, "Error: not a terminal\n", strerror(errno));
    exit(1);
  }

  terminalSetup();

  if (flagShell) {
    signal(SIGINT, sigint_handler);
    signal(SIGPIPE, sigpipe_handler);
    atexit(exitStatus);

    if (pipe(pipe2shell) < 0 || pipe(pipe2term) < 0) {
      //attrResetter();
      fprintf(stderr, "Failed to use pipes\n", strerror(errno));
      exit(1);
    }

    //Time to fork
    pid = fork();

    if (pid < 0) {
      fprintf(stderr, "fork() failed\n", strerror(errno));
      exit(1);
    }

    //Child process
    if (pid == 0)
      {
	printf("Entered child process\n", strerror(errno));

	close(pipe2shell[1]);
	close(pipe2term[0]);

	dup2(pipe2shell[0], 1);
	dup2(pipe2term[1], 2);

	close(pipe2shell[0]);
	close(pipe2term[1]);

	if (execvp(optarg, NULL) < 0) {
	  //attrResetter();
	  fprintf(stderr, "execvp() failed\n", strerror(errno));
	  exit(1);
	}
	execvp(optarg, NULL);

      } //End of child process

    //Parent process
    if (pid > 0) {
      int status = 0;
      //waitpid(pid, &status, 0);
      //printf("Child process exits with code: %d\n", (status & 0xff));

      close(pipe2shell[0]);
      close(pipe2term[1]);

      char buf[1024];
      struct pollfd pfds[2];
      int count = 0;

      pfds[0].fd = 0;
      pfds[0].events = POLLIN | POLLHUP | POLLERR;
      pfds[1].fd = pipe2term[0];
      pfds[1].events = POLLIN | POLLHUP | POLLERR;

      while (1) {
	int p;
	p = poll(pfds, 2, -1);

	if (p < 0) {
	  fprintf(stderr, "poll() failed\n", strerror(errno));
	  //attrResetter();
	  exit(1);
	}

	else if (p > 0) {
	  if (pfds[0].revents & POLLIN) {
	    count = read(0, buf, sizeof(char));
	    if (count < 0) {
	      //attrResetter;
	      fprintf(stderr, "Failed to read file. Terminating program\n", strerror(errno));
	      exit(1);
	    }

	    int i;
	    char ch;
	    
	    for (i = 0; i < count; i++) {
	      ch = buf[i];
	      if (ch == '\r' || ch == '\n') {
		write(1, &r, 1);
		write(1, &n, 1);
		write(pipe2shell[1], &n, 1);
	      }

	      if (ch = 0x04) {
		write(1, "^D\n", 1);
		close(pipe2shell[1]);
		exit(0);
	      }

	      if (ch == 0x03) {
		write(1, "^C\n", 1);
		sigint_handler();
	      }

	      else {
		write(1, &ch, 1);
		write(pipe2shell[1], &ch, 1);
	      }
	    }
	  }

	  if (pfds[1].revents & POLLIN) {
	    count = read(pipe2term[0], buf, sizeof(buf));

	    if (count < 0) {
	      //attrResetter();
	      fprintf(stderr, "read() failed\n", strerror(errno));
	      exit(1);
	    }

	    int i;
	    char ch;

	    for (i = 0; i < count; i++) {
	      ch = buf[i];
	      if (ch == '\n') {
		write(1, &r, 1);
		write(1, &n, 1);
	      }

	      if (ch == 0x04) {
		write(1, "^D\n", 2);
		exit(0);
	      }

	      else {
		write(1, &ch, 1);
	      }
	    }
	  }

	  if (pfds[0].revents & POLLHUP || pfds[1].revents & POLLHUP) {
	    atexit(exitStatus);
	    kill(pid, SIGHUP);
	    exit(1);
	  }

	  if (pfds[0].revents & POLLERR || pfds[1].revents & POLLERR) {
	    sigint_handler();
	    exit(1);
	  }
	} //else if (p > 0)
      } //while(1)
    } // if (pid > 0)
  } //if (flagShell)

  //Step 2 read & write
  char ch;
  size_t IO = 0;

  IO = read(0, &ch, 1);

  /*for (IO; IO > 0; IO = read(0, &ch, IO))
    write(1, &ch, 1);
  */
  
  while (IO > 0) {
    write(1, &ch, 1);
    IO = read(0, &ch, IO);
  }
  
  
  if (IO == 0) {
    printf("EOF. Terminating program\n");
    exit(0);
  }

  if (IO == -1) {
    fprintf(stderr, "read() failed\n", strerror(errno));
    exit(1);
  }

  if (ch == 0x04) {
    //write(1, "^D\n", 2);
    attrResetter();
    exit(0);
  }

  if (ch == '\n' || ch == '\r') {
    write(1, &r, 1);
    write(1, &n, 1);
  }

  //Reset terminal and exit(0)
  attrResetter();
  exit(0);
}
