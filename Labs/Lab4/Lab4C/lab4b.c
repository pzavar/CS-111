#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <poll.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <ctype.h>
#include <mraa.h>

int period = 1; //default
char scale = 'F'; //default
int stop_flag = 0;
int log_flag = 0;
char *in;
FILE *logfile = 0;

struct timeval ts;
struct tm *current_time;
time_t reporting_time = 0;

mraa_aio_context aio_dev;
mraa_gpio_context gpio_dev;

#define B 4275
#define R0 100000.0

void die(char *msg) {
  perror(msg);
  exit(1);
}

void print(char *text, int stdoutt, int logg) {
  if (stdoutt && logg) {
    fprintf(stdout, "%s\n", text);
    fprintf(logfile, "%s\n", text);
  }
  else if (!stdoutt && logg) {
    fprintf(logfile, "%s\n", text);
  }
  else if (stdoutt && !logg)
    fprintf(stdout, "%s\n", text);
}

float get_temp() {
  int reading = mraa_aio_read(aio_dev);
  float R = 1023.0 / ((float)reading) - 1.0;
  R = R0*R;
  float C = 1.0/(log(R/R0)/B + 1/298.15) - 273.15;
  //printf("Celcius value: %f\n", C);
  float F = (C*9)/5 + 32;
  //printf("Fahrenheit value: %f\n", F);
  if (scale == 'F')
    return F;
  else if (scale == 'C')
    return C;
  else
    return F;
}

void get_time_and_temp() {
  // clock_gettime(CLOCK_REALTIME, &ts);
  // tm = localtime(&(ts.tv_sec));
  gettimeofday(&ts, 0);
  if (stop_flag == 0 && (ts.tv_sec >= reporting_time)) {
    //if (1) {
    //printf("ENTERED FUNCTION\n");
    float temperature = get_temp();
    int TEMP = temperature*10;
    //printf("Got temperature from get_temp()\n");
    current_time = localtime(&ts.tv_sec);
    if (log_flag == 1) { 
      fprintf(stdout, "%02d:%02d:%02d %d.%d\n", current_time->tm_hour, current_time->tm_min, current_time->tm_sec, TEMP/10, TEMP%10);
      fprintf(logfile, "%02d:%02d:%02d %d.%d\n", current_time->tm_hour, current_time->tm_min, current_time->tm_sec, TEMP/10, TEMP%10);
    }
     
    else if (log_flag == 0) {
      fprintf(stdout, "%02d:%02d:%02d %d.%d\n", current_time->tm_hour, current_time->tm_min, current_time->tm_sec, TEMP/10, TEMP%10);
    }
    reporting_time = ts.tv_sec + period;
    //printf("looped in time stamp reporter\n");
  }
}

void get_input(char *arg) {
  arg[(strlen(arg)-1)] = '\0';

  if (strcmp(arg, "SCALE=F") == 0) { //temps report implausible values
    scale = 'F';
    if (log_flag == 1)
      print(arg, 0, 1);
  }
  else if (strcmp(arg, "SCALE=C") == 0) {
    scale = 'C';
    if (log_flag == 1)
      print(arg, 0, 1);
  }
  else if (strcmp(arg, "START") == 0) { //works
    stop_flag = 0;
    if (log_flag == 1)
      print(arg, 0, 1);
  }
  else if (strcmp(arg, "STOP") == 0) { //works
    stop_flag = 1;
    if (log_flag == 1)
      print(arg, 0, 1);
  }
  else if (strcmp(arg, "OFF") == 0) { //works
    current_time = localtime(&ts.tv_sec);
    fprintf(stdout, "%02d:%02d:%02d SHUTDOWN\n", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    if (log_flag == 1) {
      print(arg, 0, 1);
      fprintf(logfile, "%02d:%02d:%02d SHUTDOWN\n", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
    }
    exit(0);
  }
  
  else if (strstr(arg, "PERIOD=") == arg) { //works 
    char *ptr = arg;
    ptr += 7;
    int digit = atoi(ptr);
    if (digit == 0) {
      digit = 1;
      period = 1;
      if (log_flag == 1)
	fprintf(logfile, "PERIOD=1\n");
    }
    if (digit >= 1) {
      period = digit;
      if (log_flag == 1)
	fprintf(logfile, "PERIOD=%d\n", digit);
    }
  }  
  else if (strstr(arg, "LOG") == arg) { //works
    print(arg, 0, 1);
  }
  else if (log_flag == 1) { //works
    print(arg, 0, 1);
  }

  //arg[(strlen(arg)-1)] = '\0';
}


int main(int argc, char* argv[]) {
  int c;

  struct option options[] = {
    {"period", required_argument, NULL, 'p'},
    {"scale", required_argument, NULL, 's'},
    {"log", required_argument, NULL, 'l'},
    {0, 0, 0, 0}
  };

  while ((c = getopt_long(argc, argv, "", options, NULL)) != -1) {
    //printf("reached while loop\n");
    switch (c) {
    case 'p':
      period = atoi(optarg);
      break;

    case 'l':
      log_flag = 1;
      logfile = fopen(optarg, "w+");
      if(logfile == NULL) {
	die("ERROR on log file\n");
      }
      break;

    case 's':
      if (optarg[0] == 'F' || optarg[0] == 'C') {
	scale = optarg[0];
      }
      else
	scale = 'F';
      break;
  
    default:
      die("ERROR bad argument(s)\n");
    }
    //printf("exiting while loop\n");
  }

  aio_dev = mraa_aio_init(1);

  if (aio_dev== NULL) {
    die("ERROR on aio_dev init\n");
  }

  gpio_dev = mraa_gpio_init(60);

  if (gpio_dev == NULL) {
    die("ERROR on gpio_dev init\n");
  }

  mraa_gpio_dir(gpio_dev, MRAA_GPIO_IN);
  //mraa_gpio_isr(gpio_dev, MRAA_GPIO_EDGE_RISING, &shutdown, NULL);

  struct pollfd pfd;
  pfd.fd = STDIN_FILENO;
  pfd.events = POLLIN;

  in = (char *)malloc(1024 * sizeof(char));
  if (in == NULL) {
    die("ERROR allocating memory for input\n");
  }
  // printf("going for the functions\n");
  while(1) {
    get_time_and_temp();
    int polling = poll(&pfd, 1, 1); //timeout = 1 as recommended by alex
    if(polling) {
      fgets(in, 1024, stdin);
      get_input(in);
    }
  }

  mraa_aio_close(aio_dev);
  mraa_gpio_close(gpio_dev);

  exit(0);
}




