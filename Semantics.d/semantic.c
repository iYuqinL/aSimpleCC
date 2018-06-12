/*************************************************************************
	> File Name: semantic.c
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Fri 11 May 2018 05:24:13 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#include<stdio.h>
#include "semantic.h"
#include<stdlib.h>

static int SubTabCNT=0;

const char *FuncDefTabHeader = "____FuncDefTabHeader____";
const char *StructFieldTabHeader = "____StructFieldTabHeader____";
const char *ParaListTabHeader = "____ParaListTabHeader____";
const char *SubTabHeader = "____SubTabHeader____";
const char *FuncNameTabHeader = "____FuncNameTabHeader____";

int haschildren(node_t *node)
{
	if(node==NULL)
		return -1;
	if(node->child==NULL)
		return -1;
	if(node->child->isEmpty(node->child))
		return -1;
	return (node->child->size(node->child));
}

SbTab_t *Program(Sytree_t *sytree)
{
	if(sytree ==NULL)
		return 0;
	if(haschildren(sytree->root)==-1)
		return 0;
	SbTab_t *gTab=AllocSbTab("____GlobalTab____");
	SbTab_t *funcNameTab = AllocSbTab(FuncNameTabHeader);
	SbTab_t *funcNameTabSy = AllocSymbol(FuncNameTabHeader,TABHEADER);
	funcNameTabSy->SubTab = funcNameTab;
	funcNameTab->parent = funcNameTabSy;
	InsertSymbol(&gTab,funcNameTabSy);
	ExtDefList(&gTab,BASIC_TAB,sytree->root->child->ptr[0]);
	//do something?
	for(SbTab_t *iter = funcNameTab->hh.next;iter != NULL; iter = iter->hh.next)
	{
		SbTab_t *tmp = NULL;
		tmp = FindSymbol(&gTab,iter->name,tmp);
		if(tmp != NULL && (tmp->SbType = FUNCDEC || tmp->SbType == FUNCDEF))
		{
			FuncTab_t *func = tmp->SubTab;
			if(func->hasDef ==0)
			{
				printf( "Error type 2 at line %d: undefined function \"%s\".\n", func->lineo,func->name);
			}
		}
	}
	return gTab;
}

SbTab_t *ExtDefList(SbTab_t **tab, TabTyp_t tabtyp,node_t *node)
{
	if(haschildren(node)==-1)
		return 0;
	ExtDef(tab,tabtyp,node->child->ptr[0]);
	ExtDefList(tab,tabtyp,node->child->ptr[1]);
	return *tab;
}

int setFunParent(SbTab_t *parent,FuncTab_t **fun)
{
	if((*fun)!=NULL)
	{
		(*fun)->parent=parent;
		if((*fun)->retType!=NULL)
			(*fun)->retType->parent=parent;
		if((*fun)->paraList!=NULL)
			(*fun)->paraList->parent=parent;
		if((*fun)->FuncDef!=NULL)
			(*fun)->FuncDef->parent=parent;
	}
	return 0;
}

SbTab_t *ExtDef(SbTab_t **tab, TabTyp_t tabtyp,node_t *node)
{
	int children=0;
	if(children=haschildren(node)==-1)
		return NULL;
	node_t **childs = node->child->ptr;
	SbTab_t *type=Specifier(tab,tabtyp,childs[0]);
	node_t *child=childs[1];
	if(strcmp(child->name,"ExtDecList")==0)
	{
		//ExtDef: Specifier ExtDecList SEMI
		ExtDecList(tab,tabtyp,child,type);
	}
	else if(strcmp(child->name,"SEMI")==0)
	{
		//ExtDef: Specifier SEMI
		return 0;
	}
	else //函数定义 或声明
	{
		if(strcmp(childs[2]->name,"SEMI")==0) //dec
		{
			FuncTab_t *fun=FunDec(child,type);
			SbTab_t *sb;
			sb = FindSymbol(tab,fun->name,sb);
			if(sb==NULL)
			{
				sb=AllocSymbol(fun->name,FUNCDEC);
				sb->SubTab=fun;
				InsertSymbol(tab,sb);
				//SbTab_t **fundef = (SbTab_t**)malloc(sizeof(SbTab_t*));
				//*fundef=AllocSbTab();
				//*fundef = CompSt(sb,fundef,FUNDEF_TAB, node->child->ptr[2],fun->retType);
				//fun->FuncDef=(*fundef);
				fun->hasDef = 0;
				fun->lineo = child->lineno;
				fun->parent=sb;
				setFunParent(sb,&fun);
				//存储函数名，用于后面检查函数是否定义
				SbTab_t *funcNameTab = (*tab)->hh.next;
				funcNameTab =(SbTab_t*) funcNameTab->SubTab;
				SbTab_t *funcNameSy = AllocSymbol(fun->name,FUNCNAME);
				InsertSymbol(&funcNameTab,funcNameSy);

			}
			else if(sb->SbType==FUNCDEF)
			{
				SbTab_t **list1 = &(fun->paraList);
				FuncTab_t *fun2 = (FuncTab_t *)(sb->SubTab);
				SbTab_t **list2 = &(fun2->paraList);
				if(FunParasCmp(list1,list2)!=0)
				{
					printf( "Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", child->lineno,fun->name);
					return NULL;
				}
			}
			else if(sb->SbType == FUNCDEC)
			{
				SbTab_t **list1 = &(fun->paraList);
				FuncTab_t *fun2 = (FuncTab_t *)(sb->SubTab);
				SbTab_t **list2 = &(fun2->paraList);
				if(FunParasCmp(list1,list2)!=0)
				{
					printf( "Error type 19 at line %d: Inconsistent declaration of function \"%s\".\n", child->lineno,fun->name);
					return NULL;
				}
			}
			else if(sb->SbType==FUNCCALL)
			{
				printf( "Error type 2 at line %d: undefined function \"%s\".\n", child->lineno,fun->name);
				return NULL;
			}
		}
		else if(strcmp(childs[2]->name,"CompSt")==0) //def
		{//ExtDef: Specifier FunDec CompSt
			FuncTab_t *fun=FunDec(child,type);
			SbTab_t *sb;
			sb = FindSymbol(tab,fun->name,sb);
			if(sb==NULL)
			{
				sb=AllocSymbol(fun->name,FUNCDEF);
				sb->SubTab=fun;
				InsertSymbol(tab,sb);
				
				SbTab_t *fundef=AllocSbTab(FuncDefTabHeader);
				fundef->parent = sb;
				fundef = CompSt(&fundef,FUNDEF_TAB, node->child->ptr[2],fun->retType);
				fun->FuncDef=(fundef);
				fun->hasDef = 1;
				fun->parent=sb;
				fun->lineo = child->lineno;
				setFunParent(sb,&fun);
			}
			else if(sb->SbType==FUNCDEF)
			{
				printf( "Error type 4 at line %d: Redefined function \"%s\".\n", child->lineno,fun->name);
				return NULL;
			}
			else if(sb->SbType==FUNCCALL)
			{
				printf( "Error type 2 at line %d: undefined function \"%s\".\n", child->lineno,fun->name);
				return NULL;
			}
			else if(sb->SbType == FUNCDEC) //前面函数声明 对应的定义
			{
				// sb=AllocSymbol(fun->name,FUNCDEF);
				// sb->SubTab=fun;
				// InsertSymbol(tab,sb);
				sb->SbType = FUNCDEF;
				free(fun);
				fun = sb->SubTab;
				SbTab_t *fundef=AllocSbTab(FuncDefTabHeader);
				fundef->parent=sb;
				fundef = CompSt(&fundef,FUNDEF_TAB, node->child->ptr[2],fun->retType);
				fun->FuncDef=(fundef);
				fun->hasDef = 1;
				fun->parent=sb;
				setFunParent(sb,&fun);
			}
		}
	}
	return 0;

}

//
SbTab_t *ExtDecList(SbTab_t **tab, TabTyp_t tabtyp,node_t *node, SbTab_t *type)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;

	//ExtDecList:VarDec 
	SbTab_t *vardec=VarDec(tab, tabtyp,node->child->ptr[0],type);
	//不对符号表处理，交到VarDec去处理
	if(children>1)
	{
		//ExtDecList:VarDec COMMA ExtDecList
		ExtDecList(tab,tabtyp,node->child->ptr[2],type);
	}
	return *tab;
}

//返回一个符号，并且该符号已经插入到tab里面
SbTab_t *Specifier(SbTab_t **tab,TabTyp_t tabtyp,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	SbTab_t *type;
	node_t *child=node->child->ptr[0];
	if(strcmp(child->name,"TYPE")==0)
	{
		//int or float
		if(strcmp(child->ID_type,"int")==0)
		{
			//int
			type=AllocSymbol("int",INT_T);
		}
		else if(strcmp(child->ID_type,"float")==0)
		{
			type=AllocSymbol("float",FLOAT_T);
		}
	}
	else
	{
		type=StructSpecifier(tab,tabtyp,child);
	}
	return type;
}

//返回一个符号，并且在有名字的结构体的情况下，该符号已经插入到tab里面
SbTab_t *StructSpecifier(SbTab_t **tab, TabTyp_t tabtyp,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	node_t *child=node->child->ptr[1];
	SbTab_t *ret=NULL;
	if(strcmp(child->name,"OptTag")==0)
	{
		//StructSpecifier : STRUCT_T OptTag LC DefList RC
		if(haschildren(child)!=-1)//有名字的结构体
		{
			char *StName;
			//strcpy(StName,child->child->ptr[0]->ID_type);
			StName = child->child->ptr[0]->ID_type;
			SbTab_t *tmp=NULL;
			tmp = FindSymbol(tab,StName,tmp);
			if(tmp==NULL)
			{
				SbTab_t *StSy=AllocSymbol(StName,STRUCT_T); //往tab里插入
				InsertSymbol(tab,StSy);
				//SbTab_t **StTab = (SbTab_t **)malloc(sizeof(SbTab_t *));
				SbTab_t *StTab=AllocSbTab(StructFieldTabHeader); //结构体域符号表
				StTab->parent = StSy;
				DefList(&StTab,STRUCTFILED,node->child->ptr[3]);
				StSy->SubTab=StTab;
				ret=StSy;
			}
			else	//名字重复
			{
				switch(tabtyp)
				{
					case BASIC_TAB: printf("Error type 16 at line %d: Duplicate name \"%s\".\n",child->child->ptr[0]->lineno,StName);break;
					case STRUCTFILED: printf("Error type 15 at line %d: redefined field \"%s\".\n",child->child->ptr[0]->lineno,StName);break;
					default:break;
				}
			}
		}
		else //没有名字的结构体,不需要插入tab表
		{
			SbTab_t *StSy=AllocSymbol("StName",STRUCT_T); 
			//SbTab_t **StTab = (SbTab_t**)malloc(sizeof(SbTab_t *));
			SbTab_t *StTab=AllocSbTab(StructFieldTabHeader); //结构体域符号表
			StTab->parent = StSy;
			DefList(&StTab,STRUCTFILED,node->child->ptr[3]);
			StSy->SubTab=StTab;
			ret=StSy;
		}
	}
	else 
	{
		//StructSpecifier : STRUCT_T Tag
		char *StName;
//		strcpy(StName,child->child->ptr[0]->ID_type);
		StName=child->child->ptr[0]->ID_type;
		SbTab_t *tmp=NULL;
		tmp = LookUpSymbol(tab,StName,tmp);
		if(tmp==NULL)	//结构体未定义
		{
			printf("Error type 17 at line %d: Undefined structure \"%s\".\n",child->lineno,StName);	
			return NULL;
		}

		ret=tmp;
	}
	return ret;
}

//数组类型处理
arr_t *ArrayDec(SbTab_t **sy,arr_t *pre,node_t *node,SbType_t t)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	if(children==1) //递归基
	{
		// VarDec ： ID
		(*sy)=AllocSymbol(node->child->ptr[0]->ID_type,ARRAY);
		(*sy)->SubTab=pre;
	}
	else
	{
		arr_t *a=(arr_t*)malloc(sizeof(arr_t));
		memset(a,0,sizeof(arr_t));
		a->size=node->child->ptr[2]->I_type;
		a->type=t;
		a->array=pre;
		ArrayDec(sy,a,node->child->ptr[0],ARRAY);
	}
}

//返回一个符号
SbTab_t *VarDec(SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *type)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	SbTab_t *ret;
	if(children==1)
	{
		//VarDec : ID
		SbTab_t *tmp = NULL;
		tmp = FindSymbol(tab,node->child->ptr[0]->ID_type,tmp);
		if(tmp == NULL)
		{
			if(strcmp((*tab)->name,FuncDefTabHeader)==0) //函数里面的变量不能与列表的同名
			{
				FuncTab_t *funTab = (FuncTab_t *)((*tab)->parent->SubTab);
				if(funTab->paraList != NULL)
					tmp = FindSymbol(&(funTab->paraList),ret->name,tmp);
			}
		}
		if(tmp != NULL)
		{
			switch(tabtyp)
			{
				case STRUCTFILED: printf("Error type 15 at Line %d: Redefined variable \"%s\".\n",node->child->ptr[0]->lineno,node->child->ptr[0]->ID_type);break;
				case BASIC_TAB:printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->child->ptr[0]->lineno,node->child->ptr[0]->ID_type);break;
				case FUNDEF_TAB:printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->child->ptr[0]->lineno,node->child->ptr[0]->ID_type);break;
				default:break;
			}
			
			return NULL;
		}
		if(type==NULL)
			return NULL;
		ret=AllocSymbol(node->child->ptr[0]->ID_type,type->SbType);
		if(type->SbType==STRUCT_T)
		{
			//SbTab_t *ptmp = (SbTab_t *)type->SubTab;
			//ptmp->refcount += 1;
			ret->SubTab=type->SubTab;
			ret->parent=type->parent;
		}
		//插入符号表
		InsertSymbol(tab,ret);
	}
	else //数组类型
	{
		ArrayDec(&ret,NULL,node,type->SbType);
		if(type->SbType==STRUCT_T)
		{
			//SbTab_t *ptmp = (SbTab_t *)type->SubTab;
			//ptmp->refcount += 1;
			ret->SubTab=type->SubTab;
			ret->parent=type->parent;
		}
		SbTab_t *tmp = NULL;
		tmp = FindSymbol(tab,ret->name,tmp);
		if( tmp == NULL)
		{
			if(strcmp((*tab)->name,FuncDefTabHeader)==0) //函数里面的变量不能与列表的同名
			{
				FuncTab_t *funTab = (FuncTab_t *)((*tab)->parent->SubTab);
				if(funTab->paraList != NULL)
					tmp = FindSymbol(&(funTab->paraList),ret->name,tmp);
			}
		}
		if(tmp != NULL)
		{
			switch(tabtyp)
			{
				case STRUCTFILED: printf("Error type 15 at Line %d: Redefined variable \"%s\".\n",node->child->ptr[0]->lineno,node->child->ptr[0]->ID_type);break;
				case BASIC_TAB:printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->child->ptr[0]->lineno,node->child->ptr[0]->ID_type);break;
				case FUNDEF_TAB:printf("Error type 3 at Line %d: Redefined variable \"%s\".\n",node->child->ptr[0]->lineno,node->child->ptr[0]->ID_type);break;
				default:break;
			}
			return NULL;
		}
		
		InsertSymbol(tab,ret);	
	}
	return ret;
}

//返回函数的声明体，未包含定义部分
FuncTab_t *FunDec( node_t *node, SbTab_t *retT)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	FuncTab_t *ret;
	ret=(FuncTab_t*)malloc(sizeof(FuncTab_t));
	memset(ret,0,sizeof(FuncTab_t));
	ret->retType=retT;
	char*t;
    t=(char*)malloc(sizeof(char )*(strlen(node->child->ptr[0]->ID_type)+1));
	strcpy(t,node->child->ptr[0]->ID_type);
	ret->name=t;
	//ret->name=node->child->ptr[0]->ID_type;
	if(children>3)
	{ //FunDec : ID LP VarList RP
		//SbTab_t **ParaListTab=(SbTab_t **)malloc(sizeof(SbTab_t *));
		SbTab_t *ParaListTab=AllocSbTab(ParaListTabHeader);
		ret->paraList=VarList(&ParaListTab,FUNPARA_TAB, node->child->ptr[2]);
		//父表指针已经在上层处理
//		if(ret->paraList!=NULL)
//			ret->paraList->parent=ret;
	}
	else
	{ //FuncDec : ID LP RP
		ret->paraList=NULL;
	}
	return ret;
}

//返回一个参数列表
SbTab_t *VarList(SbTab_t **tab, TabTyp_t tabtyp, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	SbTab_t *Para, *tmp=NULL;
	Para=ParamDec(tab,tabtyp,node->child->ptr[0]);
	if(children>1)
	{
		VarList(tab,tabtyp,node->child->ptr[2]);
	}
	return *tab;
}

//返回一个符号
SbTab_t *ParamDec(SbTab_t **tab, TabTyp_t tabtyp,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	SbTab_t *type=Specifier(tab,tabtyp,node->child->ptr[0]);
	SbTab_t *ret=VarDec(tab,tabtyp, node->child->ptr[1],type);
	return ret;
}

//返回一个（子）符号表,也即传进来的tab
SbTab_t *CompSt(SbTab_t **tab, TabTyp_t tabtyp,node_t *node,SbTab_t *retT)
{
	int children=haschildren(node);
	if(children==-1)
	{
//		*tab = NULL;
		return NULL;
	}
	node_t** childs = node->child->ptr;

	DefList(tab,tabtyp,childs[1]);
	StmtList(tab,tabtyp,childs[2],retT);
	return *tab;
}

//
SbTab_t * StmtList(SbTab_t **tab, TabTyp_t tabtyp, node_t *node,SbTab_t *retT)
{
	int children=haschildren(node);
	if(children==-1)
		return 0;
	Stmt(tab,tabtyp,node->child->ptr[0],retT);
	StmtList(tab,tabtyp,node->child->ptr[1],retT);
	return *tab;
}

//
SbTab_t *Stmt(SbTab_t **tab, TabTyp_t tabtyp, node_t *node,SbTab_t *retT)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs=node->child->ptr;
	if(strcmp(childs[0]->name,"Exp")==0)
	{//Stmt: Exp SEMI
		Exp(tab,tabtyp,childs[0]);
	}
	else if(strcmp(childs[0]->name,"CompSt")==0)
	{//Stmt: CompSt
		//需要新建一个子表，并把它放到tab
		//子表的名字应该如何处理？
		//目前采用计数器的办法。SubTabCNT为一个全局变量
		char SubTabName[35];
		char tmp[21];
		strcpy(tmp,"____SubCompStTab____");
		char tmpi[10];
//		itoa(SubTabCNT,tmpi,10);
		sprintf(tmpi,"%d",SubTabCNT);
		strcat(tmp,tmpi);
		strcpy(SubTabName,tmp);
		SbTab_t *Sy=AllocSymbol(SubTabName,SUBTAB);
		//SbTab_t **SubTab = (SbTab_t **)malloc(sizeof(SbTab_t *));
		SbTab_t *SubTab=AllocSbTab(SubTabHeader);
		Sy->SubTab = SubTab;
		SubTab->parent = Sy;
		InsertSymbol(tab,Sy);
		Sy->SubTab = CompSt(&SubTab,SUBBASIC_TAB,childs[0],retT);
		// if(SubTab!=NULL)
		// 	(SubTab)->parent=Sy;
		SubTabCNT=SubTabCNT+1;
	}
	else if(strcmp(childs[0]->name,"RETURN")==0)
	{//Stmt: RETURN Exp SEMI
		SbTab_t *ret=Exp(tab,tabtyp,childs[1]);
		if(retT==NULL && ret==NULL)
			return 0;
		else if(retT==NULL || ret==NULL || typeCmp(retT,ret)!=0)
		{
			printf( "Error type 8 at line %d: Type mismatched for return.\n", childs[1]->lineno );
		}
	}
	else if(strcmp(childs[0]->name,"WHILE")==0)
	{//Stmt: WHILE LP Exp RP Stmt
		SbTab_t *whileExp = Exp(tab,tabtyp,childs[2]);
		Stmt(tab,tabtyp,childs[4],retT);
	}
	else
	{//Stmt: IF LP Exp RP Stmt (ELSE Stmt)
		SbTab_t *ifExp =Exp(tab,tabtyp,childs[2]);
		Stmt(tab,tabtyp,childs[4],retT);
		if(children>5)
		{
			Stmt(tab,tabtyp,childs[6],retT);
		}
	}
	return *tab;
}

SbTab_t* DefList(SbTab_t **tab,TabTyp_t tabtyp, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return 0;
	//DefList: Def DefList
	node_t **childs = node->child->ptr;
//	if(childs==NULL)
//		return 0;
	Def(tab,tabtyp,childs[0]);
	DefList(tab,tabtyp,childs[1]);
	return *tab;
}

//把tab返回
SbTab_t *Def(SbTab_t **tab,TabTyp_t tabtyp,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	//Def: Specifier DecList SEMI
	node_t **childs=node->child->ptr;
//	if(childs==NULL)
//		return NULL;
	SbTab_t *type=Specifier(tab,tabtyp,childs[0]);
	DecList(tab,tabtyp,childs[1],type);
	return *tab;
}

//把tab返回
SbTab_t *DecList(SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *type)
{
	int children =haschildren(node);
	if(children==-1)
		return NULL;
	//DecList: Dec (COMMA DecList)
	node_t **childs=node->child->ptr;
	Dec(tab,tabtyp,childs[0],type);
	if(children>2)
	{
		DecList(tab,tabtyp,childs[2],type);
	}
	return *tab;
}

SbTab_t *Dec(SbTab_t **tab,TabTyp_t tabtyp,node_t *node, SbTab_t *type)
{
	int children = haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs=node->child->ptr;
	//Dec: VarDec (ASSIGNOP Exp)
	SbTab_t *vardec=VarDec(tab,tabtyp,childs[0],type);
	if(vardec != NULL)
	{
		// SbTab_t *tmp=NULL;
		// tmp = FindSymbol(tab,vardec->name,tmp);
		// if(tmp==NULL)
		// {
			if(children>2)
			{//Dec: VarDec ASSIGNOP Exp
				if(tabtyp==STRUCTFILED)
				{
					printf( "Error type 15 at line %d: variable can not be initialed in struct field \"%s\".\n", childs[0]->lineno, vardec->name);
					return NULL;
				}
				SbTab_t *rval=Exp(tab,tabtyp,childs[2]);
				if(vardec != NULL && rval != NULL && typeCmp(vardec,rval)!=0)
				{
					printf( "Error type 5 at line %d: Type mismatched for assignment.\n", childs[0]->lineno);
					return NULL;
				}
            }
//			InsertSymbol(tab,vardec);
//		}
/*		else //变量名重复
		{
			printf("Error type 3 at line %d: redefined variable \"%s\".\n",childs[0]->lineno,vardec->name);		
		}*/
		return *tab;
	}

}

