#!/bin/bash
source bash_common.sh
if [ $# -ne 1 ]; then
  echo "$0"
  echo "#1: File genospecies_table"
  exit 1
fi


loadLISTC $1

sqsh-ms  -S PROTEUS  -D uniColl  -L exit_failcount=1 << EOF | sed 's/|$//1' 
  EXEC Genospecies2outliers 2 /*PAR*/
  go -m bcp
EOF
