all:
	mpiCC -std=c++11 -o ./mpi_task ./main.cpp
	mpirun --hostfile ./hostfile ./mpi_task
#   rm -rf ./mpi_task
