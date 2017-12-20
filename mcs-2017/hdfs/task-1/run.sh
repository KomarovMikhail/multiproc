#! /usr/bin/env bash
FILE=$1
curl -i -s "http://mipt-master.atp-fivt.org:50070/webhdfs/v1${FILE}?op=OPEN" | grep '^Location:' | grep -o 'http:\/\/*.*.*:[0-9]*\/' | grep -o '\/\/.*:' | grep -o '[^\/].*[^:]'
