#!/bin/bash

COMPILER=../../compiler
while read line
do
    echo $line | sed 's_^\(.*\),_\1' > tmpfile.in
    expresult=$(echo $line | sed 's_,\(.*\)$_\1' )
    $COMPILER tmpfile.in | gforth | 
done < $1
