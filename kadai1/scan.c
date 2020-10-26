// 字句解析器の本体

#include "token-list.h"
FILE *fp;
char cbuf; // 一文字分のバッファ
int linenum;
int num_attr;
char string_attr[MAXSTRSIZE];
// scan()の戻り値が文字列・名前のときその文字列を格納する、符号なし整数のときは数字列
extern char *tokenstr[NUMOFTOKEN + 1];
extern key key_keyword[KEYWORDSIZE];
extern key key_symbol[KEYWORDSIZE];

// fileをオープンする
int init_scan(char *filename)
{
    fp = fopen(filename,"r");
    if (fp == NULL)
    {
        //失敗と表示し終了
        printf("ファイルオープン失敗\n");
        return -1;
    }

    // ファイル読み込み成功したら１文字分バッファに読み込んでおく
    cbuf = fgetc(fp);
    if(cbuf < 0)return -1;
    linenum = 1;
    init_idtab();
    return 0;
}

// トークンをひとつスキャンして、次のトークンを返す
int scan(void)
{
    int i = 0;
    char token[MAXSTRSIZE];

    memset(token, 0, sizeof(token));
    if(cbuf < 0)return -1;
    // 特殊文字
    if(cbuf <= 32){
      while(1){
        // 改行
        if ((cbuf == 13) || (cbuf == 10)){
            char before_cbuf = cbuf;
            cbuf = fgetc(fp);
            // 組み合わせて改行を表現する環境に対応
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
    // アルファベット
    if((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122)){
      while(1){
          snprintf(token,MAXSTRSIZE,"%s%c",token,cbuf);

          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;

          // 文字と数字以外の時（区切り）
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
    // 数字
    else if(cbuf >= 48 && cbuf <= 57){
      while(1){
          snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;
          // 数字以外になったとき
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
    // 記号
    else if ((cbuf >= 40 && cbuf <= 46) || (cbuf >= 58 && cbuf <= 62) || cbuf == 91 || cbuf == 93){
      snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);

      // 先読み
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
      // 比較演算子
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
    // シングルクオートから始まるstring
    else if (cbuf == 39) {
      cbuf = fgetc(fp);
      while (1){

        if (cbuf < 0){
            error("文字列内でEOFが発生指定ます．");
            return -1;
        }

        // stringの終わり
        if (cbuf == 39){
          cbuf = fgetc(fp);

          // 文字のあとに文字があるとき
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
              error("文字列内でEOFが発生しています．");
              return -1;
          }
          else if ((cbuf == 13) || (cbuf == 10)){
              error("文字列内に改行が含まれています．");
              return -1;
          }
        }
      }
    }
    // {}でのコメント
    else if(cbuf == 123){
      while(1){
        cbuf = fgetc(fp);
        if (cbuf < 0){
            error("注釈内でEOFが発生しています．");
            return -1;
        }
        if (cbuf == 125){
            cbuf = fgetc(fp);
            if(cbuf < 0)return -1;
            return 0;
        }
      }
    }
    // スラッシュでのコメント
    else if(cbuf == 47){
      cbuf = fgetc(fp);
      if (cbuf < 0){
        error("注釈の開始時にEOFが発生しています．");
        return -1;
      }

      // *スタート
      if(cbuf == 42){
        while (1){
          cbuf = fgetc(fp);
          if (cbuf < 0){
            error("注釈内でEOFが発生しています．");
            return -1;
          }

          // *終わり
          if (cbuf == 42){
            cbuf = fgetc(fp);
            if (cbuf < 0){
              return -1;
            }
            // コメント終わり
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

// 最も直近でscan()に返された
int get_linenum(void)
{
    return linenum;
}
void end_scan(void){
    fclose(fp);
}
