%{
#include "stdio.h"
#include "stdlib.h"
#include "syntax.tab.h"
#include "syntaxtree.h"
#define D_Print 0
extern int errorflag;
int note2();
int yycolumn = 1;
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
    yylloc.first_column = yycolumn; \
    yylloc.last_column = yycolumn + yyleng - 1; \
    yycolumn += yyleng;
%}
%option yylineno

O   [0-7]
D   [0-9]
NZ  [1-9]
L   [a-zA-Z_]
A   [a-zA-Z_0-9]
H   [a-fA-F0-9]
HP  (0[xX])
E   ([Ee][+-]?{D}+)
P   ([Pp][+-]?{D}+)
FS  (f|F|l|L)
IS  (((u|U)(l|L|ll|LL)?)|((l|L|ll|LL)(u|U)?))
CP  (u|U|L)
SP  (u8|u|U|L)
ES  (\\(['"\?\\abfnrtv]|[0-7]{1,3}|x[a-fA-F0-9]+))
WS  [ \t\v\f]

INT_DEC         {NZ}{D}*{IS}?
INT_HEX         {HP}{H}+{IS}?
INT_OCT         [0]{O}*{IS}?
INT             ({INT_DEC})|({INT_HEX})|({INT_OCT})|({CP}?[']([^'\\\n]|{ES})+[']])
/*
FLOAT           [0-9]*[\.][0-9]+([eE][+-]?[0-9]*|[0])?f?
*/
/*
FLOAT_DOT       (([0-9]*[\.][0-9]+)|([0-9]+[\.][0-9]*)f?)|([0-9]+f)
FLOAT_EXP       [0-9]*[\.]?[0-9]+[eE][+-]?[0-9]+f?
*/

FLOAT1         {D}+{E}{FS}?
FLOAT2         {D}*[\.]{D}+{E}?{FS}?
FLOAT3         {D}+[\.]{E}?{FS}?
FLOAT4         {HP}{H}+{P}{FS}?
FLOAT5         {HP}{H}*[\.]{H}+{P}{FS}?
FLOAT6         {HP}{H}+[\.]{P}{FS}?
FLOAT7         {D}+f
FLOAT          {FLOAT1}|{FLOAT2}|{FLOAT3}|{FLOAT4}|{FLOAT5}|{FLOAT6}|{FLOAT7}

ID          {L}{A}*
SEMI        [;]
COMMA       [,]
ASSIGNOP    [=]
RELOP       [>]|[<]|[>][=]|[<][=]|[=][=]|[!][=](^[=])
PLUS        [+]
MINUS       [-]
STAR        [*]
DIV         [/]
AND         [&][&]
OR          [|][|]
DOT         [\.]
NOT         [!]
TYPE        int|float
LP          \(
RP          \)
LB          \[
RB          \]
LC          \{
RC          \}
STRUCT      struct
RETURN      return
IF          if
ELSE        else 
WHILE       while
NOTE1       [/][/]
NOTE2       [/][*]
SPACE       [ ]
ENTER       [\n]
TABLE       [\t]
/*end of definition*/

%%

{INT} {
//    yylval.type_int = atoi(yytext);
    #if D_Print
    printf("get an integer NUM: %s\n",yytext); 
    #endif
    yylval.syntaxtreenode_t = allocnewnode("INT",yylineno,yytext);
    return INT;
}
{FLOAT} { 
//    yylval.type_float = atof(yytext);
    #if D_Print
    printf("get a float NUM: %s\n",yytext); 
    #endif
    yylval.syntaxtreenode_t = allocnewnode("FLOAT",yylineno,yytext);   return FLOAT; 
}
{SEMI}  { 
    #if D_Print
    printf("get a SEMI %s\n",yytext); 
    #endif
    yylval.syntaxtreenode_t = allocnewnode("SEMI",yylineno,yytext);   return SEMI; 
}
{COMMA} { 
    #if D_Print
    printf("get a COMMA %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("COMMA",yylineno,yytext);   return COMMA; 
}
{ASSIGNOP} { 
    #if D_Print
    printf("get an ASSIGNOP %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("ASSIGNOP",yylineno,yytext);   return ASSIGNOP;
}

{PLUS} { 
    #if D_Print
    printf("get an ADD %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("ADD",yylineno,yytext);   return ADD; 
}
{MINUS} { 
    #if D_Print
    printf("get a SUB %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("SUB",yylineno,yytext);   return SUB; 
}
{DIV} { 
    #if D_Print
    printf("get a DIV %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("DIV",yylineno,yytext);   return DIV; 
}
{STAR} { 
    #if D_Print
    printf("get a MUL %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("MUL",yylineno,yytext);   return MUL; 
}

{RELOP} { 
    #if D_Print
    printf("get a RELOP %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("RELOP",yylineno,yytext);   return RELOP; 
}

{AND} { 
    #if D_Print
    printf("get an AND %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("AND",yylineno,yytext);   return LAND; 
}
{OR}  { 
    #if D_Print
    printf("get an OR %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("OR",yylineno,yytext);   return LOR; 
}
{NOT} { 
    #if D_Print
    printf("get a NOT %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("NOT",yylineno,yytext);   return LNOT; 
}

{DOT} { 
    #if D_Print
    printf("get a DOT %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("DOT",yylineno,yytext);   return DOT; 
}

{STRUCT} {
    #if D_Print
    printf("get a key word STRUCT %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("STRUCT",yylineno,yytext);   return STRUCT; 
}
{RETURN} {
    #if D_Print
    printf("get a key word RETURN %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("RETURN",yylineno,yytext);   return RETURN; 
}
{IF}  { 
    #if D_Print
    printf("get a key word IF %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("IF",yylineno,yytext);   return IF; 
}
{ELSE} { 
    #if D_Print
    printf("get a key word ELSE %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("ELSE",yylineno,yytext);   return ELSE; 
}
{WHILE} { 
    #if D_Print
    printf("get a key word WHILE %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("WHILE",yylineno,yytext);   return WHILE; 
}
{TYPE} { 
    #if D_Print
    printf("get a key word: TYPE %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("TYPE",yylineno,yytext);   return TYPE; 
}

{LP} { 
    #if D_Print
    printf("get a LP %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("LP",yylineno,yytext);   return LP; 
}
{RP} {
    #if D_Print
    printf("get a RP %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("RP",yylineno,yytext);   return RP; 
}
{LB} { 
    #if D_Print
    printf("get a LB %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("LB",yylineno,yytext);   return LB; 
}
{RB} { 
    #if D_Print
    printf("get a RB %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("RB",yylineno,yytext);   return RB; 
}
{LC} { 
    #if D_Print
    printf("get a LC %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("LC",yylineno,yytext);   return LC; 
}
{RC} { 
    #if D_Print
    printf("get a RC %s\n",yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("RC",yylineno,yytext);   return RC; 
}

{NOTE1} { 
char c = input();
while (c != '\n') c=input();
}
{NOTE2} { note2();}
{SPACE} {
/*ABANDON THESE CHARACTORS*/
//printf("get a SPACE \n")*/
}
{ENTER} { yycolumn = 1; }
{TABLE} { 
/*ABANDON THESE CHARACTORS*/
//printf("get a SPACE \n")m
}
{ID} {
    #if D_Print
    printf("get an ID: %s\n", yytext);
    #endif
    yylval.syntaxtreenode_t = allocnewnode("ID",yylineno,yytext);   return ID;
}

. { printf("Error type A at line %d: Mysterious characters \'%s\'\n",
       yylineno,yytext);
    errorflag=1;
//   return UNKT;
}


%%
int yywrap() {
return 1;
}
int note2()
{
char c,c_next;
while(1)
{
    c = input();
    if(c == '*')
    {
        c_next = input();
        if(c_next == '/')
            break;
        else
            unput(c_next);
    }
}
return 0;
}
/*
int main(int argc, char** argv) {
if (argc > 1) {
       if (!(yyin = fopen(argv[1], "r"))) {   
           perror(argv[1]);
           return 1;
       }
   }
   while (yylex());
   return 0;
}
*/