SbTab_t *Exp(SbTab_t **tab,TabTyp_t tabtyp,node_t *node)
{
	int children = haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs = node->child->ptr;
	if(strcmp(childs[0]->name,"Exp")==0)
	{//Exp : Exp xxx
		if(children==3)
		{
			SbTab_t *lvar;
			SbTab_t* rval;
			if(strcmp(childs[1]->name,"ASSIGNOP")==0)
			{//Exp: Exp ASSIGNOP Exp
				// 以下判断左表达式是否合法
				// 左表达式只可能是普通变量、数组访问表达式、结构体访问表达式
				
				int grandchildren=haschildren(childs[0]);
				if(grandchildren==-1)
				{
					printf( "Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", childs[0]->lineno );
					return NULL;
				}
				node_t **grandchilds=childs[0]->child->ptr;
				if(strcmp(grandchilds[0]->name,"ID")==0)
				{//普通变量
					lvar= Exp(tab,tabtyp,childs[0]);
				}
				else if(strcmp(grandchilds[0]->name,"Exp")==0 && grandchildren>=2 && strcmp(grandchilds[1]->name,"LB")==0)
				{//数组访问表达式
					lvar = Exp(tab,tabtyp,childs[0]);
				}
				else if( strcmp(grandchilds[0]->name,"Exp")==0 && grandchildren>=2 && strcmp(grandchilds[1]->name,"DOT")==0 ){
					//结构体访问表达式
					lvar = Exp(tab,tabtyp,childs[0]);
				}
				else{
					printf( "Error type 6 at line %d: The left-hand side of an assignment must be a variable.\n", childs[0]->lineno );
				}
				//判断左右是否类型匹配
				rval = Exp(tab,tabtyp,childs[2]);
				if(lvar == NULL || rval == NULL)
					return NULL;
				if(typeCmp(lvar,rval)==0)
					return lvar;
				else{
					printf( "Error type 5 at line %d: Type mismatched for assignment.\n", childs[0]->lineno );
					return NULL;
				}			
			}
			else if( strcmp(childs[1]->name,"RELOP")==0 || strcmp(childs[1]->name,"LAND")==0 || strcmp(childs[1]->name,"LOR")==0 || strcmp(childs[1]->name,"ADD")==0 || strcmp(childs[1]->name,"SUB")==0 || strcmp(childs[1]->name,"MUL")==0 || strcmp(childs[1]->name,"DIV")==0 )
				// Exp: Exp AND Exp
				// | Exp OR Exp
				// | Exp RELOP Exp
				// | Exp PLUS Exp
				// | Exp MINUS Exp
				// | Exp STAR Exp
				// | Exp DIV Exp
			{
				lvar = Exp( tab,tabtyp,childs[0]);
				rval = Exp( tab, tabtyp,childs[2]);
				if( lvar==NULL || rval==NULL )
					return NULL;
				//if(lvar->SbType==rval->SbType && (lvar->SbType == FLOAT_T || lvar->SbType == INT_T))
				if(typeCmp(lvar,rval)==0 && (lvar->SbType == FLOAT_T || lvar->SbType == INT_T))
					return lvar;
				else{
					printf( "Error type 7 at line %d: Type mismatched for operands.\n", childs[0]->lineno );
					return NULL;
				}
			}
			else if( strcmp(childs[1]->name,"DOT")==0 )
			{// Exp: Exp DOT ID
				lvar = Exp( tab,tabtyp, childs[0] );
				if(lvar==NULL )
					return NULL;
				if( lvar->SbType !=STRUCT_T )
				{
					printf( "Error type 13 at line %d: Illegal use of \".\".\n", childs[0]->lineno );
					return NULL;
				}
				SbTab_t **strucField = (SbTab_t**)malloc(sizeof(SbTab_t *));
				*strucField = (SbTab_t*) lvar->SubTab;
				SbTab_t *tmp=NULL;
				tmp = FindSymbol(strucField,childs[2]->ID_type,tmp);
				if(tmp==NULL)
				{
					printf( "Error type 14 at line %d: Non-existent field \"%s\".\n", childs[0]->lineno,childs[2]->ID_type );
					return NULL;
				}
				return tmp; //
			}
		}
		else if(children == 4)
		{// Exp: Exp LB Exp RB
			SbTab_t *arrSy, *rval;
			arrSy = Exp(tab,tabtyp,childs[0]);
			if(arrSy == NULL)
				return NULL;
			if(arrSy->SbType != ARRAY)
			{
				printf( "Error type 10 at line %d: \"%s\" is not an array.\n", childs[0]->lineno, arrSy->name );
				return NULL;
			}
			rval = Exp(tab,tabtyp,childs[2]);
			if(rval == NULL)
				return NULL;
			if(rval->SbType != INT_T)
			{
				printf( "Error type 12 at line %d: \"%s\" is not an integer.\n", childs[2]->lineno , rval->name );
				return NULL;
			}
			return arrSy;
		}
	}
	else if(strcmp(childs[0]->name,"ID")==0)
	{
		SbTab_t *Sy = NULL;
		Sy = LookUpSymbol(tab,childs[0]->ID_type,Sy);
		if(children==1)
		{//Exp : ID
			if(Sy==NULL)
			{
				// if(PTab->SbType == FUNCDEF)
				// {
				// 	FuncTab_t *fundef = (FuncTab_t *)(PTab->SubTab);
				// 	SbTab_t ** ParaTab = &(fundef->paraList);
				// 	Sy = FindSymbol(ParaTab,childs[0]->ID_type,Sy);
					if(Sy == NULL)
					{
						printf( "Error type 1 at line %d: Undefined variable\"%s\".\n",childs[0]->lineno,childs[0]->ID_type);	
						return NULL;
					}
				//}
			}
			return Sy;
		}
		else 
		{// Exp: ID LP (Args) RP
			if( Sy == NULL)
			{
				// if(PTab->SbType == FUNCDEF)  //函数定义刚进来
				// {
				// 	Sy = LookUpSymbol(&childs[0]->ID_type,Sy);
					if(Sy == NULL)
					{
						printf( "Error type 2 at line %d: Undefined function\"%s\".\n",childs[0]->lineno,childs[0]->ID_type);	
						return NULL;
					}
				//}
			}
			if(Sy!=NULL)
			{
				if(Sy->SbType != FUNCDEF && Sy->SbType != FUNCDEC)
				{
					printf( "Error type 11 at line %d: \"%s\" is not a function.\n", childs[0]->lineno, childs[0]->ID_type);
					return NULL;
				}
				FuncTab_t *func = (FuncTab_t*)Sy->SubTab;
				// if( func->hasDef==0)
				// {
				// 	printf( "Error type 2 at line %d: Undefined function \"%s\".\n", childs[0]->lineno, childs[0]->ID_type );
				// 	return NULL;
				// }
				SbTab_t *paralist = func->paraList;
				if(strcmp(childs[2]->name,"RP")==0)
				{
					if(paralist!=NULL)
					{
						printf( "Error type 9 at line %d: Function \"%s(", childs[2]->lineno, func->name );
						printParaTypList( paralist );
						printf( ")\" is not applicable for arguments \"()\".\n" );
					}
				}
				else 
				{
					if(Args(tab,tabtyp,childs[2],paralist)!=0)
					{
						printf( "Error type 9 at line %d: Function \"%s(", childs[2]->lineno, func->name );
						printParaTypList( paralist );
						printf( ")\" is not applicable for arguments \"(");
						printArgs(tab,tabtyp,childs[2]);
						printf( ")\".\n" );
					}
				}
				return func->retType;
			}
			if(Sy==NULL)
			{
				printf( "Error type 2 at line %d: Undefined function \"%s\".\n", childs[0]->lineno, childs[0]->ID_type );
				return NULL;
			}

		}
	}
	else if(strcmp(childs[0]->name,"INT")==0)
	{//Exp : INT
		SbTab_t *rval = AllocSymbol("INT",INT_T);
		return rval;
	}
	else if(strcmp(childs[0]->name,"FLOAT")==0)
	{
		SbTab_t *rval = AllocSymbol("FLOAT",FLOAT_T);
		return rval;
	}
	else if(strcmp(childs[0]->name,"LNOT")==0)
	{
		
	}
	else if(strcmp(childs[0]->name,"SUB")==0)
	{

	}
	return NULL;
}

