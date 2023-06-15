### read write lock

An RW lock allows concurrent access for read-only operations, while write operations
require exclusive access. This means that multiple threads can read the data in parallel
but an exclusive lock is needed for writing or modifying data. When a writer is writing
the data, all other writers or readers will be blocked until the writer is finished
writing. A common use might be to control access to a data structure in memory that cannot
be updated atomically and is invalid (and should not be read by another thread) until the
update is complete.

    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

    pthread_rwlock_init(&rwlock, NULL);

    pthread_rwlock_rdlock(&rwlock);
    pthread_rwlock_wrlock(&rwlock);

    pthread_rwlock_tryrdlock(&rwlock);
    pthread_rwlock_trywrlock(&rwlock);

    pthread_rwlock_unlock(&rwlock);

    pthread_rwlock_destroy(&rwlock);
