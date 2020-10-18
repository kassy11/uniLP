/* token-list.h  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSTRSIZE 1024
#define NORMAL 0
#define ERROR 1
#define RINT 3
#define RCHAR 4
#define RBOOL 5
#define RARRAY 6

/* Token */
#define TNAME 1       /* Name : Alphabet { Alphabet | Digit } */
#define TPROGRAM 2    /* program : Keyword */
#define TVAR 3        /* var : Keyword */
#define TARRAY 4      /* array : Keyword */
#define TOF 5         /* of : Keyword */
#define TBEGIN 6      /* begin : Keyword */
#define TEND 7        /* end : Keyword */
#define TIF 8         /* if : Keyword */
#define TTHEN 9       /* then : Keyword */
#define TELSE 10      /* else : Keyword */
#define TPROCEDURE 11 /* procedure : Keyword */
#define TRETURN 12    /* return : Keyword */
#define TCALL 13      /* call : Keyword */
#define TWHILE 14     /* while : Keyword */
#define TDO 15        /* do : Keyword */
#define TNOT 16       /* not : Keyword */
#define TOR 17        /* or : Keyword */
#define TDIV 18       /* div : Keyword */
#define TAND 19       /* and : Keyword */
#define TCHAR 20      /* char : Keyword */
#define TINTEGER 21   /* integer : Keyword */
#define TBOOLEAN 22   /* boolean : Keyword */
#define TREADLN 23    /* readln : Keyword */
#define TWRITELN 24   /* writeln : Keyword */
#define TTRUE 25      /* true : Keyword */
#define TFALSE 26     /* false : Keyword */
#define TNUMBER 27    /* unsigned integer */
#define TSTRING 28    /* String */
#define TPLUS 29      /* + : symbol */
#define TMINUS 30     /* - : symbol */
#define TSTAR 31      /* * : symbol */
#define TEQUAL 32     /* = : symbol */
#define TNOTEQ 33     /* <> : symbol */
#define TLE 34        /* < : symbol */
#define TLEEQ 35      /* <= : symbol */
#define TGR 36        /* > : symbol */
#define TGREQ 37      /* >= : symbol */
#define TLPAREN 38    /* ( : symbol */
#define TRPAREN 39    /* ) : symbol */
#define TLSQPAREN 40  /* [ : symbol */
#define TRSQPAREN 41  /* ] : symbol */
#define TASSIGN 42    /* := : symbol */
#define TDOT 43       /* . : symbol */
#define TCOMMA 44     /* , : symbol */
#define TCOLON 45     /* : : symbol */
#define TSEMI 46      /* ; : symbol */
#define TREAD 47      /* read : Keyword */
#define TWRITE 48     /* write : Keyword */
#define TBREAK 49     /* break : Keyword */

#define NUMOFTOKEN 49

/* token-list.c */

#define KEYWORDSIZE 28
#define SYMBOLSIZE 18

  /* CR type */
#define TPINT 1
#define TPCHAR 2
#define TPBOOL 3
#define TPARRAY 4
#define TPARRAYINT 5
#define TPARRAYCHAR 6
#define TPARRAYBOOL 7
#define TPPROC 8


typedef struct KEY
{
  char *keyword;
  int keytoken;
}key;
extern key key_keyword[KEYWORDSIZE];
//extern key key_symbol[KEYWORDSIZE];

extern int error(char *mes);

/* scan.c */
extern int init_scan(char *filename);
extern int scan(void);
extern int num_attr;
extern char string_attr[MAXSTRSIZE];
extern int get_linenum(void);
extern void end_scan(void);

extern int linenum;
// id-list.c
extern void init_idtab(void);
extern struct ID *search_idtab(char *np);
extern void id_countup(char *np);
extern void print_idtab(void);
extern void release_idtab(void);

//LL.c
extern int next_token(void);
extern int parse_program(void);
extern int block(void);
extern int variable_declaration(void);
extern int variable_names(void);
extern int variable_name(void);
extern int type(void);
extern int standard_type(void);
extern int array_type(void);
extern int subprogram_declaration(void);
extern int procedure_name(void);
extern int formal_parameters(void);
extern int compound_statement(void);
extern int statement(void);
extern int condition_statement(void);
extern int iteration_statement(void);
extern int exit_statement(void);
extern int call_statement(void);
extern struct TYPE *expressions(void);
extern int return_statement(void);
extern int assignment_statement(void);
extern int left_part(void);
extern int variable(void);
extern int expression(void);
extern int simple_expression(void);
extern int term(void);
extern int factor(void);
extern int constant(void);
extern int multiplicative_operator(void);
extern int additive_operator(void);
extern int relational_operator(void);
extern int input_statement(void);
extern int output_statement(void);
extern int output_formal(void);
extern int empty_statement(void);

//CR.c
extern void init_globalcr(void);
extern void init_localcr(void);
extern void init_allcr(void);
extern struct ID *search_globalcr(char *np);
extern struct ID *search_localcr(char *np);
extern void cr_globalDeclaration(void);
extern void cr_localDeclaration(int ispara);
extern void cr_procedureDeclaration(void);
extern void cr_globalsettype(int type, int is_array);
extern void cr_localsettype(int type, int is_array);
extern void cr_procedure_setparatp(int type, int is_array,int paracount);
extern void cr_globalcountup(void);
extern void cr_localcountup(void);
extern void print_globalcr(void);
extern void print_localcr(void);
extern void print_allcr(void);
extern void release_globalcr(void);
extern void release_localcr(void);
extern void release_allcr(void);
extern void copy_local(void);
extern char current_proce_name[MAXSTRSIZE];
extern int is_procedure_para;
extern void cr_check_Recursive_call(void);
extern int check_standard_type_local(void);
extern int check_standard_type_global(void);
extern int check_variable_type_local(void);
extern int check_variable_type_global(void);
extern int check_proc(void);
extern struct TYPE *count_formal_parameters(void);