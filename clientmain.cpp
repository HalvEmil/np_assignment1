#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/* You will to add includes here */

// Enable if you want debugging to be printed, see examble below.
// Alternative, pass CFLAGS=-DDEBUG to make, make CFLAGS=-DDEBUG
//#define DEBUG


// Included to get the support library
#include <calcLib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main(int argc, char *argv[]){

  /*
    Read first input, assumes <ip>:<port> syntax, convert into one string (Desthost) and one integer (port). 
     Atm, works only on dotted notation, i.e. IPv4 and DNS. IPv6 does not work if its using ':'. 
  */
  char delim[]=":";
  char *Desthost=strtok(argv[1],delim);
  char *Destport=strtok(NULL,delim);
  // *Desthost now points to a sting holding whatever came before the delimiter, ':'.
  // *Dstport points to whatever string came after the delimiter. 

  /* Do magic */
  int port=atoi(Destport);
printf("Host %s, and port %d.\n",Desthost,port);
#ifdef DEBUG
  printf("Connected to  %s, local %d\n", Desthost, port);
#endif
  struct addrinfo hints, *res;
  int sockfd;
  int byte_count;
  socklen_t fromlen;
  struct sockaddr_storage addr;
  char buf[512];
  //char ipstr[INET6_ADDRSTRLEN];

  memset(&hints, 0, sizeof (hints));
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;


  int status = getaddrinfo(Desthost, Destport, &hints, &res);
  if(status != 0){
    fprintf(stderr, "ERROR: RESOLVE ISSUE (%s)\n", gai_strerror(status));
    exit(1);
  }

  printf("Första print\n");

  sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
  
  if(connect(sockfd, res->ai_addr, res->ai_addrlen) == -1){
    perror("ERROR: CANT CONNECT");
    exit(1);
  }

  printf("Andra print\n");
  fromlen = sizeof addr;

  int i1;
  int i2;
  int iresult = NULL;
  double f1;
  double f2;
  double fresult = NULL;
  char command[10];
  int rv;
  char outbuf[64];

  while (1){
    byte_count = recv(sockfd, buf, sizeof(buf) - 1, 0 );
    if(byte_count <= 0){
      break;
    }
    buf[byte_count] = '\0';

    if( strncmp( buf , "TEXT TCP 1.0", 12) == 0){
      send(sockfd, "OK\n", 3, 0);
    }
    
    //rv = sscanf(buf ,"%s ", command);
    if(buf[0]== 'f'){
      rv = sscanf(buf ,"%s %lf %lf", command, &f1, &f2);
      if(strcmp(command, "fadd") == 0){
      fresult = f1 + f2;
      }
      else if(strcmp(command, "fsub") == 0){
      fresult = f1 - f2;
      }
      else if(strcmp(command, "fmul") == 0){
      fresult = f1 * f2;
      }
      else if(strcmp(command, "fdiv") == 0){
      fresult = f1 / f2;
      }

      //char outbuf[64];
      snprintf(outbuf, sizeof(outbuf), "%8.8g\n", fresult);
      if(fresult != NULL){

        send(sockfd, outbuf, strlen(outbuf), 0);
      }
    }
    else if(strncmp(buf, "OK", 2) == 0){
      buf[2] = '\0';
      if(command[0] == 'f'){
        printf("ASSIGNMENT: %s %8.8g %8.8g\n", command, f1, f2);
        #ifdef DEBUG
          printf("Calculated the result to %8.8g\n", fresult);
        #endif
        printf("%s my result=(%8.8g)\n", buf, fresult);
      }
      else{
        printf("ASSIGNMENT: %s %d %d\n", command, i1, i2);
                #ifdef DEBUG
          printf("Calculated the result to %d\n", iresult);
        #endif
        printf("%s my result=(%d)\n", buf, iresult);
       }
       break;
          
    }
    else{
      rv = sscanf(buf ,"%s %d %d", command, &i1, &i2);
      if(strcmp(command, "add") == 0){
        iresult = i1 + i2;
      }
      if(strcmp(command, "sub") == 0){
        iresult = i1 - i2;
      }
      if(strcmp(command, "mul") == 0){
        iresult = i1 * i2;
      }
      if(strcmp(command, "div") == 0){
        iresult = i1 / i2;
      }
      //char outbuf[8];
      snprintf(outbuf, sizeof(outbuf), "%d\n", iresult);
      if(iresult != NULL){    
        send(sockfd, outbuf, strlen(outbuf), 0);
      }
      
    }  
  }
   
}
