%{
#include <stdio.h>
#include "lex.yy.c"
#include "syntaxtree.h"

#define lno yylloc.first_line

void yyrestart  (FILE * input_file );
int yypares();
void yyerror( char* msg);
void lyyerror( char* msg);

extern struct syntaxtree_t* T;
int errorflag=0;

#ifndef D_print_sy
#define D_print_sy 0
#endif

%}
%locations
/* declared types */
%union {
    struct node_t* syntaxtreenode_t;
    double  double_t;
};

/* declared tokens */
%token <syntaxtreenode_t> INT
%token <syntaxtreenode_t> FLOAT
%token <syntaxtreenode_t> SEMI COMMA
%token <syntaxtreenode_t> ASSIGNOP
%token <syntaxtreenode_t> RELOP
%token <syntaxtreenode_t> LAND LOR
%token <syntaxtreenode_t> ADD SUB
%token <syntaxtreenode_t> MUL DIV
%token <syntaxtreenode_t> LNOT
%token <syntaxtreenode_t> DOT
%token <syntaxtreenode_t> STRUCT RETURN 
%token <syntaxtreenode_t> IF ELSE LOWER_THAN_ELSE
%token <syntaxtreenode_t> WHILE
%token <syntaxtreenode_t> TYPE
%token <syntaxtreenode_t> LP RP LB RB LC RC
%token <syntaxtreenode_t> ID
%token <syntaxtreenode_t> UNKT

%type <syntaxtreenode_t> Program ExtDefList ExtDef ExtDecList 
%type <syntaxtreenode_t> Specifier StructSpecifier OptTag Tag 
%type <syntaxtreenode_t> VarDec FunDec VarList ParamDec CompSt
%type <syntaxtreenode_t> StmtList DefList Def DecList Dec
%type <syntaxtreenode_t> Stmt Exp Args

/*%type <double_t> */
%right ASSIGNOP
%left LOR
%left LAND
%left RELOP
%left ADD SUB
%left MUL DIV
%right LNOT
%left DOT
%left LB RB
%left LP RP

%nonassoc LOWER_THAN_ELSE 
%nonassoc ELSE

%start Program

/* declared non-terminals */
/*%type <type_double> Program ExtDefList ExtDef Specifier ExtDecList VarDec VarList StructSpecifier OptTag DefList Tag  ParamDec CompSt StmtList Stmt Def Dec Exp Args*/

%%

/* high-level definitions */
Program : ExtDefList            {$$ = allocnewnode("Program",($1->lineno)," ");T->addchild($$,$1);T->setroot(T,$$); /*T->printsyntaxtree(T,T->root,0);*/}
        ;
