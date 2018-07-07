/*************************************************************************
	> File Name: IntermedCode.c
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Fri 08 Jun 2018 09:24:15 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#include<stdio.h>
#include "IntermedCode.h"
#include <memory.h>
#include <assert.h>

/*****************************************************************
 * translation about
 * ***************************************************************/

IRCodeList_t *trProgram(SbTab_t **tab, Sytree_t *sytree)
{
	if(sytree ==NULL)
		return NULL;
	if(haschildren(sytree->root)==-1)
		return NULL;
	IRCodeList_t *CodeList = AllocaInterCodeList();
	trExtDefList(tab,CodeList,sytree->root->child->ptr[0]);
	return CodeList;
}

SbTab_t *trExtDefList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	if(haschildren(node)==-1)
		return NULL;
	trExtDef(tab,codelist,node->child->ptr[0]);
	trExtDefList(tab,codelist,node->child->ptr[1]);
	return *tab;
}

SbTab_t *trExtDef(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	int children=0;
	if(children=haschildren(node)==-1)
		return NULL;
	node_t **childs = node->child->ptr;
	node_t *child=childs[1];
	if(strcmp(child->name,"ExtDecList")==0)
	{
		//ExtDef: Specifier ExtDecList SEMI
		trExtDecList(tab,codelist,child);
	}
	else if(strcmp(child->name,"SEMI")==0)
	{
		//ExtDef: Specifier SEMI
		return 0;
	}
	else //函数定义 或 声明
	{
		if(strcmp(childs[2]->name,"SEMI")==0) //dec
		{
			return 0;
		}
		else if(strcmp(childs[2]->name,"CompSt")==0) //def
		{//ExtDef: Specifier FunDec CompSt
			FuncTab_t *func = trFunDec(tab,codelist,childs[1]);
			Operand_t *funcOp = AllocaOperand(codelist, FUNCTION_OP);
			// funcOp->u.value = func->name;
			SetOperandValue(funcOp,func->name);
			InterCode_t *funCode = AllocaInterCode(FUNC_CODE);
			funCode->u.singleop.op = funcOp;
			appendIRCode(codelist,funCode);
			SbTab_t *paralist = func->paraList;
			for(;paralist!=NULL;paralist=paralist->hh.next)
			{
				if(paralist->SbType==TABHEADER)
					continue;
				Operand_t *para = AllocaOperand(codelist, VARDEC_OP);
				// para->u.value = paralist->name;
				// SetOperandValue(para,paralist->name);
				if(paralist->varNo == -1)
					paralist->varNo = para->u.no;
				else 
					printf("re-decalerate parameter %d : &%d \n",paralist->varNo,para->u.no);
				
				InterCode_t *paraCode = AllocaInterCode(PARA_CODE);
				paraCode->u.singleop.op = para;
				appendIRCode(codelist,paraCode);
			}
			SbTab_t **fundeftab = &(func->FuncDef);
			if((*fundeftab)!=NULL)
				trCompSt(fundeftab,codelist,childs[2]);
		}
	}
	return *tab;
}

SbTab_t *trExtDecList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs = node->child->ptr;
	trVarDec(tab,codelist,childs[0]);
	if(children>1)
	{
		trExtDecList(tab,codelist,childs[2]);
	}
	return *tab;
}

SbTab_t *trSpecifier(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	return *tab;
}

SbTab_t *trStructSpecifier(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	return *tab;
}



SbTab_t *trVarDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs = node->child->ptr;
	SbTab_t *tmp = NULL;
	if(children==1)
	{// VarDec ： ID
		
		tmp = FindSymbol(tab,childs[0]->ID_type,tmp);
		if(tmp->SbType == STRUCT_T) //结构体
		{
			Operand_t *decOp = AllocaOperand(codelist,VARDEC_OP);
			// decOp->u.value = tmp->name;
			// SetOperandValue(decOp,tmp->name);
			if(tmp->varNo == -1)
				tmp->varNo = decOp->u.no;
			else 
				printf("re-decalerate variable %d : &%d \n",tmp->varNo,decOp->u.no);
			InterCode_t *decCode = AllocaInterCode(DEC_CODE);
			decCode->u.dec.op = decOp;
			SbTab_t **table = (SbTab_t**)&(tmp->SubTab);
			decCode->u.dec.size = StructFieldSize(table);
			appendIRCode(codelist,decCode);
			//由于虚拟机对函数参数和变量的处理不一致，这里需要对变量取地址。
			// Operand_t *leftOp = AllocaOperand(codelist,VARIABLE_OP);
			// leftOp->u.no = decOp->u.no;
			Operand_t *leftOp = AllocaOperand(codelist,VARDEC_OP);
			tmp->varNo = leftOp->u.no;
			Operand_t *rightOp = AllocaOperand(codelist,VARIABLE_OP);			
			rightOp->u.no = decOp->u.no;
			InterCode_t *addrCode = AllocaInterCode(ADDR_CODE);
			addrCode->u.assign.left = leftOp;
			addrCode->u.assign.right = rightOp;
			appendIRCode(codelist,addrCode);
		}
		else if(tmp->SbType == ARRAY)//数组
		{
			Operand_t *decOp = AllocaOperand(codelist, VARDEC_OP);
			// decOp->u.value = tmp->name;
			// SetOperandValue(decOp,tmp->name);
			if(tmp->varNo == -1)
				tmp->varNo = decOp->u.no;
			else 
				printf("re-decalerate variable %d : &%d \n",tmp->varNo,decOp->u.no);

			InterCode_t *decCode = AllocaInterCode(DEC_CODE);
			decCode->u.dec.op=decOp;
			decCode->u.dec.size = ArraySize(tmp);
			appendIRCode(codelist,decCode);

			//由于虚拟机对函数参数和变量的处理不一致，这里需要对变量取地址。
			// Operand_t *leftOp = AllocaOperand(codelist,VARIABLE_OP);
			// leftOp->u.no = decOp->u.no;
			Operand_t *leftOp = AllocaOperand(codelist,VARDEC_OP);
			tmp->varNo = leftOp->u.no;
			Operand_t *rightOp = AllocaOperand(codelist,VARIABLE_OP);			
			rightOp->u.no = decOp->u.no;
			InterCode_t *addrCode = AllocaInterCode(ADDR_CODE);
			addrCode->u.assign.left = leftOp;
			addrCode->u.assign.right = rightOp;
			appendIRCode(codelist,addrCode);
		}
		else	//普通变量，直接使用
		{
			tmp->varNo = codeVarNo(codelist);
		}
	}
	else
	{//VarDec : VarDec LB INT RB
		trVarDec(tab,codelist,childs[0]);
	}
	return tmp;
}

FuncTab_t *trFunDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	FuncTab_t *ret;
	node_t **childs = node->child->ptr;
	SbTab_t *tmp = NULL;
	tmp = FindSymbol(tab,childs[0]->ID_type,tmp);
	if(tmp == NULL)
		return NULL;
	if(tmp->SbType != FUNCDEC && tmp->SbType != FUNCDEF)
		return NULL;
	ret = (FuncTab_t *)tmp->SubTab;
	return ret;
}

SbTab_t *trVarList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	return *tab;
}

SbTab_t *trParamDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node)
{
	return *tab;
}

SbTab_t *trCompSt(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	node_t** childs = node->child->ptr;
	trDefList(tab,codelist,childs[1]);
	trStmtList(tab,codelist,childs[2]);
	return *tab;
}

SbTab_t *trStmtList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return 0;
	node_t **childs = node->child->ptr;
	trStmt(tab,codelist,childs[0]);
	trStmtList(tab,codelist,childs[1]);
	return *tab;
}

