/*************************************************************************
	> File Name: syntaxtree.c
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Wed 11 Apr 2018 09:13:29 AM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include"syntaxtree.h"

#ifndef Debug_print
#define Debug_print 0
#endif

struct syntaxtree_t* allocnewsyntaxtree()
{
    struct syntaxtree_t* ret = (struct syntaxtree_t*)malloc(sizeof(struct syntaxtree_t));
    if(ret == NULL)
        return NULL;
    ret->setroot = &syntaxtree_setroot;
    ret->initial = &syntaxtree_initial;
    ret->addchild = &syntaxtree_addchild;
    ret->addchildren = &syntaxtree_addchildren;
    ret->printsyntaxtree = &PrintSyntaxtree;
    ret->destroy = &syntaxtree_destroy;
    syntaxtree_initial(ret);
    return ret;
}


int syntaxtree_setroot(struct syntaxtree_t* T, struct node_t* r)
{
    if(r ==NULL)
        return -1;
    T->root = r;
    return 0;
}

int syntaxtree_initial(struct syntaxtree_t* T)
{
    T->root = NULL;
    return 0;
}

int syntaxtree_addchild(struct node_t* parent, struct node_t* childval)
{
    if(parent->child == NULL)
    {
        parent->child = allocnewarray();
        if(parent->child == NULL)
            return -1;
        #if Debug_print
        printf("fist time to add child\n");
        #endif
    }
//    if(childval!=NULL)
    arraypush_back(parent->child,childval);
    return 0;
}

int syntaxtree_addchildren(struct node_t* parent, int num, ...)
{
    if(parent->child == NULL)
    {
        parent->child = allocnewarray();
        if(parent->child == NULL)
            return -1;
    }
    va_list valist; //定义变长参数列表
    va_start(valist,num);//初始化变长参数为num后的参数
    struct node_t* temp = NULL;
    for(int i=0;i<num;++i)
    {
        temp = va_arg(valist,struct node_t*);
//        if(temp != NULL)
        arraypush_back(parent->child,temp);
    }
    return 0;
}

int PrintSyntaxtree(struct syntaxtree_t* T, struct node_t* currnode, int deepth)
{
    if(currnode != NULL)
    {
        for(int i=0; i<deepth; ++i)//孩子结点相对父节点缩进2个空格
            printf("  ");
        printf("%s",currnode->name);
        if((!strcmp(currnode->name,"ID")||(!strcmp(currnode->name,"TYPE"))))
        {
            printf(": %s",currnode->ID_type);
        }
        else if(!strcmp(currnode->name,"INT"))
            printf(":%d",currnode->I_type);
        else if(!strcmp(currnode->name,"FLOAT"))
            printf(" :%f",currnode->F_type);
        else
            printf("(%d)",currnode->lineno);
        printf("\n");
    }
    if(currnode != NULL)
    {
        int deepth_tmp = deepth + 1;
        if(currnode->child != NULL)
            for(int i=0;i<= currnode->child->top;++i)
            {
                PrintSyntaxtree(T,currnode->child->ptr[i],deepth_tmp);
            }
    }
    return 0;
}

int syntaxtree_destroy(struct syntaxtree_t* T)
{
    if(T->root != NULL)
    {
        delanode(T->root);
    }
    return 0;
}

struct node_t* allocnewnode(char* nameval, int lineno, char* text)
{
    struct node_t* ret = (struct node_t*)malloc(sizeof(struct node_t));
    if(ret==NULL)
        return NULL;
    ret->child = NULL;
    ret->lineno = lineno;
    ret->ID_type = NULL;
    ret->name=(char*)malloc(sizeof(char)*(strlen(nameval)+1));
    strcpy(ret->name,nameval);
    if((!strcmp(ret->name,"ID"))||(!strcmp(ret->name,"TYPE")))
    {
        char*t;
        t=(char*)malloc(sizeof(char )*(strlen(text)+1));
        strcpy(t,text);
        ret->ID_type=t;
    }
    else if(!strcmp(ret->name,"INT")) 
    {
        ret->I_type=str2int(text);
        ret->ID_type=NULL;
    }
    else if(!strcmp(ret->name,"FLOAT"))
    {
        ret->F_type=atof(text);
        ret->ID_type=NULL;
    }
//    strncpy(ret->name,nameval,strlen(nameval));
    return ret;
}

int delanode(struct node_t* node)
{
    if(node != NULL)
    {
        if(node->name!=NULL)
        {
            free(node->name);
            node->name=NULL;
        }
        if(node->ID_type!=NULL)// && node->ID_type_need_free==1)
        {
            free(node->ID_type);
            node->ID_type=NULL;
        }
        if(node->child!=NULL)
        {
            arrayclear(node->child);
            free(node->child);
            node->child=NULL;
        }
        free(node);
        node=NULL;
    }
    return 0;
}

struct nodearray* allocnewarray()
{
    struct nodearray* ret = (struct nodearray*)malloc(sizeof(struct nodearray));
    if(ret == NULL)
        return NULL;
    ret->initial = arrayInitial;
    ret->push_back = arraypush_back;
    ret->pop_back = arraypop_back;
    ret->at=array_at;
    ret->size = arraysize;
    ret->clear = arrayclear;
    ret->isFull = arrayisFull;
    ret->isEmpty = arrayisEmpty;
    ret->recap = arrayrecap;
    if(arrayInitial(ret,4)!=0)
        if(arrayInitial(ret,4)!=0)
        {
            free(ret);
            return NULL;
        }
    return ret;
}

int arrayInitial(struct nodearray* arr, int cap)
{
    arr->top = -1;
    arr->capacity = cap;
    arr->ptr = (struct node_t**)malloc(sizeof(struct node_t*)*(arr->capacity));
    if(arr->ptr == NULL)
        return -1;
    return 0;
}

int arraypush_back(struct nodearray* arr, struct node_t* val)
{
    if(arrayisFull(arr)==1)
    {
        if(arrayrecap(arr,(*arr).capacity*2)!=0)
            return -1;
        #if Debug_print
        printf("array capacity increase to %d\n",arr->capacity);
        #endif
    }
    (arr->ptr)[++(arr->top)] = val;
    return 0;
}

struct node_t* arraypop_back(struct nodearray* arr)
{
    struct node_t* r = NULL;
    if(arrayisEmpty(arr))
        return r;
    if(arr->ptr[(arr->top)]!=NULL)
    {
        r = (*arr).ptr[(arr->top)];
//        free((*arr).ptr[(*arr).top]);
        (*arr).ptr[(arr->top)--]=NULL;
    }
    if(arraysize(arr)<(*arr).capacity/3 && (*arr).capacity>8)
        if(arrayrecap(arr,(*arr).capacity/2)!=0);
    return r;
}
struct node_t* array_at(struct nodearray *arr,int pos)
{
    if(pos>arr->top || pos<0)
        return NULL;
    else
        return arr->ptr[pos];
}

int arraysize(struct nodearray* arr)
{
    return (*arr).top+1;
}

int arrayclear(struct nodearray* arr)
{
    while(arrayisEmpty(arr)!=1)
    {
        if(arr->ptr[(arr->top)]!=NULL)
        {
            delanode(arr->ptr[(arr->top)]);
            (*arr).ptr[(arr->top)--]=NULL;
        }
        else
        {
            arr->top--;
        }
//        arr->top--;
        if(arraysize(arr)<(*arr).capacity/3 && (*arr).capacity>8)
            if(arrayrecap(arr,(*arr).capacity/2)==0)
            {
                #if Debug_print
                printf("array capacity reduce to %d\n",arr->capacity);
                #endif
            }
    }
        return 0;
}

int arrayisFull(struct nodearray* arr)
{
    if(arr->top >= arr->capacity-1)
        return 1;
    else
        return 0;
}

int arrayisEmpty(struct nodearray* arr)
{
    if(arr == NULL)
        return 1;
    if(arr->top == -1)
        return 1;
    else
        return 0;
}

int arrayrecap(struct nodearray* arr,int newcap)
{
    struct node_t** newPtr = (struct node_t**)malloc(sizeof(struct node_t*)*newcap);
    if(newPtr != NULL)
    {
        int cap_tmp;
        cap_tmp= newcap>(arr->capacity)?(arr->capacity):newcap;
        memcpy(newPtr,arr->ptr,cap_tmp*sizeof(struct node_t*));
        free(arr->ptr);
        arr->ptr = newPtr;
        arr->capacity = newcap;
        return 0;
    }
    else
    {
        return -1;
    }
}


int str2int(const char* str)
{
    int len=strlen(str);

    int ret = atoi(str);
//    printf("the int yytext len is: %d\n", len);
    if((ret==0 && len>1)||(str[0]=='0' && len>1)) //非十进制数
    {
//        printf("I not a decimal num\n");
        ret=0;
        if(str[0]=='0' && (str[1]!='x' && str[1]!='X')) //八进制
        {
//            printf("I am a octer num\n");
            for(int i = 0; i<len; ++i)
            {
                if(str[i]>='0'&&str[i]<='8')
                    ret=ret*8+(str[i]-'0');
            }
        }
        else        //十六进制
        {
//            printf("I am a hexical num\n");
            for(int i =2; i<len; ++i)
            {
                if(str[i]!='H'||str[i]!='H')
                    if(str[i]>='0'&&str[i]<='9')
                        ret=ret*16+ (str[i]-'0');
                    else if(str[i]>='A'&& str[i]<='F')
                        ret=ret*16+ (str[i]-55);
                    else if(str[i]>='a' && str[i]<='f')
                        ret=ret*16 + (str[i]-87);
            }
        }
    }
    return ret;
}
float str2float(const char* str)
{

}
