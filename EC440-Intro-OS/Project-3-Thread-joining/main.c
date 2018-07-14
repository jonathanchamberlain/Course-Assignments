#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

#define THREAD_CNT 3

// waste some time 
void *count(void *arg) {
	unsigned long int c = (unsigned long int)arg;
	unsigned long int i;
	for (i = 0; i < c; i++) {
		if ((i % 1000) == 0) {
			//printf("tid: 0x%x Just counted to %ld of %ld\n", \
			(unsigned int)pthread_self(), i, c);
		}
	}
    return arg;
}

int main(int argc, char **argv) {
	pthread_t threads[THREAD_CNT];
	int i;
	unsigned long int cnt = 100000000;
        unsigned long int total = 0;
       // unsigned long int result = 0;

	void * result;

    //create THERAD_CNT threads
	for(i = 0; i<THREAD_CNT; i++) {
		pthread_create(&threads[i], NULL, count, (void *)((i+1)*cnt));
	}

    //join all threads ... not important for proj2
	for(i = 0; i<THREAD_CNT; i++) {
		//pthread_join(threads[i], (void**)result);
		pthread_join(threads[i], &result);                
		total += (unsigned long int) result;
                printf("Total count so far is %lu\n",total);
	}
    return 0;
}
