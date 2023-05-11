# Libuthread: User-Level Thread Library

## Summary:

`Libuthread ` is a basic user-level thread library for linux, and provides a 
complete interface for applications to create and run independent thread 
concurrently.

## Our Implementation:

To build this user-level thread library, we followed a specific order to 
achieve our goal. Our phases are as followed:

1. Implement a FCFC queue with all its basic function using a linked list.
2. Implementing the User-Thread API
3. Implementing the Semaphore API
4. Implementing Preemption API
5. Building test cases and Error/Memory management


## Phase 1: Implementing a FCFS queue 

Queues are a data structure in which the oldest element would be considered the
first in line. As more elements are added to the queue, the queue becomes a 
sort of waiting line. We implemented a queue with a linked list because a 
linked list allows for dynamic sizing at runtime. This means that during 
runtime, threads can be created and space can be allocated for their Thread 
Control Block when needed. This means we can use memory efficiently in our 
program and only allocate space when needed.

The basic queue operations that we made have a runtime of O(1) besides for 
queue_delete and `queue_iterate` which are O(n), because we need to traverse the queue in both of these operations. The O(1) runtime for most of the queue 
operations is possible because of the way we implemented our queue. We have 
both a head and a tail pointer which helps us get easy access to the front and
end of the list. Whenever we need to enqueue some more data, we can use the 
tail pointer to get to the end, and vice verse with dequeuing data at the 
front of the queue. Our queue struct was holding it's own queue length so the `queue_length` function is O(1).

When we were testing queue_iterate, we ran into a segmentation fault. We 
figured out that the function being passed into queue_iterate is calling our
`queue_delete` function which deletes one of the elements in the queue and is 
therefore freeing that element. Since we have freed that element which we will
call currentNode, we can not access currentNode->next since this is an 
invalid memory address. To solve this problem, we created an extra variable 
to temporarily hold the currentNode->next and then pass it on to currentNode
without getting a segmentation fault.


## Phase 2: Implementation of Uthread API

This API allows the creation and management of threads. It gives applications 
the ability to use multithreading where they can create and start new threads,
and terminate or manipulate them.

The design of the API relies on a private data structure that stores important
information about a single thread: Thread control block (TCB). A struct is 
used to represent the TCB as it allows for efficient storage and access of the
thread's information. The API defines states such as IDLE, READY, RUNNING, 
BLOCKED, and EXITED (representing the Zombie state of a process) which help 
track the execution status of every thread. This helps manage the scheduling 
behaviour of threads, for instance, a thread with a BLOCKED state would not 
be scheduled for execution until it's unblocked.

Two queues `ready_queue` and `exit_queue` are used to store all the ready and 
exited threads (threads that have completed their task), respectively. These 
two queues allow efficient thread management.  

#### Important functions implemented in the API:

`uthread_run()`: The process to begin the execution of the user threads occurs here. It initialisesthe threads and queues and starts the scheduling loop by calling uthread_yield() until all threads have exited. Also, memory deallocation of the queues and TCB occur after the loop to prevent accidental deallocation during the execution of any thread.

`uthread_yield()`: This yields the current thread to the next ready thread. The current thread, fetched from `uthread_current()` is added to the ready or exit queue based on its state. The context switching is performed using `uthread_ctx_switch()` after dequeuing the next thread from the ready queue.

`uthread_exit()`: Updates the current thread's state to denote it has completed running, then calls`uthread_yield()` to switch to the next thread.


## Phase 3: Implementing the Semaphore API

To begin our implementation, we needed to figure out what data the semaphore 
needed to store. We realized that we needed a variable to hold the amount of 
resources that the sempahore has and a queue to hold the blocked threads that 
wanted to access that semaphore. To implement this, we made a struct called 
semaphore with and int variable holding the count and a 'queue' data structure 
to hold the blocked threads. The queue is possible due to us buidling a FCFS 
queue using a linked list.

Semaphores have a limited number of resources for threads to take and run with,
so the semaphores having a queue of blocked threads help store the threads 
that want to use that resource. The sem_down function is well needed in the 
semaphore API, because it helps keep the threads running in check by 
decrementing the amount of resources available. If there are no resources 
availible then it would call `uthread_block()` which updates the state of the 
thread from 'READY' to 'BLOCKED', and then `sem_down` would put this thread into
it's own blocked threads queue and then call `uthread_yield` to yield to the 
next thread.

In the case that a thread is done running, a resource is freed so we call 
sem_up to increase the amount of resources. In the event that the semaphore has
threads that are still blocked, then the semaphore would call `uthread_unblock`
to change the state from 'BLOCKED' to 'READY'. Since the thread is now 
unblocked, it can enqueued into the ready queue and can start running again.

Now to solve the corner case, in the sem_up function, when a thread is dequeued
from the blocked threads and enqueued into the ready thread, we do not 
imcrement the semaphore count. We do this because if a thread is unblocked and
requeued then it makes sure to give the resource back thread in order. All 
the threads in the blocked threads will keep being unblocked and requeued in 
FCFC order and the thread that was unblocked first will get the resource 
first.


## Phase 4: Implementing the Preemption API

This interrupts the ongoing execution of threads at regular intervals of time 
in a round robin manner to allow fair scheduling and prevent any thread to hog
the CPU. 

If the user wants to use preempt then in `uthread_run()`, preempy is enabled 
with`preempt_start()`. `Preempt_start()` uses the struct itimerval that helps 
us set upan alarm that fires off the SIGVTALRM every 1/100 of a second. The 1/
100 of a second is used becaused it needs to fire off the signal 100 times per 
second or 100 HZ. 

`Preempt_enable` and preempt_disable uses sigset to store all the signals that 
the program can recieve into a set and we can choose which signals to ignore or
add back in. In `preempt_enable()` the signal SIGVTALRM is added into the set 
and unblocked while `preempt_disable()` is responsible for disabling preemption
temporarily by blocking the signal. This is required when a thread attempts to 
execute its critical section, otherwise, it can be preempted by another thread,
leading to undesired behaviour.


## Testing and Error Managment

**Queue**: To validate the queue's functionality, several tests were conducted that also tackled edge cases. Tests checked if each feature performed as intended even when provided with incorrect cases such as destroying a non-empty queue or performing an action over NULL queue/data. Testing helps identify errors in the code and ensures its correctness.

**Threads**: The two given tests were used to verify uthread API's implementation. One test created a single thread which printed a message. The second test creates multiple threads that each print a message and if thread creation and yielding are implemented correctly, the messages are printed in a specific order. Additionally, the given test cases pass `NULL` as the function's parameter (`arg`). So a valid parameter was entered to verify whether the function was correctly obtained and executed it. 

**Preemption**: `test_preempt.c` creates multiple threads while enabling preemption. These threads contain an infinite while loop, for example,
```
while(1) {
    printf("thread3\n");
    sleep(1);
}
```
which continuously print a message as soon as the thread is created. Thread 1 creates Thread 2, but does explicitly call `uthread_yield()`, the only way to switch to Thread 2 without preemption is to wait for Thread 1 to complete. Thus, to prevent Thread 1 from hoggin the CPU, preemption is enabled which interrupts the currently running thread, allowing the next thread to utilise the CPU.  