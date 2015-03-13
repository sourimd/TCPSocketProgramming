/*
Steps:-
1.	Create a Socket
2.	Bind this socket with server's local address and a port
3.	Listen for client connection requests over that socket
4.	Whenever a client requests for a connection, the request is stored in a queue
5.	The server deques the queue, and create a new socket and binds this socket with the requesting client's address and port
	(This socket will be used for communication with the client)
6.	Send and Receive message to/from client over this socket
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define MAXNUMBEROFCLIENTS 5
#define RECVMESSAGEBUFFERSIZE 128

void error( const char * errorMessage){
	perror( errorMessage );
	exit(1);
}

void communicateWithClient( int clientSock ){
	const char * messageToSendToClient = "Hello Client. Got your message.";
	char messageFromClient[RECVMESSAGEBUFFERSIZE];
	int messageFromClientSize;

	// receive message from client
	messageFromClientSize = recv( clientSock, messageFromClient, RECVMESSAGEBUFFERSIZE, 0);
	if( messageFromClientSize < 0){
		error("Error in receiving message from client!!");
	}

	//send message to the client and receive from message back from it so long it is sending
	while( messageFromClientSize > 0){
		send(clientSock, messageToSendToClient, strlen(messageToSendToClient), 0);

		messageFromClientSize = recv( clientSock, messageFromClient, RECVMESSAGEBUFFERSIZE, 0);
		if( messageFromClientSize < 0){
			error("Error in receiving message from client!!");
		}
	}

}

int main( int argc, char * argv[]){

	int serverSock; // socket descriptor on which the server will listen for client's request connection
	int clientSock; // socket descriptor which will be used to send/recv messages to/from client

	struct sockaddr_in serverAddress; //local address of the server itself
	struct sockaddr_in clientAddress;

	unsigned int serverPort; // local port on which the server will listen for connection requests
	unsigned int clientLength;  // length of cleint address data structure

	if( argc != 2){
		error("Invalid number of arguments");
	}

	// user provided port
	serverPort = atoi( argv[1] );

	// create socket over which the server will listen for connectio requests
	serverSock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
	if( serverSock < 0 ){
		error("Error in creating listening socket");
	}

	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY); // Any incoming address on any network interface
	serverAddress.sin_port = htons( serverPort );

	//bind the socket to the local addr/port combination
	if( bind(serverSock, (struct sockaddr *)&serverAddress, sizeof(serverAddress) ) < 0){
		error(" Could not bind socket with local address");
	}

	// make the server listen on the socket for connection
	// also pass in the max limit of the queue
	listen( serverSock, MAXNUMBEROFCLIENTS );

	//server should listen forever
	while(1){
		clientLength = sizeof(clientAddress);
		// new client socket through which server will send/recv messages to/from the client
		clientSock = accept( serverSock, (struct sockaddr *)&clientAddress, &clientLength );
		if( clientSock < 0){
			error("Couldn't create socket for sending/receiving messages to/from client");
		}
		printf( "Serving client %s\n", inet_ntoa(clientAddress.sin_addr) );

		//recv and/or send messages to client using clientSock
		communicateWithClient( clientSock );
		close( clientSock );
	}
}