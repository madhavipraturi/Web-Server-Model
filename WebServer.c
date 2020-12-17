#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

char webpage[]=
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html; charset=UTF-8\r\n\r\n"
	"<!DOCTYPE html>\r\n"
	"<head>\r\n"
	"<title>Lab 2: A Web Server Model</title>\r\n"
	"</head>\r\n"
	"<body>\r\n"
  	"<p>\r\n"
    "<a href=\"http://www.unm.edu\">University of New Mexico</a>\r\n"
  	"</p>\r\n"
  	"<h1> CS485/585 </h1>\r\n"
  	"<h2> Lab 2: A Web Server Model </h2>\r\n"
  	"<h3> Deadline: Octobor 13 </h3>\r\n"
  	"<p> Comic of the day! </p>\r\n"
  	"<CENTER><IMG SRC=\"comic.gif\" ALIGN=\"BOTTOM\"/> </CENTER>\r\n"
	"</body>\r\n";

int main(int argc, char *argv[]){

	int port_number;

	if(argc==3 && ( strcmp(argv[2],"TCP")==0 || strcmp(argv[2],"UDP")==0 ) ){

		printf("The port# is: %s\n",argv[1]);
		printf("The protocol is: %s\n",argv[2]);
		port_number=atoi(argv[1]);//convert the string to an integer 

	}else{
		printf("%s\n", "Invalid parameters!");
		printf("%s\n", "Usage: ./Webserver port# protocal");
		return 1;
	}
	

	//Create a socket for server
	int server_socket;
	int client_socket;
	int fd_image;
	char buffer[4096];

	//AF=address family  PF=protocal family AF_INET=>use IPv4, AF_INET6=>use IPv6
	//SOCK_STREAM for TCP and SOCK_DGRAM for UDP
	if(strcmp(argv[2],"TCP")==0){
		server_socket=socket(AF_INET,SOCK_STREAM,0);
		if(server_socket<0){
			perror("socket");
			return 1;
		}
	}else{
		server_socket=socket(AF_INET,SOCK_DGRAM,0);
		if(server_socket<0){
			perror("socket");
			return 1;
		}
	}	

	
	
	if(strcmp(argv[2],"TCP")==0){

		//Define the address for the server
		struct sockaddr_in server_address, client_addr;
		server_address.sin_family= AF_INET;//use IPv4
		server_address.sin_port=htons(port_number);//assign the port number get from command line to this ports
		server_address.sin_addr.s_addr = htonl(INADDR_ANY);//assign a IP address  /*inet_addr("64.106.20.209")*/

		//bind the server address with the socket 
		if(bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address))==-1){
			perror("socket");
			close(server_socket);
			return 1;
		}
		//Activate the server with a connecting queue of size 128
		if(listen(server_socket,128)==-1){
			perror("listen");
			close(server_socket);
			return 1;
		}else{
			printf("The web server starts!\n");
		}

		//Set the server ready to accept request from clients
		socklen_t addr_size = sizeof(client_addr);
		
		//Keep accepting connection requests from different clients
		while(1){
			client_socket = accept(server_socket,(struct sockaddr *)&client_addr,&addr_size);
			if(client_socket==-1){
				perror("connection failed\n");
				continue;
			}

			printf("%s\n","New TCP Client connection in accepted.");
			if(!fork()){

				//closing child process
				close(server_socket);
				memset(buffer,0,4096);
				read(client_socket,buffer,4095);
				printf("Web Server: %s\n",buffer);

				char imageheader[] = 
					"HTTP/1.1 200 Ok\r\n"
					"Content-Type: image/jpeg\r\n\r\n";

				if(!strncmp(buffer,"GET /comic.gif",14)){
					
					//sending the image header first
					write(client_socket,imageheader,sizeof(imageheader)-1);

					//sending the image content
					fd_image=open("comic.gif",O_RDONLY);
					sendfile(client_socket,fd_image,NULL,30000);
					close(fd_image);
				}else{
					write(client_socket, webpage,sizeof(webpage)-1);

				}
				close(client_socket);
				printf("closing\n");
				return 0;
			}

			//closing parent process
			close(client_socket);
			
		}
	}else if(strcmp(argv[2],"UDP")==0){
		

		int sockfd; 
	    struct sockaddr_in servaddr, cliaddr; 
	      
	    // Creating socket file descriptor 
	    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
	        perror("socket creation failed"); 
	        exit(EXIT_FAILURE); 
	    } 
	      
	    memset(&servaddr, 0, sizeof(servaddr)); 
	    memset(&cliaddr, 0, sizeof(cliaddr)); 
	      
	    //Filling server information 
	    servaddr.sin_family = AF_INET; // IPv4 
	    servaddr.sin_addr.s_addr = INADDR_ANY; 
	    servaddr.sin_port = htons(port_number); 
	      
	    // Bind the socket with the server address 
	    if ( bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ){ 
	        perror("Bind failed"); 
	        exit(EXIT_FAILURE); 
	    } 
	      
	    int len, n; 
	    while(1){
	        n = recvfrom(sockfd, (char *)buffer, 4096,  
	                MSG_WAITALL, ( struct sockaddr *) &cliaddr, 
	                &len); 
	        buffer[n] = '\0'; 
	        printf("Client : %s\n", buffer); 
	        sendto(sockfd, (const char *)webpage, strlen(webpage),  
	            MSG_CONFIRM, (const struct sockaddr *) &cliaddr, 
	                len); 
	        printf("Webpage is sent through UDP.\n");  
	        close(server_socket);
	    }
		
	    return 0;


	}else{
		perror("Protocol not found!\n");
		return 1;
	}
	close(server_socket);
	return 0;

}