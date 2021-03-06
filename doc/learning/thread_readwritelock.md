### read write lock

An RW lock allows concurrent access for read-only operations, while write operations
require exclusive access. This means that multiple threads can read the data in parallel
but an exclusive lock is needed for writing or modifying data. When a writer is writing
the data, all other writers or readers will be blocked until the writer is finished
writing. A common use might be to control access to a data structure in memory that cannot
be updated atomically and is invalid (and should not be read by another thread) until the
update is complete.
