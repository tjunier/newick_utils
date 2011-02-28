#!/bin/sh

# valgrind_nw_prog.sh - pass all application test cases through valgrind. Since
# this takes time and is harder to automate, and since I cannot assume that
# Valgrind is present on user's systems, I did not include this program in the
# test suite. But all programs should be checked in this way.

if [ "$1" != "" ] ; then
	progs=$1
else
	progs=$(ls test_nw_*.sh)
fi

for prog in $progs; do
	[ $prog = 'test_nw_prog.sh' ] && continue

	prog=${prog%.sh}	
	prog=${prog#*_}
	args_file=test_${prog}_args

	echo "Testing program: $prog"

	if [ !  -x ../src/$prog ] ; then
		echo "$prog not found or not executable."
		exit 1
	fi

	if [ ! -r $args_file ] ; then
		echo "can't find arguments file $args_file"
		exit 1
	fi

	# Each test case in on one line. Line structure is <case name>:<prog
	# arguments>. The expected result is in a file named test_<prog
	# name>_<case name>.exp .

	pass=TRUE
	while IFS=':' read name args ; do
		# setting IFS to '' preserves whitespace through shell word
		# splitting
		echo "  $name"
		IFS='' cmd="valgrind -q --leak-check=yes ../src/$prog $args > /dev/null"
		eval $cmd 
	done < $args_file

done