//???
int Args(SbTab_t **tab, TabTyp_t tabtyp, node_t *node, SbTab_t *paralist)
{
	if( node==NULL && paralist==NULL )
		return 0;
	else if( node==NULL || paralist==NULL )
		return -1;
	int children = haschildren(node);
	if(children == -1)
		return -1;
	node_t **childs = node->child->ptr;
	SbTab_t *para = paralist;
	para =(SbTab_t *) para->hh.next;

	SbTab_t *lvar = Exp(tab,tabtyp,childs[0]);
	if(lvar == NULL)
		return -1;
	if(typeCmp(lvar,para)!=0)
		return -1;
	if(children == 3)
		return Args(tab,tabtyp,childs[2], para->hh.next);

	return 0;
}


//辅助函数
//所有比较性质的函数，当两者相同时，返回0，不同时返回-1
int typeCmp(SbTab_t *a, SbTab_t *b)
{
	if(a==NULL && b == NULL)
		return 0;
	if(a==NULL && b!=NULL)
		return -1;
	if(a!=NULL && b==NULL)
		return -1;
	if(a!=NULL && b!=NULL)
	{
		if(a->SbType!=b->SbType)
			return -1;
		switch(a->SbType)
		{
			case ARRAY:return arraytypeCmp((arr_t*)a->SubTab,(arr_t*)b->SubTab);break;
			case STRUCT_T:return StTypeCmp((SbTab_t**)(&(a->SubTab)),(SbTab_t **)(&(b->SubTab)));break;
			default:break;
		}
	}
	return 0;
}

