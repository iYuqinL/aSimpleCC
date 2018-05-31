#!/bin/bash

srcpath="./sample.d/"
srcfilename=""
posfix=".txt"

for i in $(seq 1 17)
do
#    resultfile=${resultpath}${resultfilename}${i}${posfix}

    srcfile=${srcpath}${srcfilename}${i}".c"
    echo "\033[32;1m \n\nrunning the basic test ${i}:\033[0m"

    ./parser ${srcfile}
    echo "\n\n"
done

for i in $(seq 1 6)
do
    srcfile=${srcpath}${i}"_.c"

    echo "\033[32;1m \n\nrunning the elective test ${i}:\033[0m"

    ./parser ${srcfile}
    echo "\n\n"
done
