### Mutex and conditional variable

Mutex is used for synchronized resource accessing, while conditional variable is used for
waiting for a condition to be true.


### Why I need a mutex when use conditional variable

You need condition variables, to be used with a mutex (each cond.var. belongs to a mutex)
to signal changing states (conditions) from one thread to another one.


POSIX condition variables are stateless. So it is your responsibility to maintain the
state. Since the state will be accessed by both threads that wait and threads that tell
other threads to stop waiting, it must be protected by a mutex. If you think you can use
condition variables without a mutex, then you haven't grasped that condition variables are
stateless.

Condition variables are built around a condition. Threads that wait on a condition
variable are waiting for some condition. Threads that signal condition variables change
that condition. For example, a thread might be waiting for some data to arrive. Some other
thread might notice that the data has arrived. "The data has arrived" is the condition.

Here's the classic use of a condition variable, simplified:

```c
while(1)
{
    pthread_mutex_lock(&work_mutex);

    while (work_queue_empty())       // wait for work
       pthread_cond_wait(&work_cv, &work_mutex);

    work = get_work_from_queue();    // get work

    pthread_mutex_unlock(&work_mutex);

    do_work(work);                   // do that work
}
```

See how the thread is waiting for work. The work is protected by a mutex. The wait
releases the mutex so that another thread can give this thread some work. Here's how it
would be signalled:

```c
void AssignWork(WorkItem work)
{
    pthread_mutex_lock(&work_mutex);

    add_work_to_queue(work);           // put work item on queue

    pthread_cond_signal(&work_cv);     // wake worker thread

    pthread_mutex_unlock(&work_mutex);
}
```

Notice that you need the mutex to protect the work queue. Notice that the condition
variable itself has no idea whether there's work or not. That is, a condition variable
must be associated with a condition, that condition must be maintained by your code, and
since it's shared among threads, it must be protected by a mutex.



* from man pthread_cond_timedwait

The pthread_cond_timedwait() and pthread_cond_wait() functions shall block on a condition
variable. They shall be called with mutex locked  by  the calling thread or undefined
behavior results.

These functions atomically release mutex and cause the calling thread to block on the
condition variable cond;

这些函数会 自动 释放该互斥锁，并且让本线程等待 条件变量。

atomically here means "atomically with respect to access
by another thread to the mutex and then the condition variable". That is, if
another thread is able  to  acquire  the  mutex after  the  about-to-block thread
has released it, then a subsequent call to pthread_cond_broadcast() or
pthread_cond_signal() in that thread shall behave as if it were issued after the
about-to-block thread has blocked.

此处 自动 的意思是 释放该锁，等待另外的线程获取该锁后进行后续操作并最终触发
pthread_cond_broadcast/signal() 来让本线程继续干活。

Upon successful return, the mutex shall have been locked and shall be owned by the calling
thread.


* from man pthread_cond_signal

The pthread_cond_broadcast() or pthread_cond_signal() functions may be called by a thread
whether or not it currently owns the mutex  that  threads calling  pthread_cond_wait()  or
pthread_cond_timedwait()  have associated with the condition variable during their waits;
however, if predictable scheduling behavior is required, then that mutex shall be locked
by the thread calling pthread_cond_broadcast() or pthread_cond_signal().
