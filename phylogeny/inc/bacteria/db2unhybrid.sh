#!/bin/csh -f

if ($# != 1) then
  echo "Print list of objects deleted from uniColl..Hybrid"
  echo "Update: uniColl..Hybrid"
  echo "#1: output file"
  exit 1
endif

sqsh-ms  -S PROTEUS  -D uniColl  -L exit_failcount=1 << EOF | sed 's/|$//1' > $1
  EXEC Genome_unhybrid 2;
  go -m bcp
EOF
if ($?) exit 1