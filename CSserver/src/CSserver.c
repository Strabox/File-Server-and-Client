/*
 * CSserver.c - Main file for Central Server.
 */
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "Files.h"

#define GROUP_NUMBER 13
#define CS_PORT 58000
#define BUFFER_SIZE 128
#define NAME_FILE_SIZE 20
#define MAX_QUEUE_REQUESTS 10

/* cs_port - Port where Central Server accepts requests
 * 			 working for TCP and UDP requests.*/
static int cs_port = CS_PORT + GROUP_NUMBER;

/* fd_UDP - File Descriptor for UDP server. */
static int fd_UDP;

/* fd_TCP - File Descriptor for TCP server.*/
static int fd_TCP;

/* print_error(char*,int) - print error and close program if system call 
 * fails. */
void print_error(char* message,int test){
	if(test == -1){
		printf("%s ",message);
		printf("%s\n",strerror(errno));
		exit(1);
	}
}

/* write2(int,int) - Write bytes from fd_src to the fd_dest
 * (fd_src and fd_dest are file descriptors). */
void write2(int fd_src,int fd_dest){
	int read_bytes,write_bytes;
	char buffer[BUFFER_SIZE];
	char* ptr;
	
	while((read_bytes = read(fd_src,buffer,BUFFER_SIZE)) != 0){
		printf("ola\n");
		print_error("ERR:",read_bytes);
		printf("ola2\n");	
		ptr = &buffer[0];
		printf("rrf: %d\n",read_bytes);
		while(read_bytes > 0){
			if((write_bytes = write(fd_dest,ptr,read_bytes)) <= 0){
				print_error("ERR:",write_bytes);
			}
			read_bytes -= write_bytes;
			ptr += write_bytes;
			printf("wrf: %d\n",write_bytes);
			printf("rrf: %d\n",read_bytes);
		}
	}
	printf("File Wrote\n");
	return;
}

/* getHostByAdress - */
void getHostByAdress(struct sockaddr_in address){
	struct hostent* host;
	
	host = gethostbyaddr((char*)&address.sin_addr,sizeof(struct in_addr),AF_INET);
	if(host == NULL)
		printf("sent by [%s:%hu]\n",inet_ntoa(address.sin_addr),ntohs(address.sin_port));
	else
		printf("sent by [%s:%hu]\n",host->h_name,ntohs(address.sin_port));
}

/* */
char* parse_message(char* message){
	//printf("UDP rec: %s\n",message);
	if(strncmp(message,"LST\n",4) == 0){
		return "AWL 45223 3 F1 F2 F3\n";
	}
	return "ERR\n";
}

/* launch_UDP_Server() -  */
void launch_UDP_Server(){
	int ret,nread, child_status;
	struct sockaddr_in address;
	socklen_t address_len;
	char buffer[BUFFER_SIZE];
	char* output;

	memset((void*) &address,(int)'\0',sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(cs_port);
	
	ret = bind(fd_UDP,(struct sockaddr*)&address,sizeof(address));
	if(ret == -1) exit(1);
	
	while(1){		
		address_len = sizeof(address);
		nread = recvfrom(fd_UDP,buffer,BUFFER_SIZE,0,(struct sockaddr*)&address,&address_len);
		if(nread == -1) exit(1);
		
		write(1,"echo: ",6);
		write(1,buffer,nread);
		getHostByAdress(address);		// Print user info.
		output = parse_message(buffer);
		
		ret = sendto(fd_UDP,output,strlen(output),0,(struct sockaddr*)&address,address_len);
		if(ret == -1) exit(1);
	}
	
	wait(&child_status);	// Waiting for child to terminate.
	close(fd_UDP); 			// "Close" UDP socket.
}

/* launch_TCP_Server() - */
void launch_TCP_Server(){
	int newfd,n,nw,fd_file;
	char buffer[BUFFER_SIZE], *ptr;
	struct sockaddr_in address;
	socklen_t address_len;
	//char file_name[NAME_FILE_SIZE];
	
	void (*old_handler)(int);				// SIGPIPE handler
	if((old_handler = signal(SIGPIPE,SIG_IGN)) == SIG_ERR) exit(1);
	
	memset((void*)&address,(int)'\0',sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(cs_port);
	
	if(bind(fd_TCP,(struct sockaddr*)&address,sizeof(address)) == -1)
		exit(1);
	if(listen(fd_TCP,MAX_QUEUE_REQUESTS) == -1) exit(1);
	
	// Infinite cycle of server accepting requests.
	while(1){
		address_len = sizeof(address);
		if((newfd = accept(fd_TCP,(struct sockaddr*)&address,&address_len)) == -1)
			exit(1);
		write(1,"Echo: ",6);
		while((n = read(newfd,buffer,3)) < 3);
		
		if(strncmp(buffer,"UPR",3) == 0){
			ptr = &buffer[0];
			while((n = read(newfd,ptr,1)) != 0){
				if(*ptr == '\n')
					break;
				ptr++;
			}
			strcpy(buffer,"AWR new\n");
			n = 8;
			ptr = &buffer[0];
			while(n > 0){
				if((nw = write(newfd,ptr,n)) <= 0) exit(1);
				printf("Wr1: %d\n",nw);
				n -= nw;
				ptr += nw;
			}
			
			while((n = read(newfd,buffer,4)) < 4);
			write(1,buffer,4);
			
			ptr = &buffer[0];
			while((n = read(newfd,ptr,1)) != 0){
				if(*ptr == ' '){
					break;
				}
				ptr++;
			}
			write(1,buffer,2);
			printf("vai ler file\n");
			fd_file = open("f1.txt",O_CREAT | O_WRONLY,00700);
			write2(newfd,fd_file);
			close(fd_file);
			strcpy(buffer,"AWC ok\n");
			n = 7;
			ptr = &buffer[0];
			while(n > 0){
				if((nw = write(newfd,ptr,n)) <= 0) exit(1);
				printf("Wr2: %d",nw);
				n -= nw;
				ptr += nw;
			}			
		}
		else if(strncmp(buffer,"AWS",3) == 0){
			ptr = &buffer[0];
			while((n = read(newfd,ptr,1)) != 0){
				if(*ptr == '\n')
					break;
				ptr++;
			}
		}
		/*			
		while((n = read(newfd,buffer,BUFFER_SIZE)) != 0){
			if(n == -1) exit(1);
			write(1,buffer,n);			// Prints the request.
			ptr = &buffer[0];
			while(n > 0){
				if((nw = write(newfd,ptr,n)) <= 0) exit(1);
				n -= nw;
				ptr += nw;
			}
		}*/
		getHostByAdress(address);		// Prints the source info.	
		close(newfd);
	}
	close(fd_TCP);			//"Close" TCP socket.
}

int main(int argc, char **argv){
	int child_pid;
	
	if(argc == 3){
		cs_port = (int) atoi(argv[2]);
	}
	
	if((fd_UDP = socket(AF_INET,SOCK_DGRAM,0)) == -1) exit(1);
	if((fd_TCP = socket(AF_INET,SOCK_STREAM,0)) == -1) exit(1);
	child_pid = fork();
	if(child_pid == 0){			// TCP for exchanging information.
		launch_TCP_Server();
	}
	else{						// UDP for receiving requests.
		launch_UDP_Server();
	}
	
	exit(0);
}
