#!/bin/bash

DIR_ACO_BIN=../acopost-1.8.6/bin

C2L=$DIR_ACO_BIN/cooked2lex.pl
C2N=$DIR_ACO_BIN/cooked2ngram.pl
T3=$DIR_ACO_BIN/t3

#NINI=5500
#STEP=50
#NFIN=6000
n=5550
#for n in `seq $NINI $STEP $NFIN`
#  do
#  head -n $n FINALEMO-RBC.cooked  > train.$n.cooked
  
#  $C2L < train.$n.cooked > train.$n.lex 
#  $C2N < train.$n.cooked > train.$n.ngram 
  
  $T3 train.$n.ngram train.$n.lex < test.raw > test.$n.t3

#done
