#!/bin/bash -u

# a script for testing the solver performance (Haoran Zhou)

[ -f ./path.sh ] && . ./path.sh;  # source the path

echo "$0 $@"  # print the command line for logging

if [ $# != 4 ]; then 
    echo "Usage: test-solver-speed.sh <solver-type> <difficulty> <num-problem-to-test>"
    echo "    solver-type: 1. Serial 2. MPI"
    echo "    difficulty: num of missing elemnents"
    echo "    num-problems-to-test: repeat testing for a certian amount of times"
    echo "    num-procs: for parallel solvers"
    exit 1;
fi

solver_type=$1
difficulty=$2
num_problems=$3
num_procs=$4
solve_cmd=  # command to solve

tmp_in_file="test_in.csv"
tmp_out_file="test_out.csv"

if [ $solver_type -eq 1 ] ; then
    echo "Solving using serial"
    solve_cmd="solve-sudoku ${tmp_in_file} ${tmp_out_file}";
elif [ $solver_type -eq 2 ] ; then
    echo "Solving using mpi"
    solve_cmd="mpirun -np ${num_procs} --mca mpi_cuda_support 0  solve-sudoku-mpi ${tmp_in_file} ${tmp_out_file}";
else 
    echo "Solver type not supported"
    exit 1;
fi

for i in $(seq $num_problems)
do
    generate-sudoku $difficulty $tmp_in_file
    eval $solve_cmd
done