static int SubTabCNT=0;

SbTab_t *trStmt(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs=node->child->ptr;
	if(strcmp(childs[0]->name,"Exp")==0)
	{//Stmt: Exp SEMI
		trExp(tab,codelist,childs[0],NULL);
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
		sprintf(tmpi,"%d",SubTabCNT);
		strcat(tmp,tmpi);
		strcpy(SubTabName,tmp);
		SubTabCNT=SubTabCNT+1;
		SbTab_t *SubCompStTab = NULL;
		SubCompStTab = FindSymbol(tab,SubTabName,SubCompStTab);
		SubCompStTab =(SbTab_t *)(SubCompStTab->SubTab);
		trCompSt(&SubCompStTab,codelist,childs[0]);
	}
	else if(strcmp(childs[0]->name,"RETURN")==0)
	{//Stmt: RETURN Exp SEMI
		Operand_t *place = AllocaOperand(codelist,TEMPVAR_OP);
		// place->u.no = codeTempVarNo(codelist);
		trExp(tab,codelist,childs[1],place);
		InterCode_t *retCode = AllocaInterCode(RETURN_CODE);
		retCode->u.singleop.op = place;
		appendIRCode(codelist,retCode);
	}
	else if(strcmp(childs[0]->name,"WHILE")==0)
	{//Stmt: WHILE LP Exp RP Stmt
		//while loop code:
		// label
		// Exp
		// turelabel
		// Stmt
		// goto label
		// falselabel
		
		//alloca the labels
		Operand_t *label = AllocaOperand(codelist,LABEL_OP);
		// label->u.no = codeLabelNo(codelist);
		Operand_t *True = AllocaOperand(codelist,LABEL_OP);
		// True->u.no = codeLabelNo(codelist);
		Operand_t *False = AllocaOperand(codelist,LABEL_OP);
		// False->u.no = codeLabelNo(codelist);
		//code translation:
		//label
		InterCode_t *labelCode = AllocaInterCode(LABEL_CODE);
		labelCode->u.singleop.op = label;
		appendIRCode(codelist,labelCode);
		//Exp
		trBoolExp(tab,codelist,childs[2],True,False);
		//turelabel
		InterCode_t *TureCode = AllocaInterCode(LABEL_CODE);
		TureCode->u.singleop.op = True;
		appendIRCode(codelist,TureCode);
		//stmt
		trStmt(tab,codelist,childs[4]);
		//goto label
		InterCode_t *gotoLabel = AllocaInterCode(GOTO_CODE);
		gotoLabel->u.singleop.op = label;
		appendIRCode(codelist,gotoLabel);
		//false label
		InterCode_t *FalseCode = AllocaInterCode(LABEL_CODE);
		FalseCode->u.singleop.op = False;
		appendIRCode(codelist,FalseCode);

	}
	else
	{//Stmt: IF LP Exp RP Stmt (ELSE Stmt)
		//if stmt(if else stmt) code:
		// Exp
		// Truelabel
		// stmt
		// (goto label)
		// falselabel
		// (else stmt)
		// label

		//code translation:
		// Exp
		Operand_t *True = AllocaOperand(codelist,LABEL_OP);
		// True->u.no = codeLabelNo(codelist);
		Operand_t *False = AllocaOperand(codelist,LABEL_OP);
		// False->u.no = codeLabelNo(codelist);
		trBoolExp(tab,codelist,childs[2],True,False);
		//Truelabel
		InterCode_t *TrueCode = AllocaInterCode(LABEL_CODE);
		TrueCode->u.singleop.op=True;
		appendIRCode(codelist,TrueCode);
		//stmt
		trStmt(tab,codelist,childs[4]);
		if(children>5)
		{
			//goto label
			Operand_t *label =AllocaOperand(codelist, LABEL_OP);
			// label->u.no = codeLabelNo(codelist);
			InterCode_t *gotoLabel = AllocaInterCode(GOTO_CODE);
			gotoLabel->u.singleop.op = label;
			appendIRCode(codelist,gotoLabel);
			//falselabel
			InterCode_t *FalseCode = AllocaInterCode(LABEL_CODE);
			FalseCode->u.singleop.op = False;
			appendIRCode(codelist,FalseCode);
			// else stmt
			trStmt(tab,codelist,childs[6]);
			//label
			InterCode_t *labelCode = AllocaInterCode(LABEL_CODE);
			labelCode->u.singleop.op=label;
			appendIRCode(codelist,labelCode);
		}
		else
		{
			//falselabel
			InterCode_t *FalseCode = AllocaInterCode(LABEL_CODE);
			FalseCode->u.singleop.op = False;
			appendIRCode(codelist,FalseCode);
		}
	}
	return *tab;
}

SbTab_t *trDefList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return 0;
	//DefList: Def DefList
	node_t **childs = node->child->ptr;
	trDef(tab,codelist,childs[0]);
	trDefList(tab,codelist,childs[1]);
	return *tab;
}

SbTab_t *trDef(SbTab_t **tab, IRCodeList_t *codelist,node_t *node)
{
	int children=haschildren(node);
	if(children==-1)
		return NULL;
	//Def: Specifier DecList SEMI
	node_t **childs=node->child->ptr;
	trDecList(tab,codelist,childs[1]);
	return *tab;
}
SbTab_t *trDecList(SbTab_t **tab,IRCodeList_t *codelist,node_t *node)
{
	int children =haschildren(node);
	if(children==-1)
		return NULL;
	//DecList: Dec (COMMA DecList)
	node_t **childs=node->child->ptr;
	trDec(tab,codelist,childs[0]);
	if(children>2)
		trDecList(tab,codelist,childs[2]);
	return *tab;
}

SbTab_t *trDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node)
{
	int children = haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs=node->child->ptr;
	//Dec: VarDec (ASSIGNOP Exp)
	SbTab_t *lvar = trVarDec(tab,codelist,childs[0]);
	if(children>2)
	{
		Operand_t *place = AllocaOperand(codelist, VARIABLE_OP);
		// SetOperandValue(place,lvar->name);
		// char* placeValue = place->u.value;
		place->u.no = lvar->varNo;
		int varNo = place->u.no;
		SbTab_t *rval = trExp(tab,codelist,childs[2],place);
		if(place->kind!=VARIABLE_OP ||place->u.no != varNo)
		{
			//只有place被改 才说明需要生成赋值语句, 如果没有被更改，则说明在exp里面已经正确生成赋值语句
			//left->name := Exp()->name
			Operand_t *leftOp = AllocaOperand(codelist, VARIABLE_OP);
			leftOp->u.no = lvar->varNo;
			// leftOp->u.value = lvar->name;
			// SetOperandValue(leftOp,lvar->name);
			InterCode_t *assignCode = AllocaInterCode(ASSIGN_CODE);
			assignCode->u.assign.left = leftOp;
			assignCode->u.assign.right = place;
			appendIRCode(codelist,assignCode);
		}
	}
}

