#!/bin/bash

mkdir results

./tc kadai1_samples/sample011.mpl >> results/sample011.txt
./tc kadai1_samples/sample11.mpl >> results/sample11.txt
./tc kadai1_samples/sample11p.mpl >> results/sample11p.txt
./tc kadai1_samples/sample11pp.mpl >> results/sample11pp.txt 

./tc kadai1_samples/sample12.mpl >> results/sample12.txt
./tc kadai1_samples/sample13.mpl >> results/sample13.txt

./tc kadai1_samples/sample014.mpl >> results/sample014.txt
./tc kadai1_samples/sample14.mpl >> results/sample14.txt
./tc kadai1_samples/sample14p.mpl >> results/sample14p.txt

./tc kadai1_samples/sample15.mpl >> results/sample15.txt
./tc kadai1_samples/sample15a.mpl >> results/sample15a.txt

./tc kadai1_samples/sample16.mpl >> results/sample16.txt
./tc kadai1_samples/sample17.mpl >> results/sample17.txt
./tc kadai1_samples/sample18.mpl >> results/sample18.txt
./tc kadai1_samples/sample19p.mpl >> results/sample19p.txt

echo "Now, you can check results at ./results"

