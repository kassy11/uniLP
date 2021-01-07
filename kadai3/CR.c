#include "token-list.h"
extern char current_proce_name[MAXSTRSIZE];
extern char string_attr[MAXSTRSIZE];
extern int is_procedure_para;
struct TYPE
{
  int ttype; /* TPINT TPCHAR TPBOOL TPARRAY TPARRAYINT TPARRAYCHAR TPARRAYBOOL TPPROC */
  int arraysize; /* size of array, if TPARRAY */
  struct TYPE *etp; /* pointer to element type if TPARRAY */
  struct TYPE *paratp; /* pointer to parameter's type list if ttype is TPPROC */
};
struct LINE{
  int reflinenum;
  struct LINE *nextlinep;
};
struct ID{
  char *name;
  char *procname; /* procedure name within which this name is defined */ /* NULL if global name */
  struct TYPE *itp;
  int ispara; /* 1:formal parameter, 0:else(variable) */
  int deflinenum;//宣言された行数
  struct LINE *irefp;
  struct ID *nextp;
} * globalidroot, *localidroot, *alllocalidroot; /* Pointers to root of global & local symbol tables */
void init_globalcr()
{ /* Initialise the table */
  globalidroot = NULL;
}
void init_localcr()
{ /* Initialise the table */

  localidroot = NULL;
}
void init_allcr()
{
  alllocalidroot = NULL;
}
struct ID *search_globalcr(char *np)
{ /* search the name pointed by np */
  struct ID *p;
  for (p = globalidroot; p != NULL; p = p->nextp)
  {
    if (strcmp(np, p->name) == 0)
      return (p);
  }
  return (NULL);
}
struct ID *search_localcr(char *np)
{ /* search the name pointed by np */
  struct ID *p;

