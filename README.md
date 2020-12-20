# ParrallelSudokuSolver: a highly optimized solver library for 9*9 sudoku problems
CMSC818X High Performance Computing final project @UMD

## Preface
The performance of the serial solver is already very good even several designs are compromized for compatibility with the parallel solver. Solving an easy problem normally takes only 1 ms - 2 ms on a modern machine. 

Since the performance of the serial solver is already very good. The parallel solver aims at deviding the problem better with minimum communication overhead. The performance is worse for parrallel solver if the problem itself is very easy. But if the problem is very hard, one example is that it takes 17 ms for the serial solver to solve test-data-7.csv, the mpi solver only need 3 ms with 9 processes.

## Build
make depend

make

## Run test cases
### Normal tests
make test

Run tests separately: e.g solver-test, 1) enter solver 2) type ./solver-test
### MPI tests
make testmpi

Run tests separately: e.g. solver-mpi-test, 1) enter solver 2) mpirun -np 9 ./solver-mpi-test

### Executables
generatorbin/generate-sudoku

solverbin/solve-sudoku

validatorbin/validate

### Test bash scripts

eval-script.sh

## Architecture

## Parrallization

## For developers
1. Pull your branch from develop and merge into and pull from that branch.

2. Compiler flags are in sudoku.mk, two sets of normally used flags are there already, one is for debugging and one for releasing.

## Reference
http://individual.utoronto.ca/rafatrashid/Projects/2012/SudokuReport.pdf
