#!/bin/bash
echo "\033[32;1m \n\nrunning the basic test 1:\033[0m"
./parser ./sample.d/1.c ObjCodeOut.d/1.s
echo "\033[32;1m \n\nrunning the basic test 2:\033[0m"
./parser ./sample.d/2.c ObjCodeOut.d/2.s

