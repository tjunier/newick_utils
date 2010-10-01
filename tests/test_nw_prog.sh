#!/bin/sh

# All app test scripts are links to this one. Exactly what binary to test will
# be derived from the link's name. The test cases themselves are in a file
# whose name is derived in the same way.

# set -v

# Checks if a tests applies. For now a test always applies unless it requires
# libxml but this is not used, or the other way around. If the test's name
# contains the string, 'withxml', it only applies when libxml is being used; if
# it contains 'noxml', it applies only when libxml is NOT being used; otherwise
# it applies regardless of libxml use.

check_applies()
{
	name=$1
	if echo $name | grep noxml > /dev/null ; then
		if [ "$xml" = "off" ] ; then
			return 1
		else
			return 0
		fi
	elif echo $name | grep withxml > /dev/null ; then
		if [ "$xml" = "on" ] ; then
			return 1
		else
			return 0
		fi
	else
		return 1
	fi
}

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

# Set xml flag, for checking which tests apply.

if grep '^#define.*HAVE_LIBXML2' ../config.h > /dev/null ; then
	xml='on'
else
	xml='off'
fi

# Each test case in on one line. Line structure is <case name>:<prog
# arguments>. The expected result is in a file named test_<prog name>_<case
# name>.exp .

pass=TRUE
while IFS=':' read name args ; do
	# setting IFS to '' preserves whitespace through shell word splitting
	check_applies $name
	if [ "$?" -eq "0" ] ; then
		echo "Skipping $name"
		continue
	fi
	IFS='' cmd="../src/$prog $args"
	echo -n "test '$name': '$cmd' - "
	eval $cmd > test_${prog}_$name.out
	if [ "$?" -ne "0" ] ; then
		pass=FALSE
		break
	fi
	if diff test_${prog}_$name.out test_${prog}_$name.exp ; then
		echo "pass"
	else
		echo "FAIL"
		pass=FALSE
		break
	fi
done < $args_file

if test $pass = FALSE ; then
	exit 1
else
	exit 0
fi
