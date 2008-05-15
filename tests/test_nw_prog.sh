#!/bin/sh

# All app test scripts are links to this one. Exactly what binary to test will
# be derived from the link's name. The test cases themselves are in a file
# whose name is derived in the same way.

# This is not found in the Bourne shell...
#shopt -s -o nounset

prog=${0%.sh}	# derive tested program's name from own name
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
# arguments>. The expected result is in a file named test_<prog name>_<case
# name>.exp .

pass=TRUE
while IFS=':' read name args ; do
	# setting IFS to '' preserves whitespace through shell word splitting
	IFS='' cmd="../src/$prog $args"
	echo -n "test '$name': '$cmd' - "
	eval $cmd > test_${prog}_$name.out
	if diff test_${prog}_$name.out test_${prog}_$name.exp ; then
		echo "pass"
	else
		echo "FAIL"
		pass=FALSE
	fi
done < $args_file

if test $pass = FALSE ; then
	exit 1
else
	exit 0
fi
