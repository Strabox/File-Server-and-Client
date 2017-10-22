#ifndef FILES_FILE
#define FILES_FILE

typedef struct File_Manager* file_manager;

typedef struct SS_Server* ss_server;

typedef struct SS_Server_Manager* ss_server_manager;

file_manager initFileManager(int max_files);

void addFile(file_manager fm,char* name);

#endif 
