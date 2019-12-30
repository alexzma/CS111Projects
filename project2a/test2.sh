#!/usr/local/cs/bin/bash
rm -rf lab2_list.csv
touch lab2_list.csv

./lab2_list --iterations=10 >>lab2_list.csv
./lab2_list --iterations=100 >>lab2_list.csv
./lab2_list --iterations=1000 >>lab2_list.csv
./lab2_list --iterations=10000 >>lab2_list.csv
./lab2_list --iterations=20000 >>lab2_list.csv
echo 20000 done
./lab2_list --threads=2 &>>lab2_list.csv
./lab2_list --threads=4 &>>lab2_list.csv
./lab2_list --threads=8 &>>lab2_list.csv
./lab2_list --threads=12 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=2 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=2 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=2 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=2 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=4 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=4 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=4 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=4 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=8 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=8 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=8 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=8 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=16 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=16 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=16 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=16 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=32 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=32 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=32 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=32 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=10 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=10 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=10 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=100 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=100 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=100 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 &>>lab2_list.csv
./lab2_list --threads=2 --iterations=1000 &>>lab2_list.csv
./lab2_list --threads=4 --iterations=1000 &>>lab2_list.csv
./lab2_list --threads=8 --iterations=1000 &>>lab2_list.csv
./lab2_list --threads=12 --iterations=1000 &>>lab2_list.csv
echo 12*1000 done
./lab2_list --threads=2 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --yield=i &>>lab2_list.csv
./lab2_list --threads=8 --yield=i &>>lab2_list.csv
./lab2_list --threads=12 --yield=i &>>lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=i &>>lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=i &>>lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=i &>>lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=i &>>lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=i &>>lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=i &>>lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=i &>>lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=i &>>lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=i &>>lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=i &>>lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=i &>>lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=i &>>lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=i &>>lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=i &>>lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=i &>>lab2_list.csv
echo i_yields
./lab2_list --threads=2 --yield=d &>>lab2_list.csv
./lab2_list --threads=4 --yield=d &>>lab2_list.csv
./lab2_list --threads=8 --yield=d &>>lab2_list.csv
./lab2_list --threads=12 --yield=d &>>lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=d &>>lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=d &>>lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=d &>>lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=d &>>lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=d &>>lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=d &>>lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=d &>>lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=d &>>lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=d &>>lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=d &>>lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=d &>>lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=d &>>lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=d &>>lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=d &>>lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=d &>>lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=d &>>lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=d &>>lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=d &>>lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=d &>>lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=d &>>lab2_list.csv
echo d_yields
./lab2_list --threads=2 --yield=il &>>lab2_list.csv
./lab2_list --threads=4 --yield=il &>>lab2_list.csv
./lab2_list --threads=8 --yield=il &>>lab2_list.csv
./lab2_list --threads=12 --yield=il &>>lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=il &>>lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=il &>>lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=il &>>lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=il &>>lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=il &>>lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=il &>>lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=il &>>lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=il &>>lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=il &>>lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=il &>>lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=il &>>lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=il &>>lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=il &>>lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=il &>>lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=il &>>lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=il &>>lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=il &>>lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=il &>>lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=il &>>lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=il &>>lab2_list.csv
echo il_yields
./lab2_list --threads=2 --yield=dl &>>lab2_list.csv
./lab2_list --threads=4 --yield=dl &>>lab2_list.csv
./lab2_list --threads=8 --yield=dl &>>lab2_list.csv
./lab2_list --threads=12 --yield=dl &>>lab2_list.csv
./lab2_list --threads=2 --iterations=2 --yield=dl &>>lab2_list.csv
./lab2_list --threads=4 --iterations=2 --yield=dl &>>lab2_list.csv
./lab2_list --threads=8 --iterations=2 --yield=dl &>>lab2_list.csv
./lab2_list --threads=12 --iterations=2 --yield=dl &>>lab2_list.csv
./lab2_list --threads=2 --iterations=4 --yield=dl &>>lab2_list.csv
./lab2_list --threads=4 --iterations=4 --yield=dl &>>lab2_list.csv
./lab2_list --threads=8 --iterations=4 --yield=dl &>>lab2_list.csv
./lab2_list --threads=12 --iterations=4 --yield=dl &>>lab2_list.csv
./lab2_list --threads=2 --iterations=8 --yield=dl &>>lab2_list.csv
./lab2_list --threads=4 --iterations=8 --yield=dl &>>lab2_list.csv
./lab2_list --threads=8 --iterations=8 --yield=dl &>>lab2_list.csv
./lab2_list --threads=12 --iterations=8 --yield=dl &>>lab2_list.csv
./lab2_list --threads=2 --iterations=16 --yield=dl &>>lab2_list.csv
./lab2_list --threads=4 --iterations=16 --yield=dl &>>lab2_list.csv
./lab2_list --threads=8 --iterations=16 --yield=dl &>>lab2_list.csv
./lab2_list --threads=12 --iterations=16 --yield=dl &>>lab2_list.csv
./lab2_list --threads=2 --iterations=32 --yield=dl &>>lab2_list.csv
./lab2_list --threads=4 --iterations=32 --yield=dl &>>lab2_list.csv
./lab2_list --threads=8 --iterations=32 --yield=dl &>>lab2_list.csv
./lab2_list --threads=12 --iterations=32 --yield=dl &>>lab2_list.csv
echo dl_yields
./lab2_list --threads=12 --iterations=10 --yield=i --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 --yield=d --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 --yield=il --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 --yield=dl --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=i --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=d --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=il --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=dl --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=i --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=d --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=il --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=dl --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=i --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=d --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=il --sync=s &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=dl --sync=s &>>lab2_list.csv
echo yield+sync=s
./lab2_list --threads=12 --iterations=10 --yield=i --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 --yield=d --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 --yield=il --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=10 --yield=dl --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=i --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=d --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=il --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=10 --yield=dl --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=i --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=d --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=il --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=100 --yield=dl --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=i --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=d --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=il --sync=m &>>lab2_list.csv
./lab2_list --threads=32 --iterations=100 --yield=dl --sync=m &>>lab2_list.csv
echo yield+sync=m
./lab2_list --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=m &>>lab2_list.csv
./lab2_list --iterations=1000 --sync=s &>>lab2_list.csv
./lab2_list --threads=2 --iterations=1000 --sync=s &>>lab2_list.csv
./lab2_list --threads=4 --iterations=1000 --sync=s &>>lab2_list.csv
./lab2_list --threads=8 --iterations=1000 --sync=s &>>lab2_list.csv
./lab2_list --threads=12 --iterations=1000 --sync=s &>>lab2_list.csv
./lab2_list --threads=16 --iterations=1000 --sync=s &>>lab2_list.csv
./lab2_list --threads=24 --iterations=1000 --sync=s &>>lab2_list.csv
echo sync
