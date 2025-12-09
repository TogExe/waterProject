# MultiThreaded Documentation
multitTreaded is my simple C framework for allocating multiple threads when running a c program. It allows for faster calculation at the cost of an increased cpu load.

## What shall be kept in consideration. 

> - You should avoid allocating an amount of threads superior to the ammount of cores your cpu has. 
> - If two threads try to modify the same data at the same time the data could easily get broken or corrupted.
> - The context switching between multiple threads can in the long run slow down the speed of the process.

## Why use multiple threads.
