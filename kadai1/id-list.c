// 名前についてその実体ごとにも出現個数を数えて出力するように拡張するための関数群

#include "token-list.h"

// トークンテーブル
struct ID {
    char *name;
    int count;
    struct ID *nextp;
}  *idroot;

// トークンテーブルを初期化
void init_idtab() {		/* Initialise the table */
    idroot = NULL;
}

// 引数に指定されたトークンがトークンテーブルにすでに存在するか
struct ID *search_idtab(char *np) {	/* search the name pointed by np */
    struct ID *p;

    for(p = idroot; p != NULL; p = p->nextp) {
        if(strcmp(np, p->name) == 0) return(p);
    }
    return(NULL);
}

// add token to table
void id_countup(char *np) {	
    struct ID *p;
    char *cp;

    // トークンがトークンテーブルに存在すればカウントアップし、存在しなければメモリ確保
    if((p = search_idtab(np)) != NULL) p->count++;
    else {
        // 構造体のメモリ確保
        if((p = (struct ID *)malloc(sizeof(struct ID))) == NULL) {
            printf("can not malloc in id_countup\n");
            return;
        }
        // トークン名のメモリ確保
        if((cp = (char *)malloc(strlen(np)+1)) == NULL) {
            printf("can not malloc-2 in id_countup\n");
            return;
        }
        strcpy(cp, np);
        p->name = cp;
        p->count = 1;
        p->nextp = idroot;
        idroot = p;
    }
}

// トークンテーブルを表示する
void print_idtab() {	/* Output the registered data */
    struct ID *p;

    for(p = idroot; p != NULL; p = p->nextp) {
        if(p->count != 0)
            printf("Identifier\" \"%s\"\t%d\n", p->name, p->count);
    }
}

// トークンテーブルを削除
void release_idtab() {	/* Release tha data structure */
    struct ID *p, *q;

    for(p = idroot; p != NULL; p = q) {
        free(p->name);
        q = p->nextp;
        free(p);
    }
    init_idtab();
}
