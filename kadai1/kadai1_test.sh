#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  gcc -static -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 10 'return 10;'
assert 10 'return 10;return 20;'
assert 10 'returnhoge = 10; return returnhoge;';

assert 5 'ab_cd_e139931_003=3; jfioejaoie = 5;'
assert 4 'a=1;b=2;c=a+b;z=c+a;'

assert 0 '0;'
assert 42 '42 ;'
assert 41 ' 12 + 34 - 5 ;'
assert 47 '5+6*7;'
assert 15 '5*(9-6);'
assert 4 '(3+5)/2;'
assert 8 '((3+5)*2)/2;'
assert 10 '-10+20;'
assert 200 '-10*-20;'
assert 10 '+200/+20;'

assert 1 '+200 == 200;'
assert 0 '-200 == +200;'
assert 1 '-(+300-(50+50)) == -200;'

assert 0 '+200!=200;'
assert 1 '-200!=+200;'
assert 0 '  -200!=-(+300-(50+50));'

assert 1 '1<+200;'
assert 1 '-0<+200;'
assert 1 '+0<+200;'
assert 1 '-1<+200;'
assert 0 '-(-200)<+200;'
assert 0 '-0<0;'
assert 0 '+0<-0;'
assert 0 '-1<-1;'
assert 1 '1<=1;'
assert 1 '10<=10;'
assert 2 '(1<=1)+(10<=10);'
assert 1 '0<1;'
assert 0 '1<1;'
assert 0 '2<1;'
assert 1 '0<=1;'
assert 1 '1<=1;'
assert 0 '2<=1;'

assert 1 '1>0;'
assert 0 '1>1;'
assert 0 '1>2;'
assert 1 '1>=0;'
assert 1 '1>=1;'
assert 0 '1>=2;'

echo OK