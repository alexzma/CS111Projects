#!/usr/local/cs/bin/bash
rm -rf lab2b_list.csv
touch lab2b_list.csv

./lab2_list --iterations=1000 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=16 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=24 --sync=m &>>lab2b_list.csv
echo mutex_threads
./lab2_list --iterations=1000 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=2 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=4 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=8 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=12 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=16 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --threads=24 --sync=s &>>lab2b_list.csv
echo spin_threads
./lab2_list --lists=4 --yield=id &>>lab2b_list.csv
echo none 4 1
./lab2_list --lists=4 --yield=id --threads=4 &>>lab2b_list.csv
echo none 4 4
./lab2_list --lists=4 --yield=id --threads=8 &>>lab2b_list.csv
echo none 4 8
./lab2_list --lists=4 --yield=id --threads=12 &>>lab2b_list.csv
echo none 4 12
./lab2_list --lists=4 --yield=id --threads=16 &>>lab2b_list.csv
echo none 4 16
./lab2_list --lists=4 --yield=id --iterations=2 &>>lab2b_list.csv
echo none 4 1
./lab2_list --lists=4 --yield=id --threads=4 --iterations=2 &>>lab2b_list.csv
echo none 4 4
./lab2_list --lists=4 --yield=id --threads=8 --iterations=2 &>>lab2b_list.csv
echo none 4 8
./lab2_list --lists=4 --yield=id --threads=12 --iterations=2 &>>lab2b_list.csv
echo none 4 12
./lab2_list --lists=4 --yield=id --threads=16 --iterations=2 &>>lab2b_list.csv
echo none 4 16
./lab2_list --lists=4 --yield=id --iterations=4 &>>lab2b_list.csv
echo none 4 1
./lab2_list --lists=4 --yield=id --threads=4 --iterations=4 &>>lab2b_list.csv
echo none 4 4
./lab2_list --lists=4 --yield=id --threads=8 --iterations=4 &>>lab2b_list.csv
echo none 4 8
./lab2_list --lists=4 --yield=id --threads=12 --iterations=4 &>>lab2b_list.csv
echo none 4 12
./lab2_list --lists=4 --yield=id --threads=16 --iterations=4 &>>lab2b_list.csv
echo none 4 16
./lab2_list --lists=4 --yield=id --iterations=8 &>>lab2b_list.csv
echo none 4 1
./lab2_list --lists=4 --yield=id --threads=4 --iterations=8 &>>lab2b_list.csv
echo none 4 4
./lab2_list --lists=4 --yield=id --threads=8 --iterations=8 &>>lab2b_list.csv
echo none 4 8
./lab2_list --lists=4 --yield=id --threads=12 --iterations=8 &>>lab2b_list.csv
echo none 4 12
./lab2_list --lists=4 --yield=id --threads=16 --iterations=8 &>>lab2b_list.csv
echo none 4 16
./lab2_list --lists=4 --yield=id --iterations=16 &>>lab2b_list.csv
echo none 4 1
./lab2_list --lists=4 --yield=id --threads=4 --iterations=16 &>>lab2b_list.csv
echo none 4 4
./lab2_list --lists=4 --yield=id --threads=8 --iterations=16 &>>lab2b_list.csv
echo none 4 8
./lab2_list --lists=4 --yield=id --threads=12 --iterations=16 &>>lab2b_list.csv
echo none 4 12
./lab2_list --lists=4 --yield=id --threads=16 --iterations=16 &>>lab2b_list.csv
echo unprotected multi-list
./lab2_list --lists=4 --yield=id --sync=s --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=s --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=s --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=s --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=s --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --sync=s --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=s --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=s --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=s --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=s --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --sync=s --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=s --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=s --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=s --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=s --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --sync=s --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=s --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=s --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=s --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=s --iterations=80 &>>lab2b_list.csv
echo multi-lists spin
./lab2_list --lists=4 --yield=id --sync=m --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=m --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=m --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=m --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=m --iterations=10 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --sync=m --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=m --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=m --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=m --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=m --iterations=20 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --sync=m --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=m --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=m --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=m --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=m --iterations=40 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --sync=m --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=4 --sync=m --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=8 --sync=m --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=12 --sync=m --iterations=80 &>>lab2b_list.csv
./lab2_list --lists=4 --yield=id --threads=16 --sync=m --iterations=80 &>>lab2b_list.csv
echo multi-lists mutex
./lab2_list --iterations=1000 --lists=4 --sync=m &>>lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=m &>>lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=m &>>lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=m &>>lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --lists=8 --sync=m &>>lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=m &>>lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=m &>>lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=m &>>lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=m &>>lab2b_list.csv
./lab2_list --iterations=1000 --lists=16 --sync=m &>>lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=m &>>lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=m &>>lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=m &>>lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=m &>>lab2b_list.csv
echo multi-list mutex 2
./lab2_list --iterations=1000 --lists=4 --sync=s &>>lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=4 --sync=s &>>lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=4 --sync=s &>>lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=4 --sync=s &>>lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=4 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --lists=8 --sync=s &>>lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=8 --sync=s &>>lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=8 --sync=s &>>lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=8 --sync=s &>>lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=8 --sync=s &>>lab2b_list.csv
./lab2_list --iterations=1000 --lists=16 --sync=s &>>lab2b_list.csv
./lab2_list --threads=2 --iterations=1000 --lists=16 --sync=s &>>lab2b_list.csv
./lab2_list --threads=4 --iterations=1000 --lists=16 --sync=s &>>lab2b_list.csv
./lab2_list --threads=8 --iterations=1000 --lists=16 --sync=s &>>lab2b_list.csv
./lab2_list --threads=12 --iterations=1000 --lists=16 --sync=s &>>lab2b_list.csv
echo multi-list spin 2
