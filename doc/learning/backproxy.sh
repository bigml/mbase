#!/bin/bash

TIME=`date "+%Y-%m-%d %H:%M:%S"`

# 将本机公钥配置到 authorized_keys, 以保证免密登陆
ssh -q ml@txa -p 54110 exit
RET=$?
#echo $RET

if [ $RET -ne 0 ]; then
    PIDS=$(ps aux | grep '[s]sh -o ServerAliveInterval=1800 -NTfR 54110' |  awk '{print $2}')
    PIDCOUNT=`echo "$PIDS" | wc -w`
    echo $PIDCOUNT

    echo "$TIME can't establish ssh connection, kill $PIDS and reproxy..."

    if [ $PIDCOUNT -gt 0 ]; then
        kill $PIDS
    fi
    ssh -o ServerAliveInterval=1800 -NTfR 54110:localhost:22 demo@txa
else
    echo "$TIME connection ok"
fi

