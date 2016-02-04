	.DEFAULT_GOAL := matrmax_parallel	

all: matrmax_parallel matrmax_consecutive matrmax_openmp

tester: tester.o functions.o
	gcc -pthread functions.o tester.o -o tester -fopenmp

matrmax_parallel: functions.o matrmax_parallel.o
	gcc -pthread functions.o matrmax_parallel.o -o matrmax_parallel

matrmax_parallel.o: matrmax_parallel.c
	gcc -O0 -c matrmax_parallel.c

matrmax_consecutive: functions.o matrmax_consecutive.o
	gcc -pthread functions.o matrmax_consecutive.o -o matrmax_consecutive

matrmax_consecutive.o: matrmax_consecutive.c
	gcc -O0 -c matrmax_consecutive.c

tester.o: tester.c
	gcc -c tester.c

functions.o: functions.c
	gcc -O0 -c functions.c

clean:
	rm -f *.o matrmax_parallel matrmax_consecutive matrmax_openmp

