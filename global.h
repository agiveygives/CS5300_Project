/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: global.h
 *  Includes:
 *    global variables to be used throughout the program
 */

#ifndef GLOBAL_H
#define GLOBAL_H
#include "objects.h"
#include <vector>

//global variables
extern string token;				// stores the current token
extern string tableToken;			// stores the current table for alias checking
extern string prevToken;			// stores the previous token to check for EOF
extern string attributeType;		// stores the variable type of an attribute
extern string relationalAlgebra;	// stores relational algebra string to be printed
extern string queryTree;			// stores query tree to be printed

extern statement currentStatement;	// stores current statement enum for building the relational algebra
extern int queryNum;				// stores query number to show the user which query failed/succeeded
extern bool failure;				// stores whether or not the query failed (prevents relational algebra from being generated after failure)
extern locale loc;

extern schemaLL *schema;				// Linked list of schema
extern vector<string> potentialAlias;	// stores all potential aliases from the SELECT clause
extern vector<string> select;			// stores relational algebra tokens for select operation
extern vector<string> project;			// stores relational algebra tokens for project operation
extern vector<string> cartesianProduct;	// stores relational algebra tokens for cartesian product

#endif
