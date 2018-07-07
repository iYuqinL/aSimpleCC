# Lab 4. Code Generate

## The directory tree

```
.
├── 3rdparty
├── intermedCode.c
├── intermedCode.h
├── lexical.l
├── Makefile
├── objCode.c
├── objCode.h
├── ObjCodeOut.d
├── README.md
├── runtest.sh
├── sample.d
├── semantic.c
├── semantic.h
├── SymbolTab.c
├── SymbolTab.h
├── syntax.output
├── syntaxtree.c
├── syntaxtree.h
└── syntax.y
```

the lexical.l file is the flex file, and the syntax.y is the bison file.

syntaxtree.h and syntaxtree.c are the datastruct files for gramma tree.

SymbolTab.h and SymbolTab.c are the datastruct files for symbol table.

semantic.h and semantic.c are for doing semantic analysis.

intermedCode.h and intermedCode.c are for doing intermediate code generation.

objCode.h and objCode.c are for code generation.

the runtest.sh is a shell script for run the test example that the guide book supplied.

the sample.d directory is for the test example.

## how to make and test the project.

to make and test the project is simple.

### make

for make, you just type `make` in your terminal and return. then it will be make.

### test

for test. if you want to test the example that the guided book supplied. you can just simplely run the `runtests.sh` file. I recommand you type the command as follow:

```
sh runtest.sh
```

After the command. the intermediate code result will print on the screen.

### if you want to test extra example that not in the guided book.

maybe you want test the program by other example that were not be supplied by the guided book. then you can just use the command as follow:

```
./parser testfilename IRCodeOutputfilename
```

and the intermediate code  result will be putted on the screen.