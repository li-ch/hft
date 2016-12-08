#include<stdio.h> 
#include<string.h> 
#include<stdlib.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<sys/time.h>
#include<signal.h>

#define SERVER "127.0.0.1"
#define BUFLEN 512  //Max length of buffer
#define PORT 50001  //The port on which to send data
#define NCON 200    //The number of contracts to send

int main() {
  struct sockaddr_in si_other;
  int s, i, slen=sizeof(si_other);
  char buf[BUFLEN];
 
  if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
    fprintf(stderr, "Failed to create UDP socket\n");
    exit(1);  
  }
 
  memset((char *) &si_other, 0, sizeof(si_other));
  si_other.sin_family = AF_INET;
  si_other.sin_port = htons(PORT);
     
  if (inet_aton(SERVER , &si_other.sin_addr) == 0) {
    fprintf(stderr, "inet_aton() failed\n");
    exit(1);
  }
  
  char message[300];
  memset(message, 0, sizeof(message));
  
  struct itimerval timer;
  timer.it_interval.tv_sec = timer.it_value.tv_sec = 0;
  timer.it_interval.tv_usec = timer.it_value.tv_usec = 300000;   /* 300 milliseconds */
  
  struct timespec req = {0};
  req.tv_sec = 0;
  req.tv_nsec = 5000L;
  
  if (setitimer(ITIMER_REAL, &timer, 0) < 0) {
    perror("setitimer");
    exit(1); 
  }

  sigset_t alarm_sig;
  int signum;
  sigemptyset(&alarm_sig);
  sigaddset(&alarm_sig, SIGALRM);
  while(1) {  
    sigwait(&alarm_sig, &signum); /* wait until the next signal */
    for(int t=0; t < NCON; ++t) {
      if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1) {
        fprintf(stderr, "sendto() failed\n");
        exit(1);
      }
      nanosleep(&req, (struct timespec *)NULL);
    }
  }
}

