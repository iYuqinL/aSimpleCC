/*************************************************************************
	> File Name: CodeGen.h
	> Author: yusnows
	> Mail: YusnowsLiang@gmail.com
	> Created Time: Tue 03 Jul 2018 09:44:12 PM CST
    > Copyright@yusnows: All rights reserve!
 ************************************************************************/

#ifndef _CODEGEN_H
#define _CODEGEN_H

#include "IntermedCode.h"

#define REGNUM 18
#define FSIZE 8	

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

typedef struct VarDes_t{
	Operand_t* op;
	int regNo;
	int offset;
	struct VarDes_t* next;
} VarDes;

typedef struct RegDes_t{
	char name[3];
	VarDes* var;
	int old;
} RegDes;

void clearVarList();
void addVarToList(VarDes* p);
void stVar(VarDes* var, FILE* fp);
void ldVar(VarDes* var, FILE* fp);
void initAllRegDes();
void rstAllReg();
void freeOneReg(int regNo);
int allocateRegForOp(Operand_t *op, FILE* fp);
int getReg(FILE* fp);
void fwriteAllObjCode(IRCodeList_t *codelist, char* fileName);
void fwriteOneObjCode(InterCode_t* ir, FILE* fp);

#endif
