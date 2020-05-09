#!/bin/bash

while [ True ]; do

    read line

    if [ ${#line} != 0 ]; then
        seconds=`echo $line | awk -F. '{print $1}'`
        remain=`echo $line | cut -f 2- -d.`
        ts=`date -d @$seconds +'%Y-%-m-%-d %H:%M:%S'`
        echo $ts.$remain
    fi

done
