#!/bin/sh

# Use this to check all the function-testing programs. Will stop as soon as one
# does not pass, which makes it easy to spot any problems.

for func_test in $(ls -F test_* | grep '*$' | grep -v '\.' | tr -d '*'); do
	printf "\n%s\n\n" $func_test
	./$func_test || break
done
