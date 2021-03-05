#include <iostream>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <queue>
#include <semaphore.h>
#include <ctime>
using namespace std;

#define NUM_THREADS 10
#define MEMORY_SIZE 150

struct node
{
	int id;
	int size;
};


queue<node> myqueue; // shared que
pthread_mutex_t sharedLock = PTHREAD_MUTEX_INITIALIZER; // mutex
pthread_t server; // server thread handle
sem_t semlist[NUM_THREADS]; // thread semaphores

int thread_message[NUM_THREADS]; // thread memory information
char  memory[MEMORY_SIZE]; // memory size

int usedMemory = 0;
int finishedThreads = 0;


//This function will add the struct to the queue
void my_malloc(int thread_id, int size)
{
	node* ptr = new node();
	ptr->id = thread_id;
	ptr->size = size;
	myqueue.push(*ptr);
}

//This function should grant or decline a thread depending on memory size.
void * server_function(void *)
{
	while(true){
		while (!myqueue.empty()) {
			node* ptr = &myqueue.front(); // ptr points to front of the queue
			myqueue.pop(); 								// so we do not lose that node even if we pop it
			if (MEMORY_SIZE - usedMemory < ptr->size) { // if poped item is bigger than available dump_memory
				thread_message[ptr->id] = -1;
			}
			else {
				thread_message[ptr->id] = usedMemory; //ptr->size;
			}
			sem_post(&semlist[ptr->id]);
		}
		if (finishedThreads == NUM_THREADS) {
			break;
		}
	}
	return 0;
}

//This function will create a random size, and call my_malloc
//Block
//Then fill the memory with id's or give an error prompt
void * thread_function(void * id)
{
	pthread_mutex_lock(&sharedLock); // lock

  int requestedSize = rand() % (MEMORY_SIZE / 6) + 1;
	int * id_ptr = (int *) id;
	my_malloc(*id_ptr, requestedSize);
	sem_wait(&semlist[*id_ptr]);

	if (thread_message[*id_ptr] == -1) {
			cout << "Thread " << *id_ptr << ": Not enough memory" <<endl;
	}
	else {
		for (int i = usedMemory; i < requestedSize + usedMemory; i++) {
			memory[i] = *id_ptr + '0';
		}
		usedMemory = usedMemory + requestedSize;
	}

	pthread_mutex_unlock(&sharedLock); // unlock
	return 0;
}

void init()
{
	pthread_mutex_lock(&sharedLock);	//lock
	for(int i = 0; i < NUM_THREADS; i++) //initialize semaphores
		{sem_init(&semlist[i],0,0);}
	for (int i = 0; i < MEMORY_SIZE; i++)	//initialize memory
  	{char zero = '0'; memory[i] = zero;}
  pthread_create(&server,NULL,server_function,NULL); //start server
	pthread_mutex_unlock(&sharedLock); //unlock
}


 // You need to print the whole memory array here.
void dump_memory()
{
	for (int i = 0; i < MEMORY_SIZE; i++) {
		cout << memory[i];
	}
}

int main (int argc, char *argv[])
 {
	srand(time(NULL)); // to be able to generate different random number

 	//You need to create a thread ID array here
	int thread_id[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		thread_id[i] = i;
	}

 	init();	// call init

 	//You need to create threads with using thread ID array, using pthread_create()
	pthread_t threads[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, thread_function, (void *) &thread_id[i]);
	}

 	//You need to join the threads
	pthread_mutex_lock(&sharedLock);
	finishedThreads++;
	pthread_mutex_unlock(&sharedLock);
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

 	dump_memory(); // this will print out the memory

 	printf("\nMemory Indexes:\n" );
 	for (int i = 0; i < NUM_THREADS; i++)
 	{
 		printf("[%d]" ,thread_message[i]); // this will print out the memory indexes
 	}
 	printf("\nTerminating...\n");
 }
