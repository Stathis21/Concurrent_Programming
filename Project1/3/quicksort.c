#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <math.h>
#include <time.h>


int* array;

typedef struct boundaries{		//used as argument in thread_function
	int left,right;
	int finished;				//flag used to notify parent 
}boundaries;

void* worker_thread(boundaries* bounds) {
	int pivot;
	int left = bounds->left, right = bounds->right;
	boundaries *bounds_left_thread, *bounds_right_thread;		//pointer to pass boundary and terminate arguments to children 
	
	pthread_t t1,t2;
	
	
	if (right - left < 1 || left >= right) {		//trivial sort
		bounds->finished = 1; //done
		return(NULL);
		
	}
	else {
		int i=left,j=right,temp;
		pivot = bounds->left;			//take pivot as the leftmost element
		while(i<j){		//quicksort implementation
			while(array[i]<=array[pivot]&&i<right)
				i++;
			while(array[j]>array[pivot])
				j--;
			if(i<j){
				temp=array[i];
				array[i]=array[j];
				array[j]=temp;;
			}
		}
		temp=array[pivot];
		array[pivot]=array[j];
		array[j]=temp;
		//}
		
		
		bounds_left_thread = malloc(sizeof(boundaries));//arguments for children threads are dynamically allocated
		bounds_right_thread = malloc(sizeof(boundaries));
		
		bounds_left_thread->left = left;
		bounds_left_thread->right = j-1;
		bounds_right_thread->left = j+1;
		bounds_right_thread->right = right;
		
		bounds_left_thread->finished = 0;
		bounds_right_thread->finished = 0;
			
		if(!pthread_create(&t1,NULL,(void*)worker_thread,(void*)bounds_left_thread)){//recursion
			printf("Error creating thread!\n");
		} 
		if(!pthread_create(&t2,NULL,(void*)worker_thread,(void*)bounds_right_thread)){
			printf("Error creating thread!\n");
		}
		
		while(bounds_left_thread->finished==0 || bounds_right_thread->finished==0){ //wait for children before terminating
			sched_yield();
		}
		free(bounds_left_thread);
		free(bounds_right_thread);
		
		bounds->finished = 1; // i am done
		return(NULL);
	}
}
	
	



int main(int argc,char* argv[]) {
	int i,size = argc - 1;
	pthread_t main_thread;
	boundaries* bounds;
	
	if(argc<2){
		printf("Please enter the array as an argument. a1 a2 a3.. an\n");
		return -1;
	}
	
	array = (int*) calloc(size,sizeof(int));
	if(array==NULL){
		printf("Error allocating memory!\n");
	}
	
	for (i=0; i<size; i++) {
		array[i] = atoi(argv[i+1]);
	}
	
	bounds = malloc(sizeof(boundaries));
	if(bounds==NULL){
		printf("Error allocating memory!\n");
	}
	
	bounds->left = 0;
	bounds->right = size-1;
	bounds->finished = 0;
	
	if(!pthread_create(&main_thread,NULL,(void*)worker_thread,(void*)bounds)){//main is the root
		printf("Error creating thread!\n");
	}
	
	while (!bounds->finished) { // waiting for every thread to terminate
		err = sched_yield();
		if (err == -1) {
			printf("Error yielding CPU!\n");
		}
	}
	
	printf("Sorted array:\n");
	for (i=0; i<size; i++) {
		
		printf(" %d ", array[i]);
		
	}
	printf("\n");
	
	free(bounds);
	
	
	return 0;
}
