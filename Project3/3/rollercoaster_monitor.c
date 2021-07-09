
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
#define N 5


int inqueue,terminate;
pthread_mutex_t mutex;

pthread_cond_t queue;
pthread_cond_t ridin;
pthread_cond_t train;


void monitor_init(){
	
	pthread_mutex_init(&mutex,NULL);
	pthread_cond_init(&queue,NULL);
	pthread_cond_init(&train,NULL);
	pthread_cond_init(&ridin,NULL);
	
	terminate = 0;
	
	inqueue = 0;
}


void get_in_queue(){
	pthread_mutex_lock(&mutex);
	
	
	
	printf("Passenger getting in line. %d in line already.\n",inqueue);
	inqueue++;
	if (inqueue == N){
		pthread_cond_signal(&train);
		
	}
	 
	pthread_cond_wait(&queue,&mutex);		//Oura anamonis
	
	if(terminate){
		pthread_mutex_unlock(&mutex);
		return;}
	
	pthread_cond_wait(&ridin,&mutex);
	
	
	pthread_mutex_unlock(&mutex);

	
}


void idle_train(){
	pthread_mutex_lock(&mutex);
	int i;
	while(inqueue<N){
		if (terminate){break;}
		pthread_cond_wait(&train,&mutex);
	}
	
	if(terminate) {pthread_mutex_unlock(&mutex);return;}
	for (i=0; i<N; i++) {
		inqueue--;
		pthread_cond_signal(&queue);
	}
	printf("Ride is starting! %d in line.\n",inqueue);
	pthread_mutex_unlock(&mutex);
}

void mad_max() {
	int i;
	pthread_mutex_lock(&mutex);
	
	for (i=0; i<N; i++) 
		pthread_cond_signal(&ridin);
		
	printf("Ride is over. %d in line.\n",inqueue);
	sleep(1);
	pthread_mutex_unlock(&mutex);
	
}


void monitor_kill(){
	int i;
	pthread_mutex_lock(&mutex);

	terminate = 1;
	for (i=0; i<inqueue; i++) {
		pthread_cond_signal(&queue);
	}
// 	inqueue = N+1;
	
	pthread_cond_signal(&train);
	pthread_mutex_unlock(&mutex);

}

