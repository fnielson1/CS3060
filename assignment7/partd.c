/*
* Authors:
* Frank Nielson
* Caleb Kruger

* Class: CS 3060
* Project: Assignment 7

* This code has be written by the authors
* listed above.
*/

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>


void* IncrementCounter(void*);


static int _count; // Global Variable
static pthread_mutex_t _incrementLock = PTHREAD_MUTEX_INITIALIZER;


int main()
{
	pthread_t thr1, thr2, thr3, thr4;

	_count = 0;

	// Create the threads
	pthread_create(&thr1, NULL, IncrementCounter, NULL);
	pthread_create(&thr2, NULL, IncrementCounter, NULL);
	pthread_create(&thr3, NULL, IncrementCounter, NULL);
	pthread_create(&thr4, NULL, IncrementCounter, NULL);
	
	// Wait for each thread
	pthread_join(thr1, NULL);
	pthread_join(thr2, NULL);
	pthread_join(thr3, NULL);
	pthread_join(thr4, NULL);

	// Print the value of _count
	printf("%s%d\n", "COUNT: ",  _count);

	return 0;
}


/*
** IncrementCounter

* Increments the global variable count.

* void* m: Not used.
* returns: NULL
*/
void* IncrementCounter( void* m )
{
    int i;
    for (i = 0; i < 10; ++i)
    {
		pthread_mutex_lock(&_incrementLock);
        int tempValue = _count;
        sleep(1);
        tempValue = tempValue + 1;
        _count = tempValue;
		pthread_mutex_unlock(&_incrementLock);
    }
 
	return NULL;
}
