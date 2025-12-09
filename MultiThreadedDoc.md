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
addTaskInThreads(Threads * threads, YourProcedure,PointerToTheData);
```
