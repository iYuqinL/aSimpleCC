# Lab 2. Semantic analysis

## The directory tree

```
Semantic.d
├── 3rdparty
│   └── uthash.h
├── lexical.l
├── main.c
├── Makefile
├── README.md
├── runtests.sh
├── sample.d
├── semantic.c
├── semantic.h
├── SymbolTab.c
├── SymbolTab.h
├── syntaxtree.c
├── syntaxtree.h
├── syntax.y
```

the lexical.l file is the flex file, and the syntax.y is the bison file.

syntaxtree.h and syntaxtree.c is the datastruct for gramma tree.

SymbolTab.h and SymbolTab.c is the datastruct for symbol table.

semantic.h and semantic.c is for doing semantic analysis.

3rdparty directory is for some third party code. Here, I use the uthash for my hash table.

the runtests.sh is a shell script for run the test example that the guide book supplied.

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

After the command. the result will print on the screen.

### if you want to test extra example that not in the guided book.

maybe you want test the program by other example that were not be supplied by the guided book. then you can just use the command as follow:

```
./parser testfilename
```

and the result will be putted on the screen.