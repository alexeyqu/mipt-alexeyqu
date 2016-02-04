all: matrmax_mpi

tester: tester.o functions.o functions_mpi.o functions_openmp.o
	mpicc -pthread functions.o functions_mpi.o tester.o functions_openmp.o -o tester -fopenmp

matrmax_mpi: functions.o matrmax_mpi.o functions_openmp.o functions_mpi.o
	mpicc functions.o matrmax_mpi.o functions_openmp.o functions_mpi.o -o matrmax_mpi -fopenmp

functions_mpi.o: functions_mpi.c
	mpicc -O0 -c functions_mpi.c

matrmax_openmp.o: matrmax_openmp.c
	gcc -O0 -c matrmax_openmp.c

functions_openmp.o: functions_openmp.c
	gcc -O0 -c functions_openmp.c -fopenmp

tester.o: tester.c
	mpicc -c tester.c

functions.o: functions.c
	mpicc -O0 -c functions.c

clean:
	rm -f *.o matrmax_parallel matrmax_consecutive matrmax_mpi tester

