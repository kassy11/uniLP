// Main Program

#include "token-list.h"

/* keyword list */
key key_keyword[KEYWORDSIZE] = {
        {"and", TAND},
        {"array", TARRAY},
        {"begin", TBEGIN},
        {"boolean", TBOOLEAN},
        {"break", TBREAK},
        {"call", TCALL},
        {"char", TCHAR},
        {"div", TDIV},
        {"do", TDO},
        {"else", TELSE},
        {"end", TEND},
        {"false", TFALSE},
        {"if", TIF},
        {"integer", TINTEGER},
        {"not", TNOT},
        {"of", TOF},
        {"or", TOR},
        {"procedure", TPROCEDURE},
        {"program", TPROGRAM},
        {"read", TREAD},
        {"readln", TREADLN},
        {"return", TRETURN},
        {"then", TTHEN},
        {"true", TTRUE},
        {"var", TVAR},
        {"while", TWHILE},
        {"write", TWRITE},
        {"writeln", TWRITELN}};
/* symbol list*/
key key_symbol[SYMBOLSIZE] = {
        {"+",TPLUS},
        {"-",TMINUS},
        {"*",TSTAR},
        {"=",TEQUAL},
        {"<>",TNOTEQ},
        {"<",TLE},
        {"<=",TLEEQ},
        {">",TGR},
        {">=",TGREQ},
        {"(",TLPAREN},
        {")",TRPAREN},
        {"[",TLSQPAREN},
        {"]",TRSQPAREN},
        {":=",TASSIGN},
        {".",TDOT},
        {",",TCOMMA},
        {":",TCOLON},
        {";",TSEMI}};
/* Token counter */
int numtoken[NUMOFTOKEN + 1];

/* string of each token */
char *tokenstr[NUMOFTOKEN + 1] = {
        "",
        "NAME", "program", "var", "array", "of", "begin", "end", "if", "then",
        "else", "procedure", "return", "call", "while", "do", "not", "or",
        "div", "and", "char", "integer", "boolean", "readln", "writeln", "true",
        "false", "NUMBER", "STRING", "+", "-", "*", "=", "<>", "<", "<=", ">",
        ">=", "(", ")", "[", "]", ":=", ".", ",", ":", ";", "read", "write", "break"};

// 起動時プログラム
int main(int nc, char *np[]){
    // コマンドライン引数で「test」を受け取るとテストを開始する
    if(strcmp(np[1], "test")==0){
      printf("----blackbox test----\n");
      blackbox();
      printf("\n");
      printf("----whitebox test----\n");
      // work in progress
//      whitebox();
      printf("\n");
      printf("----integration test---\n");
      printf("finnally, you can run integration test by");
      printf("\x1b[32m make test\n");
      return 0;
    }
    
    int token, i;

    if (nc < 2){
      printf("File name id not given.\n");
      return 0;
    }
    if (init_scan(np[1]) < 0){
      printf("File %s can not open.\n", np[1]);
      return 0;
    }

    // scan()を実行し、numtoken配列にトークンの数を加算していく
    while ((token = scan()) >= 0) {
      numtoken[token]++;
    }
    end_scan();
    printf("----Token count----\n");
    for(i = 1;i < NUMOFTOKEN + 1;i++){
      if(numtoken[i] >= 1){
          printf("%s   ::::  %d\n",tokenstr[i],numtoken[i]);
      }
    }
    printf("-----Name Count-----\n");
    print_idtab();
    release_idtab();
}

void error(char *mes){
  printf("\n ERROR: %s\n", mes);
  end_scan();
}