#include "token-list.h"

void blackbox(){
  init_scan_test();
}

// 成功例（適切な入力）、失敗例（mplファイルではないファイル）、失敗例（入力なし）
// をテストケースとしてブラックボックステストを実施する
void init_scan_test(){
  char files[][100] = {
    "sample1/sample011.mpl",
    "test.txt",
    ""
  };

  int results[] = {0, -1, -1};
  for(int i=0; i<sizeof(files)/sizeof(files[0]); i++){
    assert(results[i], init_scan(files[i]));
  }
}
