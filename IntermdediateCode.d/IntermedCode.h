/*************************************************************************
	> File Name: IntermedCode.h
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Fri 08 Jun 2018 09:24:29 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#ifndef _INTERMEDCODE_H
#define _INTERMEDCODE_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "semantic.h"
// #include "SymbolTab.h"

//操作数类型
typedef enum OperandKinde_t
{
	VARIABLE_OP =1,
	CONSTANT_OP =2,
	ADDRESS_OP =3,
	TEMPVAR_OP =4,
	LABEL_OP = 5,
	FUNCTION_OP =6,
	VARDEC_OP =7,
}Opkind_t;


//中间代码类型
typedef enum InterCodeKind_t
{
	LABEL_CODE =1,
	FUNC_CODE =2,
	ASSIGN_CODE =3,
	ADD_CODE =4,
	SUB_CODE =5,
	MUL_CODE =6,
	DIV_CODE =7,
	ADDR_CODE =8,
	GOTO_CODE =9,
	IF_GOTO_CODE =10,
	RETURN_CODE =11,
	DEC_CODE =12,
	ARG_CODE =13,
	CALL_CODE =14,
	PARA_CODE =15,
	WRITE_CODE =16,
	READ_CODE =17,
}IRKind_t;

//操作数数据结构
typedef struct Operand_t
{
	Opkind_t kind;
	union{
		//TEM_VAR_OP LABEL_OP
		int no;
		//VAR_OP CONST_OP FUNC_OP
		char* value;
		//ADDR_OP
		struct Operand_t* addr;
	}u;	
	//在参数列表中的时候 指向下一个参数
	struct Operand_t* nextArg;
}Operand_t;

typedef struct InterCode_t
{
	//LABEL_CODE FUNCTION_CODE ASSIGN_CODE ADD_CODE SUB_CODE MUL_CODE DIV_CODE ADDRESS_CODE GOTO_CODE IF_GOTO_CODE RETURN_CODE DEC_CODE ARG_CODE CALL_CODE PARAM_CODE READ_CODE WRITE_CODE
	IRKind_t kind;
	union{
		//useless是将除了DEC_CODE以外的CODE的左值和左值均对齐 右值和右值均对齐
		//ASSIGN_CODE ADDRESS_CODE CALL_CODE
		struct{ 
			Operand_t* left;
			Operand_t* right;
			Operand_t* useless1;
		} assign;
		
		//LABEL_CODE FUNCTION_CODE GOTO_CODE RETURN_CODE ARG_CODE PARAM_CODE READ_CODE WRITE_CODE
		struct{	
			Operand_t* useless2;
			Operand_t* op;
			Operand_t* useless3;
		} singleop;
		
		//ADD_CODE SUB_CODE MUL_CODE DIV_CODE
		struct{ 
			Operand_t* result;
			Operand_t* op1;
			Operand_t* op2; 
		} doubleop;	
		
		//IF_GOTO_CODE 
		struct{ 
			Operand_t* gotoLabel;
			Operand_t* x;
			Operand_t* y;
			char* relop;
		} tribleop;			
		
		//DEC_CODE 
		struct{ 
			Operand_t* op;
			int size; 
		} dec; 
	}u;
	struct InterCode_t *prevCode;
	struct InterCode_t *nextCode;
}InterCode_t;

InterCode_t *AllocaInterCode(IRKind_t kindv);
// {
// 	InterCode_t *ret = (InterCode_t*)malloc(sizeof(InterCode_t));
// 	memset(ret,0,sizeof(InterCode_t));
// 	ret->kind = kindv;
// 	return ret;
// }

typedef struct InterCodeList_t
{
	InterCode_t *Head;
	InterCode_t *Tail;
	int tempVarNo;
	int labelNo;
	int varNO;
	int size;
}IRCodeList_t;


int SetOperandValue(Operand_t *Op,const char *str);
Operand_t *AllocaOperand(IRCodeList_t *codelist,Opkind_t kindv);
int codeLabelNo(IRCodeList_t *codelist);
int codeTempVarNo(IRCodeList_t *codelist);
int codeVarNo(IRCodeList_t *codelist);

IRCodeList_t *AllocaInterCodeList();
int DestroyInterCodes(IRCodeList_t *CodeList);
int insertIRCode(IRCodeList_t *codelist,InterCode_t *pos, InterCode_t *code);
int appendIRCode(IRCodeList_t *codelist, InterCode_t *code);
int deleteIRCode(IRCodeList_t *codelist, InterCode_t *code);

int fwriteOp(Operand_t *op, FILE *fp);
int fwriteAllOp(IRCodeList_t *codelist, const char *filename);

/*****************************************************************
 * translation about
 * ***************************************************************/
IRCodeList_t *trProgram(SbTab_t **tab, Sytree_t *sytree);
SbTab_t *trExtDefList(SbTab_t **tab, IRCodeList_t *codelist,node_t *node);
SbTab_t *trExtDef(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);
SbTab_t *trExtDecList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);

SbTab_t *trSpecifier(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);
SbTab_t *trStructSpecifier(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);

SbTab_t *trVarDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node);

FuncTab_t *trFunDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node);
SbTab_t *trVarList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);
SbTab_t *trParamDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node);

SbTab_t *trCompSt(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);
SbTab_t *trStmtList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);
SbTab_t *trStmt(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);

SbTab_t *trDefList(SbTab_t **tab,IRCodeList_t *codelist, node_t *node);

SbTab_t *trDef(SbTab_t **tab, IRCodeList_t *codelist,node_t *node);
SbTab_t *trDecList(SbTab_t **tab,IRCodeList_t *codelist,node_t *node);
SbTab_t *trDec(SbTab_t **tab,IRCodeList_t *codelist,node_t *node);

SbTab_t *trExp(SbTab_t **tab,IRCodeList_t *codelist,node_t *node,Operand_t *place);

SbTab_t *trBoolExp(SbTab_t **tab,IRCodeList_t *codelist,node_t *node,Operand_t *True,Operand_t*False);
// arr_t *trArrayExp(SbTab_t **tab,IRCodeList_t *codelist,node_t *node,Operand_t *place);

int trArgs(SbTab_t **tab,IRCodeList_t *codelist,node_t *node, SbTab_t *paralist,Operand_t **argsListHead);

/*****************************************************************
 * optimization
 * ***************************************************************/
//用于优化代码
typedef struct LabelList_t{
	int labelNo;
	struct LabelList_t* nextLabel;
}LabelList_t;

int optInterCode(IRCodeList_t *codelist);
int optGotoCode(IRCodeList_t *codelist);
int deleteNullLebel(IRCodeList_t *codelist);
int figureOutConstCalc(IRCodeList_t *codelist);
int mergeAssignCode(IRCodeList_t *codelist);
int deleteNullGoto(IRCodeList_t *codelist);

//辅助函数
int StructFieldSize(SbTab_t **StructFiledtab);
int ArraySize(SbTab_t *arr);
int ArraySize1(SbTab_t *arr);
int ArrayOffsetWide(SbTab_t *arr);
int SizeofType(SbTab_t *Sy);

#endif
