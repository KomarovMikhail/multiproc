#! /usr/bin/env bash
FILE=$1
URL=$(curl -i -s "http://mipt-master.atp-fivt.org:50070/webhdfs/v1${FILE}?op=OPEN&length=10" | grep -o '^Location: .*' | grep -o 'http.*')

python3 ./script.py ${URL}

