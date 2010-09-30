#!/bin/sh

# All app test scripts are links to this one. Exactly what binary to test will
# be derived from the link's name. The test cases themselves are in a file
# whose name is derived in the same way.

# set -v

# I can't use these in strict Bourne shell, so I use a sed command
#prog=${0%.sh}	# derive tested program's name from own name
#prog=${prog#*_}

prog=`echo $0 | sed -e 's|\.sh$||' -e 's/^.*test_//'`
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

# Define variables that can be used for conditional running of test cases
if grep "^#define HAVE_LIBXML2 1" ../config.h ; then
	xml=1
else
	xml=0
fi

# Each test case on one line. Line structure is
# <case name>:<program arguments>[:<condition>].
# The program will run if <condition> is met (or if there is no <condition>),
# and if run will be passed <program arguments>. The expected result is in a
# file named "test_<prog name>_<case name>.exp" .

pass=TRUE
while IFS=':' read name args condition ; do
	[ '' = "$condition" ] && condition=1	# no condition => true
	if [ $condition = 1 ] ; then
		echo evaluating test $name
	else
		echo skipping
		continue
	fi
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