SbTab_t *trExp(SbTab_t **tab,IRCodeList_t *codelist,node_t *node,Operand_t *place)
{
	int children = haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs = node->child->ptr;
	if(strcmp(childs[0]->name,"Exp")==0)
	{//Exp : Exp xxx
		if(children==3)
		{
			SbTab_t *lvar = NULL;
			SbTab_t* rval= NULL;
			if(strcmp(childs[1]->name,"ASSIGNOP")==0) //赋值表达式
			{//Exp: Exp ASSIGNOP Exp
				Operand_t *leftOp = AllocaOperand(codelist, TEMPVAR_OP);
				// leftOp->u.no = codeTempVarNo(codelist);
				int leftOpNo = leftOp->u.no;
				
				int grandchildren=haschildren(childs[0]);
				if(grandchildren==-1)
				{
					return NULL;
				}
				node_t **grandchilds=childs[0]->child->ptr;
				if(strcmp(grandchilds[0]->name,"ID")==0)
				{//普通变量
					lvar = trExp(tab,codelist,childs[0],leftOp);
				}
				else if(strcmp(grandchilds[0]->name,"Exp")==0 && grandchildren>=2 && strcmp(grandchilds[1]->name,"LB")==0)
				{//数组访问表达式
					lvar = trExp(tab,codelist,childs[0],leftOp);
				}
				else if( strcmp(grandchilds[0]->name,"Exp")==0 && grandchildren>=2 && strcmp(grandchilds[1]->name,"DOT")==0 )
				{//结构体访问表达式
					lvar = trExp(tab,codelist,childs[0],leftOp);
				}
				else
				{
					return NULL;
				}
				// SetOperandValue(leftOP,lvar->name);
				//右值
				Operand_t *rightOp = AllocaOperand(codelist, TEMPVAR_OP);
				// rightOp->u.no = codeTempVarNo(codelist);
				int rightOpNo = rightOp->u.no;
				rval = trExp(tab,codelist,childs[2],rightOp);
				if(lvar == NULL || rval == NULL)
					return NULL;
				if(typeCmp(lvar,rval)==0)
				{
					if( !(rightOp->kind==TEMPVAR_OP && rightOp->u.no==rightOpNo && (leftOp->kind==TEMPVAR_OP || leftOp->kind==VARIABLE_OP)))
					{
						//rightOp被改 说明需要生成赋值语句
						// leftOp := rightOp
						InterCode_t *assignCode = AllocaInterCode(ASSIGN_CODE);
						assignCode->u.assign.left = leftOp;
						assignCode->u.assign.right = rightOp;
						appendIRCode(codelist,assignCode);
					}
					else
					{
						memcpy(rightOp, leftOp, sizeof(Operand_t));
					}
					if(place != NULL)
					{
						InterCode_t *assignCode2 = AllocaInterCode(ASSIGN_CODE);
						assignCode2->u.assign.left = place;
						assignCode2->u.assign.right = rightOp;
						appendIRCode(codelist,assignCode2);
					}
					return lvar;
				}
			}
			else if( strcmp(childs[1]->name,"ADD")==0 || strcmp(childs[1]->name,"SUB")==0 || strcmp(childs[1]->name,"MUL")==0 || strcmp(childs[1]->name,"DIV")==0 )
				// | Exp PLUS Exp
				// | Exp MINUS Exp
				// | Exp STAR Exp
				// | Exp DIV Exp
			{
				Operand_t *leftOp = AllocaOperand(codelist, TEMPVAR_OP);
				// leftOp->u.no = codeTempVarNo(codelist);
				lvar = trExp(tab,codelist,childs[0],leftOp);
				Operand_t *rightOp = AllocaOperand( codelist, TEMPVAR_OP);
				// rightOp->u.no = codeTempVarNo(codelist);
				rval = trExp(tab,codelist,childs[2],rightOp);
				
				if(place != NULL)
				{
					InterCode_t *calcCode = AllocaInterCode(ADD_CODE);
					if(strcmp(childs[1]->name,"ADD")==0)
						calcCode->kind=ADD_CODE;
					else if(strcmp(childs[1]->name,"SUB")==0)
						calcCode->kind=SUB_CODE;
					else if(strcmp(childs[1]->name,"MUL")==0)
						calcCode->kind=MUL_CODE;
					else if(strcmp(childs[1]->name,"DIV")==0)
						calcCode->kind=DIV_CODE;

					calcCode->u.doubleop.result = place;
					calcCode->u.doubleop.op1 = leftOp;
					calcCode->u.doubleop.op2 = rightOp;
					appendIRCode(codelist,calcCode);
				}
			}
			else if(strcmp(childs[1]->name,"RELOP")==0 || strcmp(childs[1]->name,"AND")==0 || strcmp(childs[1]->name,"OR")==0)
			{
				// assert(0);
			}
			else if( strcmp(childs[1]->name,"DOT")==0 )
			{// Exp: Exp DOT ID
				Operand_t *strucVarOp = AllocaOperand(codelist,TEMPVAR_OP);
				lvar = trExp(tab,codelist,childs[0],strucVarOp);
				if(lvar==NULL)
					return NULL;
				if( lvar->SbType !=STRUCT_T )
					return NULL;
				SbTab_t **strucField = (SbTab_t**)malloc(sizeof(SbTab_t *));
				*strucField = (SbTab_t*) lvar->SubTab;
				SbTab_t *tmp=NULL;
				tmp = FindSymbol(strucField,childs[2]->ID_type,tmp);
				int offset = 0;
				for(SbTab_t *iter = (*strucField);iter !=NULL;iter = iter->hh.next)
				{
					if(tmp == iter)
					{
						if(offset==0)
						{
							if(place!=NULL)
								if(iter->SbType ==INT_T || iter->SbType == FLOAT_T)
								{
									place->kind = ADDRESS_OP;
									place->u.addr = strucVarOp;
								}
								else
									memcpy(place,strucVarOp,sizeof(Operand_t));
						}
						else
						{
							//用于存储偏移量
							Operand_t *offsetOp = AllocaOperand(codelist,CONSTANT_OP);
							char* offsetStr = malloc(16*sizeof(char));
							sprintf(offsetStr, "%d", offset);
							offsetOp->u.value = offsetStr;
							// 地址 := strucVarOp ADD offsetOp
							InterCode_t *addrCode = AllocaInterCode(ADD_CODE);
							addrCode->u.doubleop.op1 = strucVarOp;
							addrCode->u.doubleop.op2 = offsetOp;
							if(iter->SbType == INT_T || iter->SbType == FLOAT_T)
							{
								//如果下一层是BASIC 则place是地址指向的位置
								Operand_t *temAddrOp = AllocaOperand(codelist,TEMPVAR_OP);
								addrCode->u.doubleop.result = temAddrOp;
								place->kind = ADDRESS_OP;
								place->u.addr = temAddrOp;
							}
							else
							{
								//如果下一层仍然是struct 则place是地址
								addrCode->u.doubleop.result = place;
							}
							appendIRCode(codelist,addrCode);
						}
						return iter;
					}
					offset += SizeofType(iter);
				}
				return tmp;
			}
			return lvar;
		}
		else if(children == 4)
		{// Exp: Exp LB Exp RB
			int children = haschildren(node);
			if(children==-1)
				return NULL;
			node_t **childs = node->child->ptr;
			Operand_t *baseOp = AllocaOperand(codelist,TEMPVAR_OP);
			SbTab_t *arrSy = trExp(tab,codelist,childs[0],baseOp);
			if(arrSy == NULL)
				return NULL;
			int subscipt = 1;
			if(strcmp(childs[2]->child->ptr[0]->name,"INT")==0)
				subscipt = childs[2]->child->ptr[0]->I_type;
			Operand_t *subscriptOp = NULL;
			if(subscipt !=0)
			{//用于存储数组下标
				subscriptOp = AllocaOperand(codelist,TEMPVAR_OP);
			}
			SbTab_t *rval = trExp(tab,codelist,childs[2],subscriptOp);
			if(rval == NULL)
				return NULL;
			if(rval->SbType != INT_T)
				return NULL;
			Operand_t *offsetOp = AllocaOperand(codelist,TEMPVAR_OP);
			if(subscipt != 0)
			{
				//用于存储宽度
				Operand_t *wideOp = AllocaOperand(codelist,CONSTANT_OP);
				// arr_t *arraylist = arrSy->SubTab;
				char* wideStr = malloc(16*sizeof(char));
				int wide = ArrayOffsetWide(arrSy->SubTab);
				sprintf(wideStr, "%d", wide);
				wideOp->u.value = wideStr;
				//offsetOp := subscriptOp MUL wideOp
				InterCode_t *offsetCode = AllocaInterCode(MUL_CODE);
				offsetCode->u.doubleop.result = offsetOp;
				offsetCode->u.doubleop.op1 = subscriptOp;
				offsetCode->u.doubleop.op2 = wideOp;
				appendIRCode(codelist,offsetCode);
				// 地址 := baseOp ADD offsetOp
				InterCode_t *addrCode = AllocaInterCode(ADD_CODE);
				addrCode->u.doubleop.op1 = baseOp;
				addrCode->u.doubleop.op2 = offsetOp;
				SbTab_t *tmp = arrSy->SubTab;
				if(tmp->SbType != ARRAY)
				{
					//如果下一层是BASIC 则place是地址指向的位置
					Operand_t *tmpAddrOp = AllocaOperand(codelist,TEMPVAR_OP);
					addrCode->u.doubleop.result = tmpAddrOp;
					place->kind = ADDRESS_OP;
					place->u.addr = tmpAddrOp;
				}
				else if(tmp->SbType == ARRAY)
				{
					//如果下一层仍然是数组 则place是地址
					addrCode->u.doubleop.result = place;
				}
				appendIRCode(codelist,addrCode);
			}
			else
			{
				// 地址 := baseOp
				InterCode_t *addrCode = AllocaInterCode(ASSIGN_CODE);
				addrCode->u.assign.right = baseOp;
				SbTab_t *tmp = arrSy->SubTab;
				if(tmp->SbType== INT_T || tmp->SbType == FLOAT_T)
				{//如果下一层是BASIC 则place是地址指向的位置
					Operand_t *tmpAddrOp = AllocaOperand(codelist,TEMPVAR_OP);
					addrCode->u.doubleop.result = tmpAddrOp;
					place->kind = ADDRESS_OP;
					place->u.addr = tmpAddrOp;
				}
				else if(tmp->SbType == ARRAY)
				{//如果下一层仍然是数组 则place是地址
					addrCode->u.doubleop.result = place;
				}
				appendIRCode(codelist,addrCode);
			}
			return (SbTab_t*)arrSy->SubTab;
		}
	}
	else if(strcmp(childs[0]->name,"LP")==0)
	{// Exp: LP Exp RP
		return trExp(tab,codelist,childs[1],place);
	}
	else if(strcmp(childs[0]->name,"SUB")==0)
	{//Exp:MINUS Exp
		Operand_t *rigthOp = AllocaOperand(codelist,TEMPVAR_OP);
		SbTab_t *rval = trExp(tab,codelist,childs[1],rigthOp);
		if(rval == NULL)
			return NULL;
		if(rval->SbType != INT_T && rval->SbType !=FLOAT_T )
			return NULL;
		Operand_t *zeroOp = AllocaOperand(codelist,CONSTANT_OP);
		SetOperandValue(zeroOp,"0");
		if(place !=NULL)
		{
			InterCode_t *minusCode = AllocaInterCode(SUB_CODE);
			minusCode->u.doubleop.result = place;
			minusCode->u.doubleop.op1 = zeroOp;
			minusCode->u.doubleop.op2 = rigthOp;
			appendIRCode(codelist,minusCode);
		}
		return rval;
	}
	else if(strcmp(childs[0]->name,"LNOT")==0)
	{
		
	}
	else if(strcmp(childs[0]->name,"INT")==0)
	{//Exp : INT
		SbTab_t *rval = AllocSymbol("INT",INT_T);
		if(place!=NULL)
		{
			place->kind = CONSTANT_OP;
			char tmpi[10];
			memset(tmpi,0,10);
			//itoa(SubTabCNT,tmpi,10);
			sprintf(tmpi,"%d",childs[0]->I_type);
			SetOperandValue(place,tmpi);
		}
		return rval;
	}
	else if(strcmp(childs[0]->name,"FLOAT")==0)
	{
		SbTab_t *rval = AllocSymbol("FLOAT",FLOAT_T);
		char tmpf[32];
		memset(tmpf,0,32);
		//itoa(SubTabCNT,tmpi,10);
		sprintf(tmpf,"%f",childs[0]->F_type);
		SetOperandValue(place,tmpf);
		return rval;
	}
	else if(strcmp(childs[0]->name,"ID")==0)
	{//Exp:ID ...
		SbTab_t *Sy = NULL;
		
		if(children==1)
		{//Exp : ID
			Sy = LookUpSymbol(tab,childs[0]->ID_type,Sy);
			if(place != NULL)
			{
				place->kind = VARIABLE_OP;
				place->u.no = Sy->varNo;
				// place->u.value = childs[0]->ID_type;
				// SetOperandValue(place,childs[0]->ID_type);
			}
			return Sy;
		}
		else 
		{// Exp: ID LP (Args) RP

			if(strcmp(childs[2]->name,"RP")==0)
			{
				if(strcmp(childs[0]->ID_type,"read")==0)
				{
					if(place !=NULL)
					{
						InterCode_t *funcCode = AllocaInterCode(READ_CODE);
						funcCode->u.singleop.op = place;
						appendIRCode(codelist,funcCode);
					}
					SbTab_t *retmp = AllocSymbol("a",INT_T);
					return retmp;
				}
				else
				{
					Sy=LookUpSymbol(tab,childs[0]->ID_type,Sy);
					if(Sy == NULL)
						return NULL;
					if(Sy->SbType != FUNCDEF && Sy->SbType != FUNCDEC)
						return NULL;
					FuncTab_t *func = (FuncTab_t*)Sy->SubTab;
					SbTab_t *paralist = func->paraList;
					Operand_t *funcOp = AllocaOperand(codelist,FUNCTION_OP);
					SetOperandValue(funcOp,func->name);
					if(place != NULL)
					{
						InterCode_t *funcCode = AllocaInterCode(CALL_CODE);
						funcCode->u.assign.left = place;
						funcCode->u.assign.right = funcOp;
						appendIRCode(codelist,funcCode);
					}
					else
					{
						Operand_t *uselessOp = AllocaOperand(codelist,TEMPVAR_OP);
						InterCode_t * funcCode = AllocaInterCode(CALL_CODE);
						funcCode->u.assign.left= uselessOp;
						funcCode->u.assign.right = funcOp;
						appendIRCode(codelist,funcCode);
					}
					return func->retType;
				}
			}
			else 
			{
				Operand_t* argsListHead =  NULL;
				Sy = LookUpSymbol(tab,childs[0]->ID_type,Sy);
				// Sy=LookUpSymbol(tab,childs[0]->ID_type,Sy);
				if(Sy == NULL)
					return NULL;
				if(Sy->SbType != FUNCDEF && Sy->SbType != FUNCDEC)
					return NULL;
				FuncTab_t *func = (FuncTab_t*)Sy->SubTab;
				SbTab_t *paralist = func->paraList;
				if(trArgs(tab,codelist,childs[2],paralist,&argsListHead)==0)
				{
					if(strcmp(childs[0]->ID_type,"write")==0)
					{
						InterCode_t *funcCode = AllocaInterCode(WRITE_CODE);
						funcCode->u.singleop.op = argsListHead;
						appendIRCode(codelist,funcCode);
					}
					else
					{
						Operand_t *argsP = argsListHead;
						while(argsP !=NULL)
						{// PARAM argsP
						//直接按照队列方向即从右向左入栈
							InterCode_t *argCode = AllocaInterCode(ARG_CODE);
							argCode->u.singleop.op = argsP;
							appendIRCode(codelist,argCode);
							argsP = argsP->nextArg;
						}
						Operand_t *funcOp = AllocaOperand(codelist,FUNCTION_OP);
						SetOperandValue(funcOp,func->name);
						if(place != NULL)
						{
							InterCode_t *funcCode = AllocaInterCode(CALL_CODE);
							funcCode->u.assign.left = place;
							funcCode->u.assign.right = funcOp;
							appendIRCode(codelist,funcCode);
						}
						else
						{
							Operand_t *uselessOp = AllocaOperand(codelist,TEMPVAR_OP);
							InterCode_t *funcCode = AllocaInterCode(CALL_CODE);
							funcCode->u.assign.left = uselessOp;
							funcCode->u.assign.right = funcOp;
							appendIRCode(codelist,funcCode);
						}
					}
				}	
				return func->retType;
			}
		}
	}
	return NULL;	
}


