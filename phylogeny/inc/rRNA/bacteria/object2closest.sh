#!/bin/bash --noprofile
source CPP_DIR/bash_common.sh
if [ $# -ne 2 ]; then
  echo "Find 100 closest sequences in the tree"
  echo "#1: new sequence"
  echo "#2: directory of #1 or ''"
  exit 1
fi
NEW_OBJ=$1
DIR="$2"


INC=`dirname $0`
if [ -z $DIR ]; then
  DIR=$INC/../seq
fi
CPP_DIR/genetics/dna_closest.sh $DIR/$NEW_OBJ $INC/seq.fa


