#! /usr/bin/env bash
BLOCK=$1
SERVER_NAME=$(hdfs fsck -blockId ${BLOCK} 2>"/dev/null" | grep -o -m 1 "^Block replica on datanode/rack: .*" | grep -o ': .* is' | cut -c '3-' | grep -o '.*/' | rev | cut -c '2-' | rev)
LOCAL_PATH=$(sudo -u hdfsuser ssh hdfsuser@${SERVER_NAME} "locate ${BLOCK} | sort | head -n 1")
echo "${SERVER_NAME}:${LOCAL_PATH}"

