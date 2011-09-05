#!/bin/sh

# All app test scripts are links to this one. Exactly what binary to test will
# be derived from the link's name. The test cases themselves are in a file
# whose name is derived in the same way.

# set -v

# Checks if a (nw_display) test _case_ applies. For now a case always applies
# unless it requires libxml but this is not used, or the other way around. If
# the test's name contains the string, 'withxml', it only applies when libxml
# is being used; if it contains 'noxml', it applies only when libxml is NOT
# being used; otherwise it applies regardless of libxml use.

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

if ../src/nw_display -h | grep 'This executable uses LibXML2.' > /dev/null
then
	xml='on'
elif ../src/nw_display -h | grep 'This executable does NOT use LibXML2' > /dev/null
then
	xml='off'
else
	echo "Can't determine if nw_display uses LibXML - Aborting" >&2
	exit 1
fi

if [ -x ../src/nw_sched ] ; then
	check_nw_sched='on'
else
	check_nw_sched='off'
fi

if [ -x ../src/nw_luaed ] ; then
	check_nw_luaed='on'
else
	check_nw_luaed='off'
fi

# When this is run through the Makefile, SRCDIR is set. Otherwise, we set it
# here.
SRCDIR=${SRCDIR:-.}

# I can't use these in strict Bourne shell, so I use a sed command
#prog=${0%.sh}	# derive tested program's name from own name
#prog=${prog#*_}

prog=`echo $0 | sed -e 's|\.sh$||' -e 's/^.*test_//'`
args_file=$SRCDIR/test_${prog}_args

# nw_sched is only tested if Guile is being used

if [ "$prog" = "nw_sched" ] ; then
	if [ "$check_nw_sched" = "off" ]; then
		echo "Guile not used - nw_sched test disabled."
		exit 0
	fi
fi

# nw_luaed is only tested if Lua is being used

if [ "$prog" = "nw_luaed" ] ; then
	if [ "$check_nw_luaed" = "off" ]; then
		echo "Lua not used - nw_luaed test disabled."
		exit 0
	fi
fi

# Ok, try to test $prog.
# TODO: check for prog here instead of setting PATH as below. If $prog is
# found, prefix it with the relative path (e.g. ../src/$prog).

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
# name>.exp Lines starting with '#' are ignored.

export PATH=../src:../_build/src:$PATH

pass=TRUE
while IFS=':' read name args ; do
	# if first word starts with '#', discard (comment)
	echo $name | grep '^#' > /dev/null && continue
	# setting IFS to '' preserves whitespace through shell word splitting
	check_applies $name
	if [ "$?" -eq "0" ] ; then
		echo "Skipping $name"
		continue
	fi
	IFS='' cmd="$prog $args"
	echo -n "test '$name': '$cmd' - "
	outfile=./test_${prog}_$name.out
	(cd $SRCDIR; eval $cmd > $outfile)
	if [ "$?" -ne "0" ] ; then
		pass=FALSE
		break
	fi
	if diff $outfile $SRCDIR/test_${prog}_$name.exp ; then
		echo "pass"
		rm $outfile
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
