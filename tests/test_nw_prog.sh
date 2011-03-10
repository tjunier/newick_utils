#!/bin/sh

# All app test scripts are links to this one. Exactly what binary to test will
# be derived from the link's name. The test cases themselves are in a file
# whose name is derived in the same way.

# set -v

# Checks if a test _case_ applies. For now a case always applies unless it
# requires libxml but this is not used, or the other way around. If the test's
# name contains the string, 'withxml', it only applies when libxml is being
# used; if it contains 'noxml', it applies only when libxml is NOT being used;
# otherwise it applies regardless of libxml use.

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

# Set flags, for checking which tests apply.

if grep '^#define.*USE_LIBXML2' ../config.h > /dev/null ; then
	xml='on'
else
	xml='off'
fi

if grep '^#define.*CHECK_NW_SCHED' ../config.h > /dev/null ; then
	check_nw_sched='on'
else
	check_nw_sched='off'
fi

# I can't use these in strict Bourne shell, so I use a sed command
#prog=${0%.sh}	# derive tested program's name from own name
#prog=${prog#*_}

prog=`echo $0 | sed -e 's|\.sh$||' -e 's/^.*test_//'`
args_file=test_${prog}_args

# nw_sched is only tested if Guile is being used

if [ "$prog" = "nw_sched" ] ; then
	if [ "$check_nw_sched" = "off" ]; then
		echo "Guile not used - nw_sched test disabled."
		exit 0
	fi
fi

# Ok, try to test $prog.

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

shopt -s -o xtrace
pass=TRUE
# NOTE: this loop actually FAILS in Bash (and Sh) because the while is in a
# pipeline and is executed as a separate process, so it has local copies of
# 'pass' and can't pass data back to its parent.
# In Bash one could do nifty things with process substitution and file
# descriptor redirection (see http://ubuntuforums.org/showthread.php?t=312017),
# but this has to run under plain sh, so we won't do it.

grep -v '^#' < $args_file | while IFS=':' read name args ; do
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
		echo $pass
		break
	fi
done

echo $pass
if test $pass = FALSE ; then
	exit 1
else
	exit 0
fi
