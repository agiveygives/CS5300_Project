#ifndef PROTOTYPES_H
#define PROTOTYPES_H
#include "objects.h"

//// Function Prototypes: /////////////////////////////////////////

void getSchema();

//function to use std input to set a value for token
void getToken();

// parsing functions
void parse_Query();

bool isKeyword();
bool isComparison();
bool isInteger();
bool isAlias();
bool isTable();
bool isAttribute();
bool isString();
bool isReal();
bool isDate();

bool checkAlias();
void setAlias(string);

void parse_UnaryOperator();
void parse_BinaryOperator();
void parse_BitwiseOperator(); //need?
void parse_LogicalOperator(); //need?
void parse_SetOperator();
void parse_AggregateOperator();

void parse_AliasAssignment();
void parse_AggregateFunction();
void parse_Member();

void parse_Expression();
void parse_BinaryExpression();
void parse_NotLikeExpression();
void parse_IsExpression();
void parse_BetweenExpression();
void parse_InExpression();

void parse_SelectStatement();
void parse_InnerSelect();
void parse_FromStatement();
void parse_InnerFrom();
void parse_WhereStatement();
void parse_GroupByStatement();
void parse_HavingStatement();
void parse_OrderByStatement();

bool parse_attributes();
bool parse_tables();

//fail functions
void fail(string error);
void success();
void checkEnd();

void buildRelationalAlgebra();
void getRelationalAlgebra();
void buildQueryTree();

#endif
