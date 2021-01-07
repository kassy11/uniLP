#!/bin/bash
rm -rf results
mkdir results

./pp sample1/sample011.mpl >> results/sample011.txt
./pp sample1/sample11.mpl >> results/sample11.txt
./pp sample1/sample11p.mpl >> results/sample11p.txt
./pp sample1/sample11pp.mpl >> results/sample11pp.txt 

./pp sample1/sample12.mpl >> results/sample12.txt
./pp sample1/sample13.mpl >> results/sample13.txt

./pp sample1/sample014.mpl >> results/sample014.txt
./pp sample1/sample14.mpl >> results/sample14.txt
./pp sample1/sample14p.mpl >> results/sample14p.txt

./pp sample1/sample15.mpl >> results/sample15.txt
./pp sample1/sample15a.mpl >> results/sample15a.txt

./pp sample1/sample16.mpl >> results/sample16.txt
./pp sample1/sample17.mpl >> results/sample17.txt
./pp sample1/sample18.mpl >> results/sample18.txt
./pp sample1/sample19p.mpl >> results/sample19p.txt

printf '\033[32m%s\033[m\n' 'Now, you can check results at ./results'

