/*************************************************************************
	> File Name: syntaxtree.h
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Wed 11 Apr 2018 09:13:11 AM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#ifndef _SYNTAXTREE_H
#define _SYNTAXTREE_H

#include <stdlib.h>
#include<stdarg.h>//变长参数函数所需的头文件
#include<string.h>

struct nodearray;
struct node_t;
struct syntaxtree_t;

extern int yylineno;//行号
extern char* yytext;//词

int str2int(const char* str);
float str2float(const char* str);


/*************************************************************************
 *  _______
 * | node1 |
 * |__Ptr__|
 *      |
 *      |<-------child                  
 *  ____|___________________________nodearray__________________________
 * | ___|_______  fraternal_node ____________  fraternal_node          |
 * || childnode1|--------------->| childnode2| ----------------> ......| 
 * ||_____Ptr___|                |_____Ptr___|                         |
 * |___________________________________________________________________|
*************************************************************************/
typedef struct syntaxtree_t
{
    struct node_t* root;

    int (*setroot)(struct syntaxtree_t* selft, struct node_t* r);
    int (*initial)(struct syntaxtree_t* selft);
    int (*addchild)(struct node_t* parent, struct node_t* child);
    int (*addchildren)(struct node_t* parent, int num, ...);
    int (*printsyntaxtree)(struct syntaxtree_t* selft, struct node_t* currnode, int deepth);
    int (*destroy)(struct syntaxtree_t* selft);
}Sytree_t;
extern struct syntaxtree_t* T;
struct syntaxtree_t* allocnewsyntaxtree();

int syntaxtree_setroot(struct syntaxtree_t* T, struct node_t* r);
int syntaxtree_initial(struct syntaxtree_t* T);
int syntaxtree_addchild(struct node_t* parent, struct node_t* child);
int syntaxtree_addchildren(struct node_t* parent, int num, ...);
int PrintSyntaxtree(struct syntaxtree_t* T, struct node_t* currnode, int deepth);
int syntaxtree_destroy(struct syntaxtree_t* T);


typedef struct node_t
{
    struct nodearray* child;
    int lineno;
    char *name;
    char *ID_type;
    union{
        int I_type;
        float F_type;
    };
}node_t;

struct node_t* allocnewnode(char* name, int lineno, char* text);
int delanode(struct node_t* node);


/*****************************************************************************
 *  |      | <----capacity
 *  |      | 
 *  |      |
 *  |      |
 *  |______|
 *  | data1| <------top
 *  |______| 
 *  | data0|
 *  |______| <------ptr
******************************************************************************/
struct nodearray
{
    int capacity;
    int top;
    struct node_t** ptr;

    int (*initial)(struct nodearray* selft,int capacity);
    int (*push_back)(struct nodearray* selft,struct node_t* val);
    struct node_t* (*pop_back)(struct nodearray* selft);
    struct node_t* (*at)(struct nodearray* selft, int pos);
    int (*size)(struct nodearray* selft);
    int (*clear)(struct nodearray* selff);
    int (*isFull)(struct nodearray* selft);
    int (*isEmpty)(struct nodearray* selft);
    int (*recap)(struct nodearray* selft,int newcap);
};

struct nodearray* allocnewarray();

int arrayInitial(struct nodearray* arr, int cap);
int arraypush_back(struct nodearray* arr, struct node_t* val);
struct node_t* arraypop_back(struct nodearray* arr);
struct node_t* array_at(struct nodearray *arr,int pos);
int arraysize(struct nodearray* arr);
int arrayclear(struct nodearray* arr);
int arrayisFull(struct nodearray* arr);
int arrayisEmpty(struct nodearray* arr);
int arrayrecap(struct nodearray* arr,int newcap);

#endif
