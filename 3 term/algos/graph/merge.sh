#!/bin/bash
echo "#define PUSH_RELABEL_" > MergedPushRelabel.cpp
for i in graph.h graph.cpp network.h network.cpp pushrelabel.h pushrelabel.cpp main.cpp ;
	do (cat "${i}" | grep --invert-match '#include "'; echo) >> MergedPushRelabel.cpp; done

echo "#define MKM_" > MergedMKM.cpp
for i in graph.h graph.cpp network.h network.cpp mkm.h mkm.cpp main.cpp ;
	do (cat "${i}" | grep --invert-match '#include "'; echo) >> MergedMKM.cpp; done