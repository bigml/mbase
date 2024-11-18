#!/bin/bash

CUR_TIME=`date +'%Y-%m-%d %H:%M:%S'`
echo `date +'%Y-%m-%d %H:%M:%S'` > $LOGFILE

pids=`ps aux | grep '[s]ucker' |  awk '{print $2}'`
PIDME=$$

for file in /etc/*.conf; do
    if [ -f $file ]; then
        echo "文件 $file 存在"
    fi
done


# -f 判断文件是否存在
# -s 判断文件是否为空
# -z 判断字符串长度是否为0 (-z "${var// /}" 判断是否为空格)
# -n 判断字符串长度是否不为0
# -x 判断文件是否存在且可执行


if [ ! -z "$var" ] || [ "$var" == "0" ]; then fi
if [[ -z "$var" || "$var" == "0" ]]; then
elif
else
fi

while [ True ]; do
done

re="${LOCATION}data/offline/([0-9]+)_([a-z]+)_([0-9]+).mp4"
if [[ $file =~ $re ]]; then
    echo "上报"
    UTC=${BASH_REMATCH[1]}
    TYPE=${BASH_REMATCH[2]}
    BEGIN=${BASH_REMATCH[3]}
fi

# 如果不存在则创建命名管道
[ -p "$PIPE_NAME" ] || mkfifo "$PIPE_NAME"


func_name() {
    name = "`basename "$1"`"
}

func_name /dev/sda1


#########################
#### 关于输入输出重定向 ####
#########################
# 0 stdin 1 stdout 2 stderr
# 创建一个文件描述符3，并将其绑定至某个命名管道（文件描述符的目标，可以是文件、管道、网络链接等）
# 后续可以用 cat <&3 或者 echo "xxxxx" >&3 来对管道进行读写
exec 3<> "$PIPE_NAME"

# >& 是一个重定向操作符，表示将一个输出流重定向到另一个流，以下两个语句完全等效
echo "this is error message" > /dev/stderr
echo "this is error message" >&2

# ################## redis subscribe example ##################
# subscriber.sh
exec 5>&-                       # close file descriptor 5
exec 5<>/dev/tcp/localhost/6379 # attach a redis port to file descriptor 5
echo 'SUBSCRIBE WS2302U' >&5

while true
do
    cat <&5
done

# ################## pipe example ##################
# writer.sh
#!/bin/bash

fifo_name="myfifo";

# Se non esiste, crea la fifo;
[ -p $fifo_name ] || mkfifo $fifo_name;

exec 3<> $fifo_name;

echo "foo" > $fifo_name;
echo "bar" > $fifo_name;

# reader.sh
#!/bin/bash
fifo_name="myfifo"
while true
do
    if read line; then
        echo $line
    fi
done <"$fifo_name"


#Duplicating File Descriptors
#       The redirection operator
#
#              [n]<&word
#
#       is used to duplicate input file descriptors.  If word expands to one or
#       more  digits,  the file descriptor denoted by n is made to be a copy of
#       that file descriptor.  If the digits in word  do  not  specify  a  file
#       descriptor  open for input, a redirection error occurs.  If word evalu
#       ates to -, file descriptor n is closed.  If n  is  not  specified,  the
#       standard input (file descriptor 0) is used.
#
#       The operator
#
#              [n]>&word
#
#       is  used  similarly  to duplicate output file descriptors.  If n is not
#       specified, the standard output (file descriptor 1)  is  used.   If  the
#       digits  in word do not specify a file descriptor open for output, a re
#       direction error occurs.  As a special case, if n is omitted,  and  word
#       does not expand to one or more digits, the standard output and standard
#       error are redirected as described previously.



#   -a file
#          True if file exists.
#   -b file
#          True if file exists and is a block special file.
#   -c file
#          True if file exists and is a character special file.
#   -d file
#          True if file exists and is a directory.
#   -e file
#          True if file exists.
#   -f file
#          True if file exists and is a regular file.
#   -g file
#          True if file exists and is set-group-id.
#   -h file
#          True if file exists and is a symbolic link.
#   -k file
#          True if file exists and its ``sticky'' bit is set.
#   -p file
#          True if file exists and is a named pipe (FIFO).
#   -r file
#          True if file exists and is readable.
#   -s file
#          True if file exists and has a size greater than zero.
#   -t fd  True if file descriptor fd is open and refers to a terminal.
#   -u file
#          True if file exists and its set-user-id bit is set.
#   -w file
#          True if file exists and is writable.
#   -x file
#          True if file exists and is executable.
