#!/bin/bash

rm -rf results
mkdir results

./cr sample3/sample032p.mpl >> results/sample032p.txt
./cr sample3/sample31p.mpl >> results/sample31p.txt
./cr sample3/sample33p.mpl >> results/sample33p.txt
./cr sample3/sample34.mpl >> results/sample34.txt
./cr sample3/sample35.mpl >> results/sample35.txt

printf '\033[32m%s\033[m\n' 'Now, you can check results at ./results'
