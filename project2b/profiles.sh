#!/usr/local/cs/bin/bash
rm -rf raw.gperf
touch raw.gperf

LD_PRELOAD=/lib64/libprofiler.so CPUPROFILE=./raw.gperf \
./lab2_list --iterations=1000 --threads=12 --sync=s

rm -rf profile.out
touch profile.out

pprof --text ./lab2_list ./raw.gperf >>profile.out
pprof --list=thread_method ./lab2_list ./raw.gperf >>profile.out
