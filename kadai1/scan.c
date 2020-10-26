// 字句解析器の本体

#include "token-list.h"
FILE *fp;
char cbuf; // buffer for one string
int linenum;
int num_attr;
char string_attr[MAXSTRSIZE];
extern char *tokenstr[NUMOFTOKEN + 1];
extern key key_keyword[KEYWORDSIZE];
extern key key_symbol[KEYWORDSIZE];

int init_scan(char *filename)
{
    fp = fopen(filename,"r");
    if (fp == NULL)
    {
        printf("Cannot open the file\n");
        return -1;
    }
    cbuf = fgetc(fp);
    if(cbuf < 0)return -1;
    linenum = 1;
    init_idtab();
    return 0;
}

int scan(void)
{
    int i = 0;
    char token[MAXSTRSIZE];

    memset(token, 0, sizeof(token));
    if(cbuf < 0)return -1;
    // special character
    if(cbuf <= 32){
      while(1){
        // new line
        if ((cbuf == 13) || (cbuf == 10)){
            char before_cbuf = cbuf;
            cbuf = fgetc(fp);
            // adapt every environment 
            if ((before_cbuf == 10 && cbuf == 13) || (before_cbuf == 13 && cbuf == 10))
            {
                cbuf = fgetc(fp);//CRLF
            }
        }
        else if(cbuf < 0){
            return -1;
        }
        else{
            cbuf = fgetc(fp);
        }
        if (cbuf >= 39){
          break;
        }
      }
    }
    // Alphabet
    if((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122)){
      while(1){
          snprintf(token,MAXSTRSIZE,"%s%c",token,cbuf);

          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;

          // Not Alphabet and Number
          if (!((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122) || (cbuf >= 48 && cbuf <= 57))){

              for(i = 0;i < KEYWORDSIZE;i++)
              {
                  if (strcmp(token, key_keyword[i].keyword) == 0)
                      return key_keyword[i].keytoken;
              }
              memset(string_attr, 0, sizeof(string_attr));
              snprintf(string_attr, MAXSTRSIZE, "%s", token);
              id_countup(string_attr);
              return TNAME;
              break;
          }
      }
    }
    // Number
    else if(cbuf >= 48 && cbuf <= 57){
      while(1){
          snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;
          // Not Number
          if (!(cbuf >= 48 && cbuf <= 57))
          {
              num_attr = atoi(token);
              memset(string_attr, 0, sizeof(string_attr));
              snprintf(string_attr, MAXSTRSIZE, "%s", token);
              return TNUMBER;
              break;
          }
      }
    }
    else if ((cbuf >= 40 && cbuf <= 46) || (cbuf >= 58 && cbuf <= 62) || cbuf == 91 || cbuf == 93){
      snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);

      char before_cbuf = cbuf;
      cbuf = fgetc(fp);
      if(cbuf < 0){
        for (i = 0; i < SYMBOLSIZE; i++){
          if (strcmp(token, key_symbol[i].keyword) == 0){
              return key_symbol[i].keytoken;
          }
        }
        return -1;
      }
      // Comparison
      if ((before_cbuf == 60 && cbuf == 62) || (before_cbuf == 60 && cbuf == 61) || (before_cbuf == 62 && cbuf == 61) || (before_cbuf == 58 && cbuf == 61)){
        snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
        cbuf = fgetc(fp);
        if(cbuf < 0)return -1;
      }

      for (i = 0; i < SYMBOLSIZE; i++){
        if (strcmp(token, key_symbol[i].keyword) == 0){
            return key_symbol[i].keytoken;
        }
      }
      return -1;//error
    }
    // string
    else if (cbuf == 39) {
      cbuf = fgetc(fp);
      while (1){

        if (cbuf < 0){
            error("EOF happened\n");
            return -1;
        }

        // end of string
        if (cbuf == 39){
          cbuf = fgetc(fp);

          // string after other string
          if (cbuf == 39) {
              cbuf = fgetc(fp);
              continue;
          }
          memset(string_attr, 0, sizeof(string_attr));
          snprintf(string_attr, MAXSTRSIZE, "%s", token);
          return TSTRING;
        }
        else
        {
          snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
          cbuf = fgetc(fp);
          if (cbuf < 0){
              error("EOF happened\n");
              return -1;
          }
          else if ((cbuf == 13) || (cbuf == 10)){
              error("included new line in string\n");
              return -1;
          }
        }
      }
    }
    // {comment}
    else if(cbuf == 123){
      while(1){
        cbuf = fgetc(fp);
        if (cbuf < 0){
            error("EOF happened\n");
            return -1;
        }
        if (cbuf == 125){
            cbuf = fgetc(fp);
            if(cbuf < 0)return -1;
            return 0;
        }
      }
    }
    // /*comment*/
    else if(cbuf == 47){
      cbuf = fgetc(fp);
      if (cbuf < 0){
        error("EOF happened\n");
        return -1;
      }

      if(cbuf == 42){
        while (1){
          cbuf = fgetc(fp);
          if (cbuf < 0){
            error("EOF happened\n");
            return -1;
          }

          if (cbuf == 42){
            cbuf = fgetc(fp);
            if (cbuf < 0){
              return -1;
            }
            if(cbuf == 47){
              cbuf = fgetc(fp);
              return 0;
            }
          }
        }
      }
    }
    return -1;
}

int get_linenum(void)
{
    return linenum;
}
void end_scan(void){
    fclose(fp);
}
