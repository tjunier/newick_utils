#!/bin/sh

# NOTE: this script is not really a test, rather, it counts how many test
# cases of each kind (Unit or App) there are.  The TESTS variable in
# Makefile.am is set so that this program gets run last, just before the
# failure/pass count.

# What counts as a test case is: i) any test_* function in the unit tests, and
# ii) any line in the app tests (see below). A unit test case generally tests
# several assertions.

nb_unit_test_cases=$( grep 'failures +=' test_*.c | wc -l)
nb_app_test_cases=$(
	wc -l test_nw_*_args | grep total | awk '{print $1}')

printf "\n"
printf "%0d unit test cases\n" $nb_unit_test_cases
printf "%0d application test cases\n" $nb_app_test_cases
printf "total: %0d cases.\n" $((nb_app_test_cases + 
	nb_unit_test_cases))
