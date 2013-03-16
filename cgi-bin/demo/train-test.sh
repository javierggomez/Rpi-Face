#!/bin/bash

DIR_ACO_BIN=../acopost-1.8.6/bin

C2L=./cooked2lex.pl
C2N=$DIR_ACO_BIN/cooked2ngram.pl
T3=$DIR_ACO_BIN/t3

#NINI=5500
#STEP=50
#NFIN=6000
#n=5550
#for n in `seq $NINI $STEP $NFIN`
#  do
#  head -n $n FINALEMO-RBC.cooked  > train.$n.cooked
  
  $C2L < demo.cooked > train.lex 
#  $C2N < demo.cooked > train.123 
  
  $T3 $1 -v 6 FINALEMO.123 train.lex < test.raw 

#done
