### system execv popen

程序里面需要执行其他程序时选用。

system() 和 popen() + pclose() 都会等待命令执行完成返回（open()不会堵塞，但pclose()会）。

system() is in C89 and C99; popen() is Posix only(though the Windows API also has one).
Both functions invoke some form of a shell to execute the command(e.g. /bin/sh on
Linux, and probably cmd.exe on Windows). If you want to execute an executable file
directly and you are on Posix, you can also look at the exec*-family of functions
in conjuction with for()(since exec() replaces the current process).

execv() 是用新的命令直接替换子进程的运行空间，除非execv() 调用出错返回-1，否
则不会执行子进程后续代码。

在需要知道命令返回状态时，使用 system(command)

在需要获取命令输出内容时，使用 popen(command)

无需跟命令交换数据，想分道扬镳时，使用 fork() + execv()

想控制命令执行时间，并获取命令返回状态时，用 fork() + execv() + alarm() + waitpid() + WEXITSTATUS() + WIFEXITED()


### system

使用了 fork() 创建子进程(fork()失败时返回-1)，waitpid() 等待子进程返回，并将返回值作为结果返回。


### popen

使用了 fork() 创建子进程，返回可读写的文件，pclose() 时调用了 waitpid() 等待子进程返回，并将返回值作为结果返回。

### execv

```c
    int number, statval;
    printf("%d: I'm the parent !\n", getpid());

    if(fork() == 0)
    {
        number = 10;
        sleep(5);
        printf("PID %d: exiting with number %d\n", getpid(), number);
        exit(number) ;
    }
    else
    {
        printf("PID %d: waiting for child\n", getpid());
        wait(&statval);
        if(WIFEXITED(statval))
            printf("Child's exit code %d\n", WEXITSTATUS(statval));
        else
            printf("Child did not terminate with exit\n");
    }
```

```c
	static void _alarm_handler(int pid)
	{
	    if (m_pid > 0) {
	        mtc_warn("time over, kill child");

	        kill(m_pid, SIGKILL);

	        m_pid = 0;
	    }
	}

	static void cameraHeartBeat(Node *be)
	{
	    NodeCamera *me = (NodeCamera*)be;
	    char command[1024];
	    int wstatus;

	    char *url = mdf_get_value(be->config_node, "rtsp", NULL);
	    if (!url) {
	        nodeErrorSet(be, NODE_STATE_NG, "no video address");
	        return;
	    }

	    signal(SIGALRM, _alarm_handler);

	    pid_t pid = fork();
	    if (pid == 0) {
	        /* child */
	        /* ffmpeg -loglevel error -y -rtsp_transport tcp -i '%s' -vframes 1 %sdata/%s.jpg */
	        char imageurl[PATH_MAX];
	        snprintf(imageurl, sizeof(imageurl), "%sdata/%s.jpg", g_location, be->id);
	        const char *argv[] = {"ffmpeg", "-loglevel", "error", "-y", "-rtsp_transport", "tcp",
	            "-i", url, "-vframes", "1", imageurl, NULL};

	        if (execv("/usr/bin/ffmpeg", (char**)argv) < 0) {
	            mtc_err("execv error %d %s", errno, strerror(errno));
	        }

	        exit(0);
	    } else if (pid > 0) {
	        /* parent */
	        //mtc_dbg("heart beat with pid %d", pid);

	        m_pid = pid;

	        /* 只给你 4 秒钟时间干完探测 */
	        alarm(4);

	        int rv = waitpid(m_pid, &wstatus, WUNTRACED | WCONTINUED);

	        //mtc_dbg("waitpid %d returned with exit status %d %d %d",
	        //        rv, wstatus, WEXITSTATUS(wstatus), WIFEXITED(wstatus));

	        if (WEXITSTATUS(wstatus) != 0 || WIFEXITED(wstatus) != 1) {
	            /* ffmpeg 返回状态不为0，或者 ffmpeg 不是通过 exit(), _exit()，main() 中返回 */
	            nodeErrorSet(be, NODE_STATE_NG, "heartbeat lost");
	            if (me->push_pid > 0) {
	                kill(me->push_pid, SIGKILL);
	                if (waitpid(me->push_pid, &wstatus, WUNTRACED | WCONTINUED) > 0)
	                    mtc_dbg("stop streaming %d ...", me->push_pid);
	                me->push_pid = 0;
	            }
	        } else {
	            be->state = NODE_STATE_OK;
	            memset(be->errmsg, 0x0, sizeof(be->errmsg));
	        }

	        m_pid = 0;
	    } else mtc_err("fork error %s", strerror(errno));
	}
```
