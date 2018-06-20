/*************************************************************************
	> File Name: SymbolTab.c
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Tue 08 May 2018 07:01:30 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#include<stdio.h>
#include "SymbolTab.h"
#include "./3rdparty/uthash.h"
#include <string.h>

int ClearArrayList(SbTab_t *a)
{
	if(a!=NULL)
		ClearArrayList((SbTab_t*)a->SubTab);
	free(a);
	return 0;
}

void *AllocTab()
{
	void *ret;
	ret=NULL;
	return ret;
}


SbTab_t *AllocSbTab(const char *tableheader)
{
	SbTab_t *ret;
	ret=NULL;
	SbTab_t *header = AllocSymbol(tableheader,TABHEADER);
	InsertSymbol(&ret,header);
	return ret;
}

SbTab_t *AllocSymbol(const char *namev,SbType_t type)
{
	SbTab_t *ret;
	ret=(SbTab_t *)malloc(sizeof(SbTab_t));
	char*t;
    t=(char*)malloc(sizeof(char )*(strlen(namev)+1));
	strcpy(t,namev);
	ret->name=t;
	ret->SbType=type;
	ret->refcount = 0;
	ret->arraySize =0;
	// ret->atraynext =NULL;
//	ret->depth=depth;
//	if(ret->SbType!=INT_T)
	ret->varNo = -1;
	ret->SubTab=NULL;
	ret->parent=NULL;
	return ret;
}

unsigned int GetSbTabSize(SbTab_t **tab)
{
	if(tab==NULL || *tab==NULL)
		return 0;
	unsigned int ret=0;
	ret=HASH_COUNT(*tab);
	return ret;
}

int InsertSymbol(SbTab_t **tab,SbTab_t *symbol)
{
	if(symbol==NULL)
		return 1;
	if(symbol->name==NULL)
		return 2;
	HASH_ADD_KEYPTR( hh, *tab, symbol->name, strlen(symbol->name), symbol );
	return 0;
}

SbTab_t *FindSymbol(SbTab_t **tab,const char *name, SbTab_t *result)
{
	if(tab==NULL || *tab==NULL)
		return NULL;
	result = NULL;
	HASH_FIND_STR(*tab,name,result);
	return result;
}

int DeleteSymbol(SbTab_t **tab,SbTab_t *symbol)
{
	if(*tab==NULL)
		return 0;
	HASH_DEL(*tab,symbol);
	if(symbol->SbType==STRUCT_T)
	{
		ClearSbTab((SbTab_t**)(&((symbol->SubTab))));
	}
	else if(symbol->SbType==FUNCDEF)
	{
		ClearFuncTab((FuncTab_t*)(symbol->SubTab));
	}
	else if(symbol->SbType==ARRAY)
	{
		// ClearSbTab((SbTab_t**)(&((symbol->SubTab))));
		ClearArrayList((SbTab_t*)(symbol->SubTab));
	}
//	printf("delete symbol: %s \n",symbol->name);
	free(symbol->name);
	free(symbol);
	return 0;
}

int ClearSbTab(SbTab_t **tab)
{
	if(tab==NULL || *tab==NULL)
		return 0;
	if((*tab)->refcount > 0 )
	{
		(*tab)->refcount -= 1;
		return 0;
	}
//	printf("clear %s table.\n",(*tab)->name);
	SbTab_t *cur, *tmp;
	HASH_ITER(hh, *tab, cur, tmp){
		DeleteSymbol(tab,cur);
//		free(cur);
	}
	return 0;
}

int PrintSymbolTab(SbTab_t **tab,int depth)
{
	SbTab_t *s;
	for(s=*tab;s!=NULL;s=s->hh.next)
	{
		if(s->SbType == TABHEADER)
			continue;
		for(int i=0;i<depth;++i)
			printf(" ");
		printf("Symbol name is: %s; ",s->name);
		switch(s->SbType)
		{
			case INT_T: 
			printf("It is a int symbol\n");break;
			case FLOAT_T: 
			 printf("It is a float symbol\n");break;
			case ARRAY: 
			printf("It is a array\n");break;
			case FUNCDEF: 
			printf("It is a Function\n");
			FuncTab_t *func = (s->SubTab);
			SbTab_t **paralist  = (SbTab_t**)(&(func->paraList));
			printf("paralist:\n");
			PrintSymbolTab(paralist,depth+4);
			SbTab_t **fundef = (SbTab_t**)(&(func->FuncDef));
			printf("fundef:\n");
			PrintSymbolTab(fundef,depth+4);
			break;
			case STRUCT_T: 
			printf("It is a Struct\n");
			SbTab_t **strucFiled = (SbTab_t**)(&(s->SubTab));
			printf("struct filed:\n");
			PrintSymbolTab(strucFiled,depth+4);
			break;
			case SUBTAB:
			printf("it is a subtab:\n");
			SbTab_t **subtab = (SbTab_t**)(&(s->SubTab));
			PrintSymbolTab(subtab,depth +4);
			break;
			default:printf("\n"); break;
		}
	}
	return 0;
}

int ClearFuncTab(FuncTab_t *tab)
{
	free(tab->name);
//	ClearSbTab(&(tab->retType));
	ClearSbTab(&(tab->paraList));
	ClearSbTab(&(tab->FuncDef));
	return 0;
}
