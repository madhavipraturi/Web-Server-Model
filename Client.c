#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>

#include <signal.h>
#include <sys/time.h>
#define TIME_OUT 5
char URL[]="www.cs585.com";

void alarm_wakeup(int i)
{
   signal(SIGALRM,alarm_wakeup);
   printf("UDP faliure, %d seconds passed, time out!\n",TIME_OUT);
   exit(0);
}



int main(int argc, char *argv[]){

	char protocol[4];

	//Get the URL and protocol from the command window.
	if(argc==2 && ( strcmp(argv[1],"TCP")==0 || strcmp(argv[1],"UDP")==0 ) ){

		//protocol=argv[1];
		strcpy(protocol,argv[1]);
		printf("The protocol used is: %s\n",protocol);
		printf("The user did not speicfy the URL.");


	}else if( argc==3 && ( strcmp(argv[2],"TCP")==0 || strcmp(argv[2],"UDP")==0 ) ){

		//protocol=argv[2];
		strcpy(protocol,argv[2]);
		printf("The protocol is: %s\n",protocol);
		printf("The URL is: %s\n", argv[1]);
		if(strcmp(argv[1],URL)!=0){
			printf("404 ERROR, Page not found!\n");
			return 0;
		}

	}else{
		printf("%s\n", "Invalid parameters!");
		printf("usage: %s\n", "./Client URL(www.cs585.com) protocol");
		return 1;
	}


	//connect to the address of web Server
	char *address;
	address="127.0.0.1";//this IP address need to be convereted later by inet_pton function, use hostname -I to find local IP

	//create the socket for client
	int client_socket;
	if( strcmp(protocol,"TCP")==0 ){
		client_socket=socket(AF_INET, SOCK_STREAM, 0);
	}else{
		client_socket=socket(AF_INET, SOCK_DGRAM, 0);
	}
	

	if(strcmp(protocol,"TCP")==0){
		//connect to an address, not the usual 80 port for HTTP
		struct sockaddr_in remote_address;
		memset(&remote_address, 0, sizeof(remote_address));
		remote_address.sin_family = AF_INET;
		remote_address.sin_port = htons(8080);
		//inet_aton(address, &remote_address.sin_addr.s_addr);//convert the IP address to net order HEX address
		inet_pton(AF_INET, address, &remote_address.sin_addr.s_addr);

		int connect_result;
		printf("%s\n","begin to make connection");
		connect_result=connect(client_socket, (struct sockaddr *) &remote_address, sizeof(remote_address) );

		if(connect_result==0){
			printf("connection to %s established\n",address);
		}else{
			printf("connection to %s faliure\n",address);
			return 1;
		}

		if(argc==2){
			char word[14];
			printf("Please input the URL down here: (Press ENTER when you are DONE)\n");
			fgets(word,sizeof(word),stdin);
			if(strcmp(URL,word)==0){
				printf("Result returned~");
			}else{
				printf("404 ERROR, Page not found!\n");
				return 0;
			}

		}

		//send the GET request to the web server
		char request[] = "GET /lab2.html HTTP/1.0\r\nHOST:cs585.org\r\n\r\n";
		write(client_socket,request,strlen(request));
		
		//respond from the server
		char response[4096];
		read(client_socket,response,4096);
		printf("%s",response);

		//send the request, and receive info from the server 
		send(client_socket, request, sizeof(request), 0);
		recv(client_socket, &response, sizeof(response), 0);

		printf("Response from the server is: %s\n", response);

		close(client_socket);
	}else if(strcmp(protocol,"UDP")==0){

		int sockfd; 
	    char buffer[1024]; 
	    char *hello = "Hello from client"; 
	    struct sockaddr_in     servaddr; 
	  
	    // Creating socket file descriptor 
	    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
	        perror("socket creation failed"); 
	        exit(EXIT_FAILURE); 
	    } 
	  
	    memset(&servaddr, 0, sizeof(servaddr)); 
	      
	    // Filling server information 
	    servaddr.sin_family = AF_INET; 
	    servaddr.sin_port = htons(8080); 
	    //servaddr.sin_addr.s_addr = INADDR_ANY; 
	    inet_pton(AF_INET, address, &servaddr.sin_addr.s_addr);
	      
	    int n, len; 
	      
	    sendto(sockfd, (const char *)hello, strlen(hello), 
	        MSG_CONFIRM, (const struct sockaddr *) &servaddr,  
	            sizeof(servaddr)); 
	    printf("Trying to make connection with server.\n"); 

	    // char *message1 = "thread1";
	    // int ret_thread1 = pthread_create(&thread1,NULL,(void *)&print_message_func,(void *)message1);


	    // double t = clock(); //start timing
	    // t=clock()-t;
	    // printf("time is: %f\n mseconds",t);
	      struct itimerval tout_val;  
		  tout_val.it_interval.tv_sec = 0;
		  tout_val.it_interval.tv_usec = 0;
		  tout_val.it_value.tv_sec = TIME_OUT; /* set timer for "INTERVAL (10) seconds */
		  tout_val.it_value.tv_usec = 0;
		  setitimer(ITIMER_REAL, &tout_val,0);
	      signal(SIGALRM,alarm_wakeup); //

	    n = recvfrom(sockfd, (char *)buffer, 1024,  
	                MSG_WAITALL, (struct sockaddr *) &servaddr, 
	                &len); 
	    buffer[n] = '\0'; 
	    printf("Server response: %s\n", buffer); 
	    
	  
	    close(sockfd); 

		
	}else{
		perror("protocol not found!\n");
		return 1;
	}



	return 0;

}