#include "token-list.h"
extern int token;
extern int linenum;
extern char *tokenstr[NUMOFTOKEN + 1];
extern int tabnum;
extern int compound_tab[10];
int is_begin_line = 1; 
int is_procedure_begintoend = 0;
int compound_count = 0;
int next_token()
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
      printf("\n");
    is_begin_line = 1;
    
    tabnum = compound_tab[compound_count-1];
    
    compound_count--;
    if(compound_count == 0)
      is_procedure_begintoend = 0;
  }
  else if(token_num == TBEGIN)
  {
    if(before_token != TSEMI)
      printf("\n");
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
    is_begin_line = 1;
  }
  if ((before_token == TTHEN || before_token == TELSE || before_token == TDO) && (token_num != TBEGIN))
  {
    if((before_token == TELSE) && (token_num != TIF))
    {
      printf("\n");
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


  if(token_num != TNAME && token_num != TNUMBER && token_num != TSTRING)
    printf("%s",tokenstr[token_num]);
  else
    printf("%s",string_attr);

  if(token_num == TBEGIN)
  {
    tabnum++;
    is_begin_line = 1;
    printf("\n");
  }
  if(token_num == TSEMI)
  {
    printf("\n");
    is_begin_line = 1;
  }

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
  if(token != TVAR) return(error("var is not found"));
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
  if(token == TINTEGER || token == TBOOLEAN || token == TCHAR)
  {
    token = next_token();
    return(NORMAL);
  }
  else return(error("standard_type is not found"));
}
int array_type()
{
  if(token != TARRAY) return(error("array is not found"));
  token = next_token();
  if(token != TLSQPAREN) return(error("[ is not found"));
  token = next_token();
  if(token != TNUMBER) return(error("number is not found"));
  token = next_token();
  if(token != TRSQPAREN) return(error("] is not found"));
  token = next_token();
  if(token != TOF) return(error("of in not found"));
  token = next_token();
  if(standard_type() == ERROR) return(ERROR);
  return(NORMAL);
}
int subprogram_declaration()
{
  if (token != TPROCEDURE) return(error("procedure is not found."));
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
    if(expression() == ERROR) return(ERROR);
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
  if(expression() == ERROR) return(ERROR);
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
  if(token == TLPAREN)
  {
    token = next_token();
    if(expressions() == ERROR) return(ERROR);
    if(token != TRPAREN) return(error(") is not found"));
    token = next_token();
  }
  return(NORMAL);
}
int expressions()
{
  if(expression() == ERROR) return(ERROR);
  while (token == TCOMMA) {
    if(token != TCOMMA) return(error("commma is not found"));
    token = next_token();
    if(expression() == ERROR) return(ERROR);
  }
  return(NORMAL);
}
int return_statement()
{
  if(token != TRETURN) return(error("return is not found"));
  token = next_token();
  return(NORMAL);
}
int assignment_statement()
{
  if(left_part() == ERROR) return(ERROR);
  if(token != TASSIGN) return(error(":= is not found"));
  token = next_token();
  if(expression() == ERROR) return(ERROR);
  return(NORMAL);
}
int left_part()
{
  if(variable() == ERROR) return(ERROR);
  return(NORMAL);
}
int variable()
{
  if(variable_name() == ERROR) return(ERROR);
  if(token == TLSQPAREN)
  {
    token = next_token();
    if(expression() == ERROR) return(ERROR);
    if(token != TRSQPAREN) return(error("] is not found"));
    token = next_token();
  }
  return(NORMAL);
}
int expression()
{
  if(simple_expression() == ERROR) return(ERROR);
  while(token >= TEQUAL && token <= TGREQ)
  {
    token = next_token();
    if(simple_expression() == ERROR) return(ERROR);
  }
  return(NORMAL);
}
int simple_expression()
{
  if(token == TPLUS || token == TMINUS)
  {
    token = next_token();
  }
  if(term() == ERROR) return(ERROR);
  while(token == TPLUS || token == TMINUS || token == TOR)
  {
    token = next_token();
    if(term() == ERROR) return(ERROR);
  }
  return(NORMAL);
}
int term()
{
  if(factor() == ERROR) return(ERROR);
  while (token == TSTAR || token == TDIV || token == TAND)
  {
    if(multiplicative_operator() == ERROR) return(ERROR);
    if(factor() == ERROR) return(ERROR);
  }
  return(NORMAL);
}
int factor()
{
  switch (token) {
    case TNAME:
      if(variable() == ERROR) return(ERROR);
      break;
    case TNUMBER:
    case TFALSE:
    case TTRUE:
    case TSTRING:
      if(constant() == ERROR) return(ERROR);
      break;
    case TLPAREN:
      if(token != TLPAREN) return(error("( is not found"));
      token = next_token();
      if(expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error(") is not found"));
      token = next_token();
      break;
    case TNOT:
      if(token != TNOT) return(error("not is not found"));
      token = next_token();
      if(factor() == ERROR) return(ERROR);
      break;
    case TINTEGER:
    case TBOOLEAN:
    case TCHAR:
      if(standard_type() == ERROR) return(ERROR);
      if(token != TLPAREN) return(error("( is not found"));
      token = next_token();
      if(expression() == ERROR) return(ERROR);
      if(token != TRPAREN) return(error(") is not found"));
      token = next_token();
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
      break;
    case TFALSE:
      if(token != TFALSE) return(error("false is not found"));
      token = next_token();
      break;
    case TTRUE:
      if(token != TTRUE) return(error("true is not found"));
      token = next_token();
      break;
    case TSTRING:
      if(token != TSTRING) return(error("string is not found"));
      token = next_token();
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
      break;
    case TDIV:
      if(token != TDIV) return(error("div is not found"));
      token = next_token();
      break;
    case TAND:
      if(token != TAND) return(error("and is not found"));
      token = next_token();
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
      break;
    case TMINUS:
      if(token != TMINUS) return(error("- is not found"));
      token = next_token();
      break;
    case TOR:
      if(token != TOR) return(error("or is not found"));
      token = next_token();
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
   return(NORMAL);
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
    if(variable() == ERROR) return(ERROR);

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
  if(token == TSTRING)
  {
    token = next_token();
  }
  else if(expression() == NORMAL)
  {
    if(token == TCOLON)
    {
      token = next_token();
      if(token != TNUMBER) return(error("number is not found"));
      token = next_token();
    }
  }
  else
  {
    return(error("output_formal error"));
  }
  return(NORMAL);
}
int empty_statement()
{
  return(NORMAL);
}
