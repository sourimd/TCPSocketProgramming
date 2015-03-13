/*
Steps:-
1.	Create a Socket
2.	Connect the socket with servers address and port of communication
3.	Send and Receive message to/from the server over that socket
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define RCVBUFFERSIZE 32

void error( const char * errorMessage){
	perror( errorMessage );
	exit(1);
}

int main( int argc, char * argv[]){
	int sock;  //Socket Descriptor
	struct sockaddr_in serverAddress; // Server Address structure
	unsigned int serverPort;    // Server Port to connect to
	char * serverIP;            // Server's IP
	char * messageToSendToServer;   // message to be send to server
	char recvMessageBuffer[RCVBUFFERSIZE];   // message send from tcp server will be stored here
	unsigned int messageLength;   // length of the message to send to the server
	int bytesRecvd, totalBytesRecvd; //bytes received in a single recv() call and total bytes received respectively
	int connectionStatus;

	if( (argc < 3) || (argc>4) ){
		error(" Invalid number ofarguments passed ");
	}

	serverIP = argv[1];
	messageToSendToServer = argv[2];

	if( argc == 4 ){
		serverPort = atoi( argv[3] );
	}
	else{
		serverPort = 8080; // if the user does not pass any port then default port 8080 will be used to connect to the server

 	}

 	//create socket over which communication will happen
 	// PF_INET indicates that TCP/IP protocol suite will used for the  communication
 	// SOCK_STREAM indicates that communication will be over reliable byte stream channel
 	// Since it is going to be reliable byte stream communication TCP has to be used as opposed
 	// to UDP, Hence IPPROTO_TCP is provided
 	sock = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP );
 	if( sock < 0){
 		error("Error in getting socket");
 	}
 	memset( &serverAddress, 0, sizeof(serverAddress) ); // ensure that there are no gibberish in the server address structure

 	// fill in the server address structure with server's address and port
 	serverAddress.sin_family = AF_INET; // IPv4 Address family
 	serverAddress.sin_addr.s_addr = inet_addr( serverIP); // put in server's IP Address
 	serverAddress.sin_port = htons( serverPort );

 	//establish connection with the server over the socket created
 	connectionStatus = connect(sock, (struct sockaddr *) &serverAddress, sizeof(serverAddress) );
 	if( connectionStatus < 0 ){
 		error("Couldn't establish connection with the server");
 	}

 	messageLength = strlen( messageToSendToServer );

 	//send message and check if the entire message has been send 
 	if( send(sock, messageToSendToServer, messageLength, 0) != messageLength ){
 		error("Didn't send the entire message it is supposed to send");
 	}

 	// Receive what is send from server
 	totalBytesRecvd = 0;
 	puts("received");

 	// whatever is received from server is put into recvMessageBuffer
 	bytesRecvd = recv( sock, recvMessageBuffer, RCVBUFFERSIZE-1, 0 );

 	puts( recvMessageBuffer );
 	printf("\n");

 	close( sock );
}