int arraytypeCmp(arr_t *a, arr_t*b)
{
	if(a==NULL && b == NULL)
		return 0;
	if(a==NULL && b!=NULL)
		return -1;
	if(a!=NULL && b==NULL)
		return -1;
	if(a!=NULL && b!=NULL)
	{
		if(a->type!=b->type)
			return -1;
		if(a->type==b->type)
			if(a->array==NULL && b->array==NULL)
				return 0;
			return arraytypeCmp(a->array,b->array);
	}	
}

int StTypeCmp(SbTab_t **a,SbTab_t **b)
{
	if(a==NULL && b==NULL)
		return 0;
	if(*a==NULL && *b==NULL)
		return 0;
	if(a!=NULL && b!=NULL)
		if(*a!=NULL && *b!=NULL)
		{
			if(GetSbTabSize(a)!=GetSbTabSize(b))
				return -1;
			SbTab_t *s;
			SbTab_t *btmp;
			for(s=*a,btmp=*b;s!=NULL&&btmp!=NULL;s=s->hh.next,btmp=btmp->hh.next)
			{
//				btmp = FindSymbol(b,s->name,btmp);
				if(typeCmp(s,btmp)!=0)
					return -1;
			}
			return 0;
		}
	return -1;
}

int FunParasCmp(SbTab_t **a,SbTab_t **b)
{
	if(a==NULL && b==NULL)
		return 0;
	if(*a==NULL && *b==NULL)
		return 0;
	if(a!=NULL && b!=NULL)
		if(*a != NULL && *b !=NULL)
		{
			if(GetSbTabSize(a)!=GetSbTabSize(b))
				return -1;
			SbTab_t *s;
			SbTab_t *btmp;
			for(s=(*a)->hh.next,btmp=(*b)->hh.next;s!=NULL&&btmp!=NULL;s=s->hh.next,btmp=btmp->hh.next)
			{
//				btmp = FindSymbol(b,s->name,btmp);
				if(typeCmp(s,btmp)!=0)
					return -1;
			}
			return 0;				
		}
	return -1;
}

