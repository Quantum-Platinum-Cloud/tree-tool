#!/bin/bash
THIS=`dirname $0`
source $THIS/../bash_common.sh
if [ $# -ne 2 ]; then
  echo "Set names to a distance tree by phenotypes"
  echo "Output: core, qual, gain_nodes, disagreement_nodes, disagreement_nodes.txt"
  echo "#1: distance tree"
  echo "#2: phen/"
  exit 1
fi
INTREE=$1
PHEN=$2


TMP=`mktemp`
echo $TMP


$THIS/makeDistTree  -threads 15  -input_tree $INTREE  -noqual  -output_feature_tree $TMP.feature_tree  > $TMP.distTree

$THIS/makeFeatureTree  -threads 15  -input_tree $TMP.feature_tree  -features $PHEN  -prefer_gain  \
  -qual qual  -gain_nodes gain_nodes  -disagreement_nodes disagreement_nodes
cut -f 1 disagreement_nodes | sort | uniq -c | sort -n -k 1 -r > disagreement_nodes.txt
set +o errexit
cat disagreement_nodes.txt | grep -v ":" | grep -v ' 1 ' > disagreement_objects
set -o errexit
echo ""
wc -l disagreement_nodes.txt


rm -f $TMP*