SbTab_t *trBoolExp(SbTab_t **tab,IRCodeList_t *codelist,node_t *node,Operand_t *True,Operand_t*False)
{
	int children = haschildren(node);
	if(children==-1)
		return NULL;
	node_t **childs=node->child->ptr;
	
	if(strcmp(childs[0]->name,"Exp")==0)
	{//Exp (...) Exp
		if(strcmp(childs[1]->name,"RELOP")==0)
		{
			Operand_t *leftOp = AllocaOperand(codelist,TEMPVAR_OP);
			SbTab_t *lval = trExp(tab,codelist,childs[0],leftOp);
			Operand_t *rightOp = AllocaOperand(codelist,TEMPVAR_OP);
			SbTab_t *rval = trExp(tab,codelist,childs[2],rightOp);
			if( lval==NULL || rval ==NULL )
				return NULL;
			if((typeCmp(lval,rval)==0 && (lval->SbType == FLOAT_T || lval->SbType == INT_T)))
			{
				//IF leftOp RELOP rightOp GOTO trueLabel
				InterCode_t *ifgotoCode = AllocaInterCode(IF_GOTO_CODE);
				ifgotoCode->u.tribleop.x = leftOp;
				ifgotoCode->u.tribleop.y = rightOp;
				ifgotoCode->u.tribleop.relop = childs[1]->ID_type;
				ifgotoCode->u.tribleop.gotoLabel = True;
				appendIRCode(codelist,ifgotoCode);
				// GOTO falseLabel
				InterCode_t *gotoFalseLabel = AllocaInterCode(GOTO_CODE);
				gotoFalseLabel->u.singleop.op = False;
				appendIRCode(codelist,gotoFalseLabel);
				return lval;
			}
			else
				return NULL;
		}
		else if(strcmp(childs[1]->name,"AND")==0)
		{// Exp: Exp AND Exp
			Operand_t *lTrue = AllocaOperand(codelist,LABEL_OP);
			SbTab_t *lval = trBoolExp(tab,codelist,childs[0],lTrue,False);
			InterCode_t * lTrueLabelCode = AllocaInterCode(LABEL_CODE);
			lTrueLabelCode->u.singleop.op = lTrue;
			appendIRCode(codelist,lTrueLabelCode);
			SbTab_t *rval = trBoolExp(tab,codelist,childs[2],True,False);
			if( lval==NULL || rval==NULL )
				return NULL;	
			if(typeCmp(lval,rval)==0 && (lval->SbType == FLOAT_T || lval->SbType == INT_T))
				return lval;
			else
				return NULL;
		}
		else if(strcmp(childs[1]->name,"OR")==0)
		{// Exp: Exp OR Exp
			Operand_t *lFalse = AllocaOperand(codelist,LABEL_OP);
			SbTab_t *lval = trBoolExp(tab,codelist,childs[0],True,lFalse);
			InterCode_t *lFalseLableCode = AllocaInterCode(LABEL_CODE);
			lFalseLableCode->u.singleop.op = lFalse;
			appendIRCode(codelist,lFalseLableCode);
			SbTab_t *rval = trBoolExp(tab,codelist,childs[2],True,False);
			if(typeCmp(lval,rval)==0 && (lval->SbType == FLOAT_T || lval->SbType == INT_T))
				return lval;
			else
				return NULL;
		}

	}
	else if(strcmp(childs[0]->name,"NOT")==0)
	{
		SbTab_t *rval = trBoolExp(tab,codelist,childs[1],False,True);
		if(rval ==NULL)
			return NULL;
		if(rval->SbType != INT_T)
			return NULL;
		return rval;
	}
	else if(strcmp(childs[0]->name,"ID")==0)
	{
		if(children == 1)
		{//Exp : ID
			Operand_t *varOp = AllocaOperand(codelist,VARIABLE_OP);
			SbTab_t *var = trExp(tab,codelist,node,varOp);
			if(var==NULL)
				return NULL;
			if(var->SbType == INT_T)
			{
				//IF leftOp RELOP rightOp GOTO trueLabel
				Operand_t *zeroOp = AllocaOperand(codelist,CONSTANT_OP);
				SetOperandValue(zeroOp,"0");
				InterCode_t *ifgotoCode = AllocaInterCode(IF_GOTO_CODE);
				ifgotoCode->u.tribleop.x = varOp;
				ifgotoCode->u.tribleop.y = zeroOp;
				char *tmp = malloc(sizeof(char));
				strcpy(tmp,">");
				ifgotoCode->u.tribleop.relop = tmp;
				ifgotoCode->u.tribleop.gotoLabel = True;
				appendIRCode(codelist,ifgotoCode);
				// GOTO falseLabel
				InterCode_t *gotoFalseLabel = AllocaInterCode(GOTO_CODE);
				gotoFalseLabel->u.singleop.op = False;
				appendIRCode(codelist,gotoFalseLabel);
				return var;
			}
			else
				return NULL;
		}
		else
		{//Exp : ID LP (Args) RP
			Operand_t *retValOp = AllocaOperand(codelist,TEMPVAR_OP);
			SbTab_t *retSy = trExp(tab,codelist,node,retValOp);
			if(retSy==NULL)
				return NULL;
			if(retSy->SbType==INT_T)
			{
				//IF leftOp RELOP rightOp GOTO trueLabel
				Operand_t *zeroOp = AllocaOperand(codelist,CONSTANT_OP);
				SetOperandValue(zeroOp,"0");
				InterCode_t *ifgotoCode = AllocaInterCode(IF_GOTO_CODE);
				ifgotoCode->u.tribleop.x = retValOp;
				ifgotoCode->u.tribleop.y = zeroOp;
				char *tmp = malloc(sizeof(char));
				strcpy(tmp,">");
				ifgotoCode->u.tribleop.relop = tmp;
				ifgotoCode->u.tribleop.gotoLabel = True;
				appendIRCode(codelist,ifgotoCode);
				// GOTO falseLabel
				InterCode_t *gotoFalseLabel = AllocaInterCode(GOTO_CODE);
				gotoFalseLabel->u.singleop.op = False;
				appendIRCode(codelist,gotoFalseLabel);
			}
		}
	}
	else if(strcmp(childs[0]->name,"INT")==0)
	{
		SbTab_t *ret = AllocSymbol("INT",INT_T);
		int val = childs[0]->I_type;
		if(val>0||val<0)
		{//goto ture
			InterCode_t *gotoFalseLabel = AllocaInterCode(GOTO_CODE);
			gotoFalseLabel->u.singleop.op = True;
			appendIRCode(codelist,gotoFalseLabel);
		}
		else
		{//goto alse
			InterCode_t *gotoFalseLabel = AllocaInterCode(GOTO_CODE);
			gotoFalseLabel->u.singleop.op = False;
			appendIRCode(codelist,gotoFalseLabel);
		}
		return ret;
	}
	else
	{
		return NULL;
	}
}

