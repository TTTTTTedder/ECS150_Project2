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
      
      context switching) and linking a first thread to commence
      
      the threading library. Continuous yielding ensures that
      
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

   multiple threads.
   
   + Structure
   
      Each semaphore holds a count variable and a queue to 
      
      represent threads that are waiting on this specific

      semaphore.
      
   + ```sem_down()```
   
      Decrements the semaphore count if non-zero. If the count
      
      is zero, push the thread onto the semaphore's queue and
      
      block the thread.
      
   + ```sem_up()```
   
      Increments the semaphore count. If there is a thread
      
      waiting on the semaphore, unblock the thread to
      
      the ready queue.
      
   + ```blocked``` flag
   
      We use a ```blocked``` global variable as a flag to help 
      
      enqueue the thread either into the ready queue, or a 
      
      semaphore specific queue, by understand whether the
      
      thread to be enqueued is blocked or not.
   

* (Phase 4) **Preempt** 

   We create a signal handler that handles the ```SIGVTALRM```
   
   signal. We also create a timer alarm that signals the
   
   ```SIGVTALRM``` every 100Hz (every 0.01 seconds).
   
   + Signal Catcher
   
      The signal catcher ```sigaction``` performs on a 
      
      ```new_action``` and stores the current action in
      
      ```old_action```, which helps to restore the states
      
      in ```preempt_stop``` before the program ends.
      
   + Signal Handler
   
      The detection of a ```SIGVTALRM``` signal forces the
      
      current thread to yield.


## Test

* ```queue_tester``` 

    In the ```queue_tester``` program, we test our queue by 
    
    calling all functions implemented in ```queue.c``` file.
    
    We test the basic functions ```create()```, ```enqueue()```,
    
    and ```dequeue()``` by simply calling them during the 
    
    program. We also test the more complicated functions 
    
    ```delete()``` and ```iterate()``` to make sure it works 
    
    perfectly. By running this testing program, we make sure that 
    
    our queue structure is soundly built for further use.

* ```uthread_hello``` 

    This program simply creates and runs a thread to print

    infomation in the terminal.

    By using this testing program, we simply test if our thread 
    
    API is able to be accessed, and make sure a thread can 
    
    be created by calling the thread API functions.

* ```uthread_yield``` 

    We use this program to test the ```yield``` function in our 
    
    thread API. By calling function in this program, we test if 
    
    current running thread is able to stop and yield to the next 
    
    thread waiting in the queue. 

    As it print the information with the order we expect, the 

    ```yield()``` function is guaranteed to work well when 
    
    multi-threading is taking place.

* ```sem_simple``` 

    By using this program, we test if the semaphores we 
    
    implemented in phase 3 are able to interrupt the current 
    
    thread and release resources to next thread.

    Since it does print out information we expect, it guarantees 
    
    the ```down()``` and ```up()``` functions are able to allow a 
    
    thread to grab and release resources.

* ```sem_buffer``` 

    In this program, we test if the semaphores are able to work 
    
    in complicated case. The consumer-producer like test cases lets 
    
    us know if the semaphores are able to limit only one 
    
    thread to use the same resource at a time.

* ```sem_count``` 

    In this program, we test the synchronization of two threads 
    
    sharing two semaphores. Since it counts and prints from 0 to 19 
    
    in numeric order, our semaphores are proved to work for 
    
    synchronization.

* ```sem_prime```

    We use this program to test if our semaphores are able to work 
    
    in a sensitive case. In expectation, our semaphores should help 

    control threads during processing and print out all the 
    
    prime numbers in numeric order. However, after multiple attempts
    
    to modify our code, we are missing some prime numbers altogether.
    

## Challenges

* The first challenge we met is understanding yielding between threads,

  which we spent most of our time on.

* Using semaphores in sensitive case is the second challenge we faced.

  Since we found that we can not run the prime test perfectly at

  a very late time, determining whether to restructure our semaphore
  
  implementation is a hard task.

* Understanding how to keep track of blocked threads took some time.

  We initially thought of having two global queues for states, but
  
  having a queue linked to every semaphore made more sense to keep
  
  track of blocked threads.



## Sources

[Leetcode 707. Design Linked List](https://leetcode.com/problems/design-linked-list/) 

[System-V-Context](https://www.gnu.org/software/libc/manual/html_mono/libc.html#System-V-contexts) 

[Signal Actions](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Signal-Actions) 

[Signal Alarm](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Setting-an-Alarm) 

[Blocking Signals](https://www.gnu.org/software/libc/manual/html_mono/libc.html#Blocking-Signals)
