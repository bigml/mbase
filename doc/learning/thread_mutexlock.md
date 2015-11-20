### mutex lock

The  pthread_mutex_trylock() function shall be equivalent to pthread_mutex_lock(), except
that if the mutex object referenced by mutex is currently locked (by any thread, including
the current thread), the call shall return immediately. If the mutex type is
PTHREAD_MUTEX_RECURSIVE and the mutex is  currently  owned by the calling thread, the
mutex lock count shall be incremented by one and the pthread_mutex_trylock() function
shall immedi-ately return success.
