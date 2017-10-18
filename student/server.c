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

//Basic socket level code refrenced from TCP-IP Sockets in C.

int main (int argc, char **argv){
  //Declares go here
  int currConnectedClients;
  int serverSocket;
  int clientSocket;
  int recievedMessageSize;
  struct sockaddr_in serverAddress = {0};
  struct sockaddr_in clientAddress = {0};
  unsigned short serverPort;
  unsigned int clientLength;
  char buffer[MAX_STR_SIZE];

  //Check right number of inputs
  if (argc < 2){
    printf(stderr, "Usage: %s <Server Port>\n", argv[0]);
    exit(1);
  }

  serverPort = atoi(argv[1]);

  //Make socket
  if ((serverSocket = socket(PF_INET, SOCK_STREAM, IPROTO_TCP)) < 0){
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
  while(true){


    //Waiting for the client to connect
    if ((clientSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, clientLength)) < 0){
      perror("Accept failed");
      exit(1);
    }

    printf("Handling client %s\n", inet_ntoa(clientAddress.sin_addr));
    if ((recievedMessageSize = recv(clientSocket, buffer, MAX_STR_SIZE, 0)) < 0){
      perror("Recieve failed");
      exit(1);
    }

    printf("Message: %s", echoBuffer);

  }


}
