# MultiThreaded Documentation
multitTreaded is my simple C framework for allocating multiple threads when running a c program. It allows for faster calculation at the cost of an increased cpu load.

## What shall be kept in consideration. 

> - You should avoid allocating an amount of threads superior to the ammount of cores your cpu has. 
> - If two threads try to modify the same data at the same time the data could easily get broken or corrupted.
> - The context switching between multiple threads can in the long run slow down the speed of the process.

## Why use multiple threads.
Using multiple threads can decrease calculation time by ofloading task from the main process into different threads. This method also allows for asynchronous computing at the cost of increased cpu load.

# Usage
*note : this framework uses notions such as dereferencing and pointers to functions wich are quite simple but important to know.*

**Setting up your threads.**
```c
Threads * threads = setupThreads();
```
*notice : if you want to change the amount of threads wich our program will allocate you can modify `multiThreaded.h`*

**Adding new tasks to our threads.**
*Before this step i would recomend researching about dereferencing pointers and usage of procedures/function pointers*
```c
addTaskInThreads(threads, procedure,data);
```
this adds the task to the thread wich has the least amount of scheduled tasks.
**Handling the threads**
*quick note : I made it so that when you are handling the threads you wait until all of the threads completed their tasks.*
```c
handleThreads(threads);
```
After this all the child processes are killed so you dont need to worry about anything else.

# Simple exemple

This is a sample program wich can help you understand how to use my framework.
```c
#include "multiThreaded.c"

void printanumber(void * data)
{	
	int value = *(int*)data;
	sleep(rand()%10);
	printf("Extracted the value %d",value+);
	*(int*)data =value+1;
}

int main()
{	
	Threads * threads = setupThreads();
	int * numbers[100];
	printf("Entering distribution of tasks inside of the threads we have added a fake lag to simulate complicated calculations\n");
	for (int i=0; i<50;i++){
		numbers[i] = malloc(sizeof(int));
		*numbers[i] = i;
		 addTaskInThreads(threads,printanumber,numbers[i]);
	 }
	 printf("Executing all the tasks and waiting for all of them to be completed\n");
	 handleThreads(threads);
	 printf("all the tasks have been executed now printing all the modified values\n");
	 for(int i=0; i<50;i++){
	 	printf("the %d number is : %d \n",i,*numbers[i]);
	 }
	 return 0;
}
```
---
*if you have any question do not hesitate to ask [me](https://github.com/TogExe).*