int trArgs(SbTab_t **tab,IRCodeList_t *codelist,node_t *node, SbTab_t *paralist,Operand_t **argsListHead)
{//Args : Exp COMMA Args
	//	| Exp
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

	Operand_t *argOp = AllocaOperand(codelist,TEMPVAR_OP);
	SbTab_t *lvar = trExp(tab,codelist,childs[0],argOp);
	argOp->nextArg = *argsListHead;
	*argsListHead = argOp;
	if(lvar == NULL)
		return -1;
	if(typeCmp(lvar,para)!=0)
		return -1;
	if(children == 3)
		return trArgs(tab,codelist,childs[2], para->hh.next,argsListHead);
}


//数据结构相关
IRCodeList_t *AllocaInterCodeList()
{
	IRCodeList_t *ret = malloc(sizeof(IRCodeList_t));
	memset(ret,0,sizeof(IRCodeList_t));
	return ret;
}
int DestroyInterCodes(IRCodeList_t *CodeList)
{

}
int insertIRCode(IRCodeList_t *codelist,InterCode_t *pos, InterCode_t *code)
{
	if(code == NULL || pos ==NULL)
	{
		printf("can not append a NULL ptr to codelist\n");
		return -1;
	}
	if(codelist == NULL)
	{
		printf("codelist is NULL \n");
		return -1;
	}
	if(codelist->Head==NULL)
	{
		codelist->Head = code;
		codelist->Tail = code;
	}
	else if(pos == codelist->Tail)
	{
		code->prevCode = pos;
		pos->nextCode = code;
		codelist->Tail = code;
	}
	else
	{
		code->prevCode = pos;
		pos->nextCode = code;
	}
	codelist->size += 1;
	return 0;
}
int appendIRCode(IRCodeList_t *codelist,InterCode_t *code)
{
	if(code == NULL)
	{
		printf("can not append a NULL ptr to codelist\n");
		return -1;
	}
	if(codelist == NULL)
	{
		printf("codelist is NULL \n");
		return -1;
	}
	if(codelist->Head==NULL)
	{
		codelist->Head = code;
		codelist->Tail = code;
	}
	else
	{
		code->prevCode = codelist->Tail;
		codelist->Tail->nextCode = code;
		codelist->Tail = code;
	}
	codelist->size +=1;
	return 0;
}
int deleteIRCode(IRCodeList_t *codelist,InterCode_t *code)
{
	if(code==NULL)
	{
		printf("code is NULL, don't need delete \n");
		return -1;
	}
	if(codelist ==NULL)
	{
		printf("codelist is NULL\n");
		return -1;
	}
	if(code==codelist->Head){
		codelist->Head = code->nextCode;
	}
	else{
		code->prevCode->nextCode = code->nextCode;
	}
	if(code==codelist->Tail){
		codelist->Tail = code->prevCode;
	}
	else{
		code->nextCode->prevCode = code->prevCode;
	}
	codelist->size -=1;
	free(code);
	return 0;
}


