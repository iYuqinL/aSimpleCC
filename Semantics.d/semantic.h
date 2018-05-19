/*************************************************************************
	> File Name: semantic.h
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Fri 11 May 2018 05:24:07 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#ifndef _SEMANTIC_H
#define _SEMANTIC_H
#include "syntaxtree.h"
#include "SymbolTab.h"

int haschildren(node_t *node);

typedef enum ERRORTYPE_T
{
	VAR_REDEF=-11,
	FUNC_REDEF=-12,
	TYPE_CONFLICT=-13,
	FUNC_RECUR=-14, //函数嵌套定义
	FUNC_NDEF=-15,
}Err_t;

typedef enum TableType_t
{
	BASIC_TAB=100,
	STRUCTFILED=101,
	FUNDEF_TAB=102,
	FUNRET_TAB=103,
	FUNPARA_TAB=104,
	SUBBASIC_TAB=105,
}TabTyp_t;

SbTab_t *Program(SbTab_t *PTab, Sytree_t *sytree);
int ExtDefList(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp, node_t *node);
int ExtDef(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp,node_t *node);
int ExtDecList(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp,node_t *node, SbTab_t *type);

SbTab_t *Specifier(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node);
SbTab_t *StructSpecifier(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp, node_t *node);
SbTab_t *VarDec(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *type);

FuncTab_t *FunDec(SbTab_t *PTab, node_t *node, SbTab_t *retT);
SbTab_t *VarList(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp, node_t *node);
SbTab_t *ParamDec(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp,node_t *node);

SbTab_t *CompSt(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp,node_t *node,SbTab_t *retT);
int StmtList(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp, node_t *node,SbTab_t *retT);
int Stmt(SbTab_t *PTab,SbTab_t **tab, TabTyp_t tabtyp, node_t *node,SbTab_t *retT);

SbTab_t *DefList(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp, node_t *node);
SbTab_t *Def(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node);
SbTab_t *DecList(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *type);
SbTab_t *Dec(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *type);

SbTab_t *Exp(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node);
int Args(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *paralist);

//辅助函数
int typeCmp(SbTab_t *a, SbTab_t *b);
int arraytypeCmp(arr_t *a, arr_t*b);
int StTypeCmp(SbTab_t **a,SbTab_t **b);

SbTab_t *LookUpSymbol(SbTab_t **tab,char *name, SbTab_t *result);

int printParaTypList(SbTab_t *paralist);
int printArgs(SbTab_t *PTab,SbTab_t **tab,TabTyp_t tabtyp, node_t *node);
#endif
