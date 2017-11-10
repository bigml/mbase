## background job

A background-job (ie. started with &) still has it's stdin, stdout and stderr connected to the terminal it
was started in.

You may of course redirect stdout and stderr to a file or to /dev/null to prevent the background-job from
writing to the terminal.

A background-job can also be put in foreground - eg. the current foreground-job is stopped, and the  fg
(foreground) command is used to put a background-job in the foreground. A background-job can also be reached
by signals from the terminal - eg. SIGHUP when you close the terminal, which usually end (most) programs
started in the terminal.


## daemon
A daemon - like the ones started automatically by init.d, but which also can be started manually from a
terminal - on the other hand, runs disconnected from any terminals. Even if it was manually started from a
terminal, a daemon will be disconnected from the terminal, so it can neither write (stdout, stderr) nor read
(stdin) it. It's also "immune" to signals sent "automatically" by the terminal. (though you can send signals
to it by using kill -signal pid).

If you use ps with the options that shows which terminal a process uses, you will see that both fore-and
backgroundjobs are assosciated with a terminal (eg. tty2). Daemons on the other hand, have a "?" in this
field.

Daemons usually beahaves as such, even if they're started manually. Creating a daemon of you own, is quite a
bit of work - there are some trickery involved to totally disconnect it from the terminal.

* You should create it's own user/group to run as.

* You must usually use /tmp, /var/tmp or /var/run if you want it to create files - it usually shouldn't have
  rights anywhere else.

* Since it can't report errors to a terminal, you should have it write to a log-file (eg. it's own logfile
 in /var/log).

* Daemons should make an entry in /var/run with it's current PID, and should check if another instance of it
  was already running. It should respect locks (/var/lock) for files or devices where applicable.

* It should respond to SIGHUP by reloading it's config-files and use updated configurations.


## alternative
Using nohup or disown on a background-job is usually a good enough alternative, as it keeps the process
alive even if the terminal closes. It's often a good idea to redirect stdout and stderr to a file or to
/dev/null though. For more interactive programs, screen is a good way to put something "away" until you need
it.  at, batch and crontab is also worth to concider.
