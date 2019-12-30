#include "SortedList.h"
#include <time.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>

char lockVal = '\0';
pthread_mutex_t plock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t plock2 = PTHREAD_MUTEX_INITIALIZER;
char syncLock = 0;
char syncLock2 = 0;
SortedListElement_t** takenElements = NULL;
unsigned long long numThreads = 1;

void handler(int sigNum){
	if(sigNum != SIGSEGV){
		char buf[] = "Signal caught\n";
		write(2, buf, strlen(buf));
		_exit(2);
	} else {
		char buf[] = "Segmentation fault\n";
		write(2, buf, strlen(buf));
		_exit(2);
	}
}

struct thread_input{
	SortedList_t* list;
	SortedListElement_t* elements;
	int iterations;
	int thread_num;
};

void SortedList_insert_m(SortedList_t *list, SortedListElement_t *element){
	const char* orig = element->key;
	pthread_mutex_lock(&plock);
	if(list->key == NULL){
		list = list->next;
	}
	while(list->key != NULL){
		const char* temp = list->key;
		if(strcmp(orig, temp) <= 0){
			if(opt_yield & INSERT_YIELD)
				sched_yield();
			element->next = list;
			element->prev = list->prev;
			list->prev->next = element;
			list->prev = element;
			pthread_mutex_unlock(&plock);
			return;
		}
		list = list->next;
	}
	if(opt_yield & INSERT_YIELD)
		sched_yield();
	element->next = list;
	element->prev = list->prev;
	list->prev->next = element;
	list->prev = element;
	pthread_mutex_unlock(&plock);
}

void SortedList_insert_s(SortedList_t *list, SortedListElement_t *element){
	const char* orig = element->key;
	while(__sync_lock_test_and_set(&syncLock, 1)){continue;}
	if(list->key == NULL){
		list = list->next;
	}
	while(list->key != NULL){
		const char* temp = list->key;
		if(strcmp(orig, temp) <= 0){
			if(opt_yield & INSERT_YIELD)
				sched_yield();
			element->next = list;
			element->prev = list->prev;
			list->prev->next = element;
			list->prev = element;
			__sync_lock_release(&syncLock);
			return;
		}
		list = list->next;
	}
	if(opt_yield & INSERT_YIELD)
		sched_yield();
	element->next = list;
	element->prev = list->prev;
	list->prev->next = element;
	list->prev = element;
	__sync_lock_release(&syncLock);
}

int SortedList_delete_m( SortedListElement_t *element){
	pthread_mutex_lock(&plock);
	if(element->next->prev != element || element->prev->next != element){
		pthread_mutex_unlock(&plock);
		return 1;
	}
	if(opt_yield & DELETE_YIELD)
		sched_yield();
	element->prev->next = element->next;
	element->next->prev = element->prev;
	pthread_mutex_unlock(&plock);
	return 0;
}

int SortedList_delete_s( SortedListElement_t *element){
	while(__sync_lock_test_and_set(&syncLock, 1)){continue;}
	if(element->next->prev != element || element->prev->next != element){
		__sync_lock_release(&syncLock);
		return 1;
	}
	if(opt_yield & DELETE_YIELD)
		sched_yield();
	element->prev->next = element->next;
	element->next->prev = element->prev;
	__sync_lock_release(&syncLock);
	return 0;
}

SortedListElement_t *SortedList_lookup_m(SortedList_t *list, const char *key){
	if(key == NULL){
		pthread_mutex_lock(&plock);
		while(list->key != NULL){
			if(opt_yield & LOOKUP_YIELD)
				sched_yield();
			list = list->next;
		}
		pthread_mutex_unlock(&plock);
		return list;
	}
	pthread_mutex_lock(&plock);
	if(list->key == NULL){
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		list = list->next;
	}
	while(list->key != NULL){
		const char* temp = list->key;
		if(strcmp(key, temp) == 0){
			if(opt_yield & LOOKUP_YIELD)
				sched_yield();
			pthread_mutex_unlock(&plock);
			return list;
		}
		list = list->next;
	}
	pthread_mutex_unlock(&plock);
	return NULL;
}