Operand_t *AllocaOperand(IRCodeList_t *codelist,Opkind_t kindv)
{
	Operand_t *ret = (Operand_t*) malloc(sizeof(Operand_t));
	memset(ret,0,sizeof(Operand_t));
	ret->kind = kindv;
	switch(kindv)
	{
		case VARDEC_OP:
		ret->u.no = codeVarNo(codelist);break;
		case LABEL_OP:
		ret->u.no = codeLabelNo(codelist);break;
		case TEMPVAR_OP:
		ret->u.no = codeTempVarNo(codelist);break;
		default: break;
	}
	return ret;
}

int SetOperandValue(Operand_t *Op,const char *str)
{
	char *t;
    t=(char*)malloc(sizeof(char )*(strlen(str)+1));
	strcpy(t,str);
	Op->u.value = t;
	return 0;
}

InterCode_t *AllocaInterCode(IRKind_t kindv)
{
	InterCode_t *ret = (InterCode_t*)malloc(sizeof(InterCode_t));
	memset(ret,0,sizeof(InterCode_t));
	ret->kind = kindv;
	return ret;
}


int codeLabelNo(IRCodeList_t *codelist)
{
	++(codelist->labelNo);
	return (codelist->labelNo);
}

int codeTempVarNo(IRCodeList_t *codelist)
{
	++(codelist->tempVarNo);
	return (codelist->tempVarNo);
}

int codeVarNo(IRCodeList_t *codelist)
{
	++(codelist->varNO);
	return (codelist->varNO);
}


int fwriteOp(Operand_t *p, FILE *fp)
{
	if(p == NULL)
		return -1;
	char str[50];
	memset(str, 0, sizeof(str));
	switch(p->kind){
		case VARIABLE_OP:
			sprintf(str, "v%d", p->u.no);
			fputs(str, fp);
			printf("%s",str);
			break;
		case VARDEC_OP:
			sprintf(str, "v%d", p->u.no);
			fputs(str, fp);
			printf("%s",str);
			break;
		case CONSTANT_OP:
			sprintf(str, "#%s ", p->u.value);
			fputs(str, fp);
			printf("%s",str);
			break;
		case ADDRESS_OP:
			fputs("*", fp);
			printf("*");
			assert(p->u.addr!=NULL);
			fwriteOp(p->u.addr, fp);
			break;
		case TEMPVAR_OP:
			sprintf(str, "t%d ", p->u.no);
			fputs(str, fp);
			printf("%s",str);
			break;
		case LABEL_OP:
			sprintf(str, "label%d ", p->u.no);
			fputs(str, fp);
			printf("%s",str);
			break;
		case FUNCTION_OP:
			sprintf(str, "%s ", p->u.value);
			fputs(str, fp);
			printf("%s",str);
			break;
	}
}
int fwriteAllOp(IRCodeList_t *codelist, const char *filename)
{
	FILE* fp = fopen(filename, "w");
	if(fp==NULL){
		printf("ERROR: open file \"%s\" fail!",filename);
		return -1;
	}
	InterCode_t* p=codelist->Head;
	while(p!=NULL){
		switch(p->kind){
			case LABEL_CODE:
				fputs("LABEL ", fp);
				printf("LABEL ");
				fwriteOp(p->u.singleop.op, fp);
				fputs(": ", fp);
				printf(": ");
				break;
			case FUNC_CODE:
				fputs("FUNCTION ", fp);
				printf("FUNCTION ");
				fwriteOp(p->u.singleop.op, fp);
				fputs(": ", fp);
				printf(": ");
				break;
			case ASSIGN_CODE:
				fwriteOp(p->u.assign.left, fp);
				fputs(" := ", fp);
				printf(" := ");
				fwriteOp(p->u.assign.right, fp);
				break;
			case ADD_CODE:
				fwriteOp(p->u.doubleop.result, fp);
				fputs(" := ", fp);
				printf(" := ");
				fwriteOp(p->u.doubleop.op1, fp);
				fputs(" + ", fp);
				printf(" + ");
				fwriteOp(p->u.doubleop.op2, fp);
				break;
			case SUB_CODE:
				fwriteOp(p->u.doubleop.result, fp);
				fputs(" := ", fp);
				printf(" := ");
				fwriteOp(p->u.doubleop.op1, fp);
				fputs(" - ", fp);
				printf(" - ");
				fwriteOp(p->u.doubleop.op2, fp);
				break;
			case MUL_CODE:
				fwriteOp(p->u.doubleop.result, fp);
				fputs(" := ", fp);
				printf(" := ");
				fwriteOp(p->u.doubleop.op1, fp);
				fputs(" * ", fp);
				printf(" * ");
				fwriteOp(p->u.doubleop.op2, fp);
				break;
			case DIV_CODE:
				fwriteOp(p->u.doubleop.result, fp);
				fputs(" := ", fp);
				printf(" := ");
				fwriteOp(p->u.doubleop.op1, fp);
				fputs(" / ", fp);
				printf(" / ");
				fwriteOp(p->u.doubleop.op2, fp);
				break;
			case ADDR_CODE:
				fwriteOp(p->u.assign.left, fp);
				fputs(" := &", fp);
				printf(" := &");
				fwriteOp(p->u.assign.right, fp);
				break;
			case GOTO_CODE:
				fputs("GOTO ", fp);
				printf("GOTO ");
				fwriteOp(p->u.singleop.op, fp);
				break;
			case IF_GOTO_CODE:
				fputs("IF ", fp);
				printf("IF ");
				fwriteOp(p->u.tribleop.x, fp);
				fputs(" ", fp);
				fputs(p->u.tribleop.relop, fp);
				printf(" %s",p->u.tribleop.relop);
				fputs(" ", fp);
				printf(" ");
				fwriteOp(p->u.tribleop.y, fp);
				fputs("GOTO ", fp);
				printf("GOTO ");
				fwriteOp(p->u.tribleop.gotoLabel, fp);
				break;
			case RETURN_CODE:
				fputs("RETURN ", fp);
				printf("RETURN ");
				fwriteOp(p->u.singleop.op, fp);
				break;
			case DEC_CODE:
				fputs("DEC ", fp);
				printf("DEC ");
				fwriteOp(p->u.dec.op, fp);
				char sizeStr[16];
				sprintf(sizeStr, " %d", p->u.dec.size);
				printf(" %d",p->u.dec.size);
				fputs(sizeStr, fp);
				break;
			case ARG_CODE:
				fputs("ARG ", fp);
				printf("ARG ");
				fwriteOp(p->u.singleop.op, fp);
				break;
			case CALL_CODE:
				fwriteOp(p->u.assign.left, fp);
				fputs(" := CALL ", fp);
				printf(" := CALL ");
				fwriteOp(p->u.assign.right, fp);
				break;
			case PARA_CODE:
				fputs("PARAM ", fp);
				printf("PARAM ");
				fwriteOp(p->u.singleop.op, fp);
				break;
			case READ_CODE:
				fputs("READ ", fp);
				printf("READ ");
				fwriteOp(p->u.singleop.op, fp);
				break;
			case WRITE_CODE:
				fputs("WRITE ", fp);
				printf("WRITE ");
				fwriteOp(p->u.singleop.op, fp);
				break;
			default:
				assert(0);
		}
		fputs("\n", fp);
		printf("\n");
		p=p->nextCode;
	}
	fclose(fp);
}