SbTab_t *LookUpSymbol(SbTab_t **tab,char *name, SbTab_t *result)
{
	SbTab_t *ret=NULL;
	while(ret==NULL && tab!=NULL && *tab!=NULL)
	{
		ret = FindSymbol(tab,name,result);
		
		if((*tab)->parent !=NULL && ret ==NULL)
		{
			tab=&((*tab)->parent);
			if(ret == NULL && tab !=NULL )
				if((*tab)->SbType == FUNCDEF)
				{
					FuncTab_t *fundef = (FuncTab_t *)((*tab)->SubTab);
					if(fundef !=NULL)
					{
						SbTab_t ** ParaTab = &(fundef->paraList);
						ret = FindSymbol(ParaTab,name,result);
					}
				}
		}
		else if(((*tab)->hh.prev)!=NULL)
		{
			while (((*tab)->hh.prev)!=NULL)
			{
				tab = (SbTab_t **)(&((*tab)->hh.prev));
			}
		}
		else break;
	}
	return ret;
}


int printParaTypList(SbTab_t *paralist)
{
	SbTab_t *para = paralist->hh.next;
	for(;para!=NULL;para=(SbTab_t *)(para->hh.next))
	{
		switch(para->SbType)
		{
			case INT_T:printf("int");break;
			case FLOAT_T: printf("float");break;
			case ARRAY: printf(" ");break;
			case STRUCT_T:printf(" ");break;
			default:break;
		}
		if(para->hh.next != NULL)
			printf(" ,");
	}
	return 0;
}

int printArgs(SbTab_t **tab,TabTyp_t tabtyp, node_t *node)
{
	int children = haschildren(node);
	if(children == -1)
		return -1;
	node_t **childs = node->child->ptr;

	SbTab_t *rval = Exp(tab,tabtyp,childs[0]);
	printf("%s",rval->name);
	if(children==3)
	{
		printf(", ");
		printArgs(tab,tabtyp,childs[2]);
	}
	return 0;
}
