#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sched.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include "prime_calc_monitor.h"

int available_threads, no_of_threads,main_done;
pthread_cond_t zero_available_threads,waiting_on_threads;
pthread_cond_t* waiting_for_job;
pthread_mutex_t mutex;



void monitor_init(int number_of_threads){
	int i;
	available_threads = 0;
	
	pthread_mutex_init(&mutex,NULL);
    pthread_cond_init(&zero_available_threads,NULL);
	pthread_cond_init(&waiting_on_threads,NULL);

	main_done = 0;
	waiting_for_job = (pthread_cond_t*)calloc(number_of_threads, sizeof(pthread_cond_t));
	has_job = (int*)calloc(number_of_threads, sizeof(int));
	
	no_of_threads = number_of_threads;
	
	for (i=0; i<number_of_threads; i++) {	
		pthread_cond_init(&waiting_for_job[i],NULL);
	}
}


void distribute_jobs(thread_info* thread_array, job* job_array,int jobs){										//H main perimenei gia available thread kai moirazei tis douleies
	int i;
	pthread_mutex_lock(&mutex);
	while(jobs>0){
		while(available_threads==0)
			pthread_cond_wait(&zero_available_threads,&mutex);
		
		for (i=0; i<no_of_threads; i++) {
			if(thread_array[i].available)				//Search for (specific) available thread
				break;
		}
		available_threads--;
		thread_array[i].available = 0;
		thread_array[i].thread_job = &job_array[jobs-1];
		jobs--;
		pthread_cond_signal(&waiting_for_job[i]);
	}
	
	pthread_mutex_unlock(&mutex);
	
	//ready to assign job(where?)
}



void get_job(int i,thread_info* thread_array,int* terminate){							//Eimai ready kai perimenw douleia
	
	pthread_mutex_lock(&mutex);
	
	available_threads++;
	//thread_array[i].available = 1;
	if(available_threads==1)
		pthread_cond_signal(&zero_available_threads);
	
	if(*terminate && thread_array[i].thread_job->is_prime!=-1  ){
		pthread_mutex_unlock(&mutex);
		return;
	}
	
	if(available_threads== no_of_threads && main_done)
		pthread_cond_signal(&waiting_on_threads);
	
	while(thread_array[i].available)
		pthread_cond_wait(&waiting_for_job[i],&mutex);
	
	if(!(*terminate))
		printf("I am thread %d and my job is %d\n", i , thread_array[i].thread_job->number); 
		
	
	
	pthread_mutex_unlock(&mutex);
	
}

void job_finished(int i,thread_info* thread_array) {
	pthread_mutex_lock(&mutex);
	thread_array[i].available = 1;			//Job done,waiting for new
	printf("Worker no: %d finished his job.\n", i);
	pthread_mutex_unlock(&mutex);
	
	
}

void wait_on_threads(thread_info* thread_array) {
	pthread_mutex_lock(&mutex);
	
	main_done = 1;
	
	while(available_threads != no_of_threads){
		pthread_cond_wait(&waiting_on_threads,&mutex);
	}

	pthread_mutex_unlock(&mutex);


}

void broadcast_workers(thread_info* thread_array){
	int i;
	pthread_mutex_lock(&mutex);
	for (i=0; i<no_of_threads; i++) {	
		thread_array[i].available = 0;		//break the workers' waiting condition
		pthread_cond_signal(&waiting_for_job[i]);
	}
	pthread_mutex_unlock(&mutex);
}


















