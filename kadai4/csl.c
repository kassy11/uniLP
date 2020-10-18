#include "token-list.h"
int label_number = 0;
struct DCnode{
  char *label;
  char *value;
  struct DCnode *next;
} *DCroot;
extern FILE *outfp;
void write_label_DL(char *label)
{
  fprintf(outfp,"$%s\n",label);
}
void write_label(char *label)
{
  fprintf(outfp,"%s\n", label);
}
void write_label_DC(char *label,char *value)
{
  fprintf(outfp,"%s\tDC\t%s\n", label,value);
}
void write_label_DS(char *label, char *value)
{
  fprintf(outfp,"%s\tDS\t%s\n", label, value);
}
//void DC(char *label,char *value)
//{
//  fprintf(outfp,"\t%s\tDC\t%s\n",label,value);
//}
void RPUSH()
{
  fprintf(outfp,"\tRPUSH\n");
}
void RPOP()
{
  fprintf(outfp,"\tRPOP\n");
}
void OUT(char *val1,char *val2)
{
  fprintf(outfp,"\tOUT\t%s, %s\n",val1,val2);
}
void IN(char *val1, char *val2)
{
  fprintf(outfp,"\tIN\t%s, %s\n", val1, val2);
}
void LD_rr(char *r1,char *r2)
{
  fprintf(outfp,"\tLD\t%s, %s\n",r1,r2);
}
void LD_ra(char *r,char *adr,char *x)
{
  if(x == NULL)
    fprintf(outfp,"\tLD\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tLD\t%s, %s, %s\n", r, adr, x);
}
void ST(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tST\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tST\t%s, %s, %s\n", r, adr, x);
}
void LAD(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tLAD\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tLAD\t%s, %s, %s\n", r, adr, x);
}
void ADDA_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tADDA\t%s, %s\n", r1, r2);
}
void ADDA_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tADDA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tADDA\t%s, %s, %s\n", r, adr, x);
}
void ADDL_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tADDL\t%s, %s\n", r1, r2);
}
void ADDL_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tADDL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tADDL\t%s, %s, %s\n", r, adr, x);
}
void SUBA_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tSUBA\t%s, %s\n", r1, r2);
}
void SUBA_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSUBA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tSUBA\t%s, %s, %s\n", r, adr, x);
}
void SUBL_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tSUBL\t%s, %s\n", r1, r2);
}
void SUBL_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSUBL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tSUBL\t%s, %s, %s\n", r, adr, x);
}
void AND_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tAND\t%s, %s\n", r1, r2);
}
void AND_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tAND\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tAND\t%s, %s, %s\n", r, adr, x);
}
void OR_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tOR\t%s, %s\n", r1, r2);
}
void OR_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tOR\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tOR\t%s, %s, %s\n", r, adr, x);
}
void XOR_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tXOR\t%s, %s\n", r1, r2);
}
void XOR_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tXOR\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tXOR\t%s, %s, %s\n", r, adr, x);
}
void MULA_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tMULA\t%s, %s\n", r1, r2);
}
void MULA_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tMULA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tMULA\t%s, %s, %s\n", r, adr, x);
}
void MULL_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tMULL\t%s, %s\n", r1, r2);
}
void MULL_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tMULL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tMULL\t%s, %s, %s\n", r, adr, x);
}
void DIVA_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tDIVA\t%s, %s\n", r1, r2);
}
void DIVA_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tDIVA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tDIVA\t%s, %s, %s\n", r, adr, x);
}
void DIVL_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tDIVL\t%s, %s\n", r1, r2);
}
void DIVL_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tDIVL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tDIVL\t%s, %s, %s\n", r, adr, x);
}
void CPA_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tCPA\t%s, %s\n", r1, r2);
}
void CPA_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tCPA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tCPA\t%s, %s, %s\n", r, adr, x);
}
void CPL_rr(char *r1, char *r2)
{
  fprintf(outfp,"\tCPL\t%s, %s\n", r1, r2);
}
void CPL_ra(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tCPL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tCPL\t%s, %s, %s\n", r, adr, x);
}
void SLA(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSLA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tSLA\t%s, %s, %s\n", r, adr, x);
}
void SRA(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSRA\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tSRA\t%s, %s, %s\n", r, adr, x);
}
void SLL(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSLL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tSLL\t%s, %s, %s\n", r, adr, x);
}
void SRL(char *r, char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSRL\t%s, %s\n", r, adr);
  else
    fprintf(outfp,"\tSRL\t%s, %s, %s\n", r, adr, x);
}
void JPL(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tJPL\t%s\n",adr);
  else
    fprintf(outfp,"\tJPL\t%s, %s\n",adr, x);
}
void JMI(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tJMI\t%s\n", adr);
  else
    fprintf(outfp,"\tJMI\t%s, %s\n", adr, x);
}
void JNZ(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tJNZ\t%s\n", adr);
  else
    fprintf(outfp,"\tJNZ\t%s, %s\n", adr, x);
}
void JZE(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tJZE\t%s\n", adr);
  else
    fprintf(outfp,"\tJZE\t%s, %s\n", adr, x);
}
void JOV(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tJOV\t%s\n", adr);
  else
    fprintf(outfp,"\tJOV\t%s, %s\n", adr, x);
}
void JUMP(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tJUMP\t%s\n", adr);
  else
    fprintf(outfp,"\tJUMP\t%s, %s\n", adr, x);
}
void PUSH(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tPUSH\t%s\n", adr);
  else
    fprintf(outfp,"\tPUSH\t%s, %s\n", adr, x);
}
void POP(char *r)
{
  fprintf(outfp,"\tPOP\t%s\n", r);
}
void CALL(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tCALL\t%s\n", adr);
  else
    fprintf(outfp,"\tCALL\t%s, %s\n", adr, x);
}
void RET()
{
  fprintf(outfp,"\tRET\n");
}
void SVC(char *adr, char *x)
{
  if (x == NULL)
    fprintf(outfp,"\tSVC\t%s\n", adr);
  else
    fprintf(outfp,"\tSVC\t%s, %s\n", adr, x);
}
void NOP()
{
  fprintf(outfp,"\tNOP\n");
}
char *next_calllabel()
{
  char *result;
  result = (char *)malloc(sizeof(char) * 6);
  label_number++;
  snprintf(result, 6, "L%04d", label_number);
  return(result);
}
void init_DCList()
{
  DCroot = NULL;
}
void add_DCList(char *l,int mode)
{
  //mode == 0 : DC 'string'
  //mode == 1 : DC 0
  struct DCnode *node,*n,*before_n;
  if ((node = (struct DCnode *)malloc(sizeof(struct DCnode))) == NULL)
  {
    printf("can not malloc in add_DCnode\n");
    return;
  }
  if ((node->label = (char *)malloc(MAXSTRSIZE)) == NULL)
    printf("can not malloc add_DCnode\n");
  else
    strcpy(node->label, l);
  if ((node->value = (char *)malloc(MAXSTRSIZE)) == NULL)
    printf("can not malloc add_DCnode\n");

  if(mode == 0)
    strcpy(node->value, string_attr);
  else
    strcpy(node->value, "0");
  node->next = NULL;
  if(DCroot == NULL)
    DCroot = node;
  else
  {
    for(n = DCroot;n != NULL;n = n->next)
    {
      before_n = n;
    }
    before_n->next = node;
  }
}
void output_DCList()
{
  struct DCnode *node;

  for(node = DCroot;node != NULL;node = node->next)
  {
    fprintf(outfp,"%s\tDC\t%s\n",node->label,node->value);
  }
}
void DC_print_global(char *name, int num) //<$aaa DC> num
{
  if(num != 0)
    fprintf(outfp, "$%s\tDS\t%d\n", name, num);
  else
    fprintf(outfp, "$%s\tDC\t%d\n", name, num);
}
void DC_print_local(char *name, char *pname, int num) //<$aaa DC> num
{
  if (num != 0)
    fprintf(outfp, "$%s%%%s\tDS\t%d\n", name, pname, num);
  else
    fprintf(outfp, "$%s%%%s\tDC\t%d\n", name, pname, num);
}
void relational_casl_code(int rela)
{
  //printf("\n rela %d\n",rela);
  //fflush(stdout);
  if(rela < TEQUAL || rela > TGREQ)
  {
    return;
  }
  POP(gr2);
  CPA_rr(gr2,gr1);
  char *next_label1 = next_calllabel();
  char *next_label2 = next_calllabel();
  switch (rela)
  {
  case TEQUAL:
    JZE(next_label1, NULL);
    LD_rr(gr1, gr0);
    JUMP(next_label2, NULL);
    write_label(next_label1);
    LAD(gr1, "1", NULL);
    write_label(next_label2);
    break;
  case TNOTEQ:
    JNZ(next_label1, NULL);
    LD_rr(gr1, gr0);
    JUMP(next_label2, NULL);
    write_label(next_label1);
    LAD(gr1, "1", NULL);
    write_label(next_label2);
    break;
  case TLE:
    JMI(next_label1, NULL);
    LD_rr(gr1, gr0);
    JUMP(next_label2, NULL);
    write_label(next_label1);
    LAD(gr1, "1", NULL);
    write_label(next_label2);
    break;
  case TLEEQ:
    JPL(next_label1, NULL);
    LAD(gr1, "1",NULL);
    JUMP(next_label2, NULL);
    write_label(next_label1);
    LD_rr(gr1,gr0);
    write_label(next_label2);
    break;
  case TGR:
    JPL(next_label1, NULL);
    LD_rr(gr1, gr0);
    JUMP(next_label2, NULL);
    write_label(next_label1);
    LAD(gr1, "1", NULL);
    write_label(next_label2);
    break;
  case TGREQ:
    JMI(next_label1, NULL);
    LAD(gr1, "1", NULL);
    JUMP(next_label2, NULL);
    write_label(next_label1);
    LD_rr(gr1, gr0);
    write_label(next_label2);
    break;
  default:
    error("relational_casl_code error");
    break;
  }
  //char *next_label1 = next_calllabel();
  //char *next_label2 = next_calllabel();
  //JZE(next_label1, NULL);
  //LD_rr(gr1,gr0);
  //JUMP(next_label2,NULL);
  //write_label(next_label1);
  //LAD(gr1,"1",NULL);
}
void EOVF()
{
  write_label("EOVF");
  CALL("WRITELINE",NULL);
  LAD(gr1,"EOVF1",NULL);
  LD_rr(gr2,gr0);
  CALL("WRITESTR",NULL);
  CALL("WRITELINE",NULL);
  SVC("1",NULL);
  write_label_DC("EOVF1", "\'***** Run-Time Error : Overflow *****\'");
}
void E0DIV()
{
  write_label("E0DIV");
  JNZ("EOVF",NULL);
  CALL("WRITELINE",NULL);
  LAD(gr1, "E0DIV1",NULL);
  LD_rr(gr2, gr0);
  CALL("WRITESTR",NULL);
  CALL("WRITELINE",NULL);
  SVC("2",NULL);
  write_label_DC("E0DIV1","\'***** Run-Time Error : Zero-Divide *****\'");
}
void EROV()
{
  write_label("EROV");
  CALL("WRITELINE",NULL);
  LAD(gr1, "EROV1",NULL);
  LD_rr(gr2, gr0);
  CALL("WRITESTR",NULL);
  CALL("WRITELINE",NULL);
  SVC("3",NULL);
  write_label_DC("EROV1", "\'***** Run-Time Error : Range-Over in Array Index *****\'");

}
void WRITECHAR()
{
  write_label("WRITECHAR");
  RPUSH();
  LD_ra(gr6, "SPACE",NULL);
  LD_ra(gr7, "OBUFSIZE",NULL);
  write_label("WC1");
  SUBA_ra(gr2, "ONE",NULL);
  JZE("WC2",NULL);
  JMI("WC2",NULL);
  ST(gr6, "OBUF",gr7);
  CALL("BOVFCHECK",NULL);
  JUMP("WC1",NULL);
  write_label("WC2");
  ST(gr1, "OBUF",gr7);
  CALL("BOVFCHECK",NULL);
  ST(gr7, "OBUFSIZE",NULL);
  RPOP();
  RET();
}
void WRITESTR()
{
  write_label("WRITESTR");
  RPUSH();
  LD_rr(gr6, gr1);
  write_label("WS1");
  LD_ra(gr4, "0",gr6);
  JZE("WS2",NULL);
  ADDA_ra(gr6, "ONE",NULL);
  SUBA_ra(gr2, "ONE",NULL);
  JUMP("WS1",NULL);
  write_label("WS2");
  LD_ra(gr7, "OBUFSIZE",NULL);
  LD_ra(gr5, "SPACE",NULL);
  write_label("WS3");
  SUBA_ra(gr2, "ONE",NULL);
  JMI("WS4",NULL);
  ST(gr5, "OBUF",gr7);
  CALL("BOVFCHECK",NULL);
  JUMP("WS3",NULL);
  write_label("WS4");
  LD_ra(gr4, "0",gr1);
  JZE("WS5",NULL);
  ST(gr4, "OBUF",gr7);
  ADDA_ra(gr1, "ONE",NULL);
  CALL("BOVFCHECK",NULL);
  JUMP("WS4",NULL);
  write_label("WS5");
  ST(gr7, "OBUFSIZE",NULL);
  RPOP();
  RET();
  write_label("BOVFCHECK");
  ADDA_ra(gr7, "ONE",NULL);
  CPA_ra(gr7, "BOVFLEVEL",NULL);
  JMI("BOVF1",NULL);
  CALL("WRITELINE",NULL);
  LD_ra(gr7, "OBUFSIZE",NULL);
  write_label("BOVF1");
  RET();
  write_label_DC("BOVFLEVEL","256");
}
void WRITEINT()
{
  write_label("WRITEINT");
  RPUSH();
  LD_rr(gr7, gr0);
  CPA_rr(gr1, gr0);
  JPL("WI1", NULL);
  JZE("WI1", NULL);
  LD_rr(gr4, gr0 );
  SUBA_rr(gr4, gr1);
  CPA_rr(gr4, gr1);
  JZE("WI6",NULL);
  LD_rr(gr1, gr4);
  LD_ra(gr7, "ONE",NULL);
  write_label("WI1");
  LD_ra(gr6, "SIX",NULL);
  ST(gr0, "INTBUF",gr6);
  SUBA_ra(gr6, "ONE",NULL);
  CPA_rr(gr1, gr0);
  JNZ("WI2",NULL);
  LD_ra(gr4, "ZERO",NULL);
  ST(gr4, "INTBUF",gr6);
  JUMP("WI5",NULL);
  write_label("WI2");
  CPA_rr(gr1, gr0);
  JZE("WI3",NULL);
  LD_rr(gr5, gr1);
  DIVA_ra(gr1, "TEN",NULL);
  LD_rr(gr4, gr1);
  MULA_ra(gr4, "TEN",NULL);
  SUBA_rr(gr5, gr4);
  ADDA_ra(gr5, "ZERO",NULL);
  ST(gr5, "INTBUF",gr6);
  SUBA_ra(gr6, "ONE",NULL);
  JUMP("WI2",NULL);
  write_label("WI3");
  CPA_rr(gr7, gr0);
  JZE("WI4",NULL);
  LD_ra(gr4, "MINUS",NULL);
  ST(gr4, "INTBUF",gr6);
  JUMP("WI5",NULL);
  write_label("WI4");
  ADDA_ra(gr6, "ONE",NULL);
  write_label("WI5");
  LAD(gr1, "INTBUF",gr6);
  CALL("WRITESTR",NULL);
  RPOP();
  RET();
  write_label("WI6");
  LAD(gr1, "MMINT",NULL);
  CALL("WRITESTR",NULL);
  RPOP();
  RET();
  write_label_DC("MMINT","-32768");
}
void WRITEBOOL()
{
  write_label("WRITEBOOL");
  RPUSH();
  CPA_rr(gr1, gr0);
  JZE("WB1",NULL);
  LAD(gr1, "WBTRUE",NULL);
  JUMP("WB2",NULL);
  write_label("WB1");
  LAD(gr1, "WBFALSE",NULL);
  write_label("WB2");
  CALL("WRITESTR",NULL);
  RPOP();
  RET();
  write_label_DC("WBTRUE","'TRUE'");
  write_label_DC("WBFALSE","'FALSE'");
}
void WRITELINE()
{
  write_label("WRITELINE");
  RPUSH();
  LD_ra(gr7, "OBUFSIZE",NULL);
  LD_ra(gr6, "NEWLINE",NULL);
  ST(gr6, "OBUF",gr7);
  ADDA_ra(gr7, "ONE",NULL);
  ST(gr7, "OBUFSIZE",NULL);
  OUT("OBUF", "OBUFSIZE");
  ST(gr0, "OBUFSIZE",NULL);
  RPOP();
  RET();
}
void FLUSH()
{
  write_label("FLUSH");
  RPUSH();
  LD_ra(gr7, "OBUFSIZE",NULL);
  JZE("FL1",NULL);
  CALL("WRITELINE",NULL);
  write_label("FL1");
  RPOP();
  RET();
}
void READCHAR()
{
  write_label("READCHAR");
  RPUSH();
  LD_ra(gr5, "RPBBUF",NULL);
  JZE("RC0",NULL);
  ST(gr5, "0",gr1);
  ST(gr0, "RPBBUF",NULL);
  JUMP("RC3",NULL);
  write_label("RC0");
  LD_ra(gr7, "INP",NULL);
  LD_ra(gr6, "IBUFSIZE",NULL);
  JNZ("RC1",NULL);
  IN("IBUF", "IBUFSIZE");
  LD_rr(gr7, gr0);
  write_label("RC1");
  CPA_ra(gr7, "IBUFSIZE",NULL);
  JNZ("RC2",NULL);
  LD_ra(gr5, "NEWLINE",NULL);
  ST(gr5, "0",gr1);
  ST(gr0, "IBUFSIZE",NULL);
  ST(gr0, "INP",NULL);
  JUMP("RC3",NULL);
  write_label("RC2");
  LD_ra(gr5, "IBUF",gr7);
  ADDA_ra(gr7, "ONE",NULL);
  ST(gr5, "0",gr1);
  ST(gr7, "INP",NULL);
  write_label("RC3");
  RPOP();
  RET();
}
void READINT()
{
  write_label("READINT");
  RPUSH();
  write_label("RI1");
  CALL("READCHAR",NULL);
  LD_ra(gr7, "0",gr1);
  CPA_ra(gr7, "SPACE",NULL);
  JZE("RI1",NULL);
  CPA_ra(gr7, "TAB",NULL);
  JZE("RI1",NULL);
  CPA_ra(gr7, "NEWLINE",NULL);
  JZE("RI1",NULL);
  LD_ra(gr5, "ONE",NULL);
  CPA_ra(gr7, "MINUS",NULL);
  JNZ("RI4",NULL);
  LD_ra(gr5, gr0,NULL);
  CALL("READCHAR",NULL);
  LD_ra(gr7, "0",gr1);
  write_label("RI4");
  LD_ra(gr6, gr0 ,NULL);
  write_label("RI2");
  CPA_ra(gr7, "ZERO",NULL);
  JMI("RI3",NULL);
  CPA_ra(gr7, "NINE",NULL);
  JPL("RI3",NULL);
  MULA_ra(gr6, "TEN",NULL);
  ADDA_rr(gr6, gr7);
  SUBA_ra(gr6, "ZERO",NULL);
  CALL("READCHAR",NULL);
  LD_ra(gr7, "0",gr1);
  JUMP("RI2",NULL);
  write_label("RI3");
  ST(gr7, "RPBBUF",NULL);
  ST(gr6, "0",gr1);
  CPA_rr(gr5, gr0);
  JNZ("RI5",NULL);
  SUBA_rr(gr5, gr6);
  ST(gr5, "0",gr1);
  write_label("RI5");
  RPOP();
  RET();
}
void READLINE()
{
  write_label("READLINE");
  ST(gr0, "IBUFSIZE",NULL);
  ST(gr0, "INP",NULL);
  ST(gr0, "RPBBUF",NULL);
  RET();
}
void other()
{
  write_label_DC("ONE","1");
  write_label_DC("SIX","6");
  write_label_DC("TEN","10");
  write_label_DC("SPACE","#0020");
  write_label_DC("MINUS","#002D");
  write_label_DC("TAB","#0009");
  write_label_DC("ZERO","#0030");
  write_label_DC("NINE","#0039");
  write_label_DC("NEWLINE","#000A");
  write_label_DS("INTBUF","8");
  write_label_DC("OBUFSIZE","0");
  write_label_DC("IBUFSIZE","0");
  write_label_DC("INP", "0");
  write_label_DS("OBUF","257");
  write_label_DS("IBUF","257");
  write_label_DC("RPBBUF","0");
	fprintf(outfp,"\tEND\n");
}