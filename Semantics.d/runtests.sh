#!/bin/bash

srcpath="./sample/"
srcfilename=""
posfix=".txt"

for i in $(seq 1 17)
do
#    resultfile=${resultpath}${resultfilename}${i}${posfix}

    srcfile=${srcpath}${srcfilename}${i}".c"
    echo "\033[32;1m \n\n\nrunning the basic test ${i}:\033[0m"

    ./parser ${srcfile}
done
