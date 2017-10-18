#include <stdio.h>
#include <string.h>	/* for using strtok, strcmp, etc */
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "common.h"

#define SPACE " "
#define DOT "."
#define HELLO "HELLO"
#define STATUS "STATUS"
#define SERVER_BYE "SERVER_BYE"
#define CLIENT_BYE "CLIENT_BYE"

//Basic socket level code refrenced from TCP-IP Sockets in C.

int main (int argc, char **argv){
  //Declares go here
  int currConnectedClients;
  int serverSocket;
  int clientSocket;
  int recievedMessageSize;
  int cookie;
  struct sockaddr_in serverAddress = {0};
  struct sockaddr_in clientAddress = {0};
  unsigned short serverPort;
  unsigned int clientLength;
  char buffer[MAX_STR_SIZE];
  char clientIP[MAX_STR_SIZE];
  char cookHolder[5];
  char userName[MAX_STR_SIZE];
  char name[MAX_STR_SIZE];

  //Check right number of inputs
  if (argc < 2){
    printf(stderr, "Usage: %s <Server Port>\n", argv[0]);
    exit(1);
  }

  serverPort = atoi(argv[1]);
  //Make socket
  if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0){
    perror("Socket Failed");
    exit(1);
  }

  //Setting up the server address
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons(serverPort);

  //Bind to socket
  if (bind(serverSocket, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
    perror("Bind Failed");
    exit(1);
  }

  //Open socket for listening
  if (listen(serverSocket, MAXPENDING) < 0){
    perror("Listen failed");
    exit(1);
  }

  clientLength = sizeof(clientAddress);
  //Infinite loop while the server waits
  for(;;){
    //Waiting for the client to connect
    if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLength)) < 0){
      perror("Accept failed");
      exit(1);
    }

    fflush(stdout);
    if ((recievedMessageSize = recv(clientSocket, buffer, MAX_STR_SIZE, 0)) < 0){
      perror("Recieve failed");
      exit(1);
    }
    buffer[recievedMessageSize] = '\0';

    strcpy(clientIP, inet_ntoa(clientAddress.sin_addr));

    if (strcmp(strtok(buffer, SPACE), MAGIC_STRING) != 0){
      printf("**Error** from %s:%d\n", clientIP, clientAddress.sin_port);
      fflush(stdout);
    } else if (strcmp(strtok(NULL, SPACE), HELLO) != 0){
      printf("**Error** from %s:%d\n", clientIP, clientAddress.sin_port);
      fflush(stdout);
    } else if (strcpy(userName, strtok(NULL, SPACE)) == NULL){
      printf("**Error** from %s:%d\n", clientIP, clientAddress.sin_port);
      fflush(stdout);
    } else if (strcpy(name, strtok(NULL, SPACE)) == NULL){
      printf("**Error** from %s:%d\n", clientIP, clientAddress.sin_port);
      close(clientSocket);
      fflush(stdout);
    } else if (strtok(NULL, SPACE) != NULL){
      printf("**Error** from %s:%d\n", clientIP, clientAddress.sin_port);
      fflush(stdout);
    } else{

      cookie = (atoi(strtok(clientIP, DOT)) + atoi(strtok(NULL, DOT)) + atoi(strtok(NULL, DOT)) + atoi(strtok(NULL, DOT)))*13 % 1111;
      snprintf(cookHolder, sizeof(cookHolder), "%d", cookie);
      snprintf(buffer, sizeof(buffer), "%s %s %d %s:%d", MAGIC_STRING, STATUS, cookie, inet_ntoa(clientAddress.sin_addr), clientAddress.sin_port);

      if (send(clientSocket, buffer, strlen(buffer),0) != strlen(buffer)){
        perror("Send Failure");
        exit(1);
      }

      if ((recievedMessageSize = recv(clientSocket, buffer, MAX_STR_SIZE, 0)) < 0){
        perror("Recieve Failure");
        exit(1);
      }

      buffer[recievedMessageSize] = '\0';

      if (strcmp(strtok(buffer, SPACE), MAGIC_STRING) != 0){
        printf("**Error** from %s:%d\n", inet_ntoa(clientAddress.sin_addr), clientAddress.sin_port);
        fflush(stdout);
        break;
      } else if (strcmp(strtok(NULL, SPACE), CLIENT_BYE) != 0){
        printf("**Error** from %s:%d\n", inet_ntoa(clientAddress.sin_addr), clientAddress.sin_port);
        fflush(stdout);
        break;
      } else if (strcmp(strtok(NULL, SPACE), cookHolder) != 0){
        printf("**Error** from %s:%d\n", inet_ntoa(clientAddress.sin_addr), clientAddress.sin_port);
        fflush(stdout);
        break;
      } else if (strtok(NULL, SPACE) != NULL){
        printf("**Error** from %s:%d\n", clientIP, clientAddress.sin_port);
        fflush(stdout);
        break;
      } else {
        snprintf(buffer, sizeof(buffer), "%s %s\n", MAGIC_STRING, SERVER_BYE);

        if (send(clientSocket, buffer, strlen(buffer),0) != strlen(buffer)){
          perror("Send Failure");
          exit(1);
        }
        printf("%s %s %s from %s:%d\n", cookHolder, userName, name, inet_ntoa(clientAddress.sin_addr),clientAddress.sin_port);
        fflush(stdout);
      }
    }
    close(clientSocket);
  }


}
