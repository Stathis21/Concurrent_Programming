#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#define _GNU_SOURCE

char * fifo;
char * start;
char * end;
int write_index;
int read_index;
int elements;
int capacity;
int closed = 0;

volatile int crit_write = 0;	//flags gia critical section elements
volatile int crit_read = 0;


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
	
	while(elements>=capacity){						//Condition
	
	}
	
	*(fifo+write_index) = c;
	write_index = (write_index + 1) % capacity;
	
	while (crit_read == 1) {						//Critical section,element incrementation
	}
	crit_write = 1;
	elements++;
	crit_write = 0;
		

}

int pipe_read(char *c){								//Function that reads from fifo 
	
	while(elements==0 ){
		if(closed == 1){
			return 0;
		}
		
	}
	
	*c = *(fifo + read_index);
	read_index = (read_index + 1) % capacity;
	
	while (crit_write == 1) {						//Critical section,element incrementation
	}
	crit_read = 1;
	elements--;
	crit_read = 0;
	return 1;
}

void pipe_close(){
	closed = 1;
	
}

