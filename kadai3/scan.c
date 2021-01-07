#include "token-list.h"
FILE *fp;
char cbuf;
int linenum;
int num_attr;
char string_attr[MAXSTRSIZE];
int tabnum;
extern char *tokenstr[NUMOFTOKEN + 1];
extern key key_keyword[KEYWORDSIZE];
extern key key_symbol[KEYWORDSIZE];
int init_scan(char *filename)
{
  fp = fopen(filename,"r");
  if (fp == NULL)
  {
    printf("ファイルオープン失敗\n");
    return -1;
  }
  cbuf = fgetc(fp);
  if(cbuf < 0)return -1;
  linenum = 1;
  tabnum = 0;
  init_idtab();
  return 0;
}
int scan(void)
{
  int i = 0;
  char token[MAXSTRSIZE];

  memset(token, 0, sizeof(token));

  if(cbuf < 0)return -1;

  while(cbuf <= 32 || cbuf == 123 || cbuf == 47)
  {
    if ((cbuf == 13) || (cbuf == 10))
    {
      char before_cbuf = cbuf;
      cbuf = fgetc(fp);
      if ((before_cbuf == 10 && cbuf == 13) || (before_cbuf == 13 && cbuf == 10))
      {
        cbuf = fgetc(fp);//CRLF
      }
    }
    else if(cbuf == 123)//{}コメント文
    {
      while(1)
      {
        cbuf = fgetc(fp);
        if (cbuf < 0)
        {
          error("注釈内でEOFが発生しています．");
          return -1;
        }
        if (cbuf == 125)
        {
          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;
          break;
        }
      }
    }
    else if(cbuf == 47)// /
    {
      cbuf = fgetc(fp);
      if (cbuf < 0)
      {
        error("注釈の開始時にEOFが発生しています．");
        return -1;
      }

      if(cbuf == 42)// * コメント文開始
      {
        while (1)
        {
          cbuf = fgetc(fp);//コメント文中身
          if (cbuf < 0)
          {
            error("注釈内でEOFが発生しています．");
            return -1;
          }

          if (cbuf == 42) // *
          {
            cbuf = fgetc(fp);
            if (cbuf < 0)
            {
              return -1;
            }
            if(cbuf == 47)// /
            {
              cbuf = fgetc(fp);
              break;
              //return 0;
            }
          }
        }
      }
    }
    else if(cbuf < 0)
        return -1;

    else
      cbuf = fgetc(fp);

  }
  if((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122))//アルファベット
  {
    while(1){
      //strcat(token,cbuf);
      snprintf(token,MAXSTRSIZE,"%s%c",token,cbuf);

      cbuf = fgetc(fp);
      

      if (!((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122) || (cbuf >= 48 && cbuf <= 57)))
      {//文字でも数字でもなければ

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
      }else if(cbuf == -1)
      {
        return -1;
      }
    }
  }
  else if(cbuf >= 48 && cbuf <= 57)//数字
  {
    while(1){
      snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
      cbuf = fgetc(fp);
      if(cbuf < 0)return -1;
      if (!(cbuf >= 48 && cbuf <= 57))//数字でなくなったら
      {
        num_attr = atoi(token);
        memset(string_attr, 0, sizeof(string_attr));
        snprintf(string_attr, MAXSTRSIZE, "%s", token);
        return TNUMBER;
        break;
      }
    }
  }
  else if ((cbuf >= 40 && cbuf <= 46) || (cbuf >= 58 && cbuf <= 62) || cbuf == 91 || cbuf == 93)//記号
  {
    snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
    char before_cbuf = cbuf;
    cbuf = fgetc(fp);if(cbuf < 0)
    {
      for (i = 0; i < SYMBOLSIZE; i++)
      {
        if (strcmp(token, key_symbol[i].keyword) == 0)
        {
          return key_symbol[i].keytoken;
        }
      }
      return -1;
    }if(cbuf < 0)return -1;
    if ((before_cbuf == 60 && cbuf == 62) || (before_cbuf == 60 && cbuf == 61) || (before_cbuf == 62 && cbuf == 61) || (before_cbuf == 58 && cbuf == 61)) //<> or <= or >= or :=
    {
      snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
      cbuf = fgetc(fp);
      if(cbuf < 0)return -1;
    }

    for (i = 0; i < SYMBOLSIZE; i++)
    {
      if (strcmp(token, key_symbol[i].keyword) == 0)
      {
        return key_symbol[i].keytoken;
      }
    }
    return -1;//error
  }
  else if (cbuf == 39) //string
  {
    snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
    cbuf = fgetc(fp);
    while (1)
    {

      if (cbuf < 0)
      {
        error("文字列内でEOFが発生指定してます．");
        return -1;
      }

      if (cbuf == 39)//文字終了
      {
        snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
        cbuf = fgetc(fp);

        if (cbuf == 39) //連続で文字列があったら，
        {
          snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
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
        if (cbuf < 0)
        {
          error("文字列内でEOFが発生しています．");
          return -1;
        }
        else if ((cbuf == 13) || (cbuf == 10))
        {
          error("文字列内に改行が含まれています．");
          return -1;
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
