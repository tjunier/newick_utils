#!/bin/sh

# Tests the binaries

ls ../src/nw_* | grep -v '\.py$' | while read bin; do
	test=$(printf "test_%s.sh" $(basename $bin))
	if ! ./$test ; then
		echo "FAIL"
		exit
	fi
done

echo
echo "All tests PASS."
