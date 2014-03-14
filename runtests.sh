#!/bin/bash

# this shell script runs tests in a given directory. TESTDIR
# must be set to the desired directory when running this script.

for test in ${TESTDIR}/*
do
	testname=$(basename $test)
	if [[ $testname =~ .*good.* ]] ; then
		str="success"
	else
		str="failure"
	fi
	echo "running test ${testname} (expected ${str})"
	echo "============================================="
	cat $test
	echo "---------------------------------------------"
	./compiler -o a.out $test
    returncode=$?
    if [[ $returncode == 0 ]] ; then
        cat a.out
        echo "---------------------------------------------"
        echo "GFORTH OUTPUT"
        gforth a.out
        if [[ $? != 0 ]] ; then
            echo "gforth error"
        fi
        echo
    fi
	echo
done