int optInterCode(IRCodeList_t *codelist)
{
	optGotoCode(codelist);
	deleteNullLebel(codelist);
	figureOutConstCalc(codelist);
	mergeAssignCode(codelist);
	deleteNullGoto(codelist);
	deleteNullLebel(codelist);
	return 0;
}

int optGotoCode(IRCodeList_t *codelist)
{
	InterCode_t *p = codelist->Head;
	while(p!=NULL){
		if(p->kind == IF_GOTO_CODE){
			//1.转化为fall模式的IF_GOTO: 将
			//IF v1 RELOP v2 GOTO LABEL 1
			//GOTO LABEL 2
			//LABEL 1
			//转化为
			//IF v1 ~RELOP v2 GOTO LABEL2
			//LABEL 1
			InterCode_t* c1 = p;
			InterCode_t* c2 = p->nextCode;
			if(c2==NULL){
				p = p->nextCode;
				continue;
			}
			InterCode_t* c3 = c2->nextCode;
			if(c3==NULL){
				p = p->nextCode;
				continue;
			}
			if(c2->kind==GOTO_CODE && c3->kind==LABEL_CODE && c1->u.tribleop.gotoLabel==c3->u.singleop.op && c2->u.singleop.op!=c3->u.singleop.op){
				c1->u.tribleop.gotoLabel = c2->u.singleop.op;
				deleteIRCode(codelist,c2);
				//下面将RELOP取反
				char* newRelop = malloc(3*sizeof(char));
				memset(newRelop, 0, 3*sizeof(char));
				if( strcmp(c1->u.tribleop.relop,"==")==0 ) strcpy(newRelop,"!=");
				if( strcmp(c1->u.tribleop.relop,"!=")==0 ) strcpy(newRelop,"==");
				if( strcmp(c1->u.tribleop.relop,"<")==0 ) strcpy(newRelop,">=");
				if( strcmp(c1->u.tribleop.relop,">")==0 ) strcpy(newRelop,"<=");
				if( strcmp(c1->u.tribleop.relop,"<=")==0 ) strcpy(newRelop,">");
				if( strcmp(c1->u.tribleop.relop,">=")==0 ) strcpy(newRelop,"<");
				c1->u.tribleop.relop = newRelop;
			}
		}
		else if(p->kind == GOTO_CODE){
			//2.去除冗余的GOTO：将
			//GOTO LABEL 1
			//LABEL 1
			//转化为
			//LABEL 1
			InterCode_t* c1 = p;
			InterCode_t* c2 = p->nextCode;
			if(c2==NULL){
				p = p->nextCode;
				continue;
			}
			if(c2->kind==LABEL_CODE && c1->u.singleop.op==c2->u.singleop.op){
				p = p->nextCode;
				deleteIRCode(codelist, c1);
				continue;
			}
		}
		p = p->nextCode;
	}
	return 0;
}
int deleteNullLebel(IRCodeList_t *codelist)
{
	//将没有GOTO语句或者IF_GOTO语句指向的LEBEL删去
	LabelList_t* usedLabelListHead = NULL;
	InterCode_t* p =codelist->Head;
	//建链表
	while(p!=NULL){
		if(p->kind==GOTO_CODE){
			LabelList_t* usedLabel = malloc(sizeof(LabelList_t));
			usedLabel->labelNo = p->u.singleop.op->u.no;
			usedLabel->nextLabel = usedLabelListHead;
			usedLabelListHead = usedLabel;
		}
		else if(p->kind==IF_GOTO_CODE){
			LabelList_t* usedLabel = malloc(sizeof(LabelList_t));
			usedLabel->labelNo = p->u.tribleop.gotoLabel->u.no;
			usedLabel->nextLabel = usedLabelListHead;
			usedLabelListHead = usedLabel;
		}
		p = p->nextCode;
	}
	p =codelist->Head;
	while(p!=NULL){
		if(p->kind==LABEL_CODE){
			int thisNo = p->u.singleop.op->u.no;
			LabelList_t* labelP = usedLabelListHead;
			while(labelP!=NULL){
				if(labelP->labelNo == thisNo){
					break;
				}
				labelP = labelP->nextLabel;
			}
			if(labelP==NULL){
				//说明链表中没有该no
				InterCode_t* tem = p;
				p = p->nextCode;
				deleteIRCode(codelist, tem);
				continue;
			}
		}
		p = p->nextCode;
	}
	//回收链表所占内存
	while(usedLabelListHead!=NULL){
		LabelList_t* tem = usedLabelListHead;
		usedLabelListHead = usedLabelListHead->nextLabel;
		free(tem);
	}
	//合并重复的Label
	p =codelist->Head;
	while(p!=NULL){
		InterCode_t* c1 = p;
		InterCode_t* c2 = p->nextCode;
		if(c2==NULL){
			p = p->nextCode;
			continue;
		}
		if(c1->kind==LABEL_CODE && c2->kind==LABEL_CODE){
			c1->u.singleop.op->u.no = c2->u.singleop.op->u.no;
			p = c2;
			deleteIRCode(codelist, c1);
			continue;
		}
		p = p->nextCode;
	}
	return 0;
}
int figureOutConstCalc(IRCodeList_t *codelist)
{
	//将常数之间的运算计算出来 
	//如果存储结果的是一个临时变量 则将该临时变量直接替换为该结果常数
	InterCode_t* p =codelist->Head;
	while(p!=NULL){
		if(p->kind==ADD_CODE || p->kind==SUB_CODE || p->kind==MUL_CODE || p->kind==DIV_CODE ){
			Operand_t* result = p->u.doubleop.result;
			Operand_t* op1 = p->u.doubleop.op1;
			Operand_t* op2 = p->u.doubleop.op2;
			if(op1->kind==CONSTANT_OP && op2->kind==CONSTANT_OP){
				int op1Int=atoi(op1->u.value);
				int op2Int=atoi(op2->u.value);
				int resultInt;
				switch(p->kind){
					case ADD_CODE:
						resultInt = op1Int+op2Int;
						break;
					case SUB_CODE:
						resultInt = op1Int-op2Int;
						break;
					case MUL_CODE:
						resultInt = op1Int*op2Int;
						break;
					case DIV_CODE:
						resultInt = op1Int/op2Int;
							break;
					default:assert(0);
				}
				char* resultStr = malloc(16*sizeof(char));
				sprintf(resultStr, "%d", resultInt);
				if(result->kind==TEMPVAR_OP){
					result->kind = CONSTANT_OP;
					result->u.value = resultStr;
					InterCode_t* tem = p;
					p = p->nextCode;
					deleteIRCode(codelist,tem);
					continue;
				}
				else{
					p->kind = ASSIGN_CODE;
					p->u.assign.left = result;
					p->u.assign.right = op1;
					op1->u.value = resultStr;
				}
			}
		}
		p = p->nextCode;
	}
	return 0;
}
int mergeAssignCode(IRCodeList_t *codelist)
{
	//观察赋值语句前后语句 在可以直接替换的情况下直接进行变量替换 从而将赋值语句合并
	InterCode_t* p =codelist->Head;
	while(p!=NULL){
		InterCode_t* c1 = p;
		InterCode_t* c2 = p->nextCode;
		if(c2!=NULL && c1->kind==ASSIGN_CODE && c2->kind!=DEC_CODE ){
			//当前语句下一句的右值等于当前语句的左值 并且该左值是临时的 则该左值是不需要的
			if(c2->u.doubleop.op1==c1->u.assign.left && c1->u.assign.left->kind==TEMPVAR_OP){
				c2->u.doubleop.op1 = c1->u.assign.right;
				p = p->nextCode;
				deleteIRCode(codelist,c1);
				continue;
			}
			else if(c2->u.doubleop.op2==c1->u.assign.left){
				c2->u.doubleop.op2 = c1->u.assign.right;
				p = p->nextCode;
				deleteIRCode(codelist,c1);
				continue;
			}
		}
		InterCode_t* c0 = p->prevCode;
		if(c0!=NULL && c1->kind==ASSIGN_CODE && c0->kind!=DEC_CODE && c0->kind!=IF_GOTO_CODE ){
			//当前语句上一句的左值等于当前语句的右值 并且该右值是临时的 则该右值是不需要的
			if(c0->u.doubleop.result==c1->u.assign.right && c1->u.assign.left->kind != ADDRESS_OP && c1->u.assign.right->kind==TEMPVAR_OP){
				c0->u.doubleop.result=c1->u.assign.left;
				p = p->nextCode;
				deleteIRCode(codelist,c1);
				continue;
			}
		}
		p = p->nextCode;
	}
	return 0;
}
int deleteNullGoto(IRCodeList_t *codelist)
{
	InterCode_t* p =codelist->Head;
	while(p!=NULL){
		if(p->kind == RETURN_CODE){
			InterCode_t* c1 = p;
			InterCode_t* c2 = p->nextCode;
			if(c2==NULL){
				p = p->nextCode;
				continue;
			}
			if(c2->kind==GOTO_CODE)
				deleteIRCode(codelist,c2);
		}
		p = p->nextCode;
	}
	return 0;
}


