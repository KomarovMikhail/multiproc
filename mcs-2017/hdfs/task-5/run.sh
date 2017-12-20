#! /usr/bin/env bash
FILE=$(dd if=/dev/zero of=output.dat bs=$1 count=1 2>"/dev/null")
hdfs dfs -put output.dat 2>"/dev/null"
REAL_SIZE=$(hdfs dfs -du output.dat 2>"/dev/null" | perl -ne '/(\d+)./&& print $1')
RES=$(expr $REAL_SIZE - $1)
rm output.dat
hdfs dfs -rm output.dat 2>"/dev/null"
echo $RES

