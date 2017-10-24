#ifndef GLOBAL_H
#define GLOBAL_H
#include "objects.h"

//global variables
extern string token;
extern string tableToken;
extern string prevToken;

const int COMPARE_SIZE = 6;
const string COMPARE[COMPARE_SIZE] = {"=", ">", ">=", "<", "<=", "<>"};

extern schemaLL schema;		// creates an empty schema

#endif
