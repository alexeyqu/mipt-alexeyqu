all: clean judge

judge:
	sh merge.sh
	g++ MergedSAIS.cpp -o judge

sais:
	g++ sais.cpp main.cpp -o sais -D NOJUDGE

clean:
	rm -f *.o sais judge