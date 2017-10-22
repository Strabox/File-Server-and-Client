/*
 * Files.c - File Manager. 
 */
#include <stdlib.h>
#include "Files.h"

#define MAX_FILES 30
#define MAX_FILE_NAME_SIZE 30

struct File_Manager{
	char** file_list;
	int num_files;
};

/* */
file_manager initFileManager(int max_files){
	int i;
	file_manager fm = (file_manager) malloc(sizeof(struct File_Manager));
	fm->num_files = 0;
	fm->file_list = (char**) malloc(sizeof(char*)*MAX_FILES);
	for(i = 0; i < MAX_FILES;i++)
		*(fm->file_list+i) = NULL;

	return fm;
}

/* */
void addFile(file_manager fm,char* name){
	if(fm->num_files < MAX_FILES){
		*(fm->file_list + fm->num_files) = (char*) malloc(sizeof(char)*MAX_FILE_NAME_SIZE);
		fm->num_files++;
	}
}

/* */
void destroyFileManager(){
	
}

//######################################

struct SS_Server{
	char* name;
	int port;
};

struct SS_Server_Manager{
	ss_server* servers;
	int num_serv;
};

/* */
ss_server_manager initSSservers(int num_servers){
	ss_server_manager sm = (ss_server_manager) malloc(sizeof(struct SS_Server_Manager));
	
	sm->servers = (ss_server*) malloc(sizeof(ss_server)*num_servers);
	sm->num_serv = num_servers;
	return NULL;
}

/* */
ss_server getClosestServer(ss_server_manager sm){
	
	return NULL;
}

