#include "token-list.h"
extern int token;
extern int linenum;
extern char *tokenstr[NUMOFTOKEN + 1];
extern int tabnum;
extern int compound_tab[10];
int is_begin_line = 1; 
int is_procedure_begintoend = 0;
int compound_count = 0;
int is_variable_declaration = 0;//現在変数宣言部かどうか
int is_subprogram_declaration = 0;//現在副プログラム部かどうか
char current_proce_name[MAXSTRSIZE]; //現在の副プログラムの名前
int is_array = 0;//現在arrayかどうか
int is_procedure_para = 0;//現在procedureの仮引数部かどうか
int procedure_para_count = 0;//procedureの仮引数の数
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
};
int next_token() //最終的に削除される
{
  int before_token = token;
  int token_num = scan();
  if(token_num == -1)
    return token_num;
  else if(token_num == TPROCEDURE)
  {
    is_procedure_begintoend = 1;
    tabnum = 1;
  }
  else if(token_num == TVAR)
  {
    if(is_procedure_begintoend)
      tabnum = 2;
    else
      tabnum = 1;
  }
  else if(token_num == TEND)
  {
    if(before_token != TSEMI)
    {
      printf("\n");
      linenum++;
    }
    is_begin_line = 1;
    
    tabnum = compound_tab[compound_count-1];
    
    compound_count--;
    if(compound_count == 0)
      is_procedure_begintoend = 0;
  }
  else if(token_num == TBEGIN)
  {
    if(before_token != TSEMI)
    {
      printf("\n");
      linenum++;
    }
    compound_count++;
    is_begin_line = 1;
    
    if(compound_count == 1 && is_procedure_begintoend == 0)
      tabnum = 0;
    if(compound_count == 1 && is_procedure_begintoend == 1)
      tabnum = 1;

    compound_tab[compound_count - 1] = tabnum;
  }
  else if(token_num == TELSE)
  {
    printf("\n");
    linenum++;
    is_begin_line = 1;
  }
  if ((before_token == TTHEN || before_token == TELSE || before_token == TDO) && (token_num != TBEGIN))
  {
    if((before_token == TELSE) && (token_num != TIF))
    {
      printf("\n");
      linenum++;
      tabnum++;
      is_begin_line = 1;
    }
  }
  if(is_begin_line)
  {
    is_begin_line = 0;
    for(int i = 0;i < tabnum * 4;i++)
      printf(" ");
  }
  else if(token_num != TSEMI && token_num != TSTRING && token_num != TRPAREN  && token_num != TDOT )
    if(!(before_token == TLPAREN && token_num != TSTRING))
      printf(" ");

  //-----------↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
  if(is_variable_declaration == 1 && token_num == TNAME)//変数宣言時
  {
    if(is_subprogram_declaration == 1)//副プログラムならlocal
    {
      cr_localDeclaration(0);
    }
    else//global
    {
      cr_globalDeclaration();
    }
  }
  else if(is_variable_declaration == 1 && (token_num == TCHAR || token_num == TINTEGER || token_num == TBOOLEAN))
  {//型名セット
    if (is_subprogram_declaration == 1) //副プログラムならlocal
    {
      cr_localsettype(token_num,is_array);
    }
    else
    {
      cr_globalsettype(token_num, is_array);
    }
  }
  else if(before_token == TPROCEDURE && token_num == TNAME)
  {
    //procedure宣言
    procedure_para_count = 0;
    cr_procedureDeclaration();
  }
  else if(is_procedure_para == 1 && token_num == TNAME)
  {
    //procedure変数セット(パラメータの)
    procedure_para_count++;
    cr_localDeclaration(1);
  }
  else if (is_procedure_para == 1 && (token_num == TCHAR || token_num == TINTEGER || token_num == TBOOLEAN))
  {
    //procesdure型セット(パラメータの)
    cr_localsettype(token_num, is_array);
    //さっき定義したprocedureのparatpを設定しないといけない
    cr_procedure_setparatp(token_num,is_array,procedure_para_count);
    procedure_para_count = 0;
    }
  else if (is_variable_declaration == 0 && token_num == TNAME && before_token != TPROGRAM)
  {                 //参照されたらrefに記録する．ここもprocedureが必要
    if (is_subprogram_declaration == 1) //副プログラムならlocal
    {
      if(search_localcr(string_attr) != NULL)
      {
        cr_localcountup();
      }
      else if(search_globalcr(string_attr) != NULL)
      {
        //再帰はできないルールの追加
        cr_check_Recursive_call();
        cr_globalcountup();
      }
      else
        error("There are undefined variables ");
    }
    else
    {
      cr_globalcountup();
    }
  }
  //-----------↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
  if(token_num != TNAME && token_num != TNUMBER && token_num != TSTRING)
    printf("%s",tokenstr[token_num]);
  else
    printf("%s",string_attr);

  if(token_num == TBEGIN)
  {
    tabnum++;
    is_begin_line = 1;
    printf("\n");
    linenum++;
  }
  if(token_num == TSEMI)
  {
    if(is_procedure_para == 0)
    {
      printf("\n");
      linenum++;
      is_begin_line = 1;
    }
  }
  fflush(stdout);
  return token_num;
}
int parse_program() {
  if(token != TPROGRAM) return(error("Keyword 'program' is not found"));
  token = next_token();
  if(token != TNAME) return(error("Program name is not found"));
  token = next_token();
  if(token != TSEMI) return(error("Semicolon is not found"));
  token = next_token();
  if(block() == ERROR) return(ERROR);
  if(token != TDOT) return(error("Period is not found at the end of program"));
  token = next_token();
  return(NORMAL);
}
int block()
{
  while(token == TPROCEDURE || token == TVAR)
  {
    if(token == TPROCEDURE)
    {
      if(subprogram_declaration() == ERROR) return(ERROR);
    }
    if(token == TVAR)
    {
      
      if(variable_declaration() == ERROR) return(ERROR);
    }

  }
  if(compound_statement() == ERROR) return(ERROR);

  return(NORMAL);
}
int variable_declaration()
{
  fflush(stdout);
  if(token != TVAR) return(error("var is not found"));
  is_variable_declaration = 1;
  token = next_token();
  
  if(variable_names() == ERROR) return(ERROR);
  if(token != TCOLON) return(error(": is not found"));
  token = next_token();
  if(type() == ERROR) return(ERROR);
  //token = next_token();
  if(token != TSEMI) return(error("; is not found"));
  token = next_token();



  while(token == TNAME){
    if(variable_names() == ERROR) break;
    if(token != TCOLON) return(error(": is not found"));
    token = next_token();
    if(type() == ERROR) return(ERROR);
    if(token != TSEMI) return(error("; is not found"));
    token = next_token();
  }
  is_variable_declaration = 0;
  return(NORMAL);
}
int variable_names()
{
  if(variable_name() == ERROR) return(ERROR);

  while(token == TCOMMA)
  {
    token = next_token();
    if(variable_name() == ERROR) return(ERROR);
  }
  return(NORMAL);
}
int variable_name()
{
  if(token != TNAME) return(error("name is not found"));
  token = next_token();
  return(NORMAL);
}
int type()
{
  if(token == TINTEGER || token == TBOOLEAN || token == TCHAR)
  {
    if(standard_type() == ERROR) return(ERROR);
  }
  else if(token == TARRAY)
  {
    if(array_type() == ERROR) return(ERROR);
  }
  else
    return(error("type error"));
  return(NORMAL);
}
int standard_type()
{
  int result;
  if(token == TINTEGER || token == TBOOLEAN || token == TCHAR)
  {
    if(token == TINTEGER)
      result = RINT;
    else if(token == TBOOLEAN)
      result = RBOOL;
    else if(token == TCHAR)
      result = RCHAR;
    token = next_token();
    return(result);
  }
  else return(error("standard_type is not found"));
}
int array_type()
{
  is_array = 1;
  if(token != TARRAY) return(error("array is not found"));
  token = next_token();
  if(token != TLSQPAREN) return(error("[ is not found"));
  token = next_token();
  if(token != TNUMBER) return(error("number is not found"));
  if(num_attr < 1)
    return (error("The value of the array must be 1 or more"));
  token = next_token();
  if(token != TRSQPAREN) return(error("] is not found"));
  token = next_token();
  if(token != TOF) return(error("of in not found"));
  token = next_token();
  if(standard_type() == ERROR) return(ERROR);
  is_array = 0;
  return(NORMAL);
}
int subprogram_declaration()
{
  if (token != TPROCEDURE) return(error("procedure is not found."));
  is_subprogram_declaration = 1;
  token = next_token();
  if(procedure_name() == ERROR) return(ERROR);
  if(token == TLPAREN)
  {
    if(formal_parameters() == ERROR) return(ERROR);
    token = next_token();

  }
  if(token != TSEMI) return(error("semicolon is not found"));
  token = next_token();
  if(token == TVAR)
  {
    if(variable_declaration() == ERROR) return(ERROR);
  }
  if(compound_statement() == ERROR) return(ERROR);
  if(token != TSEMI) return(error("semicolon is not found"));
  token = next_token();
  is_subprogram_declaration = 0;
  copy_local();
  
  //printf("------------printlocal----------\n");
  //print_localcr();
  //printf("------------printall----------\n");
  //print_allcr();
  //printf("------------releaselocal----------\n");
  release_localcr();
  return(NORMAL);
}
int procedure_name()
{
  if(token != TNAME) return(error("Don't name"));
  //strcpy(current_proce_name,string_attr);
  token = next_token();
  return(NORMAL);
}
int formal_parameters()
{
  printf("AAAA");
  fflush(stdout);
  if(token != TLPAREN) return(error("( is not found"));
  is_procedure_para = 1;
  token = next_token();
  if(variable_names() == ERROR) return(ERROR);
  
  if(token != TCOLON) return(error(": is not found"));
  token = next_token();
  if(type() == ERROR) return(ERROR);
  while (token != TRPAREN)
  {
    
    if(token != TSEMI) return(error("; is not found"));
    token = next_token();
    if(variable_names() == ERROR) return(ERROR);
    
    if(token != TCOLON) return(error(": is not found"));
    token = next_token();
    if(type() == ERROR) return(ERROR);
  }
  is_procedure_para = 0;
  
  return(NORMAL);
}
int compound_statement()
{
  if(token != TBEGIN) return(error("begin is not found"));
  token = next_token();
  
  if(statement() == ERROR) return(ERROR);
  while (token == TSEMI) {
    token = next_token();
    if(statement() == ERROR) return(ERROR);
  }
  if(token != TEND) return(error("end is not found"));
  token = next_token();
  return(NORMAL);
}


