#!/bin/bash -u

# a script for testing the solver performance (Haoran Zhou)

[ -f ./path.sh ] && . ./path.sh;  # source the path

echo "$0 $@"  # print the command line for logging

if [ $# != 3 ]; then 
    echo "Usage: compare-solver-speed.sh <difficulty> <num-problem-to-test> <num-procs>"
    echo "    difficulty: num of missing elemnents"
    echo "    num-problems-to-test: repeat testing for a certian amount of times"
    echo "    num-of-procs: num procs parrallel solver is going to use"
    exit 1;
fi

difficulty=$1
num_problems=$2
num_of_procs=$3

tmp_in_file="test_in.csv"
tmp_out_file="test_out.csv"

solve_serial_cmd="solve-sudoku ${tmp_in_file} ${tmp_out_file}";

solve_mpi_cmd="mpirun -np ${num_of_procs} solve-sudoku-mpi ${tmp_in_file} ${tmp_out_file}";

for i in $(seq $num_problems)
do
    generate-sudoku $difficulty $tmp_in_file
    echo "====================================="
    echo "Solve using serial solver"
    eval $solve_serial_cmd
    echo "Solve using mpi solver"
    eval $solve_mpi_cmd

done
