#!/bin/csh -f

if ($# != 1) then
  echo "#1: File to sort and uniq"
  exit 1
endif

set TmpFNam = `mktemp`
sort $1 | uniq > $TmpFNam
if ($?) exit 1
mv $TmpFNam $1
if ($?) exit 1