int statement()
{
  switch (token) {
    case TNAME:
      if(assignment_statement() == ERROR) return(ERROR);
      break;
    case TIF:
      if(condition_statement() == ERROR) return(ERROR);
      break;
    case TWHILE:
      if(iteration_statement() == ERROR) return(ERROR);
      break;
    case TBREAK:
      if(exit_statement() == ERROR) return(ERROR);
      break;
    case TCALL:
      if(call_statement() == ERROR) return(ERROR);
      break;
    case TRETURN:
      if(return_statement() == ERROR) return(ERROR);
      break;
    case TREAD:
    case TREADLN:
      if(input_statement() == ERROR) return(ERROR);
      break;
    case TWRITE:
    case TWRITELN:
      if(output_statement() == ERROR) return(ERROR);
      break;
    case TBEGIN:
      if(compound_statement() == ERROR) return(ERROR);
      break;
    default: empty_statement();break;
  }
  return(NORMAL);
}
int condition_statement()
{
    if(token != TIF) return(error("if is not found"));
    token = next_token();
    int result = expression();
    if(result == ERROR) return(ERROR);
    if(result != RBOOL) return(error("condition_statement is not boolean"));
    if(token != TTHEN) return(error("then is not fuond"));
    token = next_token();
    if(statement() == ERROR) return(ERROR);
    if(token == TELSE)
    {
      token = next_token();
      if(statement() == ERROR)return(ERROR);
    }
    return(NORMAL);
}
int iteration_statement()
{
  if(token != TWHILE) return(error("while is not found"));
  token = next_token();

  int result = expression();
  if (result == ERROR) return (ERROR);
  if (result != RBOOL) return (error("condition_statement is not boolean"));
  //if(expression() == ERROR) return(ERROR);
  if(token != TDO) return(error("do is not found"));
  token = next_token();
  if(statement() == ERROR) return(ERROR);
  return(NORMAL);
}
int exit_statement()
{
  if(token != TBREAK) return(error("break is not found"));
  token = next_token();
  return(NORMAL);
}
int call_statement()
{
  if(token != TCALL) return(error("call is not found"));
  token = next_token();
  if(procedure_name() == ERROR) return(ERROR);
  if(check_proc() == ERROR) return(ERROR);
  struct TYPE *proc_para;
  
  if((proc_para = count_formal_parameters()) == NULL) return(error("count_formal_parameters error"));
  //このprocedureのパラメータの数と型の並びを取得する必要あり
  if(token == TLPAREN)
  {
    struct TYPE *t,*i,*j;
    token = next_token();
    t = expressions();
    //if(expressions() == ERROR) return(ERROR);
    if(token != TRPAREN) return(error(") is not found"));
    token = next_token();
    i = proc_para->paratp;
    j = t;
    //printf("---------------\n");
    //struct TYPE *aaa;
    //for(aaa = i;aaa != NULL;aaa = aaa->paratp)
    //{
    //  printf("aaa->ttype : %d\n",aaa->ttype);
    //}
    //printf("---------------\n");
    while (1)
    {
      //printf("i->ttype : %d || j->ttype : %d\n", i->ttype, j->ttype);
      //fflush(stdout);
      if (i->ttype != j->ttype)
        return (error("call_statement error"));
      //printf("CCC");
      //fflush(stdout);
      
      i = i->paratp;
      j = j->paratp;
      if (i == NULL)
      {
        if (j == NULL)
        {
          //printf("A");
          //fflush(stdout);
          return(NORMAL);
        }
        else
        {
          //printf("B");
          //fflush(stdout);
          return (error("call_statement error"));
        }
      }
      else if(j == NULL)
      {
        //printf("C");
        //fflush(stdout);
        return (error("call_statement error"));
      }
    }
  }
  else 
    return(NORMAL);//引数なし
  return(NORMAL);
}
struct TYPE *expressions()
{
  struct TYPE *t, *tmp, *before_tmp,*nul;
  if ((t = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
  {
    printf("can not malloc in expressions\n");
    return(NULL);
  }
  switch (expression())
  {
    case RINT:
      t->ttype = TPINT;
      break;
    case RCHAR:
      t->ttype = TPCHAR;
      break;
    case RBOOL:
      t->ttype = TPBOOL;
      break;
    default:
      printf("\nexpressions ERROR\n");
      return(NULL);
      break;
  }
  t->paratp = NULL;
  
  //if(expression() == ERROR) return(ERROR);
  while (token == TCOMMA) {//式の数をカウント，なければ0→なければそもそも呼ばれない？
    struct TYPE *newt;
    if ((newt = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
    {
      printf("can not malloc in expressions\n");
      return(NULL);
    }

    if(token != TCOMMA)
    {
      printf("\nexpressions ERROR\n");
      return(NULL);
    }
    token = next_token();
    switch (expression())
    {
    case RINT:
    //printf("\nRINT\n");
      newt->ttype = TPINT;
      //printf("--");
      break;
    case RCHAR:
      //printf("\nRCHAR\n");
      newt->ttype = TPCHAR;
      break;
    case RBOOL:
      //printf("\nBOOL\n");
      newt->ttype = TPBOOL;
      break;
    default:
      printf("\nexpressions ERROR\n");
      return (NULL);
      break;
    }
    //printf("A-1");
    newt->paratp = NULL;
    for(tmp = t;tmp != NULL;tmp = tmp->paratp)
    {
      before_tmp = tmp;
    }
    //printf("A-2");
    before_tmp->paratp = newt;
    //printf("A-3");
    if(token != TCOMMA)
    {
      //printf("GG");
      before_tmp->paratp->paratp = NULL;
    }
    //printf("A-4");
    //if(expression() == ERROR) return(ERROR);
  }
  return(t);
  //return(NORMAL);
}
int return_statement()
{
  if(token != TRETURN) return(error("return is not found"));
  token = next_token();
  return(NORMAL);
}
int assignment_statement()
{
  int expre,type;
  if(left_part() == ERROR) return(ERROR);
  //------------
  if (is_subprogram_declaration == 1) //副プログラム
  {
    if (search_localcr(string_attr) != NULL)
    {
      if ((type = check_standard_type_local()) == ERROR)
        return (ERROR);
    }
    else if (search_globalcr(string_attr) != NULL)
    {
      if ((type = check_standard_type_global()) == ERROR)
        return (ERROR);
    }
    else
      error("There are undefined variables aaaaaaa");
  }
  else
  {
    if ((type = check_standard_type_global()) == ERROR)
      return (ERROR);
  }

  //------------
  if(token != TASSIGN) return(error(":= is not found"));
  token = next_token();
  expre = expression();
  if(expre == ERROR) return(ERROR);
  if(type != expre)
    return (error("assignment statement error"));
  
  return (NORMAL);
}
int left_part()
{
  int type = variable();
  if(type == ERROR) return(ERROR);
  return(type);
}
int variable()
{
  int type,result;
  struct ID *p;
  //変数の型を返す
  if(variable_name() == ERROR) return(ERROR);
  //------------
  if (is_subprogram_declaration == 1) //副プログラムならlocal
  {
    //printf("\nstring attr %s\n",string_attr);
    if ((p = search_localcr(string_attr)) != NULL)
    {
      //if (check_standard_type_local() == ERROR)return (ERROR);
      type = check_variable_type_local();
    }
    else if ((p = search_globalcr(string_attr)) != NULL)
    {
      //if (check_standard_type_global() == ERROR)return (ERROR);
      type = check_variable_type_global();
    }
    else
      error("There are undefined variables aaaaaaa");
  }
  else
  {
    p = search_globalcr(string_attr);
    type = check_variable_type_global();
  }

  //------------
  if(token == TLSQPAREN)
  {
    if(type != RARRAY) return(error("variable type error"));
    token = next_token();
    result = expression();
    if(result == ERROR) return(ERROR);
    if(result != RINT) return(error("variable error"));
    if(token != TRSQPAREN) return(error("] is not found"));
    token = next_token();
    if(p->itp->etp->ttype == TPARRAYINT)
      return(RINT);
    else if(p->itp->ttype == TPARRAYCHAR)
      return(RCHAR);
    else if(p->itp->ttype == TPARRAYBOOL)
      return(RBOOL);
    else  
      return(error("variable array error"));
  }
  else
    return(type);
  return(NORMAL);
}
int expression()
{ 
  int result1,result2,resultrela;
  result1 = simple_expression();
  if (result1 == ERROR) return (ERROR);
  if(!(token >= TEQUAL && token <= TGREQ))
  {
    return(result1);
  }
  while(token >= TEQUAL && token <= TGREQ)
  {
    resultrela = relational_operator();
    if(resultrela == ERROR)return(ERROR);
    result2 = simple_expression();
    if(result2 == ERROR) return (ERROR);
    if(result1 != result2)
    {
      return (error("expression error"));
    }
  }
  return(resultrela);
}
int simple_expression()
{
  int resulttok1,resulttok2,resultadd,is_pm = 0;
  if(token == TPLUS || token == TMINUS)
  {
    is_pm = 1;
    token = next_token();
  }
  resulttok1 = term();
  if(resulttok1 == ERROR) return(ERROR);
  if(is_pm == 1 && resulttok1 != RINT)
  {
    return(error("simple_expression error"));
  }
  if(!(token == TPLUS || token == TMINUS || token == TOR))
  {
    return(resulttok1);
  }

  while(token == TPLUS || token == TMINUS || token == TOR)
  {
    //token = next_token();
    resultadd = additive_operator();
    if(resultadd == ERROR) return(ERROR);
    resulttok2 = term();
    if(resulttok2 == ERROR) return(ERROR);
    if (!((resulttok1 == RINT && resulttok2 == RINT && resultadd == RINT) ||
          (resulttok1 == RBOOL && resulttok2 == RBOOL && resultadd == RBOOL)))
    {
      return (error("simple_expression error"));
    }
  }
  return(resultadd);
}
int term()
{
  int resultfac1, resultfac2, resultmulti;
  resultfac1 = factor();
  if(resultfac1 == ERROR) return(ERROR);
  if(!(token == TSTAR || token == TDIV || token == TAND))
  {
    return(resultfac1);
  }
  while (token == TSTAR || token == TDIV || token == TAND)
  {
    resultmulti = multiplicative_operator();
    if(resultmulti == ERROR) return(ERROR);
    resultfac2 = factor();
    if (resultfac2 == ERROR) return (ERROR);

    if (!((resultfac1 == RINT && resultfac2 == RINT && resultmulti == RINT) || 
    (resultfac1 == RBOOL && resultfac2 == RBOOL && resultmulti == RBOOL)))
    {
      return(error("term error"));
    }
  }
  return(resultmulti);
  //return(NORMAL);
}

int factor()
{
  int result;
  switch (token) {
    case TNAME:
      result = variable();
      if(result == ERROR) return(ERROR);
      return(result);
      break;
    case TNUMBER:
    case TFALSE:
    case TTRUE:
    case TSTRING:
      result = constant();
      if(result == ERROR) return(ERROR);
      return(result);
      break;
    case TLPAREN:
      if(token != TLPAREN) return(error("( is not found"));
      token = next_token();
      result = expression();
      if(result == ERROR) return(ERROR);
      if(token != TRPAREN) return(error(") is not found"));
      token = next_token();
      return(result);
      break;
    case TNOT:
      if(token != TNOT) return(error("not is not found"));
      token = next_token();
      result = factor();
      if(result == ERROR) return(ERROR);
      else if(result != RBOOL)return (error("The result of factor must be Bool"));
      printf("\nD\n");
      return(result);//bool
      break;
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      result = standard_type();
      if(result == ERROR) return(ERROR);
      if(token != TLPAREN) return(error("( is not found"));
      token = next_token();
      if(expression() == ERROR) return(ERROR);
      //------------
      if (is_subprogram_declaration == 1) //副プログラムならlocal
      {
        //printf("\nstring attr %s\n",string_attr);
        if (search_localcr(string_attr) != NULL)
        {
          if(check_standard_type_local() == ERROR) return(ERROR);
        }
        else if (search_globalcr(string_attr) != NULL)
        {
          if(check_standard_type_global() == ERROR) return(ERROR);
        }
        else
          error("There are undefined variables aaaaaaa");
      }
      else
      {
        if(check_standard_type_global() == ERROR) return (ERROR);
      }

      //------------
      
      if(token != TRPAREN) return(error(") is not found"));
      token = next_token();
      return(result);
      break;
    default: return(error("factor error"));break;
  }
  return(NORMAL);
}
int constant()
{
  switch (token) {
    case TNUMBER:
      if(token != TNUMBER) return(error("number is not found"));
      token = next_token();
      return(RINT);
      break;
    case TFALSE:
      if(token != TFALSE) return(error("false is not found"));
      token = next_token();
      return(RBOOL);
      break;
    case TTRUE:
      if(token != TTRUE) return(error("true is not found"));
      token = next_token();
      return(RBOOL);
      break;
    case TSTRING:
      if(token != TSTRING) return(error("string is not found"));
      token = next_token();
      if((strlen(string_attr)-2) != 1)
      {
        //printf(":: strlen : %d :%s:\n",strlen(string_attr),string_attr);
        return(error("Constant character is one character"));
      }
      return(RCHAR);
      break;
    default: return(error("constant error"));break;
  }
  return(NORMAL);
}
int multiplicative_operator()
{
  switch (token) {
    case TSTAR:
      if(token != TSTAR) return(error("* is not found"));
      token = next_token();
      return(RINT);
      break;
    case TDIV:
      if(token != TDIV) return(error("div is not found"));
      token = next_token();
      return(RINT);
      break;
    case TAND:
      if(token != TAND) return(error("and is not found"));
      token = next_token();
      return(RBOOL);
      break;
    default: return(error("multiplicative_operator error"));break;
  }
  return(NORMAL);
}
int additive_operator()
{
  switch (token) {
    case TPLUS:
      if(token != TPLUS) return(error("+ is not found"));
      token = next_token();
      return(RINT);
      break;
    case TMINUS:
      if(token != TMINUS) return(error("- is not found"));
      token = next_token();
      return(RINT);
      break;
    case TOR:
      if(token != TOR) return(error("or is not found"));
      token = next_token();
      return(RBOOL);
      break;
    default: return(error("additive_operator error"));break;
  }
  return(NORMAL);
}
int relational_operator()
{
   switch (token) {
     case TEQUAL:
       if(token != TEQUAL) return(error("= is not found"));
       token = next_token();
      break;
    case TNOTEQ:
      if(token != TNOTEQ) return(error("<> is not found"));
      token = next_token();
      break;
    case TLE:
      if(token != TLE) return(error("< is not found"));
      token = next_token();
      break;
    case TLEEQ:
      if(token != TLEEQ) return(error("<= is not found"));
      token = next_token();
      break;
    case TGR:
      if(token != TGR) return(error("> is not found"));
      token = next_token();
      break;
    case TGREQ:
      if(token != TGREQ) return(error(">= is not found"));
      token = next_token();
      break;
    default: return(error("relational_operator error"));break;
   }
   return(RBOOL);
}
int input_statement()
{
  if(token == TREAD)
  {
    token = next_token();
  }
  else if(token == TREADLN)
  {
    token = next_token();
  }
  else
  {
    return( error("read or readln is not found"));
  }

  if(token == TLPAREN)
  {

    token = next_token();
    int result = variable();
    if(result == ERROR) return(ERROR);
    if(!(result == RINT || result == RCHAR)) return(error("input statement error"));
    while (token == TCOMMA) {
      token = next_token();
      if(variable() == ERROR) return(ERROR);

    }
    if(token != TRPAREN) return(error(") is not found"));
    token = next_token();
  }
  return(NORMAL);
}
int output_statement()
{
  if(token == TWRITE)
  {
    token = next_token();
  }
  else if(token == TWRITELN)
  {
    token = next_token();
  }
  else
  {
    return( error("write or writeln is not found"));
  }
  if(token == TLPAREN)
  {
    token = next_token();
    if(output_formal() == ERROR)return(ERROR);
    while (token == TCOMMA) {
      token = next_token();
      if(output_formal() == ERROR) return(ERROR);
    }
    if(token != TRPAREN) return(error(") is not found"));
    token = next_token();
  }
  return(NORMAL);
}
int output_formal()
{
  int result = 0;
  if(token == TSTRING)
  {
    if(strlen(string_attr) == 1)
      return(error("output_formal strlen error"));
    token = next_token();
  }
  else
  {
    result = expression();
    if(!(result == RINT || result == RCHAR || result == RBOOL))
      return(error("output_formal error"));
    if(token == TCOLON)
    {
      token = next_token();
      if(token != TNUMBER) return(error("number is not found"));
      token = next_token();
    }
  }
  
  return(NORMAL);
}
int empty_statement()
{
  return(NORMAL);
}
