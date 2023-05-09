#!/bin/bash
set -x
set -e

DATE=s=$(date '+%m%Y')

COPY_DIR=/home/fernando/ChipIR${DATE}

#for i in logs/*; do
filename=GAP9_ChipIR_$(date +'%Y_%m_%d_%H_%M').tar.gz
tar czf "$filename" data/logs
#done

scp fernando@192.168.1.5:./*.tar.gz "${COPY_DIR}"/
