/*
 * SSserver.c - Main file for Storage Server.
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
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h> 

#define SS_PORT 59000
#define BUFFER_SIZE 128
#define MAX_QUEUE_REQUESTS 10

/* ss_port - Server Storage port. */
static int ss_port = SS_PORT;

/* getHostByAdress(struct) - */
void getHostByAdress(struct sockaddr_in address){
	struct hostent* host;
	
	host = gethostbyaddr((char*)&address.sin_addr,sizeof(struct in_addr),AF_INET);
	if(host == NULL)
		printf("sent by [%s:%hu]\n",inet_ntoa(address.sin_addr),ntohs(address.sin_port));
	else
		printf("sent by [%s:%hu]\n",host->h_name,ntohs(address.sin_port));
}

/* list_dir_files() - USELESS NOW */
void list_dir_files(char* dir_path){
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dir_path)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			printf ("%s\n", ent->d_name);
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
		exit(1);
	}
}

/* parse_input() -  */
void parse_input(int flag_pos,char** argv){
	if(strcmp(argv[flag_pos],"-p") == 0)
		ss_port = (int) atoi(argv[++flag_pos]);
}

/* */
void write_file(int fd_src,int fd_dest){
	int read_bytes,write_bytes;
	char buffer[BUFFER_SIZE];
	char* ptr;
	
	while((read_bytes = read(fd_src,buffer,BUFFER_SIZE)) != 0){
	
		ptr = &buffer[0];
		while(read_bytes > 0){
			if((write_bytes = write(fd_dest,ptr,read_bytes)) <= 0){
				printf("ERR: %s\n",strerror(errno));
				exit(1);
			}
			read_bytes -= write_bytes;
			ptr += write_bytes;
		}
	}
}

/* launch_TCP_Server() - */
void launch_TCP_Server(){
	int newfd,n,fd_TCP,fd_file;
	char buffer[BUFFER_SIZE];
	struct sockaddr_in address;
	socklen_t address_len;
	
	if((fd_TCP = socket(AF_INET,SOCK_STREAM,0)) == -1) exit(1);
		
	memset((void*)&address,(int)'\0',sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = htonl(INADDR_ANY);
	address.sin_port = htons(ss_port);
	
	if(bind(fd_TCP,(struct sockaddr*)&address,sizeof(address)) == -1)
		exit(1);
	if(listen(fd_TCP,MAX_QUEUE_REQUESTS) == -1) exit(1);
	
	// Infinite cycle of server accepting requests.
	while(1){
		address_len = sizeof(address);
		if((newfd = accept(fd_TCP,(struct sockaddr*)&address,&address_len)) == -1)
			exit(1);
		getHostByAdress(address);
		
		fd_file = open("./TestFiles/F1",O_RDONLY);	
		write_file(fd_file,newfd);
		close(fd_file);
				
		close(newfd);
	}
	close(fd_TCP);			//"Close" TCP socket.
}

/* main(int,char**) -  */
int main(int argc,char** argv){
	
	if(argc == 3)			
		parse_input(1,argv);
	
	launch_TCP_Server();
	
	exit(0);
}
