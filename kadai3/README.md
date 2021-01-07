## kadai3 cross referencer

### Goal
- プログラミング言語 MPPL で書かれたプログラムを読み込み，コンパイルエラー，すなわち，構文エ
ラーもしくは制約エラー(型の不一致や未定義な変数の出現等)がなければ，クロスリファレンス表を出力し，エラーがあれば，そのエラーの情報(エラーの箇所，内容等)を少なくとも一つ出力するプログラ ムを作成する
  - 制約規則は課題説明を参照

### Input
- MPPLファイル

### Output
- 入力のプログラム中に構文的な誤りや制約規則違反がなければ，そのプログラムのクロスリファレンス表を出力
- 構文的な誤りや制約規則違反があれば，それを最初に検 出した時点で，検出した行の番号と，誤りまたは違反の内容を出力

### Usage
- compile: `make`
- run: `./cr sample3/sample35.mpl`
  - you can use sample file at `./sample3`
- test:
  - unit test: `./cr test`
  - integration test:  `make test`