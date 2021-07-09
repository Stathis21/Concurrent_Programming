#define CCR_DECLARE(label) \
	pthread_mutex_t label ## _mtx;\
	pthread_cond_t label ## _queue1;\
	int label ## _n;\
	int label ## _to_be_accounted;\
	int label ## _from_outside;

#define CCR_INIT(label) \
	pthread_mutex_init(&label ## _mtx, NULL);\
	pthread_cond_init(&label ## _queue1, NULL);\
	label ## _n = 0;\
	label ## _to_be_accounted = 0;

#define CCR_EXEC(label, cond, body) \
	pthread_mutex_lock(&label ## _mtx);\
	label ## _n++;\
	label ## _from_outside = 1;\
	while(!(cond)) {\
		if (!label ## _from_outside)\
			label ## _to_be_accounted++;\
		label ## _from_outside = 0;\
		if (!(cond))\
			pthread_cond_wait(&label ## _queue1, &label ## _mtx);\
	}\
	{body}\
	label ## _n += label ## _to_be_accounted;\
	label ## _to_be_accounted = 0; \
	//pthread_cond_signal(&label ## _queue1);
	while(label ## _n>0) {\
		label ## _n--;\
		pthread_cond_signal(&label ## _queue1);\
	}\
	pthread_mutex_unlock(&label ## _mtx);
