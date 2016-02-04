#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "commands.h"

#define NOPCODE -112

const int Max_Name_Len = 50;
const int Max_Str_Len = 200;
const int BT_Version = 1;
const int Max_Code_Len = 500;

void BT_Empty_File_Generate (char elfname[], int* nofnops);

void BT_Translate (FILE* elfile, FILE* cpufile, int nofnops);

void bin_printf (FILE* outstream, char c);