//辅助函数
int StructFieldSize(SbTab_t **StructFiledtab)
{
	int ret =0;
	for(SbTab_t *iter = (*StructFiledtab)->hh.next;iter !=NULL;iter=iter->hh.next)
	{
		if(iter->SbType == ARRAY)
		{
			ret += ArraySize(iter);
		}
		else if(iter->SbType == STRUCT_T)
		{
			SbTab_t **table = (SbTab_t**)&(iter->SubTab);
			ret += StructFieldSize(table);
		}
		else if(iter->SbType == INT_T)
		{
			ret += 4;
		}
		else if(iter->SbType == FLOAT_T)
		{
			ret += 4;
		}
		else
		{

		}
	}
	return ret;
}
int ArraySize(SbTab_t *arr)
{
	int ret =0 ;
	ret = ArraySize1((SbTab_t*) arr->SubTab);
	return ret;
}

int ArraySize1(SbTab_t *arr)
{
	int ret = 1;
	SbTab_t *a;
	for(a = arr;a!=NULL&&a->SubTab!=NULL&&a->SbType==ARRAY;a=a->SubTab)
	{
		ret *= a->arraySize;
	}
	// a = a->SubTab;
	ret *=a->arraySize;
	if(a->SbType == INT_T)
	{	
		ret *= 4;
	}
	else if(a->SbType == FLOAT_T)
	{
		ret *= 4;
	}
	else if(a->SbType == STRUCT_T)
	{
		ret *= StructFieldSize((SbTab_t**)&(a->SubTab));
	}
	return ret;
}

int ArrayOffsetWide(SbTab_t *arr)
{
	int ret =1;
	SbTab_t *a =arr;
	for(a = arr;a!=NULL&&a->SubTab!=NULL&&a->SbType==ARRAY;a=a->SubTab)
	{
		ret *= a->arraySize;
	}
	// a = a->SubTab;
	ret *=a->arraySize;
	if(a->SbType == INT_T)
	{	
		ret *= 4;
	}
	else if(a->SbType == FLOAT_T)
	{
		ret *= 4;
	}
	else if(a->SbType == STRUCT_T)
	{
		ret *= StructFieldSize((SbTab_t**)&(a->SubTab));
	}
	ret = ret/(a->arraySize);
	return ret;
}

int SizeofType(SbTab_t *Sy)
{
	if(Sy->SbType == INT_T)
		return 4;
	else if(Sy->SbType == FLOAT_T)
		return 4;
	else if(Sy->SbType == STRUCT_T)
		return StructFieldSize((SbTab_t**)&(Sy->SubTab));
	else if(Sy->SbType == ARRAY)
		return ArraySize(Sy);
	return 0;
}


int opCmp(Operand_t* a, Operand_t* b)
{
	if(a->kind!=b->kind)
		return 1;
	switch(a->kind){
		case FUNCTION_OP :
		case CONSTANT_OP :
			return strcmp(a->u.value, b->u.value);
		case VARDEC_OP:
		case VARIABLE_OP:
		{
			if(a->u.no == b->u.no)
				return 0;
			else
				return 1;
		}
		case ADDRESS_OP :
			return (a->u.addr!=b->u.addr);
		case TEMPVAR_OP :
		case LABEL_OP :
			return (a->u.no!=b->u.no);
	}
}
