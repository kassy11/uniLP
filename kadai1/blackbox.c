#include "token-list.h"

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
