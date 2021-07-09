#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include "mycoroutines.h"

#define _GNU_SOURCE


co_t mainco,producer,consumer;


char * fifo;
char * start;
char * end;
int write_index;
int read_index;
int elements;
int capacity;
int closed = 0;

int read_finished=0;		//flags gia join
int write_finished=0;


void pipe_init(int size){							//Initialization
	fifo = malloc(size * sizeof(char));
	if (fifo == NULL) 
		printf("Error allocating memory\n");
	start = fifo;
	end = fifo + (size - 1)*sizeof(char);
	write_index = 0;
	read_index = 0;
	elements = 0;
	capacity = size;
}

void pipe_write(char c){
	
	if(elements>=capacity){						//Condition
		mycoroutines_switchto(&consumer);
	}
	
	*(fifo+write_index) = c;
	write_index = (write_index + 1) % capacity;
	

	elements++;

}

int pipe_read(char *c){								//Function that reads from fifo 
	
	if(elements==0 ){
		
		if(closed == 1){
			mycoroutines_switchto(&mainco);
			return 0;
		}
		
		mycoroutines_switchto(&producer);
	}
	
	*c = *(fifo + read_index);
	read_index = (read_index + 1) % capacity;
	
	elements--;
	
	return 1;
}

void pipe_close(){
	closed = 1;
	
}

void write_process(char* file_name) {				//Function that reads from file and writes to fifo
	int fd;
	char scanned;
	
	fd = open(file_name,0,S_IRWXU);
	if (fd == -1)
		printf("Error opening file.\n");
	
	while (read(fd,&scanned,1)) {
		pipe_write(scanned);
	}
	close(fd);
	pipe_close();
	
	mycoroutines_switchto(&consumer);
	
	
	
	
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
	
}

int main(int argc,char* argv[]){
	char file_name[255];
	char diff_command[30];
	if (argc<2) {
		printf("Please enter file name as argument!\n");
		return 0;
	}
	strcpy(file_name,argv[1]);
	pipe_init(64);


	mycoroutines_init(&mainco);
	
	
	
	mycoroutines_create(&producer, (void*)write_process,(void*)file_name);
	mycoroutines_create(&consumer, (void*)read_process,NULL);
	mycoroutines_switchto(&producer);
	
	
	mycoroutines_destroy(&producer);
	mycoroutines_destroy(&consumer);
	mycoroutines_destroy(&mainco);
	printf("Executing diff between %s and CopiedFile:\n",file_name);
	sprintf(diff_command,"diff %s CopiedFile\n",file_name);
	system(diff_command);
	
	
	return 0;
}






