#! /bin/sh


prog=$1

pass=TRUE
while IFS=':' read name args ; do
	echo -n "test: $name - "
	../src/$prog $args > test_${prog}_$name.out
	if diff test_${prog}_$name.out test_${prog}_$name.exp ; then
		echo "pass"
	else
		echo "FAIL"
		pass=FALSE
	fi
done < test_${prog}_args

if test $pass = FALSE ; then
	exit 1
else
	exit 0
fi
