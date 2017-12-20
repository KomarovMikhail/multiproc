#! /usr/bin/env bash
FILE=$1
hdfs fsck ${FILE} 2>"/dev/null" | grep -o "Total blocks (validated):.*" | grep -o ":.*(" | grep -o "[0-9]*"

