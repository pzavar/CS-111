#include "stdio.h"
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <signal.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

void sigsegv_handler(){
  fprintf(stderr, "ERROR: segmentation fault caught. Terminating program.\n");
  //perror("");
  exit(4);
}

int main(int argc, char **argv){
  int c;
  int fd0 = 0;
  int fd1 = 1;
  int segflag = 0;
  int catflag = 0;

  static struct option long_options[] = 
    {
      {"input", required_argument, 0, 'i'},
      {"output", required_argument, 0, 'o'},
      {"segfault", no_argument, 0, 's'},
      {"catch", no_argument, 0, 'c'},
      {0,0,0,0}
    };

  while (1) {
    c = getopt_long(argc, argv, "", long_options, NULL);

    if (c == -1)
      break;
    
    switch(c)
      {
      case 'i':
	fd0 = open(optarg, O_RDONLY);
	if (fd0 >= 0) {
	  close(0);
	  dup(fd0);
	  close(fd0);
	  // printf("opened successfully\n");
	}
	else {
	  fprintf(stderr, "ERROR: failed to get input. Terminating program. %s\nReason: ", strerror(errno));
	  perror("");
	  exit(2);
	}
	break;
	
      case 'o':
	if ((fd1 = creat(optarg, 777)) >= 0) {
	  close(1);
	  dup(fd1);
	  close(fd1);
	  // printf("closed successfully\n");
	}
	else {
	  fprintf(stderr, "ERROR: failed to get output. Terminating program. %s\nReason: ", strerror(errno));
	  perror("");
	  exit(3);
	}
	break;
      
      case 's':
	segflag = 1;
	break;

      case 'c':
	catflag = 1;
	break;

      default:
	fprintf(stderr, "ERROR: no option and/or incorrect option(s) selected. Terminating program. %s\n", strerror(errno));
	exit(1);
      } //switch statement
  } //big while loop

  if (segflag && catflag){
    signal(SIGSEGV, sigsegv_handler);
  }

  if (segflag) {
    char **ptr = NULL;
    printf("%s", *ptr);
  }
  
  /* read & write */
    
  fd0 = 0;
  fd1 = 1;
  char *buf = (char*)malloc(sizeof(char));
  size_t IO = read(fd0, buf, 1); 

  while (IO > 0) {
    write(fd1, buf, 1);
    IO = read(fd0, buf, 1);
  }    
  if (IO == -1){
    fprintf(stderr, "ERROR: could not read file. Terminating program. %s\nReason: ", strerror(errno));
      perror("");
      exit(2);
      }
  free(buf);
  exit(0);
}
