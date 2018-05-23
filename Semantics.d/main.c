/*************************************************************************
	> File Name: main.c
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Fri 06 Apr 2018 07:36:47 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#include<stdio.h>
#include "syntax.tab.h"
#include "syntaxtree.h"
#include "semantic.h"

struct syntaxtree_t* T;
extern int errorflag;

int main(int argc, char** argv)
{
    if(argc <= 1) return 1;
    FILE* f = fopen(argv[1],"r");
    if(!f)
    {
        perror(argv[1]);
        return 1;
    }
//    yydebug = 1;
    T = allocnewsyntaxtree();
    if(T==NULL)
    {
        printf("can not alloc a syntax tree!\n");
        return -1;
    }
    yyrestart(f);
    yyparse();
    SbTab_t *gtab=NULL;
    if(errorflag==0)
    {
        //T->printsyntaxtree(T,T->root,0);
        gtab = Program(T);
    }    
    T->destroy(T);
//    ClearSbTab(&gtab);
    return 0;
}
