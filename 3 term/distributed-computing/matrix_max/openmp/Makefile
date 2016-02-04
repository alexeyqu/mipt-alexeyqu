all: matrmax_openmp

tester: tester.o functions.o functions_openmp.o
	gcc -pthread functions.o functions_openmp.o tester.o -o tester -fopenmp

matrmax_openmp: functions.o functions_openmp.o matrmax_openmp.o
	gcc functions_openmp.o functions.o matrmax_openmp.o -o matrmax_openmp -fopenmp

matrmax_openmp.o: matrmax_openmp.c
	gcc -O0 -c matrmax_openmp.c

functions_openmp.o: functions_openmp.c
	gcc -O0 -c functions_openmp.c -fopenmp

tester.o: tester.c
	gcc -c tester.c

functions.o: functions.c
	gcc -O0 -c functions.c

clean:
	rm -f *.o matrmax_parallel matrmax_consecutive matrmax_openmp

