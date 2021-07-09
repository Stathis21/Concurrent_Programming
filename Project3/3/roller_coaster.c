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
#include "rollercoaster_monitor.h"

#define MAX_PASSENGERS 300


int to_be_boarded,waiting,main_stopped = 0;


void* passenger_threads() {
	
	get_in_queue();

	return NULL;
	
}

void* train_thread() {

	while(1) {
		
		idle_train();
		
		if(main_stopped) {break;}
		sleep(5);
		mad_max();
	
	}
	
	return NULL;
	
}





int main(int argc,char* argv[]) {
	pthread_t t1,passenger[MAX_PASSENGERS];
	int i=0,no_passenger,sum;
	
	
	monitor_init();
	pthread_create(&t1,NULL,train_thread,NULL);

	//no_passenger = atoi(argv[1]);

	/*for (i=0; i<no_passenger; i++) {
		pthread_create(&passenger[i],NULL,passengers,NULL);

	}*/
	printf("How many passengers arrived(-1 to quit)?\n");
	while(1) {
		scanf(" %d",&no_passenger);
		if (no_passenger == -1) 
			break;
		for (i=0; i<no_passenger; i++) {
			sum++;
			pthread_create(&passenger[sum],NULL,passenger_threads,NULL);
		}
			
	}
	monitor_kill();
	main_stopped = 1;
	for (i=0; i<sum; i++) {
		pthread_join(passenger[sum],NULL);
	}
	pthread_join(t1,NULL);
	
	
	
	return 0;
}
