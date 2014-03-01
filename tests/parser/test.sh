#!/bin/bash

# this shell script runs tests in a given directory. TESTDIR
# must be set to the desired directory when running this script.

RUNFLAGS=
for test in ${TESTDIR}/*
do
	testname=$(basename $test)
	outname=$(echo $testname | sed 's/\(.*\)\.in/\1.out/')
	if [[ $testname =~ .*good.* ]] ; then
		str="success"
	else
		str="failure"
	fi
	echo "running test ${testname} (expected ${str})"
	echo "============================================="
	cat $test
	echo "---------------------------------------------"
	./compiler $RUNFLAGS $test > $outname
	cat $outname
	echo
done

