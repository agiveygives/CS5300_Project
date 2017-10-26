#ifndef GLOBAL_H
#define GLOBAL_H
#include "objects.h"
#include <vector>

//global variables
extern string token;
extern string tableToken;
extern string prevToken;
extern string relationalAlgebra;
extern string queryTree;

extern statement currentStatement;
extern int queryNum;
extern bool failure;
extern locale loc;

const int COMPARE_SIZE = 6;
const string COMPARE[COMPARE_SIZE] = {"=", ">", ">=", "<", "<=", "<>"};

extern schemaLL *schema;		// creates an empty schema
extern vector<string> potentialAlias;
extern vector<string> select;
extern vector<string> project;
extern vector<string> cartesianProduct;

#endif
