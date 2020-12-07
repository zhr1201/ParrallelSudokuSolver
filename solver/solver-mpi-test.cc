#include <vector>
#include "util/global.h"
#include "solver/problem-state.h"
#include "util/string-utils.h"
#include "solver/solver-mpi.h"
#include "validator/validator.h"
#include "solver/sudoku-problem.h"


namespace sudoku {


void TestSolverMPI() {
    
    const std::string in_csv = "test-data-7.csv";
 
    SSudoku ts(in_csv);
    SolverMPI *ss = SolverMPI::GetInstance();
    SudokuAnswer answer;

    int mpi_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Barrier(MPI_COMM_WORLD);
    double start = 0;
    if (mpi_rank == 0)
        start = MPI_Wtime();
    bool ret = ss->Solve(ts, answer);
    if (mpi_rank == 0) {
        double end = MPI_Wtime();
        std::cout << "Take " << end - start << " to solve" << std::endl;
            
        assert(ret);
        Validator val;
        assert(val.Validate(&answer, &ts));
    }
    MPI_Barrier(MPI_COMM_WORLD);

}

}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);
    using namespace sudoku;
    TestSolverMPI();
    MPI_Finalize();
    return 0;
}
