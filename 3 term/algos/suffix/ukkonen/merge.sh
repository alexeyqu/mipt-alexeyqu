#!/bin/bash
echo "#define _ESKO__" > MergedEsko.cpp
for i in esko.h esko.cpp main.cpp ;
	do (cat "${i}" | grep --invert-match '#include "'; echo) >> MergedEsko.cpp; done
