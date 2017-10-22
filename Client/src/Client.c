/*
 *	CLient.c - Client program. 
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
#include <signal.h>
#include "header.h"
#include <fcntl.h>

#define PORT 58000
#define GROUP_NUMBER 13
#define COMMAND_SIZE 60
#define BUFFER_SIZE 256

/* cs_port - Central Server port to contact. */
static int cs_port = PORT + GROUP_NUMBER;			//Default port.

/* host_name - Central Server Host Name. */
static char* host_name;

/* SS_server_data - IP address struct to contact SS servers.*/
static struct in_addr SS_server_data;

/* SS_Port - Storage Server port to contact.*/	
static int SS_Port;



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
		print_error("ERR:",read_bytes);	
		ptr = &buffer[0];
		while(read_bytes > 0){
			if((write_bytes = write(fd_dest,ptr,read_bytes)) <= 0){
				print_error("ERR:",write_bytes);
			}
			read_bytes -= write_bytes;
			ptr += write_bytes;
			printf("WrF: %d\n",write_bytes);
		}
	}
	printf("File wrote\n");
}

/* getHostName() - Returns the computer host name. */
char* getHostName(){
	char* buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
	
	if(gethostname(buffer,BUFFER_SIZE) == -1) 
		exit(1);
	return buffer;
}

/* getHostByName(char*) - Function get IP address struct through the 
 * host name given. */
struct in_addr* getHostByName(char* name){
	struct hostent *h;
	struct in_addr *a;
	
	if((h = gethostbyname(name)) == NULL) exit(1);
	
	a = (struct in_addr*) h->h_addr_list[0];
	
	return a;
}

