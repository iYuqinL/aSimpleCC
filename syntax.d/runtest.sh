#!/bin/bash

mkdir -p ./test_result/basic
resultpath="./test_result/basic/"
resultfilename="test_"
srcpath="./test.d/"
srcfilename="rtest"
posfix=".txt"

for i in $(seq 1 4)
do
    resultfile=${resultpath}${resultfilename}${i}${posfix}
    srcfile=${srcpath}${srcfilename}${i}".c"
    echo "\033[32;1m \n\n\nrunning the basic test ${i}:\033[0m"

    ./parser ${srcfile} | tee  ${resultfile}
done

mkdir -p ./test_result/elective
srcfilename="test"
resultpath="./test_result/elective/"

for i in $(seq 1 6)
do
    resultfile=${resultpath}${resultfilename}${i}${posfix}
    srcfile=${srcpath}${srcfilename}${i}".c"
    echo "\033[32;1m \n\n\nrunning the elective test ${i}:\033[0m"
    ./parser ${srcfile} | tee  ${resultfile}
done
