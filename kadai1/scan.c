// 字句解析器の本体

#include "token-list.h"
FILE *fp;
char cbuf; // 文字バッファ（次の字句は、先頭の文字である程度わかることを利用）
int linenum;
int num_attr; // scan()の戻り値が「符号なし整数」のとき、その値を格納
char string_attr[MAXSTRSIZE]; // scan()の戻り値が「名前」「文字列」のとき、その文字列（もしくは数字列）を格納
extern char *tokenstr[NUMOFTOKEN + 1];
extern key key_keyword[KEYWORDSIZE];
extern key key_symbol[KEYWORDSIZE];

// ファイルをオープンする
int init_scan(char *filename){
    // .mplファイルのみを読み込む
    char mpl[] = ".mpl";
    
    fp = fopen(filename,"r");
    if ((fp == NULL) || (strstr(filename, mpl)==NULL)){
        printf("init_scan: Cannot open the file\n");
        return -1;
    }
    // 最初の1文字目を読み出す
    cbuf = fgetc(fp);
    if(cbuf < 0)return -1;
    linenum = 1;
    init_idtab();
    return 0;
}

// 字句解析を行い、次の字句のトークンコードを返す（参照: token-list.h）
// cbufはasciiコードで分岐する
int scan(void){
    int i = 0;
    char token[MAXSTRSIZE];

    memset(token, 0, sizeof(token));
    if(cbuf < 0)return -1;
    // 分離子はじまりのとき
    // 読み進めるだけで読み飛ばす（
    if(cbuf <= 32){
        // cbuf = 10 改行
      while(1){
          // 復帰と改行
        if ((cbuf == 13) || (cbuf == 10)){
            char before_cbuf = cbuf;
            cbuf = fgetc(fp);
            // 組み合わせて改行を表現する環境にも適応させる
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
        // 分離子でなくなったら抜ける
        if (cbuf >= 39){
          break;
        }
      }
    }
    // アルファベットはじまりのとき
    if((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122)){
      while(1){
          snprintf(token,MAXSTRSIZE,"%s%c",token,cbuf);

          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;

          // 次のバッファがアルファベットでも数字でもなくなったら（切れ目）
          if (!((cbuf >= 65 && cbuf <= 90) || (cbuf >= 97 && cbuf <= 122) || (cbuf >= 48 && cbuf <= 57))){
              // キーワードテーブルに存在すれば（予約語であれば）その番号を返す
              for(i = 0;i < KEYWORDSIZE;i++)
              {
                  if (strcmp(token, key_keyword[i].keyword) == 0)
                      return key_keyword[i].keytoken;
              }
              // 予約語でなければ、トークンテーブルに追加する
              memset(string_attr, 0, sizeof(string_attr));
              snprintf(string_attr, MAXSTRSIZE, "%s", token);
              id_countup(string_attr);
              return TNAME;
              break;
          }
      }
    }
    // 数字はじまりのとき
    else if(cbuf >= 48 && cbuf <= 57){
      while(1){
          snprintf(token, MAXSTRSIZE, "%s%c", token, cbuf);
          cbuf = fgetc(fp);
          if(cbuf < 0)return -1;
          // 数字でなくなったら
          if (!(cbuf >= 48 && cbuf <= 57))
          {
              num_attr = atoi(token);
              memset(string_attr, 0, sizeof(string_attr));
              snprintf(string_attr, MAXSTRSIZE, "%s", token);
              id_countup(string_attr);
              return TNUMBER;
              break;
          }
      }
    }
    // 記号（key_symbol[]にあるもの）はじまりのとき
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
      return -1;
    }
    // シングルクオーテーション)はじまりの文字列のとき
    else if (cbuf == 39) {
      cbuf = fgetc(fp);
      while (1){

        if (cbuf < 0){
            error("EOF happened\n");
            return -1;
        }

        // 文字列の終わり
        if (cbuf == 39){
          cbuf = fgetc(fp);

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
    // ダブルクオーテーション)はじまりの文字列のとき
    else if (cbuf == 34) {
        cbuf = fgetc(fp);
        while (1){

            if (cbuf < 0){
                error("EOF happened\n");
                return -1;
            }

            if (cbuf == 34){
                cbuf = fgetc(fp);

                if (cbuf == 34) {
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
    // コメントは読み飛ばす
    // { からはじまるコメントのとき
    else if(cbuf == 123){
      while(1){
        cbuf = fgetc(fp);
        if (cbuf < 0){
            error("EOF happened\n");
            return -1;
        }
          // コメント内容は読み飛ばし

          // } のコメント終わり
        if (cbuf == 125){
            cbuf = fgetc(fp);
            if(cbuf < 0)return -1;
            return 0;
        }
      }
    }
    // /* comment */ の形式のコメントのとき
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
          // コメント内容は読み飛ばし
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

// 直近でscan()で返されたトークンが存在する行番号を返す
int get_linenum(void){
    return linenum;
}

// ファイルのクローズ
void end_scan(void){
    fclose(fp);
}