/* connect_TCP() - */
void connect_TCP(char* message,struct in_addr* addr,int port){
	int n,fd_TCP,nleft,nread,nwritten,nbytes,o;
	struct sockaddr_in address;	
	socklen_t address_len;	
	char buffer[BUFFER_SIZE],*ptr;
	
	void (*old_handler)(int);				// SIGPIPE handler
	if((old_handler = signal(SIGPIPE,SIG_IGN)) == SIG_ERR) exit(1);
	
	fd_TCP = socket(AF_INET,SOCK_STREAM,0); //Initializes TCP socket.
	print_error("ERR:",fd_TCP);
	
	address_len = sizeof(address);
	memset((void*) &address, (int)'\0',sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr = *addr;
	address.sin_port = htons(port);
	n = connect(fd_TCP,(struct sockaddr*)&address,address_len);
	print_error("ERR:",n);
	
	
	ptr = strcpy(buffer,message);
	nbytes = strlen(message);	
	nleft = nbytes;
	while(nleft > 0){
		nwritten = write(fd_TCP,ptr,nleft);
		print_error("ERR:",nwritten);
		if(nwritten <= 0) exit(1);
		nleft -= nwritten;
		ptr += nwritten;
	}
	nleft = 8;								// Bytes to receive.
	ptr = &buffer[0];					// Ptr in beginning of buffer.
	while(nleft > 0){
		nread = read(fd_TCP,ptr,nleft);
		printf("%d\n",nread);
		print_error("ERR:",nread);
		if(nread == 0) break;
		ptr += nread;
		nleft -= nread; 
	}
	write(1,buffer,8);		// Print AWR new
	
	ptr = strcpy(buffer,"UPC 30 ");
	nbytes = strlen("UPC 30 ");	
	nleft = nbytes;
	while(nleft > 0){
		nwritten = write(fd_TCP,ptr,nleft);
		printf("wr: %d\n",nwritten);
		print_error("ERR:",nwritten);
		if(nwritten <= 0) exit(1);
		nleft -= nwritten;
		ptr += nwritten;
	}
	
	o = open("ju.txt",O_RDWR | O_ASYNC);
	write2(o,fd_TCP);

	nleft = 7;								// Bytes to receive.
	ptr = &buffer[0];						// Ptr in beginning of buffer.
	while(nleft > 0){	
		printf("before read\n");
		nread = read(fd_TCP,ptr,nleft);
		printf("r: %d\n",nread);
		print_error("ERR:",nread);
		if(nread == 0) break;
		ptr += nread;
		nleft -= nread; 
	}
	write(1,buffer,7);
	
	close(fd_TCP);							// "Close" TCP socket.
	close(o);
}

int verify_end_message(char* message){
	int i,strLen;
	strLen = strlen(message);
	for(i = 0; i < strLen;i++){
		if(*(message+i) == '\n')
			return 1;
	}
	return 0;
}

/* control_protocol_UDP(char*) - Splits the message protocol. */
void control_protocol_UDP(char* buffer){
	int n;
	char* token;
	char* IP;
	char* Port;
	/*int counter = 0;
	token = strtok(buffer," ");
	while(token != NULL){
		if(verify_end_message(token) == 1) 
			break;
		if(counter == 0)
			
		token = strtok(NULL," ");
	} */
	token = strtok(buffer," ");
	if(strcmp(token,"AWL") == 0){
		token = strtok(NULL," ");
		IP = token;
		token = strtok(NULL," ");
		Port = token;
	}
	 n = inet_pton(AF_INET,IP,&SS_server_data);
	 print_error("ERR:",n);
	 SS_Port = (int)atoi(Port);
}

/* connect_UDP(char*) - Open a UDP connection to send a message and
 * receive other. */
void connect_UDP(char* send,struct in_addr* addr,int port){
	int num_bytes,n,i,fd_UDP;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in address;
	socklen_t address_len;
		
	fd_UDP = socket(AF_INET,SOCK_DGRAM,0);	//Initializes UDP socket.
	print_error("ERR:",fd_UDP);
	
	memset((void*) &address, (int)'\0',sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr = *(addr);
	address.sin_port = htons(port);
	
	n = sendto(fd_UDP,send,strlen(send),0,(struct sockaddr*)&address,sizeof(address));
	print_error("ERR:",n);

	address_len = sizeof(address);
	num_bytes = recvfrom(fd_UDP,buffer,BUFFER_SIZE,0,(struct sockaddr*)&address,&address_len);
	print_error("ERR:",num_bytes);
		
	for(i = 0; i < BUFFER_SIZE-1; i++){	
		if(buffer[i] == '\n'){
			buffer[i+1] = '\0';
			break;
		}
	}
	printf("echo: %s",buffer);
	
	control_protocol_UDP(buffer);
	close(fd_UDP);			// "Close" UDP socket.			
}


/*parse_input(int,char**) -  */
void parse_input(int flag_pos,char** argv){
	if(strcmp(argv[flag_pos],"-n") == 0){
		free(host_name);
		host_name = argv[++flag_pos];
	}
	else if(strcmp(argv[flag_pos],"-p") == 0)
		cs_port = (int) atoi(argv[++flag_pos]);
}

/* main(int,char**) - Main function of client program. */
int main(int argc, char** argv){
	char* user_input = (char*) malloc(sizeof(char)*COMMAND_SIZE);
	char* user_input2 = (char*) malloc(sizeof(char)*COMMAND_SIZE);
	int list = 0;				// Client didnt ask CS for SS server,
	
	host_name = getHostName();
	
	if (argc == 3){
		parse_input(1,argv);
	}
	else if(argc == 5){
		parse_input(1,argv);
		parse_input(3,argv);
	}

	//Client infinite cycle of requests.
	while(1){
		scanf("%s",user_input);
		if(strcmp(user_input,"list") == 0){
			connect_UDP("LST\n",getHostByName(host_name),cs_port);
			list = 1;
		}
		else if ((strcmp(user_input,"retrieve") == 0) &&  (list == 1)){
			scanf("%s",user_input2);				// get File Name.
			strcpy(user_input,"REQ");
			strcat(user_input," ");
			strcat(user_input,user_input2);
			strcat(user_input,"\n");
			connect_TCP(user_input,getHostByName(host_name),cs_port);
		}
		else if (strcmp(user_input,"upload") == 0){
			scanf("%s",user_input2);				// File to upload.
			strcpy(user_input,"UPR");
			strcat(user_input," ");
			strcat(user_input,user_input2);
			strcat(user_input,"\n");
			connect_TCP(user_input,getHostByName(host_name),cs_port);
		}
		else if (strcmp(user_input,"exit") == 0){
			break;
		}
	}
	exit(0);
} 
