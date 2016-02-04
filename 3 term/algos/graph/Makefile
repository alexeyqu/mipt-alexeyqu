all: clean esko judge

judge:
	sh merge.sh
	g++ MergedPushRelabel.cpp -o judgePushRelabel
	g++ MergedMKM.cpp -o judgeMKM

pushrelabel:
	g++ graph.h graph.cpp network.h network.cpp pushrelabel.h pushrelabel.cpp main.cpp -o pushrelabel -D PUSH_RELABEL_

mkm:
	g++ graph.h graph.cpp network.h network.cpp mkm.h mkm.cpp main.cpp -o mkm -D MKM_

clean:
	rm -f *.o pushrelabel mkm