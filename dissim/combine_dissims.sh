#!/bin/bash
THIS=`dirname $0`
source $THIS/../bash_common.sh
if [ $# -ne 10 ]; then
  echo "Compute dissimilarities"
  echo "#1: file with pairs <Object1> <Object2>"
  echo "#2: Output file"
  echo "#3: directory with: <Object hash>/<Object>/<Object>.hash-{CDS|PRT}"
  echo "#4: hashes intersection_min"
  echo "#5: hashes ratio_min"
  echo "#6: dissim_scale"
  echo "#7: hmm-univ.stat"
  echo "#8: 1 - BLOSUM62, 0 - PAM30"
  echo "#9: power for universal proteins dissimilarity"
  echo "#10: log file (delete on success)"
  exit 1
fi
IN=$1
OUT=$2
GENOME_DIR=$3
HASH_INTERSECTION_MIN=$4
HASH_RATIO_MIN=$5
DISSIM_SCALE=$6
AVERAGE_MODEL=$7
BLOSUM62_ARG=$8
POWER=$9
LOG=${10}


N=`cat $DISSIM_SCALE | wc -l`
if [ $N -ne 2 ]; then
  echo "$DISSIM_SCALE must have 2 lines" >> $LOG
  exit 1
fi

L=(`head -1 $DISSIM_SCALE`)
DISSIM_MAX=${L[2]}

BLOSUM62=""
if [ $BLOSUM62_ARG == 1 ]; then
  BLOSUM62="-blosum62"
fi


TMP=`mktemp`
#echo $TMP


cat $IN | awk '{print $1};' > $TMP.1
cat $IN | awk '{print $2};' > $TMP.2
$THIS/../trav $TMP.1 "echo %n $GENOME_DIR/%h/%f/%f.hash-PRT" > $TMP.f1
$THIS/../trav $TMP.2 "echo %n $GENOME_DIR/%h/%f/%f.hash-PRT" > $TMP.f2
join  -1 1  -2 1  $TMP.f1 $TMP.f2 | cut -d ' ' -f 2,3 > $TMP.req

# $TMP.prt, $OUT
$THIS/hash_request2dissim $TMP.req $TMP.prt  -intersection_min $HASH_INTERSECTION_MIN  -ratio_min $HASH_RATIO_MIN   -log $LOG 
set +o errexit
grep -w  nan $TMP.prt                           >  $TMP.prt-univ
grep -wv nan $TMP.prt | awk '$3 >  '$DISSIM_MAX >> $TMP.prt-univ
grep -wv nan $TMP.prt | awk '$3 <= '$DISSIM_MAX  | tr '\t' ' ' | sed 's|'$GENOME_DIR'/[^/]\+/[^/]\+/||g' | sed 's/\.hash-... / /g' | awk '{printf "%s %s %.8g\n", $1, $2, $3 * '${L[0]}'};' > $OUT
set -o errexit

# $TMP.prt-univ -> $TMP.univ
cat $TMP.prt-univ | tr '\t' ' ' | sed 's/ [^ ]\+$/ /1' | sed 's/\.hash-PRT /.prot-univ /g' > $TMP.req1
$THIS/prots_pair2dissim  -log $LOG  -power $POWER  $BLOSUM62  $AVERAGE_MODEL $TMP.req1 $TMP.univ

cat $TMP.prt-univ $TMP.univ | tr '\t' ' ' | sed 's|'$GENOME_DIR'/[^/]\+/[^/]\+/||g' | sed 's/\.hash-PRT / /g' | sed 's/\.prot-univ / /g' > $TMP.combo
$THIS/combine_dissims $TMP.combo $DISSIM_SCALE  -log $LOG >> $OUT


rm -f $LOG
rm $TMP*