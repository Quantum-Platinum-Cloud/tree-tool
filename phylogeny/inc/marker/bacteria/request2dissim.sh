#!/bin/bash
source bash_common.sh
if [ $# -ne 3 ]; then
  echo "$0"
  echo "#1: dissimilaruty requests (input)"
  echo "#2: dissim (output)"
  echo "#3: log"
  exit 1
fi
REQUEST=$1
DISSIM=$2
LOG=$3

dna_pairs2dissim  -log $LOG  -coeff 0.00155  $REQUEST /home/brovervv/panfs/marker/bacteria/seq 600 $DISSIM
  # was: 1200
rm -f $LOG
