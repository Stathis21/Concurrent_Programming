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
#include "CCR.h"

#define N 5
#define S 10
#define MAX_THREADS 300

CCR_DECLARE(bridge);

int cars;
pthread_mutex_t mutex;
int current_dir,passed,change_direction;
int waiting_cars[2];
pthread_cond_t waitq[2];



int n,main_finished=0;

char* itoa[2];

void* car_thread(int  direction) {
	//Enter Bridge
	CCR_EXEC(bridge,1,waiting_cars[direction]++;);
	
	CCR_EXEC(bridge,!((direction!=current_dir || cars>=N) && current_dir != -1) && change_direction==0,
	waiting_cars[direction]--;
	cars++;
	current_dir = direction;
	printf("Car of type:%d passing through!%d in bridge .\n", direction, cars);
	);
	
	
	//Doing trip
	sleep(S);
	
	//Exit Bridge
	
	CCR_EXEC(bridge,1,
	passed++;
	cars--;
	printf("Car of type:%d got  through!%d in bridge\n", direction , cars);
	
	if(waiting_cars[!direction] != 0){
 		if(passed == N ) 
 			change_direction = 1;
		if(cars == 0){
			printf("Waiting cars on the opposite:%d\n",waiting_cars[!direction]);
			passed = 0;
			if(change_direction)
				current_dir = !direction;
				
			change_direction = 0;
			if (current_dir == direction)
				current_dir = !direction;
		}
 	}
	);

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
	
	CCR_INIT(bridge);
	
	CCR_EXEC(bridge,1,			//ARXIKOPOISI
	
	waiting_cars[0] = 0;
	waiting_cars[1] = 0;
	change_direction = 0;
	passed = 0;
	current_dir = -1;);											//TELOS ARXIKOPOISIS
	
	
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
