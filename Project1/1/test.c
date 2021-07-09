#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>

#include "pipe.h"

#define _GNU_SOURCE





int read_finished=0;		//flags gia join
int write_finished=0;

void write_process(char* file_name) {				//Function that reads from file and writes to fifo
	int fd;
	char scanned;
	
	fd = open(file_name,0,S_IRWXU);
	if (fd == -1)
		printf("Error opening file.\n");
	
	while (read(fd,&scanned,1)) {
		pipe_write(scanned);
	}
	
	pipe_close();
	write_finished=1;
	
	
	
}
void read_process() {											//function that reads from fifo,and writes to a file that it creates.
	char p;
	int fd,err;
	fd = open("CopiedFile",O_CREAT|O_RDWR,S_IRWXU);
	if (fd == -1)
		printf("Error creating file.\n");

	while(pipe_read(&p)!=0) {
		err = write(fd,&p,sizeof(char));
		if (err == -1) {
			printf("Error writing to file\n");
		}
	}
	read_finished=1;
	
}

int main(int argc,char* argv[]){
	char file_name[255];
	int err;
	pthread_t write_thread,read_thread;
	if (argc<2) {
		printf("Please enter file name as argument!\n");
		return 0;
	}
	strcpy(file_name,argv[1]);
	pipe_init(64);
	if (pthread_create(&write_thread,NULL,(void*)write_process,(void*)file_name))
		printf("Error creating thread!\n");
	if (pthread_create(&read_thread,NULL,(void*)read_process,NULL))
		printf("Error creating thread!\n");


	while(read_finished==0 || write_finished==0){
		err = sched_yield();
		if (err == -1) 
			printf("Error yielding CPU!\n");
		
	}
	
	
	return 0;
}
