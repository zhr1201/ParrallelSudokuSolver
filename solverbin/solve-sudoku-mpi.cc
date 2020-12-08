// A tool for solve sudoku problems using mpi solver
// Usage: mpirun -np <num procs> solve-sudoku <intput_file (sudoku problems)> <output_file (answers)>
//

#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "solver/solver-mpi.h"
#include "util/timer.h"
#include "generator/file-parser.h"


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    try {
        using namespace sudoku;
        const char *usage = 
            "Solve sudoku problems using mpi and export them to a file. \n"
            "Usage: mpirun -np <num procs> solve-sudoku <intput_file (sudoku problems)> <output_file (answers) \n";
        
        if (argc != 3) {
            fprintf(stderr, "%s", usage);
            exit(1);
        }

        std::string infile(argv[1]);
        std::string outfile(argv[2]);

        Solvable *read_sudoku = nullptr;
        SudokuAnswer answer;

        FileParser fp;
        fp.ReadFromFile(infile, read_sudoku);

        SolverBase *solver = SolverMPI::GetInstance();

        int mpi_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
        MPI_Barrier(MPI_COMM_WORLD);
        double start, end;
        if (mpi_rank == 0)
            start = MPI_Wtime();
        solver->Solve(*read_sudoku, answer);
        if (mpi_rank == 0) {
            end = MPI_Wtime();
            std::cout << "Took " << end - start << " to solve! \n" << std::endl; 

            AnswerIO ans_writer;
            ans_writer.WriteToFile(outfile, &answer);
        }
        delete read_sudoku;
        MPI_Barrier(MPI_COMM_WORLD);
        MPI_Finalize();
        return 0;
    } catch (const std::exception &e) {
        std::cerr << e.what();
        return -1;
    }
}