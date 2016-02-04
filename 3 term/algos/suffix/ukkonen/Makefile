all: clean esko judge

judge:
	sh merge.sh
	g++ MergedEsko.cpp -o judge

esko:
	g++ esko.cpp main.cpp -o esko -D NOJUDGE

clean:
	rm -f *.o esko judge