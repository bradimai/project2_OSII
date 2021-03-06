/******************************************************************************
* Concurrency Assignment #2
* CS444
* Creators: Bradley Imai & Daniel Ross
* 5/5/2017
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <limits.h>
#include <time.h>
#include "mt19937ar.h"

pthread_mutex_t forks[5];

unsigned int randomval(){
        unsigned int value;
        unsigned int eax;
        unsigned int ecx;
        unsigned int edx;
        unsigned int ebx;
        char vendor[13];

        eax = 0x01;

        __asm__ __volatile__(
                "cpuid;"
                : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                : "a"(eax)
        );

        if(ecx & 0x40000000){
                __asm__("rdrand  %[value]"
                        : [value] "=r" (value)
                        :
                        : "cc"
                );
        }else{
//use mt19937
                unsigned long seed = time(NULL) + pthread_self();
                init_genrand(seed);
                unsigned long rand = genrand_int32();
                value = rand & UINT_MAX;
        }
        return value;
}

unsigned int randomminmax(unsigned int min, unsigned int max){
        unsigned int value;
        if(min > max){
                printf("IMPROPER RAND ARGS \n");
                value = 0;
                return value;
        }
        unsigned int range = (max - min)+1;
        value = randomval();
        value = value%range;
        value = value+min;
        return value;
}
int identify(char* name){
	int id = 0;
	if (!(strcmp(name,"Aristotle"))){
		return id;
	}else if(!(strcmp(name,"Socrates"))){
		id = 1;
		return id;
	}else if(!(strcmp(name,"Plato"))){
		id = 2;
		return id;
	}else if(!(strcmp(name,"Decartes"))){
		id = 3;
		return id;
	}else if(!(strcmp(name,"Confucius"))){
		id = 4;
		return id;
	}else{
		printf("IMPROPER THREAD ID \n");
		return id;
	}
}
void think(char* IDvar){
	unsigned int think_time = 0;
	int id = identify(IDvar);
	think_time = randomminmax(1,20);
	printf("%s thinks for %u \n",IDvar,think_time);
	sleep(think_time);

}
void eat(char* IDvar){
	unsigned int eat_time = 0;
	int id = identify(IDvar);
	eat_time = randomminmax(2,9);
	printf("%s eats for %u \n",IDvar,eat_time);
	sleep(eat_time);

}
void getforks(char* IDvar){
	int id = identify(IDvar);
	int secondfork = ((id+1)%5);
	pthread_mutex_trylock(&forks[secondfork]);
	pthread_mutex_lock(&forks[id]);
	pthread_mutex_unlock(&forks[secondfork]);
	printf("%s picks up fork %d \n",IDvar,id);
	pthread_mutex_lock(&forks[secondfork]);
	printf("%s picks up fork %d \n",IDvar,secondfork);
}
void putforks(char* IDvar){
	int id = identify(IDvar);
	pthread_mutex_unlock(&forks[id]);
	printf("%s puts down fork %d \n",IDvar,id);
	int secondfork = ((id+1)%5);
	pthread_mutex_unlock(&forks[secondfork]);
	printf("%s puts down fork %d \n",IDvar,secondfork);
}

void *philosphize(void *IDvar){
	int alive = 1;
	int count = 0;
	printf("Hello my name is %s \n",IDvar);
	while(alive == 1){
		think(IDvar);
		getforks(IDvar);
		eat(IDvar);
		putforks(IDvar);
		if(count > 100){
				alive = 0;
		}
		count++;
	}
}

int main(int argc, char **argv){
		//printf("\n  check actual 0");
        if(argc > 1){
                printf("Incorrect number of arguments");
                return 1;
        }
        int i,j;
        time_t t=time(0);
		//printf("check 0");
        for(i=0; i<5; i++){
			pthread_mutex_init(&(forks[i]), NULL);
		}
        //printf("check 1 \n");
		pthread_t philosophers[5];
		char* name1 = "Aristotle";
		char* name2 = "Plato";
		char* name3 = "Socrates";
		char* name4 = "Decartes";
		char* name5 = "Confucius";
		//printf("check 2 \n");
        // Create five threads
        pthread_create(&(philosophers[0]), NULL, philosphize, (void *) name1);
		//printf("check 3 \n");
		pthread_create(&(philosophers[2]), NULL, philosphize, (void*) name2);
		//printf("check 4 \n");
        pthread_create(&(philosophers[1]), NULL, philosphize, (void*) name3);
		//printf("check 5 \n");
	    pthread_create(&(philosophers[3]), NULL, philosphize, (void*) name4);
		//printf("check 6 \n");
	    pthread_create(&(philosophers[4]), NULL, philosphize, (void*) name5);
		//printf("check 7 \n");
		for(j=0; j<5; j++)
			pthread_join(philosophers[j],NULL);
		//printf("check 8 \n");
		int million = 1;
		int	count = 0;
        while(million){
			
			count++;
			if(count > 1000000000000000000){
				million =0;
			}
		}
        return 0;
}

