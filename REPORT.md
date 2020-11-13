# ECS150 Project 2 

Akash Arun Malode, 914706364 

Zhengyu Wu, 916951023



## Purpose

   The goal of this project is to implement a basic user-level thread

   library for Linux, which provides a complete interface for applications

   to create and run independent threads concurrently.

## Design Choices

* (Phase 1) **Queue** 

   Queue is the very basic and most important data structure in this

   project. We chose to implement the queue by designing a 

   linked-list with the FIFO (First In First Out) rule. 

   Thus, every new node we enqueue will be linked to the tail of 

   the current queue, and we can only dequeue the node from the head

   of this queue.

* (Phase 2) **Uthread** 

   Thread management functions are implemented as per the interface
   
   provided.
   
   + ```idle``` thread
   
      Acts as a starting point for the multithreading process, by
      
      initializing an ```idle``` thread (to provide a basis for 
      
      context switching)and linking a first thread to commence
      
      the threading library. Later continuous yielding ensures that
      
      threads can be created/destroyed/run.
      
   +  TCB
   
      Every thread is initialized with the following blocks of
      
      information:
      
         + Context
         
         + Stack (independent for each thread)
    
   + Current Thread
   
      Holds the currently running thread in the program. Stored 
      
      as a global variable, which is accessible by other ```c```
      
      files with the use of ```uthread_current``` function.
      
   + Ready Threads (queue)
   
      Holds the TCBs of all threads ready to run. Threads stored
      
      in this queue are not blocked by semaphores. This is also a
      
      global variable to ensure all ```uthread``` functions have
      
      access to it.
      
   + Yielding
      
      Context switching occurs between the current running thread,
      
      and the first thread in the ready queue.

* (Phase 3) **Semaphores** 

   Semaphores are used to control access to common resources by

   multiple threads. The semaphores will be controlled

   by ```down``` and ```up``` functions. We set a queue inside the 

   semaphore structure to store the order of the threads that are 

   waiting the resource, in first come first serve order.



* (Phase 4) **Preempt** 

 ***EDIT HERE***


## Execution Procedure 

+ Create Threads Queue

+ Run Single Thread

+ Run Multiple Thread 
  + Yielding Between Threads 

  - Threads Control 
  - Grab Resource Semaphore
  - Release Resource Semaphore
  - Preemption 



## Test

* ```queue_tester``` 

 We use the ```queue_tester``` program, we test our queue by call

 all the functions implemented in ```queue.c``` file. We test

 the basic function ```create()```, ```enqueue()```, and ```dequeue()```

 by simply calling them during the program. We also test the 

 advance functions ```delete()``` and ```iterate()``` to make sure

 it works perfectly. 

 By running this testing program, we make sure that our queue are 

 quality enough for further use.

* ```uthread_hello``` 

 This program simply creating and running a thread to print

 infomation in the terminal.

 By using this testing program, we simply test if our thread APIs

 are able to be accessed, and make sure a thread can be created

 by calling the thread APIs.

* ```uthread_yield``` 

 We use this program to test the ```yield``` function in our thread

 API. By calling function in this program, we test if current 

 running thread is able to stop and yield to next thread waitting 

 in the queue. 

 As it print the information with the order we expect, the 

 ```yield()``` function is guaranteed to work well when multi-threads

 are running.

* ```sem_simple``` 

 By using this program, we test if the semaphores we implemented in

 phrase 3 are able to interrupt current thread and release resources

 to next thread.

 Since it does print out information we expect, it guarantees the

 ```down()``` and ```up()``` functions are able to allow a thread

 grabbing and release resources. 

* ```sem_buffer``` 

 In this program, we test if the semaphores are able to work in

 complicate case. The consumer-producer like test cases let us able 

 to know if the semaphores are able to limit only one thread uses

 the same resources at a time.

* ```sem_count``` 

 In this program, we test the synchronization of two threads sharing

 two semaphores. Since it counts and prints from 0 to 19 in 

 numeric order, our semaphores are proved to work for synchronization.

* ```sem_prime```

 We use this program to test if our semaphores are able to work in

 a sensitive case. In expectation, our semaphores should help 

 controlling threads during processing and print out all the prime 

 numbers in numeric order. However, even we have tried our best

 to modificate our code, it still has some error when running this

 test. 

## Challenges

* The first challenge we met is the yielding between threads. That's

 the part we spent most of our time to understand the requirements.

* Using semaphores in sensitive case is the second challenge we have.

 Since we found that we can not run the prime test perfectly at

 a very late time, it becomes really hard to determine if we should

 implement our semaphores again, in anotehr way.

* 3 ***\*EDIT HERE IF ANY\****



## Source 



[Leetcode 707. Design Linked List](https://leetcode.com/problems/design-linked-list/) 

[System-V-Context](https://www.gnu.org/software/libc/manual/html_mono/libc.html#System-V-contexts) 

[Signal Actions](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Signal-Actions) 

[Signal Alarm](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm) 

[Blocking Signals](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Blocking-Signals)
