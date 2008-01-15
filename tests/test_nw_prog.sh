#! /bin/sh


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

pass=TRUE
while IFS=':' read name args ; do
	# setting IFS to '' preserves whitespace through shell word splitting
	IFS='' cmd="../src/$prog $args"
	echo -n "test: '$cmd' - "
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
