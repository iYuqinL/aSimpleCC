# Lab 1. Gramma analysis

## the directory tree  

```
syntax.d
├── lexical.l
├── main.c
├── Makefile
├── README.MD
├── runtest.sh
├── syntaxtree.c
├── syntaxtree.h
├── syntax.y
└── test.d
    ├── rtest1.c
    ├── rtest2.c
    ├── rtest3.c
    ├── rtest4.c
    ├── test1.c
    ├── test2.c
    ├── test3.c
    ├── test4.c
    ├── test5.c
    └── test6.c
```  

the lexical.l file is the flex file, and the syntax.y is the bison file.  

syntaxtree.h and syntaxtree.c is the datastruct for Gramma tree.  

the runtest.sh is a shell script for run the test example that the guide book supplied.  

the test.d directory is for the test example, rtest_.c means required test example, test_.c means the elective test example.  

## how to make and test the project.

to make and test the project is simple.  

### make

for make, you just type ` make ` in your terminal and return. then it will be make.

### test

for test. if you want to test the example that the guided book supplied. you can just simplely run the ` runtest.sh ` file. I recommand you type the command as follow:
```
sh runtest.sh
```
After you run the runtest.sh shell script. then it will create a directory named ` test_result `, and the result will be put in the directory. of course, the result will also print on the terminal screen. the directory tree after that showed as follow:
```
.
├── lexical.l
├── lex.yy.c
├── main.c
├── Makefile
├── parser
├── README.MD
├── runtest.sh
├── syntax.o
├── syntax.tab.c
├── syntax.tab.h
├── syntaxtree.c
├── syntaxtree.h
├── syntaxtree.o
├── syntax.y
├── test.d
│   ├── rtest1.c
│   ├── rtest2.c
│   ├── rtest3.c
│   ├── rtest4.c
│   ├── test1.c
│   ├── test2.c
│   ├── test3.c
│   ├── test4.c
│   ├── test5.c
│   └── test6.c
└── test_result
    ├── basic
    │   ├── test_1.txt
    │   ├── test_2.txt
    │   ├── test_3.txt
    │   └── test_4.txt
    └── elective
        ├── test_1.txt
        ├── test_2.txt
        ├── test_3.txt
        ├── test_4.txt
        ├── test_5.txt
        └── test_6.txt
```
I think it is quite clear for the structure of the project and the test result.

### if you want to test extra example that not in the guided book.

maybe you want test the program by other example that were not be supplied by the guided book. then you can just use the command as follow:
```
./parser testfilename
```
and the result will be putted on the screen.