SortedListElement_t *SortedList_lookup_s(SortedList_t *list, const char *key){
	if(key == NULL){
		while(__sync_lock_test_and_set(&syncLock, 1)){continue;}
		while(list->key != NULL){
			if(opt_yield & LOOKUP_YIELD)
				sched_yield();
			list = list->next;
		}
		__sync_lock_release(&syncLock);
		return list;
	}
	while(__sync_lock_test_and_set(&syncLock, 1)){continue;}
	if(list->key == NULL){
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		list = list->next;
	}
	while(list->key != NULL){
		const char* temp = list->key;
		if(strcmp(key, temp) == 0){
			if(opt_yield & LOOKUP_YIELD)
				sched_yield();
			__sync_lock_release(&syncLock);
			return list;
		}
		list = list->next;
	}
	__sync_lock_release(&syncLock);
	return NULL;
}

int SortedList_length_m(SortedList_t *list){
	SortedList_t* start = list;
	pthread_mutex_lock(&plock);
	if(list->next->prev != list || list->prev->next != list){
		pthread_mutex_unlock(&plock);
		return -1;
	}
	list = list->next;
	int count = 0;
	while(list != start){
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		if(list->next->prev != list || list->prev->next != list){
			pthread_mutex_unlock(&plock);
			return -1;
		}
		list = list->next;
		count++;
	}
	pthread_mutex_unlock(&plock);
	return count;
}

int SortedList_length_s(SortedList_t *list){
	SortedList_t* start = list;
	while(__sync_lock_test_and_set(&syncLock, 1)){continue;}
	if(list->next->prev != list || list->prev->next != list){
		__sync_lock_release(&syncLock);
		return -1;
	}
	list = list->next;
	int count = 0;
	while(list != start){
		if(opt_yield & LOOKUP_YIELD)
			sched_yield();
		if(list->next->prev != list || list->prev->next != list){
			__sync_lock_release(&syncLock);
			return -1;
		}
		list = list->next;
		count++;
	}
	__sync_lock_release(&syncLock);
	return count;
}

void* thread_method(void* pointer){
	struct thread_input input = *(struct thread_input*)(pointer);
	SortedList_t* list = input.list;
	SortedListElement_t* elements = input.elements;
	int iterations = input.iterations;
	int thread_num = input.thread_num;
	int i = 0;
	for(i = 0; i < iterations; i++){
		if(lockVal == 'm'){
			SortedList_insert_m(list, &elements[i]);
		}
		else if(lockVal == 's'){
			SortedList_insert_s(list, &elements[i]);
		}
		else {
			SortedList_insert(list, &elements[i]);
		}
	}
	int length;
	if(lockVal == 'm'){
		length = SortedList_length_m(list);
	}
	else if(lockVal == 's'){
		length = SortedList_length_s(list);
	}
	else {
		length = SortedList_length(list);
	}
	if(length == -1){
		char buf[] = "Corrupted list1\n";
		write(2, buf, strlen(buf));
		_exit(2);
	}
	for(i = 0; i < iterations; i++){
		SortedListElement_t* lookup;
		while(i != -1){
			if(lockVal == 'm'){
				pthread_mutex_lock(&plock2);
			}
			if(lockVal == 's'){
				while(__sync_lock_test_and_set(&syncLock2, 1)){continue;}
			}
			if(lockVal == 'm'){
				lookup = SortedList_lookup(list, elements[i].key);
			}
			else if(lockVal == 's'){
				lookup = SortedList_lookup(list, elements[i].key);
			}
			else {
				lookup = SortedList_lookup(list, elements[i].key);
			}
			if(lookup == NULL){
				char buf[] = "Corrupted list3\n";
				write(2, buf, strlen(buf));
				_exit(2);
			}
			unsigned long long j = 0;
			for(j = 0; j < numThreads; j++){
				if(takenElements[j] == lookup){
					break;
				}
			}
			if(j == numThreads){
				takenElements[thread_num] = lookup;
				if(lockVal == 'm'){
					pthread_mutex_unlock(&plock2);
				}
				if(lockVal == 's'){
					__sync_lock_release(&syncLock2);
				}
				break;
			}
			if(lockVal == 'm'){
				pthread_mutex_unlock(&plock2);
			}
			if(lockVal == 's'){
				__sync_lock_release(&syncLock2);
			}
		}
		int status;
		if(lockVal == 'm'){
			status = SortedList_delete_m(lookup);
		}
		else if(lockVal == 's'){			
			status = SortedList_delete_s(lookup);
		}
		else {
			status = SortedList_delete(lookup);
		}
		if(status == 1){
			char buf[] = "Corrupted list2\n";
			write(2, buf, strlen(buf));
			_exit(2);
		}
	}
	return NULL;
}

