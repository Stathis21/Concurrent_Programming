
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
#define N 5


int cars;
pthread_mutex_t mutex;
int current_dir,passed;
int waiting_cars[2];
pthread_cond_t waitq[2];


void monitor_init(){
	
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&waitq[0],NULL);
	pthread_cond_init(&waitq[1],NULL);
	
	
	
	waiting_cars[0] = 0;
	waiting_cars[1] = 0;
	
	passed = 0;
	current_dir = -1;		// whoever goes first
}


void enter_bridge(int dir){
	pthread_mutex_lock(&mutex);
	
	while((dir!=current_dir || cars>=N) && current_dir != -1 ){
		waiting_cars[dir]++;
		pthread_cond_wait(&waitq[dir],&mutex);
	}
	
	current_dir = dir;
	cars++;			
	

	printf("Car of type:%d passing through!%d in bridge .\n", dir, cars);
	pthread_mutex_unlock(&mutex);

	
}


void exit_bridge(int dir){
	pthread_mutex_lock(&mutex);
	passed++;
	cars--;
	printf("Car of type:%d got  through!%d in bridge\n", dir , cars);
	
	if(waiting_cars[!dir] != 0){
 		if(passed >= N) 
 			current_dir = !dir;
		
		if(cars == 0){
			printf("Waiting cars on the opposite:%d\n",waiting_cars[!dir]);
			passed = 0;
		 
			if (current_dir == dir)
				current_dir = !dir;
			while (waiting_cars[!dir]) {
				waiting_cars[!dir]--;
				pthread_cond_signal(&waitq[!dir]);
			}
		}
	}
	else if (waiting_cars[dir]!=0) {
		while (waiting_cars[dir]) {
				waiting_cars[dir]--;
				pthread_cond_signal(&waitq[dir]);
		}
	}
	else {
		current_dir = -1;
		passed = 0;
	}
		
	
	pthread_mutex_unlock(&mutex);

	
}


