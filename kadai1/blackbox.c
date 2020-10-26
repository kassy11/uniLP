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

void blackbox(){
  init_scan_test();
}

void init_scan_test(){
  char files[][100] = {
    "kadai1_samples/sample011.mpl",
    "test.txt",
    ""
  };

  int results[] = {0, -1, -1};
  for(int i=0; i<sizeof(files)/sizeof(files[0]); i++){
    assert(results[i], init_scan(files[i]));
  }
}
