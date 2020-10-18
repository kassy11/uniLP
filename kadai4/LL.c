#include "token-list.h"
extern int token;
extern int linenum;
extern char *tokenstr[NUMOFTOKEN + 1];
extern int tabnum;
extern int compound_tab[10];
extern FILE *outfp;
int is_begin_line = 1; 
int is_procedure_begintoend = 0;
int compound_count = 0;
int is_variable_declaration = 0;//現在変数宣言部かどうか
int is_subprogram_declaration = 0;//現在副プログラム部かどうか
char current_proce_name[MAXSTRSIZE]; //現在の副プログラムの名前
int is_array = 0;//現在arrayかどうか
int is_procedure_para = 0;//現在procedureの仮引数部かどうか
int procedure_para_count = 0;//procedureの仮引数の数
int is_output_format = 0;//現在出力指定かどうか
int is_opr = 0;//現在oprが続いているかどうか
int is_expre_opr = 0;//最後に読んだ式でoprは1になったかどうか
int current_relational = 0;//現在の関係演算子
int is_callsta = 0;//現在callかどうか
int is_constant_TRUE = 0;//定数のTRUEが出現したかどうか
int is_constant_FALSE = 0; //定数のFALSEが出現したかどうか
int is_pp = 1;//プリティプロントするか否か
char *current_array_name;//現在の配列の名前
int is_left_array = 0;//現在の代入文の左辺部が配列かどうか
int is_left_part = 0;//現在左辺部かどうか
char break_label[MAXSTRSIZE];//breakで戻るべきラベル
int is_vari_left_part = 0;//現在の代入文の左辺部がパラメータでない変数であるかどうか
char left_variable[MAXSTRSIZE];//現在の左辺部の変数名
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
      if(is_pp)
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
      if (is_pp)
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
    if (is_pp)
      printf("\n");
    linenum++;
    is_begin_line = 1;
  }
  if ((before_token == TTHEN || before_token == TELSE || before_token == TDO) && (token_num != TBEGIN))
  {
    if((before_token == TELSE) && (token_num != TIF))
    {
      if (is_pp)
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
      if (is_pp)
        printf(" ");
  }
  else if(token_num != TSEMI && token_num != TSTRING && token_num != TRPAREN  && token_num != TDOT )
    if(!(before_token == TLPAREN && token_num != TSTRING))
      if (is_pp)
        printf(" ");

  //-----------↓↓↓↓↓↓↓↓↓↓↓↓↓↓↓
  if(is_variable_declaration == 1 && token_num == TNAME)//変数宣言時
  {
    if(is_subprogram_declaration == 1)//副プログラムならlocal
    {
      cr_localDeclaration(0);
      //DC_print_local();
    }
    else//global
    {
      cr_globalDeclaration();
      //DC_print_global();
    }
  }
  else if(is_variable_declaration == 1 && (token_num == TCHAR || token_num == TINTEGER || token_num == TBOOLEAN))
  {//型名セット
    //DC_print_num(is_array);
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
    //DC_print_local();
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
  {
    if (is_pp)
      printf("%s", tokenstr[token_num]);
  }
  else
  {
    if(is_pp)
      printf("%s", string_attr);
  }

  if(token_num == TBEGIN)
  {
    tabnum++;
    is_begin_line = 1;
    if (is_pp)
      printf("\n");
    linenum++;
  }
  if(token_num == TSEMI)
  {
    if(is_procedure_para == 0)
    {
      if (is_pp)
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
  //---------------mplc--------------
  fprintf(outfp, "$$%s\tSTART\n", string_attr);
  LAD(gr0,"0",NULL);
  CALL(next_calllabel(),NULL);
  CALL("FLUSH",NULL);
  SVC("0",NULL);

  current_array_name = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  // = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  //---------------------------------

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
  write_label("L0001");
  if(compound_statement() == ERROR) return(ERROR);
  RET();
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
  int is_formal = 0;
  if (token != TPROCEDURE) return(error("procedure is not found."));
  is_subprogram_declaration = 1;
  token = next_token();
  if(procedure_name() == ERROR) return(ERROR);
  
  if(token == TLPAREN)
  {
    if(formal_parameters() == ERROR) return(ERROR);
    is_formal = 1;
    token = next_token();

  }
  //print_localcr();
  
  if(token != TSEMI) return(error("semicolon is not found"));
  token = next_token();
  if(token == TVAR)
  {
    if(variable_declaration() == ERROR) return(ERROR);
  }
  write_label_DL(current_proce_name);
  //------------mpplc---------------
  if(is_formal)
    formal_para_ST();
  //--------------------------------
  if(compound_statement() == ERROR) return(ERROR);
  if(token != TSEMI) return(error("semicolon is not found"));
  token = next_token();
  is_subprogram_declaration = 0;
  copy_local();
  release_localcr();
  RET();
  return(NORMAL);
}
int procedure_name()
{
  if(token != TNAME) return(error("Don't name"));
  token = next_token();
  return(NORMAL);
}
int formal_parameters()
{
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
    //------------mpplc-----------
    char *next_label = next_calllabel();
    //relational_casl_code(current_relational);
    CPA_rr(gr1,gr0);
    JZE(next_label,NULL);
    //----------------------------
    if(token != TTHEN) return(error("then is not fuond"));
    token = next_token();
    if(statement() == ERROR) return(ERROR);
    if(token == TELSE)
    {
      //----------mpplc-----------
      char *next2_label = next_calllabel();
      JUMP(next2_label,NULL);
      write_label(next_label);
      //----------------------------
      token = next_token();
      if(statement() == ERROR)return(ERROR);
      write_label(next2_label);
    }
    else
      write_label(next_label);
    return(NORMAL);
}
int iteration_statement()
{
  char *roop;
  roop = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  roop = next_calllabel();
  write_label(roop);
  if(token != TWHILE) return(error("while is not found"));
  token = next_token();

  int result = expression();
  if (result == ERROR) return (ERROR);
  if (result != RBOOL) return (error("condition_statement is not boolean"));
  //------------mpplc-----------
  char *next_label = next_calllabel();
  snprintf(break_label,MAXSTRSIZE,"%s",next_label);
  //relational_casl_code(current_relational);
  //POP(gr1);
  CPA_rr(gr1, gr0);

  JZE(next_label, NULL);

  //----------------------------
  //if(expression() == ERROR) return(ERROR);
  if(token != TDO) return(error("do is not found"));
  token = next_token();
  if(statement() == ERROR) return(ERROR);
  JUMP(roop,NULL);
  write_label(next_label);
  return(NORMAL);
}
int exit_statement()
{
  if(token != TBREAK) return(error("break is not found"));
  JUMP(break_label,NULL);
  token = next_token();
  return(NORMAL);
}
int call_statement()
{
  if(token != TCALL) return(error("call is not found"));
  is_callsta = 1;
  token = next_token();
  if(procedure_name() == ERROR) return(ERROR);
  char *proname;
  proname = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  //strcpy(proname,string_attr);
  snprintf(proname, MAXSTRSIZE, "$%s", string_attr);
  if(check_proc() == ERROR) return(ERROR);
  struct TYPE *proc_para;
  
  if((proc_para = count_formal_parameters()) == NULL) return(error("count_formal_parameters error"));
  //このprocedureのパラメータの数と型の並びを取得する必要あり
  if(token == TLPAREN)
  {
    struct TYPE *t,*i,*j;
    token = next_token();
    t = expressions();
    if(token != TRPAREN) return(error(") is not found"));
    token = next_token();
    i = proc_para->paratp;
    j = t;

    is_callsta = 0;
    while (1)
    {
      if (i->ttype != j->ttype)
        return (error("call_statement error"));
      
      i = i->paratp;
      j = j->paratp;
      if (i == NULL)
      {
        if (j == NULL)
        {
          break;
        }
        else
        {
          return (error("call_statement error"));
        }
      }
      else if(j == NULL)
      {
        return (error("call_statement error"));
      }
    }
    //if(is_expre_opr == 0)
    //  PUSH("00",gr1);
  }
  CALL(proname,NULL);
  is_callsta = 0;
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
  //is_opr = 0;
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
  if (is_expre_opr == 1)
  {
    char *tmp;
    tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
    tmp = next_calllabel();
    LAD(gr2,tmp,NULL);
    ST(gr1,"0",gr2);
    PUSH("0",gr2);
    add_DCList(tmp,1);
  }
  else
    PUSH("0", gr1);

  //if(expression() == ERROR) return(ERROR);
  while (token == TCOMMA) {//式の数をカウント，なければ0→なければそもそも呼ばれない？
    struct TYPE *newt;
    if ((newt = (struct TYPE *)malloc(sizeof(struct TYPE))) == NULL)
    {
      printf("can not malloc in expressions\n");
      return(NULL);
    }
    //is_opr = 0;
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
    PUSH("0", gr1);
  }
  return(t);
  //return(NORMAL);
}
int return_statement()
{
  if(token != TRETURN) return(error("return is not found"));
  RET();
  token = next_token();
  return(NORMAL);
}
int assignment_statement()
{
  int expre,type;
  struct ID *i = left_part();
  if(i == NULL) return(ERROR);

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
      error("There are undefined variables");
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
  //------------------mpplc----------------
  if (is_vari_left_part == 0)
    POP(gr2);

  if (is_vari_left_part)
  {
    //if(is_left_array == 0)
    //{//localかglobalで処理変わる
      struct ID *i;
      char *tmp;
      tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
      //ローカルを探して投げればグローバル
      if ((i = search_localcr(left_variable)) != NULL)
      {
        snprintf(tmp, MAXSTACKSIZE, "$%s%%%s", left_variable, i->procname);

        if (is_left_array == 0)
          ST(gr1, tmp, NULL);
        else if(is_left_array == 1)
          ST(gr1, tmp, gr2);
      }
      else if ((i = search_globalcr(left_variable)) != NULL)
      {
        snprintf(tmp, MAXSTACKSIZE, "$%s", left_variable);
        
        if (is_left_array == 0)
          ST(gr1, tmp, NULL);
        else if (is_left_array == 1)
          ST(gr1, tmp, gr2);
      }
      free(tmp);
     //}
     //else if(is_left_array == 1)
     //{
     // tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
     // snprintf(tmp, MAXSTACKSIZE, "$%s", current_array_name);
     // ST(gr1, tmp, gr2);
     // free(tmp);
    //}
  }
  else if(is_left_array)
  {
    char *tmp;
    tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
    snprintf(tmp,MAXSTACKSIZE,"$%s",current_array_name);
    ST(gr1,tmp,gr2);
    free(tmp);
  }
  else
    ST(gr1,"0",gr2);
  is_left_array = 0;
  is_left_part = 0;
  is_vari_left_part = 0;
  return (NORMAL);
}
struct ID *left_part()
{
  is_left_part = 1;

  int type = variable();
  
  if(type == ERROR) return(NULL);
  //------------------------mpplc-------------------
  char *tmp;
  struct ID *i;
  tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  //ローカルを探して投げればグローバル
  if ((i = search_localcr(string_attr)) != NULL)
  {
    //TODO これでいいか不明
    if(i->ispara)
    {
      snprintf(tmp, MAXSTRSIZE, "$%s%%%s", i->name, i->procname);
      LD_ra(gr1, tmp, NULL);
      PUSH("0", gr1);
    }
    else
      is_vari_left_part = 1;
    }
  else if ((i = search_globalcr(string_attr)) != NULL)
  {
    is_vari_left_part = 1;
    //  snprintf(tmp, MAXSTRSIZE, "$%s", string_attr);
    //  LD_ra(gr1, tmp, NULL);
  }
  
  //------------------------------------------------
  is_left_part = 0;
  //snprintf(left_variable,MAXSTRSIZE,"%s",i->name);
  return(i);
  //return (type);
}
int variable()
{
  int type,result;
  struct ID *p;
  char *vari_name = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  snprintf(vari_name,MAXSTRSIZE,"%s",string_attr);
  //printf("\nAAAAstring attr : %s\n", string_attr);
  //変数の型を返す
  if(variable_name() == ERROR) return(ERROR);
  if(is_left_part)
    snprintf(left_variable, MAXSTRSIZE, "%s", string_attr);
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
      error("There are undefined variables ");
  }
  else
  {
    p = search_globalcr(string_attr);
    type = check_variable_type_global();
  }

  //------------
  if(token == TLSQPAREN)//array
  {
    if(type != RARRAY) return(error("variable type error"));
    token = next_token();
    result = expression();
    if(result == ERROR) return(ERROR);
    if(result != RINT) return(error("variable error"));
    if(token != TRSQPAREN) return(error("] is not found"));
    token = next_token();
    //---------mpplc--------
    snprintf(current_array_name, MAXSTRSIZE, "%s", vari_name);
    if(is_left_part == 1)
      is_left_array = 1;
    if(is_variable_declaration == 0)
    {
      char *tmp;
      struct ID *i;
      tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
      //ローカルを探して無ければグローバル
      if ((i = search_localcr(vari_name)) != NULL)
      {
        //printf("LOCAL");
        snprintf(tmp,MAXSTRSIZE,"%d",i->itp->arraysize);
        LAD(gr2,tmp,NULL);
      }
      else if ((i = search_globalcr(vari_name)) != NULL)
      {
        //printf("GLOBAL %d\n",i->itp->arraysize);
        //printf("array name %s\n",i->name);
        snprintf(tmp, MAXSTRSIZE, "%d", i->itp->arraysize);
        LAD(gr2, tmp, NULL);
        
      }
      CPA_rr(gr2,gr1);
      JMI("EROV",NULL);
      JZE("EROV",NULL);
      CPA_rr(gr1,gr0);
      JMI("EROV",NULL);
    }
    //----------------------
    if (p->itp->etp->ttype == TPARRAYINT)
      return (RINT);
    else if (p->itp->ttype == TPARRAYCHAR)
      return (RCHAR);
    else if (p->itp->ttype == TPARRAYBOOL)
      return (RBOOL);
    else
      return (error("variable array error"));
  }
  else
    return(type);
  return(NORMAL);
}
int expression()
{ 
  int result1,result2,resultrela,opr;
  is_expre_opr = 0;
  is_opr = 0;
  result1 = simple_expression();
  if (result1 == ERROR) return (ERROR);
  if(!(token >= TEQUAL && token <= TGREQ))
  {
    return(result1);
  }
  while(token >= TEQUAL && token <= TGREQ)
  {
    is_opr = 1;
    is_expre_opr = 1;
    opr = token;
    PUSH("0",gr1);
    resultrela = relational_operator();
    if(resultrela == ERROR)return(ERROR);
    result2 = simple_expression();
    if(result2 == ERROR) return (ERROR);
    if(result1 != result2)
    {
      return (error("expression error"));
    }
    is_opr = 0;
    relational_casl_code(current_relational);
  }
  return(resultrela);
}
int simple_expression()
{
  int resulttok1,resulttok2,resultadd,is_minus = 0,opr;
  if(token == TPLUS || token == TMINUS)
  {
    //is_opr = 1;
    is_minus = 1;
    token = next_token();
  }
  resulttok1 = term();
  if(resulttok1 == ERROR) return(ERROR);
  if (is_minus == 1 && resulttok1 != RINT)
  {
    return(error("simple_expression error"));
  }
  if(is_minus)
  {
    LAD(gr2,"-1",NULL);
    MULA_rr(gr1,gr2);
    JOV("EOVF",NULL);
  }
  if(!(token == TPLUS || token == TMINUS || token == TOR))
  {
    return(resulttok1);
  }

  while(token == TPLUS || token == TMINUS || token == TOR)
  {
    is_expre_opr = 1;
    is_opr = 1;
    opr = token;
    PUSH("0", gr1);
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
    POP(gr2);
    
    if(opr == TPLUS)
    {
      ADDA_rr(gr1,gr2);
      JOV("EOVF",NULL);
    }
    else if(opr == TMINUS)
    {
      SUBA_rr(gr2,gr1);
      JOV("EOVF",NULL);
      LD_rr(gr1,gr2);
    }
    else if(opr == TOR)
    {
      OR_rr(gr1,gr2);
    }
    is_opr = 0;
  }
  return(resultadd);
}
int term()//caslii
{
  int resultfac1, resultfac2, resultmulti, opr;
  resultfac1 = factor();
  if(resultfac1 == ERROR) return(ERROR);
  if(!(token == TSTAR || token == TDIV || token == TAND))
  {
    //PUSH("0",gr1);
    return(resultfac1);
  }
  //--------------mpplc-------------
  //char *tmp;
  struct ID *i;
  //tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
  //ローカルを探して投げればグローバル
  if ((i = search_localcr(string_attr)) != NULL)
  {
    //snprintf(tmp, MAXSTRSIZE, "$%s%%%s", i->name, i->procname);
    if (i->ispara && is_callsta == 1)
      LD_ra(gr1, "0", gr1);
  }
  //else if ((i = search_globalcr(string_attr)) != NULL)
  //{
  //  snprintf(tmp, MAXSTRSIZE, "$%s", string_attr);
  //  if (is_callsta == 0)
  //    LD_ra(gr1, tmp, NULL);
  //  else if (is_callsta == 1)
  //    LAD(gr1, tmp, NULL);
  //}
  //free(tmp);
  //LD_ra(gr1,"0",gr1);
  
  //--------------------------------
  while (token == TSTAR || token == TDIV || token == TAND)
  {
    is_expre_opr = 1;
    is_opr = 1;
    opr = token;
    PUSH("0", gr1);
    resultmulti = multiplicative_operator();
    if(resultmulti == ERROR) return(ERROR);
    resultfac2 = factor();
    if (resultfac2 == ERROR) return (ERROR);
    if (!((resultfac1 == RINT && resultfac2 == RINT && resultmulti == RINT) || 
    (resultfac1 == RBOOL && resultfac2 == RBOOL && resultmulti == RBOOL)))
    {
      return(error("term error"));
    }
    //--------------mpplc-------------
    POP(gr2);
    //POP(gr1);
    if(opr == TSTAR)
    {
      MULA_rr(gr1,gr2);
      JOV("EOVF",NULL);
    }
    else if(opr == TDIV)
    {
      DIVA_rr(gr2,gr1);
      JOV("E0DIV", NULL);
      LD_rr(gr1,gr2);
    }
    else if(opr == TAND)
      AND_rr(gr1,gr2);
    //PUSH("0",gr1);
    //--------------------------------
    is_opr = 0;
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
      //------------mpplc--------------
      char *tmp;
      struct ID *i;
      tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
      //ローカルを探して無ければグローバル
      if((i = search_localcr(string_attr)) != NULL)
      {
        snprintf(tmp, MAXSTRSIZE, "$%s%%%s", i->name, i->procname);
        if(i->ispara == 0 && is_callsta == 1)
          LAD(gr1, tmp, NULL);
        else
          LD_rr(gr1, tmp);

        //if(is_opr == 0 && i->ispara == 1 && is_callsta == 0)
   //NG     //if ((is_opr == 0) && !(token == TCOMMA || token == TRPAREN) && is_procedure_para == 0)
        //if ((is_opr == 0) || is_procedure_para == 1)
        //if (i->ispara == 1 && is_procedure_para == 1)
        if (i->ispara == 1 && is_callsta == 0)
        {
          LD_ra(gr1,"0",gr1);
          //PUSH("0",gr1);
        }
        //else
        //{
        //  
        //  LD_ra(gr1, "aaaaaaaa", gr1);
        //}
      }
      else if((i = search_globalcr(string_attr)) != NULL)
      {
        snprintf(tmp, MAXSTRSIZE, "$%s", string_attr);
        if(is_callsta == 0)
        {
          LD_ra(gr1, tmp, NULL);
        }
        else if(is_callsta == 1)
        {
          LAD(gr1, tmp, NULL);
          if ((is_opr == 0) && !(token == TCOMMA || token == TRPAREN) && is_procedure_para == 0)
          //if (!(is_procedure_para && (token == TCOMMA || token == TRPAREN) && !is_opr))
          {
            LD_ra(gr1, "0", gr1);
          }
        }
        //TODO callの時はLADにするなどの処理

        //if(is_output_format)
        //  LD_ra(gr1,tmp,NULL);
        //else
        //  LAD(gr1, tmp, NULL);
      }
      //if(is_subprogram_declaration == 1)//副プログラム内なら
      //{
      //  snprintf(tmp, MAXSTRSIZE, "$%s%%%s", string_attr,current_proce_name);
      //  LD_rr(gr1,tmp);
      //}
      //else
      //{
      //  snprintf(tmp, MAXSTRSIZE, "$%s", string_attr);
      //  LAD(gr1, tmp, NULL);
      //}
      free(tmp);
      //-------------------------------
      return(result);
      break;
    case TNUMBER:
    case TFALSE:
    case TTRUE:
    case TSTRING:
      result = constant();
      if(result == ERROR) return(ERROR);
      //--------------mpplc-------------
      if(token == TNUMBER)
      {
        char *tmp;
        tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
        snprintf(tmp, MAXSTRSIZE, "%d", num_attr);
        LAD(gr1,tmp,NULL);
        free(tmp);
      }
      else if(token == TFALSE)
      {
        //LAD(gr1, "0", NULL);
        LD_rr(gr1,gr0);
        is_constant_FALSE = 1;
      }
      else if(token == TTRUE)
      {
        //LAD(gr1,"1",NULL);
        LAD(gr1,"1",NULL);
        is_constant_TRUE = 1;
      }
      else if(token == TSTRING)
      {
        //("\n token == TSTRING : %s\n",string_attr);
        if(strlen(string_attr)-2 != 1)
          return (error("The string must be one character"));
        //char n;
        //n = string_attr[1];
        char *tmp;
        if ((tmp = (char *)malloc(MAXSTACKSIZE)) == NULL)
          printf("can not malloc globalDeclaration\n");
        snprintf(tmp,MAXSTRSIZE,"%d",(int)string_attr[1]);
        LAD(gr1,tmp,NULL);
        free(tmp);
      } 
      //--------------------------------
      token = next_token();
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
      //TODO 論理反転
      char *next = next_calllabel();
      LAD(gr2,"1",NULL);
      CPA_rr(gr0,gr1);
      JZE(next,NULL);
      LD_rr(gr1,gr2);
      write_label(next);
      XOR_rr(gr1,gr2);
      return(result);//bool
      break;
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      result = standard_type();
      if(result == ERROR) return(ERROR);
      if(token != TLPAREN) return(error("( is not found"));
      token = next_token();
      int expres_type = expression();//式の型

      if(expres_type == ERROR) return(ERROR);
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
          error("There are undefined variables");
      }
      else
      {
        if(check_standard_type_global() == ERROR) return (ERROR);
      }

      //------------
      //--------mpplc----------------
      if(expres_type == RINT)
      {
        if(result == RINT)
        {
          //何もしない
        }
        else if(result == RBOOL)
        {
          CPA_rr(gr1, gr0);
          char *label = next_calllabel();
          JZE(label, NULL);
          LAD(gr1, "1", NULL);
          write_label(label);
        }
        else if(result == RCHAR)
        {
          LAD(gr2,"127",NULL);
          AND_rr(gr1,gr2);
        }
      }
      else if(expres_type == RBOOL)
      {
        if (result == RINT)
        {
          //("---------");
          if(is_constant_TRUE == 0 && is_constant_FALSE == 0)
          {
            CPA_rr(gr1,gr0);
            char *label = next_calllabel();
            JZE(label, NULL);
            LAD(gr1, "1", NULL);
            write_label(label);
          }
          is_constant_FALSE = 0;
          is_constant_TRUE = 0;
        }
        else if (result == RBOOL)
        {
          //何もしない
        }
        else if (result == RCHAR)
        {
          if (is_constant_TRUE == 0 && is_constant_FALSE == 0)
          {
            CPA_rr(gr1, gr0);
            char *label = next_calllabel();
            JZE(label, NULL);
            LAD(gr1, "1", NULL);
            write_label(label);
          }
          is_constant_FALSE = 0;
          is_constant_TRUE = 0;
        }
      }
      else if(expres_type == RCHAR)
      {
        if (result == RINT)
        {
        }
        else if (result == RBOOL)
        {
          CPA_rr(gr1, gr0);
          char *label = next_calllabel();
          JZE(label, NULL);
          LAD(gr1, "1", NULL);
          write_label(label);
        }
        else if (result == RCHAR)
        {

        }
      }
      //-----------------------------
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
      //token = next_token();
      return(RINT);
      break;
    case TFALSE:
      if(token != TFALSE) return(error("false is not found"));
      //token = next_token();
      return(RBOOL);
      break;
    case TTRUE:
      if(token != TTRUE) return(error("true is not found"));
      //token = next_token();
      return(RBOOL);
      break;
    case TSTRING:
      if(token != TSTRING) return(error("string is not found"));
      //token = next_token();
      //if((strlen(string_attr)-2) != 1)
      //{
      //  //printf(":: strlen : %d :%s:\n",strlen(string_attr),string_attr);
      //  return(error("Constant character is one character"));
      //}
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
  current_relational = token;
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
  int is_ln;
  if(token == TREAD)
  {
    is_ln = 0;
    token = next_token();
  }
  else if(token == TREADLN)
  {
    is_ln = 1;
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
    //---------------mpplc--------------
    char *tmp;
    struct ID *i;
    tmp = (char *)malloc(sizeof(char) * MAXSTRSIZE);
    //ローカルを探して投げればグローバル
    if ((i = search_localcr(string_attr)) != NULL)
    {
      snprintf(tmp, MAXSTRSIZE, "$%s%%%s", i->name, i->procname);
      if(i->ispara)
        LD_ra(gr1,tmp,NULL);
      else
        LAD(gr1,tmp,NULL);
    }
    else
    {
      snprintf(tmp, MAXSTRSIZE, "$%s", string_attr);
      LAD(gr1, tmp, NULL);
    }
    free(tmp);
    //----------------------------------
    if(!(result == RINT || result == RCHAR)) return(error("input statement error"));
    if(result == RINT)
      CALL("READINT", NULL);
    else if (result == RCHAR)
      CALL("READCHAR", NULL);
      

    while (token == TCOMMA) {
      token = next_token();
      result = variable();
      if (result == ERROR) return (ERROR);
      //---------------mpplc--------------
      char *tmp2;
      struct ID *i2;
      tmp2 = (char *)malloc(sizeof(char) * MAXSTRSIZE);
      //ローカルを探して投げればグローバル
      if ((i2 = search_localcr(string_attr)) != NULL)
      {
        snprintf(tmp2, MAXSTRSIZE, "$%s%%%s", i2->name, i2->procname);
        if (i2->ispara)
          LD_ra(gr1, tmp2, NULL);
        else
          LAD(gr1, tmp2, NULL);
      }
      else
      {
        snprintf(tmp2, MAXSTRSIZE, "$%s", string_attr);
        LAD(gr1, tmp2, NULL);
      }
      free(tmp2);
      if (!(result == RINT || result == RCHAR))
        return (error("input statement error"));
      if (result == RINT)
        CALL("READINT", NULL);
      else if (result == RCHAR)
        CALL("READCHAR", NULL);
      //----------------------------------
    }
    if(token != TRPAREN) return(error(") is not found"));
    token = next_token();
  }
  if(is_ln)
    CALL("READLINE",NULL);
  return(NORMAL);
}
int output_statement()
{
  int is_ln;
  if(token == TWRITE)
  {
    is_ln = 0;
    token = next_token();
  }
  else if(token == TWRITELN)
  {
    is_ln = 1;
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
  if(is_ln)
    CALL("WRITELINE",NULL);
  return(NORMAL);
}
int output_formal()
{
  int result = 0;
  is_output_format = 1;
  if(token == TSTRING)
  {
    //if(strlen(string_attr) == 1)
    //  return(error("output_formal strlen error"));
    if((strlen(string_attr) - 2)== 1)
    {
      char *num;
      num = (char *)malloc(sizeof(char) * MAXSTRSIZE);
      snprintf(num, MAXSTRSIZE, "%d", (int)string_attr[1]);
      LAD(gr1,num,NULL);
      LD_rr(gr2,gr0);
      CALL("WRITECHAR",NULL);
    }
    else
    {
      char *label = next_calllabel();
      LAD(gr1, label, NULL);
      add_DCList(label, 0);
      LD_rr(gr2, gr0);
      CALL("WRITESTR", NULL);
    }
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
      char *num;
      num = (char *)malloc(sizeof(char) * MAXSTRSIZE);
      snprintf(num,MAXSTACKSIZE,"%d",num_attr);
      LAD(gr2,num,NULL);
      token = next_token();
    }
    else
      LD_rr(gr2,gr0);

    if(result == RINT)
    {
      CALL("WRITEINT",NULL);
    }
    else if(result == RCHAR)
    {
      CALL("WRITECHAR", NULL);
    }
    else if(result == RBOOL)
    {
      CALL("WRITEBOOL", NULL);
    }
  }
  is_output_format = 0;
  return(NORMAL);
}
int empty_statement()
{
  return(NORMAL);
}
