#include "token-list.h"

void assert(int expected, int actual){
  if(expected == actual){
    printf("\x1b[32m <pass!>\n");
    printf("\x1b[m");
    printf("expected: %d actual: %d\n", expected, actual);
  }else{
    printf("\x1b[31m <failed!>\n");
    printf("\x1b[m");
    printf("expected %d but actual %d\n", expected, actual);
    exit(1);
  }
}

void assert_str(char *expected, char *actual){
    if((strcmp(expected, actual))==0){
        printf("\x1b[32m <pass!>\n");
        printf("\x1b[m");
        printf("expected: %s actual: %s\n", expected, actual);
    }else{
        printf("\x1b[31m <failed!>\n");
        printf("\x1b[m");
        printf("expected %s but actual %s\n", expected, actual);
        exit(1);
    }
}