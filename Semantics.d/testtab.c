/*************************************************************************
	> File Name: testtab.c
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Tue 22 May 2018 11:32:33 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#include<stdio.h>
#include "SymbolTab.h"

int testinsert(SbTab_t **tab, SbTab_t *sy);

int main(int argc, char const *argv[])
{
	SbTab_t *tab = AllocSbTab("test_tab");
	SbTab_t *syb = AllocSymbol("int",INT_T);
	testinsert(&tab,syb);
	SbTab_t *tmp;
	for(tmp = tab;tmp != NULL; tmp=(SbTab_t *) tmp->hh.next)
	{
		printf("%s\n",tmp->name);
	}
	return 0;
}

int testinsert(SbTab_t **tab, SbTab_t *sy)
{
	InsertSymbol(tab,sy);
	return 0;
}
