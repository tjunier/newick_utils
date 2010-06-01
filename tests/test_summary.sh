#!/bin/sh

# NOTE: this script is not really a test, rather, it counts how many test
# clauses of each kind (Unit or App) there are.  The TESTS variable in
# Makefile.am is set so that this program gets run last, just before the
# failure/pass count.

nb_unit_test_clauses=$( grep 'failures +=' test_*.c | wc -l)
nb_app_test_clauses=$(
	wc -l test_nw_*_args | grep total | awk '{print $1}')

printf "%0d unit test clauses\n" $nb_unit_test_clauses
printf "%0d application test clauses\n" $nb_app_test_clauses
printf "total: %0d clauses.\n" $((nb_app_test_clauses + 
	nb_unit_test_clauses))
