#!/bin/sh

for func_test in $(ls -F test_* | grep '*$' | grep -v '\.' | tr -d '*'); do
	printf "\n%s\n\n" $func_test
	./$func_test || break
done