ExtDefList : ExtDef ExtDefList  {$$=allocnewnode("ExtDefList",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
           |           {$$=NULL;
                        #if D_print_sy 
                        printf("encounter an empty in ExtDefList\n");
                        #endif
                        }
           ;
ExtDef : Specifier ExtDecList SEMI      {$$=allocnewnode("ExtDef",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
       | Specifier SEMI                 {$$=allocnewnode("ExtDef",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
       | Specifier FunDec CompSt        {$$=allocnewnode("ExtDef",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
       | Specifier error                {errorflag=1;lyyerror("missing ';'"); }
       ;
ExtDecList : VarDec                     {$$=allocnewnode("ExtDecList",($1->lineno)," ");T->addchild($$,$1);}
           | VarDec COMMA ExtDecList    {$$=allocnewnode("ExtDecList",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
           | VarDec error ExtDecList    {errorflag=1;lyyerror("missing ','");}
           ;

/* Specifiers */
Specifier : TYPE                    {$$=allocnewnode("Specifier",($1->lineno)," ");T->addchild($$,$1);}
          | StructSpecifier         {$$=allocnewnode("Specifier",($1->lineno)," ");T->addchild($$,$1);}
          ;
StructSpecifier : STRUCT OptTag LC DefList RC {$$=allocnewnode("StructSpecifier",($1->lineno)," ");T->addchildren($$,5,$1,$2,$3,$4,$5);}
                | STRUCT Tag        {$$=allocnewnode("StructSpecifier",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
                ;
OptTag : ID                         {$$=allocnewnode("OptTag",($1->lineno)," ");T->addchild($$,$1);}
       |                            {$$=NULL;
                                    #if D_print_sy
                                    printf("encounter an empty in OptTag\n");
                                    #endif
                                    }
/*       |                            {$$=allocnewnode("OptTag",lno," ");printf("encounter an empty in ExtDefList\n");}*/
       ;
Tag : ID                            {$$=allocnewnode("Tag",($1->lineno)," ");T->addchild($$,$1);}
    ;
/* Declarators   */
VarDec : ID                         {$$=allocnewnode("VarDec",($1->lineno)," ");T->addchild($$,$1);}
       | VarDec LB INT  RB          {$$=allocnewnode("VarDec",($1->lineno)," ");T->addchildren($$,4,$1,$2,$3,$4);}
       | VarDec LB error RB         {errorflag=1;lyyerror("missing ']'"); }
       ;
FunDec : ID LP VarList  RP          {$$=allocnewnode("FunDec",($1->lineno)," ");T->addchildren($$,4,$1,$2,$3,$4);}
       | ID LP RP                   {$$=allocnewnode("FunDec",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3); }
       | ID LP error RP             {errorflag=1; lyyerror("missing ')'"); }
       ;
VarList: ParamDec COMMA VarList     {$$=allocnewnode("VarList",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
       | ParamDec                   {$$=allocnewnode("VarList",($1->lineno)," ");T->addchild($$,$1);}
       ;
ParamDec : Specifier VarDec             {$$=allocnewnode("ParamDec",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
         ;
/* statements */
CompSt :  LC DefList StmtList  RC       {$$=allocnewnode("CompSt",($1->lineno)," ");T->addchildren($$,4,$1,$2,$3,$4);}
       ;
StmtList : Stmt StmtList                {$$=allocnewnode("StmtList",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
         |                              {$$=NULL;
                                        #if D_print_sy
                                        printf("encounter an empty in StmtList\n");
                                        #endif
                                        }
         ;
Stmt : Exp  SEMI                    {$$=allocnewnode("Stmt",($1->lineno)," ");T->addchildren($$,2,$1,$2); }
     | Exp error SEMI               {errorflag=1; lyyerror("missing ';'");}
     | CompSt                       {$$=allocnewnode("Stmt",($1->lineno)," ");T->addchild($$,$1); }
     | RETURN Exp  SEMI             {$$=allocnewnode("Stmt",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
     | IF LP Exp  RP Stmt %prec LOWER_THAN_ELSE  {$$=allocnewnode("Stmt",($1->lineno)," ");T->addchildren($$,5,$1,$2,$3,$4,$5);}
     | IF LP Exp  RP Stmt ELSE Stmt {$$=allocnewnode("Stmt",($1->lineno)," ");T->addchildren($$,7,$1,$2,$3,$4,$5,$6,$7);}
     | WHILE LP Exp  RP Stmt        {$$=allocnewnode("Stmt",($1->lineno)," ");T->addchildren($$,5,$1,$2,$3,$4,$5);}
     | error SEMI                   {errorflag=1; lyyerror("missing ';'");}
     | Exp error                    {errorflag=1;lyyerror("missing ';'"); }
     | RETURN Exp error             {errorflag=1;lyyerror("missing ';'"); }
     | IF LP error RP Stmt %prec LOWER_THAN_ELSE { errorflag=1;lyyerror("missing ')'"); }
     | IF LP error RP Stmt ELSE Stmt        {errorflag=1;lyyerror("missing ')'"); } 
     | WHILE LP error RP Stmt               {errorflag =1; lyyerror("missing ')'");}
     ;
/* Local Definitions */
DefList : Def DefList               {$$=allocnewnode("DefList",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
        |                           {$$=NULL;
                                    #if D_print_sy
                                    printf("encounter an empty in DefList\n");
                                    #endif
                                    }
        ;
Def : Specifier DecList  SEMI       {$$=allocnewnode("Def",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Specifier DecList error       {errorflag=1; lyyerror("missing ';'"); }
    | Specifier error SEMI          {errorflag=1; lyyerror("missing ';'");}
    ;
DecList : Dec                       {$$=allocnewnode("DecList",($1->lineno)," ");T->addchild($$,$1);}
        | Dec COMMA DecList         {$$=allocnewnode("DecList",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
        ;
Dec : VarDec                        {$$=allocnewnode("Dec",($1->lineno)," ");T->addchild($$,$1);}
    | VarDec ASSIGNOP Exp           {$$=allocnewnode("Dec",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    ;
/* Expressions*/
Exp : Exp ASSIGNOP Exp              {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp LAND Exp                  {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp LOR Exp                   {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp RELOP Exp                 {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp ADD Exp                   {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp SUB Exp                   {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp MUL Exp                   {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp DIV Exp                   {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | LP Exp  RP                    {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | SUB Exp                       {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
    | LNOT Exp                      {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,2,$1,$2);}
    | ID LP Args  RP                {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,4,$1,$2,$3,$4);}
    | ID LP  RP                     {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | Exp LB Exp  RB                {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,4,$1,$2,$3,$4);}
    | Exp DOT ID                    {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
    | ID                            {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,1,$1);}
    | INT                           {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,1,$1);}
    | FLOAT                         {$$=allocnewnode("Exp",($1->lineno)," ");T->addchildren($$,1,$1);}
    | Exp ASSIGNOP error            {errorflag=1;lyyerror("syntax error, maybe ASSIGNOP");}
    | Exp LAND error                {errorflag=1;lyyerror("syntax error, maybe &&");}
    | Exp LOR error                 {errorflag=1;lyyerror("syntax error, maybe ||");}
    | Exp RELOP error               {errorflag=1;lyyerror("syntax error, maybe RELOP");}
    | Exp ADD error                 {errorflag=1;lyyerror("syntax error, maybe +");}
    | Exp SUB error                 {errorflag=1;lyyerror("syntax error, maybe -");}
    | Exp MUL error                 {errorflag=1;lyyerror("syntax error, maybe *");}
    | Exp DIV error                 {errorflag=1;lyyerror("syntax error, maybe /");}
    | LP error RP                   {errorflag=1;lyyerror("missing ')'");}
    | SUB error                     {errorflag=1;lyyerror("syntax error, maybe -");}
    | LNOT error                    {errorflag=1;lyyerror("syntax error, maybe !");}
    | ID LP error RP                {errorflag=1;lyyerror("missing ')'");}
    | Exp LB error RB               {errorflag=1;lyyerror("missing ']'");}
    ;
Args : Exp COMMA Args               {$$=allocnewnode("Args",($1->lineno)," ");T->addchildren($$,3,$1,$2,$3);}
     | Exp                          {$$=allocnewnode("Args",($1->lineno)," ");T->addchild($$,$1);}
     ;


%%
/*int main()
{
    yypares();
}*/
void yyerror(char* msg){
    //fprintf(stderr," Type B at line %d: %s\n",yylloc.first_line, msg);
}

void lyyerror(char* msg)
{
    printf("Error Type B at line %d, column %d: %s\n",yylloc.first_line,yylloc.first_column, msg);
}