int main(int argc, char* const argv[]){
	opterr = 0;
	struct option options[5];
	options[0].name = "threads";
	options[0].has_arg = 1;
	int flag0[1] = {0};
	options[0].flag = flag0;
	options[0].val = 1;

	options[1].name = "iterations";
	options[1].has_arg = 1;
	int flag1[1] = {0};
	options[1].flag = flag1;
	options[1].val = 1;

	options[2].name = "yield";
	options[2].has_arg = 1;
	int flag2[1] = {0};
	options[2].flag = flag2;
	options[2].val = 1;

	options[3].name = "sync";
	options[3].has_arg = 1;
	int flag3[1] = {0};
	options[3].flag = flag3;
	options[3].val = 1;

	options[4].name = 0;
	options[4].has_arg = 0;
	options[4].flag = 0;
	options[4].val = 0;

	int longInd = 0;
	unsigned long long iterations = 1;
	char* yieldopts = NULL;
	char* syncopts = NULL;
	int output = getopt_long(argc, argv, "-", options, &longInd);
	while(output != -1){
		if(output == '?'){
			fprintf(stderr,	"Invalid Usage\n");
			return 1;
		}
		if(longInd == 0){
			int len = strlen(optarg);
                        int i = 0;
                        for(i = 0; i < len; i++){
                                if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
                                        return 1;
                                }
                        }
                        numThreads = atoi(optarg);
		} else if(longInd == 1){
			int len = strlen(optarg);
                        int i = 0;
                        for(i = 0; i < len; i++){
                                if(optarg[i] < '0' || optarg[i] > '9'){
					fprintf(stderr,	"Invalid Usage\n");
                                        return 1;
                                }
                        }
                        iterations = atoi(optarg);
		} else if(longInd == 2){
			int len = strlen(optarg);
			int i = 0;
			for(i = 0; i < len; i++){
				switch(optarg[i]){
					case 'i':
						opt_yield = opt_yield | 1;
						break;
					case 'd':
						opt_yield = opt_yield | 2;
						break;
					case 'l':
						opt_yield = opt_yield | 4;
						break;
					default:
						fprintf(stderr,	"Invalid Usage\n");
						return 1;
				}
			}
			yieldopts = optarg;
		} else if(longInd == 3){
			int len = strlen(optarg);
			if(len != 1){
				fprintf(stderr,	"Invalid Usage\n");
				return 1;
			}
			switch(optarg[0]){
				case 'm':
					lockVal = 'm';
					break;
				case 's':
					lockVal = 's';
					break;
				default:
					fprintf(stderr,	"Invalid Usage\n");
					return 1;
			}
			syncopts = optarg;
		}
		output = getopt_long(argc, argv, "-", options, &longInd);
	}
	unsigned long long elementNum = numThreads*iterations;
	SortedListElement_t* elements = malloc(elementNum*sizeof(SortedListElement_t));
	if(elements == NULL){
		fprintf(stderr, "Malloc error\n");
		return 1;
	}
	srand(time(0));
	unsigned long long i = 0;
	for(i = 0; i < elementNum; i++){
		char temp[21];
		temp[0] = rand()%128;
		temp[1] = rand()%128;
		temp[2] = rand()%128;
		temp[3] = rand()%128;
		temp[4] = rand()%128;
		temp[5] = rand()%128;
		temp[6] = rand()%128;
		temp[7] = rand()%128;
		temp[8] = rand()%128;
		temp[9] = rand()%128;
		temp[10] = rand()%128;
		temp[11] = rand()%128;
		temp[12] = rand()%128;
		temp[13] = rand()%128;
		temp[14] = rand()%128;
		temp[15] = rand()%128;
		temp[16] = rand()%128;
		temp[17] = rand()%128;
		temp[18] = rand()%128;
		temp[19] = rand()%128;
		temp[20] = '\0';
		elements[i].key = temp;
	}
	SortedList_t list[1];
	list->prev = list;
	list->next = list;
	list->key = NULL;
	__sighandler_t sigoutput = signal(SIGSEGV, &handler);
	if(sigoutput == SIG_ERR){
		fprintf(stderr, "Signal error\n");
		free(elements);
		return 1;
	}
	pthread_t* pthreads = malloc(numThreads*sizeof(pthread_t));
	if(pthreads == NULL){
		fprintf(stderr, "Malloc error\n");
		free(elements);
		return 1;
	}
	struct thread_input* input = malloc(numThreads*sizeof(struct thread_input));
	if(input == NULL){
		fprintf(stderr, "Malloc error\n");
		free(elements);
		free(pthreads);
		return 1;
	}
	int offset = 0;
	for(i = 0; i < numThreads; i++){
		input[i].list = list;
		input[i].elements = elements+offset;
		input[i].iterations = iterations;
		input[i].thread_num = i;
		offset += iterations;
	}
	takenElements = malloc(numThreads*sizeof(SortedListElement_t*));
	if(takenElements == NULL){
		free(input);
		free(pthreads);
		free(elements);
		return 1;
	}
	for(i = 0; i < numThreads; i++){
		takenElements[i] = NULL;
	}
	struct timespec init_time;
        output = clock_gettime(CLOCK_MONOTONIC, &init_time);
        if(output == -1){
		fprintf(stderr, "Clock error\n");
		free(takenElements);
		free(input);
                free(pthreads);
		free(elements);
                return 1;
        }
	for(i = 0; i < numThreads; i++){
		int output = pthread_create(&pthreads[i], NULL, &thread_method, &input[i]);
		if(output != 0){
			fprintf(stderr, "Pthread error\n");
			free(takenElements);
			free(input);
        	        free(pthreads);
			free(elements);
			return 1;
		}
	}
	for(i = 0; i < numThreads; i++){
		int output = pthread_join(pthreads[i], NULL);
		if(output != 0){
			fprintf(stderr, "Pthread error\n");
			free(takenElements);
			free(input);
        	        free(pthreads);
			free(elements);
			return 1;
		}
	}
	struct timespec fin_time;
	output = clock_gettime(CLOCK_MONOTONIC, &fin_time);
	if(output == -1){
		fprintf(stderr, "Clock error\n");
		free(takenElements);
		free(input);
		free(pthreads);
		free(elements);
		return 1;
	}
	unsigned long long nanoseconds = fin_time.tv_nsec + fin_time.tv_sec*1000000000 - init_time.tv_nsec - init_time.tv_sec*1000000000;
	if(SortedList_length(list) != 0){
		fprintf(stderr, "Corrupted list4\n");
		return 2;
	}
	fprintf(stdout, "list");
	if(yieldopts != NULL){
		fprintf(stdout, "-%s", yieldopts);
	} else {
		fprintf(stdout, "-none");
	}
	if(syncopts != NULL){
		fprintf(stdout, "-%s", syncopts);
	} else {
		fprintf(stdout, "-none");
	}
	fprintf(stdout, ",%llu", numThreads);
	fprintf(stdout, ",%llu", iterations);
	fprintf(stdout, ",1");
	fprintf(stdout, ",%llu", numThreads*iterations*3);
	fprintf(stdout, ",%llu", nanoseconds);
	fprintf(stdout, ",%llu", nanoseconds/numThreads/iterations/3);
	fprintf(stdout, "\n");
	free(takenElements);
	free(input);
	free(pthreads);
	free(elements);
	return 0;
}
