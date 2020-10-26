﻿
/* token-list.h  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAXSTRSIZE 1024

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

typedef struct KEY
{
    char *keyword;
    int keytoken;
}key;
extern key key_keyword[KEYWORDSIZE];
//extern key key_symbol[KEYWORDSIZE];

extern void error(char *mes);

/* scan.c */
extern int init_scan(char *filename);
extern int scan(void);
extern int num_attr; // scan()の戻り値が符号なし整数のとき、その値を格納
extern char string_attr[MAXSTRSIZE];
extern int get_linenum(void);
extern void end_scan(void);

// id-list.c
extern void init_idtab(void);
extern struct ID *search_idtab(char *np);
extern void id_countup(char *np);
extern void print_idtab(void);
extern void release_idtab(void);

//test-utils.c
extern void assert(int expected, int actual);
extern void assert_str(char *expected, char *actual);

// blackbox.c
extern void blackbox();
extern void init_scan_test();

// whitebox.c
//extern void whitebox();
//extern void search_idtab_test();
//extern void id_countup_test();
