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
#include "car_monitor.h"
#define S 10
#define MAX_THREADS 300

int n,main_finished=0;

int active[2];
int waiting[2];

char* itoa[2];

void* car_thread(int  direction) {
	
	enter_bridge(direction);
	sleep(S);
	exit_bridge(direction);
 	
	sleep(1);
	return 0;
}


int main(int argc,char* argv[]) {
	//int right_cars,left_cars;
	int i=0,j;
	int type,delay;
	pthread_t* array;
	
	itoa[0] = (char*)malloc(sizeof(char)*6);
	itoa[1] = (char*)malloc(sizeof(char)*6);
	strcpy(itoa[0],"Left");
	strcpy(itoa[1],"Right");
	

	n = -1;
	waiting[0] =waiting[1]= 0;
	active[0] =active[1]= 0;

	
  	printf("Input: x y , where x is type of thread(1 left 2 right) and y is creation delay. -1,-1 to quit\n");
 	array = (pthread_t*)malloc(MAX_THREADS*sizeof(pthread_t));
 	scanf("%d%d", &type, &delay);
 	while(type!=-1 && delay!=-1){
 		sleep(delay);
 		if( type == 1){
 			pthread_create(array+i*sizeof(pthread_t),NULL,(void*)car_thread,(void*)0);
 		}
 		else if(type == 2){
 			pthread_create(array+i*sizeof(pthread_t),NULL,(void*)car_thread,(void*)1);
 		}
 		else
 			printf("Wrong input.\n");
 		i++;
 		scanf("%d%d", &type, &delay);
 	}
 	
 	/*
 	left_cars = atoi(argv[1]);
 	right_cars = atoi(argv[2]);
 	
 	array = (pthread_t*)calloc(left_cars + right_cars,sizeof(pthread_t));
 	
 	for (i = 0; i<left_cars; i++) 
 		pthread_create(&array[i],NULL,(void*)car_thread,(void*)0);
 	
 	
 	for (i = 0; i<right_cars; i++) 
 		pthread_create(&array[i],NULL,(void*)car_thread,(void*)1);
 	
 	*/
 	
 	
	main_finished = 1;
	
	for (j=0; j<i; j++) 
		pthread_join(array[j*sizeof(pthread_t)],NULL);
		
	

	
	free(itoa[0]);
	
	free(itoa[1]);
	free(array);
	

	return 0;
}