  for (p = localidroot; p != NULL; p = p->nextp)
  {
    if (strcmp(np, p->name) == 0)
      return (p);
  }
  return (NULL);
}
void cr_globalDeclaration() //変数宣言時変数だけ
{
  struct ID *newid;
  if ((newid = (struct ID *)malloc(sizeof(struct ID))) == NULL)
  {
    printf("can not malloc in cr_globalcountup : newid\n");
    return;
  }
  if(search_globalcr(string_attr) != NULL)
    error("Duplicate variable");

  if((newid->name = (char *)malloc(strlen(string_attr) + 1)) == NULL)
    printf("can not malloc globalDeclaration\n");
  else
    strcpy(newid->name,string_attr);
  newid->procname = NULL;
  newid->itp = NULL;
  newid->ispara = 0;
  newid->deflinenum = get_linenum();
  newid->irefp = NULL;
  newid->nextp = globalidroot;
  globalidroot = newid;
}
void cr_localDeclaration(int ispara) //変数宣言時変数だけ
{
  struct ID *newid;
  if ((newid = (struct ID *)malloc(sizeof(struct ID))) == NULL)
  {
    printf("can not malloc in cr_globalcountup : newid\n");
    return;
  }
  if (search_localcr(string_attr) != NULL)
    error("Duplicate variable");
  if ((newid->name = (char *)malloc(strlen(string_attr) + 1)) == NULL)
    printf("can not malloc globalDeclaration\n");
  else
    strcpy(newid->name, string_attr);
  if ((newid->procname = (char *)malloc(strlen(string_attr) + 1)) == NULL)
    printf("can not malloc globalDeclaration\n");
  else
    strcpy(newid->procname, current_proce_name);
  newid->itp = NULL;
  newid->ispara = ispara;
  newid->deflinenum = get_linenum();
  newid->irefp = NULL;
  newid->nextp = localidroot;
  localidroot = newid;
}
void cr_procedureDeclaration()
{
  struct ID *newid;
  struct TYPE *proctype;
  if ((newid = (struct ID *)malloc(sizeof(struct ID))) == NULL)
  {
    printf("can not malloc in cr_procedureDeclaration\n");
    return;
  }
  if ((proctype = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
  {
    printf("can not malloc in cr_procedureDeclaration\n");
    return;
  }
  if (search_globalcr(string_attr) != NULL)
    error("Duplicate variable");
  
  if ((newid->name = (char *)malloc(strlen(string_attr) + 1)) == NULL)
    printf("can not malloc cr_procedureDeclaration\n");
  else
    strcpy(newid->name, string_attr);

  if ((newid->procname = (char *)malloc(strlen(string_attr) + 1)) == NULL)
    printf("can not malloc cr_procedureDeclaration\n");
  else
    strcpy(newid->procname, current_proce_name);

  proctype->ttype = TPPROC;
  proctype->arraysize = -1;
  proctype->paratp = NULL;
  newid->itp = proctype;
  newid->ispara = 0;

  newid->deflinenum = get_linenum();

  
  newid->irefp = NULL;
  newid->nextp = globalidroot;
  globalidroot = newid;
  
  strcpy(current_proce_name, string_attr);
}


void cr_globalsettype(int type, int is_array)
{
  
  struct ID *p;
  for(p = globalidroot;p != NULL;p = p->nextp)
  {
    if(p->itp == NULL)
    {
      struct TYPE *newtype;
      if ((newtype = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
      {
        printf("can not malloc in cr_globalsettype\n");
        return;
      }
      if(is_array == 0)
      {
        if (type == TCHAR)
          newtype->ttype = TPCHAR;
        else if (type == TINTEGER)
          newtype->ttype = TPINT;
        else if (type == TBOOLEAN)
          newtype->ttype = TPBOOL;

        newtype->arraysize = -1;
        newtype->etp = NULL;
      }
      else
      {
        struct TYPE *tarray;
        if ((tarray = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
        {
          printf("can not malloc in cr_globalsettype\n");
          return;
        }

        if (type == TCHAR)
          tarray->ttype = TPARRAYCHAR;
        else if (type == TINTEGER)
          tarray->ttype = TPARRAYINT;
        else if (type == TBOOLEAN)
          tarray->ttype = TPARRAYBOOL;
        
        newtype->ttype = TPARRAY;
        newtype->arraysize = num_attr;
        newtype->etp = tarray;
      }
      
      newtype->paratp = NULL;

      p->itp = newtype;
    }
  }
}
void cr_localsettype(int type, int is_array)
{
  if(is_procedure_para == 1 && is_array == 1)
  {
    error("formal parameters must not be array");
  }
  struct ID *p;
  for (p = localidroot; p != NULL; p = p->nextp)
  {
    if (p->itp == NULL)
    {
      struct TYPE *newtype;
      if ((newtype = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
      {
        printf("can not malloc in cr_localsettype\n");
        return;
      }
      if (is_array == 0)
      {
        if (type == TCHAR)
          newtype->ttype = TPCHAR;
        else if (type == TINTEGER)
          newtype->ttype = TPINT;
        else if (type == TBOOLEAN)
          newtype->ttype = TPBOOL;

        newtype->arraysize = -1;
        newtype->etp = NULL;
      }
      else
      {
        struct TYPE *tarray;
        if ((tarray = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
        {
          printf("can not malloc in cr_localsettype\n");
          return;
        }
        if (type == TCHAR)
          tarray->ttype = TPARRAYCHAR;
        else if (type == TINTEGER)
          tarray->ttype = TPARRAYINT;
        else if (type == TBOOLEAN)
          tarray->ttype = TPARRAYBOOL;

        newtype->ttype = TPARRAY;
        newtype->arraysize = num_attr;
        newtype->etp = tarray;
      }

      newtype->paratp = NULL;

      p->itp = newtype;
    }
  }
}
void cr_procedure_setparatp(int type,int is_array,int paracount)
{
  printf("\ncr_procedure_setparatp\n"); 
  struct ID *l;
  for(l = globalidroot;l != NULL;l = l->nextp)
  {
    if(l->itp->ttype == TPPROC && strcmp(l->name,current_proce_name) == 0)
    {
      for(int i = 0;i < paracount;i++)
      {
        printf("ss");
        struct TYPE *proctype;
        if ((proctype = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL) //領域確保
        {
          printf("can not malloc in cr_procedure_setparatp\n");
          return;
        }
        
        if (is_array == 0)
        {
          if (type == TCHAR)
            proctype->ttype = TPCHAR;
          else if (type == TINTEGER)
            proctype->ttype = TPINT;
          else if (type == TBOOLEAN)
            proctype->ttype = TPBOOL;

          proctype->arraysize = -1;
          proctype->etp = NULL;
          proctype->paratp = NULL;
        }
        else
        {
          struct TYPE *tarray;
          if ((tarray = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
          {
            printf("can not malloc in cr_globalsettype\n");
            return;
          }
          if (type == TCHAR)
            tarray->ttype = TPARRAYCHAR;
          else if (type == TINTEGER)
            tarray->ttype = TPARRAYINT;
          else if (type == TBOOLEAN)
            tarray->ttype = TPARRAYBOOL;

          proctype->ttype = TPARRAY;
          proctype->arraysize = num_attr;
          proctype->etp = tarray;
          proctype->paratp = NULL;
        }
        struct TYPE *t, *before_t;
        for (t = l->itp; t != NULL; t = t->paratp)
        {
          before_t = t;
          
        }
        before_t->paratp = proctype;

      }
      
      return;
    }
  }
  return;
}
void cr_globalcountup()
{ /* Register and count up the name pointed by np */
  struct ID *p;
  char *cp;
  if ((p = search_globalcr(string_attr)) != NULL)
  {
    struct LINE *newl;                                           //末尾につけるLINE
    if((newl = (struct LINE *)malloc(sizeof(struct LINE))) == NULL)//領域確保
    {
      printf("can not malloc in cr_globalcountup\n");
      return;
    }
    newl->reflinenum = get_linenum();//行数取得
    newl->nextlinep = NULL;//次はまだ未定なのでNULL

    struct LINE *l,*before_l;
    if((l = (struct LINE *)malloc(sizeof(struct LINE))) == NULL)//領域確保
    {
      printf("can not malloc in cr_globalcountup\n");
      return;
    }
    if ((before_l = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) //領域確保
    {
      printf("can not malloc in cr_globalcountup\n");
      return;
    }
    //before_l = NULL;

    if (p->irefp == NULL) //初回のみ
    {
      p->irefp = newl;
    }
    else
    {
      for (l = p->irefp; l != NULL; l = l->nextlinep)
      {
        before_l = l;
      }
      before_l->nextlinep = newl;
    }
  }
  else
    error("There are undefined variables : global");
}
void cr_localcountup()
{ /* Register and count up the name pointed by np */
  struct ID *p;
  char *cp;
  if ((p = search_localcr(string_attr)) != NULL)
  {
    struct LINE *newl;                                               //末尾につけるLINE
    if ((newl = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) //領域確保
    {
      printf("can not malloc in cr_globalcountup\n");
      return;
    }
    newl->reflinenum = get_linenum(); //行数取得
    newl->nextlinep = NULL;           //次はまだ未定なのでNULL

    struct LINE *l, *before_l;
    if ((l = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) //領域確保
    {
      printf("can not malloc in cr_localcountup\n");
      return;
    }
    if ((before_l = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) //領域確保
    {
      printf("can not malloc in cr_localcountup\n");
      return;
    }
    //before_l = NULL;
    if (p->irefp == NULL)
      p->irefp = newl;
    else
    {
      for (l = p->irefp; l != NULL; l = l->nextlinep)
      {
        before_l = l;
      }
      before_l->nextlinep = newl;
    }
  }
  else
    error("There are undefined variables : local");
}
void print_globalcr()
{ /* Output the registered data */
  struct ID *p;
  printf("-----------print global-------------\n");
  printf("NAME\tTYPE\t\tDef\tRef\n");
    printf("----------------------------------------------------\n");
  for (p = globalidroot; p != NULL; p = p->nextp)
  {
    if(p->itp->ttype == TPINT)
    {
      printf("%s\tinteger\t\t%d |", p->name, p->deflinenum);
    }
    else if (p->itp->ttype == TPCHAR)
    {
      printf("%s\tchar\t\t%d |", p->name, p->deflinenum);
    }
    else if (p->itp->ttype == TPBOOL)
    {
      printf("%s\tboolean\t\t%d |", p->name, p->deflinenum);
    }
    else if(p->itp->ttype == TPARRAY)
    {
      if (p->itp->etp->ttype == TPARRAYINT)
      {
        printf("%s\tarray[%d] of integer\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
      else if (p->itp->etp->ttype == TPARRAYCHAR)
      {
        printf("%s\tarray[%d] of char\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
      else if (p->itp->etp->ttype == TPARRAYBOOL)
      {
        printf("%s\tarray[%d] of boolean\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
    }
    
    else if(p->itp->ttype == TPPROC)
    {
      printf("%s\tprocedure", p->name);
      if(p->itp->paratp != NULL)
      {
        printf("(");
        struct TYPE *t;
        for(t = p->itp->paratp;t != NULL;t = t->paratp)
        {
          if(t->ttype == TPINT) 
            printf("integer");
          else if (t->ttype ==TPCHAR) 
            printf("char");
          else if (t->ttype ==TPBOOL)
            printf("boolean");
          else if (t->ttype == TPARRAY)
          {
            if (t->paratp->ttype == TPARRAYINT)
              printf("array[%d] of integer",t->arraysize);
            else if (t->paratp->ttype == TPARRAYCHAR)
              printf("array[%d] of char", t->arraysize);
            else if (t->paratp->ttype == TPARRAYBOOL)
              printf("array[%d] of boolean", t->arraysize);
          }
        }
        printf(")");
      }
      printf("\t\t %d | ", p->deflinenum);
    }
    struct LINE *l;
    for(l = p->irefp;l != NULL;l = l->nextlinep)
    {
      printf(" %d",l->reflinenum);
    }
    printf("\n");
  }
  printf("----------------------------------------------\n");
}
void print_localcr()
{ /* Output the registered data */
  struct ID *p;

  for (p = localidroot; p != NULL; p = p->nextp)
  {
    //printf("======%d==========\n", p->itp->ttype);
    if (p->itp->ttype == TPINT)
    {
      printf("%s:%s \tinteger\t\t%d |",p->name,p->procname, p->deflinenum);
    }
    else if (p->itp->ttype == TPCHAR)
    {
      printf("%s:%s \tchar\t\t%d |",p->name,p->procname, p->deflinenum);
    }
    else if (p->itp->ttype == TPBOOL)
    {
      printf("%s:%s \tboolean\t\t%d |",p->name,p->procname, p->deflinenum);
    }
    else if (p->itp->ttype == TPARRAY)
    {
      if (p->itp->etp->ttype == TPARRAYINT)
      {
        printf("%s\tarray[%d] of integer\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
      else if (p->itp->etp->ttype == TPARRAYCHAR)
      {
        printf("%s\tarray[%d] of char\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
      else if (p->itp->etp->ttype == TPARRAYBOOL)
      {
        printf("%s\tarray[%d] of boolean\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
    }

    else if (p->itp->ttype == TPPROC)
    {
      printf("%s\tprocedure", p->name);
      if (p->itp->paratp != NULL)
      {
        printf("(");
        struct TYPE *t;
        for (t = p->itp->paratp; t != NULL; t = t->paratp)
        {
          if (t->ttype == TPINT)
            printf("integer");
          else if (t->ttype == TPCHAR)
            printf("char");
          else if (t->ttype == TPBOOL)
            printf("boolean");
          else if (t->ttype == TPARRAY)
          {
            if (t->paratp->ttype == TPARRAYINT)
              printf("array[%d] of integer", t->arraysize);
            else if (t->paratp->ttype == TPARRAYCHAR)
              printf("array[%d] of char", t->arraysize);
            else if (t->paratp->ttype == TPARRAYBOOL)
              printf("array[%d] of boolean", t->arraysize);
          }
        }
        printf(")");
      }
      printf("\t\t %d | ", p->deflinenum);
    }
    struct LINE *l;
    for (l = p->irefp; l != NULL; l = l->nextlinep)
    {
      printf(" %d", l->reflinenum);
    }
    printf("\n");
  }
}
void print_allcr()
{
  struct ID *p;
  
  for (p = alllocalidroot; p != NULL; p = p->nextp)
  {
    
    if (p->itp->ttype == TPINT)
    {
      printf("%s:%s \tinteger\t\t%d |", p->name, p->procname, p->deflinenum);
    }
    else if (p->itp->ttype == TPCHAR)
    {
      printf("%s:%s \tchar\t\t%d |", p->name, p->procname, p->deflinenum);
    }
    else if (p->itp->ttype == TPBOOL)
    {
      printf("%s:%s \tboolean\t\t%d |", p->name, p->procname, p->deflinenum);
    }
    else if (p->itp->ttype == TPARRAY)
    {
      if (p->itp->etp->ttype == TPARRAYINT)
      {
        printf("%s\tarray[%d] of integer\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
      else if (p->itp->etp->ttype == TPARRAYCHAR)
      {
        printf("%s\tarray[%d] of char\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
      else if (p->itp->etp->ttype == TPARRAYBOOL)
      {
        printf("%s\tarray[%d] of boolean\t\t%d |", p->name, p->itp->arraysize, p->deflinenum);
      }
    }

    else if (p->itp->ttype == TPPROC)
    {
      printf("%s\tprocedure", p->name);
      if (p->itp->paratp != NULL)
      {
        printf("(");
        struct TYPE *t;
        for (t = p->itp->paratp; t != NULL; t = t->paratp)
        {
          if (t->ttype == TPINT)
            printf("integer");
          else if (t->ttype == TPCHAR)
            printf("char");
          else if (t->ttype == TPBOOL)
            printf("boolean");
          else if (t->ttype == TPARRAY)
          {
            if (t->paratp->ttype == TPARRAYINT)
              printf("array[%d] of integer", t->arraysize);
            else if (t->paratp->ttype == TPARRAYCHAR)
              printf("array[%d] of char", t->arraysize);
            else if (t->paratp->ttype == TPARRAYBOOL)
              printf("array[%d] of boolean", t->arraysize);
          }
        }
        printf(")");
      }
      printf("\t\t %d | ", p->deflinenum);
    }
    struct LINE *l;
    if ((l = (struct LINE *)malloc(sizeof(struct LINE))) == NULL) //領域確保
    {
      printf("can not malloc in cr_localcountup\n");
      return;
    }
    l = p->irefp;
    while(l != NULL)
    {
      printf(" %d", l->reflinenum);
      if(l->nextlinep != NULL)
        l = l->nextlinep;
      else
        break;
    }
    printf("\n");
  }
}
void release_globalcr()
{ /* Release tha data structure */
  struct ID *p, *q;
  struct TYPE *t, *u;
  struct LINE *l, *m;
  for (p = globalidroot; p != NULL; p = q)
  {
    free(p->name);
    free(p->procname);


    for(t = p->itp; t != NULL; t = u)
    {
      if(t->ttype == TPARRAY)
        free(t->etp);
      u = t->paratp;
      free(t);
    }
    for (l = p->irefp; l != NULL; l = m)
    {
      m = l->nextlinep;
      free(l);
    }
    q = p->nextp;
    free(p);
  }
  init_globalcr();
}
void release_allcr()
{
  struct ID *p, *q;
  struct TYPE *t, *u;

  struct LINE *l, *m;
  for (p = alllocalidroot; p != NULL; p = q)
  {
    free(p->name);
    free(p->procname);
    for (t = p->itp; t != NULL; t = u)
    {
      if (t->ttype == TPARRAY)
        free(t->etp);
      u = t->paratp;
      free(t);
    }

    for (l = p->irefp; l != NULL; l = m)
    {
      m = l->nextlinep;
      free(l);
    }

    q = p->nextp;
    free(p);
  }
  init_allcr();
}
void release_localcr()
{ /* Release tha data structure */
  struct ID *p, *q;
  struct TYPE *t, *u;
  struct LINE *l,*m;
  for (p = localidroot; p != NULL; p = q)
  {
    free(p->name);
    free(p->procname);
    for (t = p->itp; t != NULL; t = u)
    {
      if (t->ttype == TPARRAY)
        free(t->etp);
      u = t->paratp;
      free(t);
    }
    for(l = p->irefp;l != NULL;l = m)
    {
      m = l->nextlinep;
      free(l);
    }
    q = p->nextp;
    free(p);
  }
  init_localcr();
}

void copy_local()
{
  struct ID *l;
  if ((l = (struct ID *)malloc(sizeof(struct ID))) == NULL) //領域確保
  {
    printf("can not malloc in cr_localcountup\n");
    return;
  }
  
  for (l = localidroot;l != NULL;l = l->nextp)
  {
    struct ID *all;
    if ((all = (struct ID *)malloc(sizeof(struct ID))) == NULL) //領域確保
    {
      printf("can not malloc in cr_localcountup\n");
      return;
    }
    if ((all->name = (char *)malloc(strlen(l->name) + 1)) == NULL)
    {
      printf("can not malloc in copy_local\n");
      return;
    }
    strcpy(all->name,l->name);
    if ((all->procname = (char *)malloc(strlen(l->procname) + 1)) == NULL)
    {
      printf("can not malloc in copy_local\n");
      return;
    }
    strcpy(all->procname, l->procname);
    struct TYPE *t, *alt;
    if ((alt = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
    {
      printf("can not malloc in copy_local\n");
      return;
    }
    
    //配列の時はその要素の型まで追う
    if (l->itp->ttype == TPARRAY)
    {
      fflush(stdout);
      alt = all->itp;
      alt->ttype = l->itp->ttype;
      alt->arraysize = l->itp->arraysize;
      struct TYPE *newetp;
      if ((newetp = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
      {
        printf("can not malloc in copy_local\n");
        return;
      }
      newetp->ttype = l->itp->etp->ttype;
      newetp->arraysize = l->itp->etp->arraysize;
      alt->etp = newetp;
      free(newetp);
    }
    else
    {
      alt->ttype = l->itp->ttype;
      alt->arraysize = l->itp->arraysize;
      alt->etp = NULL;
      alt->paratp = NULL;
      all->itp = alt;
    }
    all->ispara = l->ispara;
    all->deflinenum = l->deflinenum;

    if(l->irefp != NULL)
    {
      struct LINE *newline,*temp;
      if ((newline = (struct LINE *)malloc(sizeof(struct LINE))) == NULL)
      {
        printf("can not malloc in copy_local\n");
        return;
      }
      newline->reflinenum = l->irefp->reflinenum;
      newline->nextlinep = NULL;
      all->irefp = newline;
      for(temp = l->irefp;temp != NULL;temp = temp->nextlinep)
      {
        
        struct LINE *line,*nl,*before_nl,*ll;
        if ((line = (struct LINE *)malloc(sizeof(struct LINE))) == NULL)
        {
          printf("can not malloc in copy_local\n");
          return;
        }
        if ((ll = (struct LINE *)malloc(sizeof(struct LINE))) == NULL)
        {
          printf("can not malloc in copy_local\n");
          return;
        }
        for(nl = newline;nl != NULL;nl = nl->nextlinep)
        {
          before_nl = nl;
          if(nl->reflinenum == 0)
            break;
        }
        before_nl->reflinenum = temp->reflinenum;
        if(temp->nextlinep == NULL)
          before_nl->nextlinep = NULL;
        else
          before_nl->nextlinep = ll;
      }
    }
    else
      all->irefp = NULL;
    
    all->nextp = alllocalidroot;
    alllocalidroot = all;
  }
}
void cr_check_Recursive_call()
{
  struct ID *p;
  if ((p = search_globalcr(string_attr)) != NULL)
  {
    if(strcmp(current_proce_name,string_attr) == 0)
    {
      printf("current_proc_name : %s : string attr : %s\n",current_proce_name,string_attr);
      error("Recursive calls can not be made");
    }
  }
}
int check_standard_type_local()
{
  struct ID *p;
  if ((p = search_localcr(string_attr)) != NULL)
  {
    if(p->itp->ttype == TPARRAY)
      return (error("The result of the standard type expression of factor must be standard type : local"));
    else if (p->itp->ttype == TPINT)
      return (RINT);
    else if (p->itp->ttype == TPCHAR)
      return (RCHAR);
    else if (p->itp->ttype == TPBOOL)
      return (RBOOL);
  }
  return(NORMAL);
}
int check_standard_type_global()
{
  //printf("CCCCCCCCCC");
  struct ID *p;
  if ((p = search_globalcr(string_attr)) != NULL)
  {
    if (p->itp->ttype >= TPARRAY)
      return (error("The result of the standard type expression of factor must be standard type : global"));
    else if (p->itp->ttype == TPINT)
      return (RINT);
    else if (p->itp->ttype == TPCHAR)
      return (RCHAR);
    else if (p->itp->ttype == TPBOOL)
      return (RBOOL);
  }
  return (NORMAL);
}
int check_variable_type_local()
{
  struct ID *p;
  if ((p = search_localcr(string_attr)) != NULL)
  {
    if(p->itp->ttype == TPINT)
      return(RINT);
    else if (p->itp->ttype == TPCHAR)
      return (RCHAR);
    else if (p->itp->ttype == TPBOOL)
      return (RBOOL);
    else if(p->itp->ttype == TPARRAY)
      return (RARRAY);
  }
  return(ERROR);
}
int check_variable_type_global()
{
  struct ID *p;
  if ((p = search_globalcr(string_attr)) != NULL)
  {
    if (p->itp->ttype == TPINT)
      return (RINT);
    else if (p->itp->ttype == TPCHAR)
      return (RCHAR);
    else if (p->itp->ttype == TPBOOL)
      return (RBOOL);
    else if (p->itp->ttype == TPARRAY)
      return (RARRAY);
  }
  return (ERROR);
}
int check_proc()
{
  struct ID *p;
  if ((p = search_globalcr(string_attr)) != NULL)
  {
    if(p->itp->ttype != TPPROC) return(error("This name is no Procedure"));
    else return(NORMAL);
  }
  return(ERROR);
}
struct TYPE *count_formal_parameters()
{
  struct ID *p;
  struct TYPE *t,*result,*r;
  if ((p = search_globalcr(string_attr)) != NULL)
  {
    if(p->itp->ttype == TPPROC)
    {
      return(p->itp);
    }
  }
  return(NULL);
}