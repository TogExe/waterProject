#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
// Defining essential elements

#define maxthreads 18

clock_t start, stop;

// predefining the structures ig
typedef struct node node;

// DEFINING IMPORTANT SHI

// basic and common types

typedef struct node{
	void * content;
	node * next;
}Node;

typedef struct{
	Node * head;
}NodeGroup;

// types used for scheduling tasks in threads;

typedef struct {
	void * data;
	void (*task)(void *param);
}Task;

typedef struct {
	int occupency[maxthreads];
	NodeGroup scheduledTasks[maxthreads]; //a NodeGroup
	pthread_t threads[maxthreads];
	void *(*doall)(void * self);
}Threads;

void initNodeGroup(NodeGroup *ng);

void * doallTasks(void*arg);
void addTaskToGroup(NodeGroup g, Task * task);
void addTaskInThreads(Threads * t, void (*task)(void*param),void*data);
void handleThreads(Threads* t);

Threads * setupThreads();
