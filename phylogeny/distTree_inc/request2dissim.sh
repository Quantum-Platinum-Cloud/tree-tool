#!/bin/bash
if [ $# -ne 3 ]; then
  echo "Compute dissimilarities"
  echo "#1: file with pairs <Object1> <Object2>"
  echo "#2: output file"
  echo "#3: log (temporary)"
  exit 1
fi
REQ=$1
OUT=$2
LOG=$3


exit 1