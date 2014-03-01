#!/bin/bash

cd tests/generator

COMPILER=../../compiler
TESTLIST=testlist
FLOAT_DELTA=0.001
i=0
while read line
do
    testfile=$(echo $line | sed 's_^\(.*\),.*$_\1_' )
    exp_result=$(echo $line | sed 's_^.*,\(.*\)$_\1_' )

    echo "test ${i}:"
    echo "==========================================="
    echo "INPUT: "
    cat $testfile
    echo "OUTPUT: "
    $COMPILER -o deleteme.f $testfile
    returncode=$?
    if [[ $returncode == 0 ]] ; then
        result=$(gforth deleteme.f )
        cat deleteme.f
    else
        result="error"
    fi
    echo
    echo "EXPECTED: $exp_result"
    echo "ACTUAL: $result"

    exp_result=$( echo $exp_result | sed 's_^[[:space:]]*\(.*\)[[:space:]]*$_\1_' )
    result=$( echo $result | sed 's_^[[:space:]]*\(.*\)[[:space:]]*$_\1_' )

    if [[ $exp_result == 'true' && $result == '-1' ||
          $exp_result == 'false' && $result == '0' ||
          $exp_result == 'error' && $result == 'error' ||
          $result == $exp_result ]] ; then
        echo "PASS"
    elif [[ $exp_result =~ .*\..* ]] ; then
        # compare float results
        isequal=$(echo "print abs($result - $exp_result) / $exp_result  < $FLOAT_DELTA" | python)
        if [[ $isequal =~ t.* || $isequal =~ T.* ]] ; then
            echo "PASS"
        else
            echo "FAIL"
        fi
    else
        echo "FAIL"
    fi
        
    echo
    i=$( expr $i + 1 )
done < $TESTLIST


