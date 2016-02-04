#!/bin/bash
echo "#define _SAIS__" > MergedSAIS.cpp
for i in sais.h sais.cpp main.cpp ;
	do (cat "${i}" | grep --invert-match '#include "'; echo) >> MergedSAIS.cpp; done
