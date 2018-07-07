#!/bin/bash

srcpath="./sample.d/"
srcfilename=""
posfix=".txt"

IRcodeOutputPath="./IRCodeOutput.d/"
IRcodeFileName="IRcode"
IRcodepostfix=".ir"

for i in $(seq 1 2)
do
    srcfile=${srcpath}${srcfilename}${i}".c"
    IRcodeFile=${IRcodeOutputPath}${IRcodeFileName}${i}${IRcodepostfix}
    echo "\033[32;1m \n\nrunning the basic test ${i}:\033[0m"

    ./parser ${srcfile} ${IRcodeFile}
    echo "\n\n"
done

for i in $(seq 1 2)
do

    IRcodeFile=${IRcodeOutputPath}${IRcodeFileName}${i}"_"${IRcodepostfix}
    srcfile=${srcpath}${i}"_.c"

    echo "\033[32;1m \n\nrunning the elective test ${i}:\033[0m"

    ./parser ${srcfile} ${IRcodeFile}
    echo "\n\n"
done
