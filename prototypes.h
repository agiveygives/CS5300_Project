#ifndef PROTOTYPES_H
#define PROTOTYPES_H
#include "objects.h"

//// Function Prototypes: /////////////////////////////////////////

void getSchema();

//function to use std input to set a value for token
void getToken();

// parsing functions
bool parse_query();



	bool isKeyword();
	bool isInteger();
	bool isAlias();
	bool isTable();
	bool isAttribute();
	bool isString();
	bool isReal();
	bool isDate();

	void checkAlias(); //??

	void parse_UnaryOperator();
	void parse_BinaryOperator();
	void parse_BitwiseOperator(); //need?
	void parse_LogicalOperator(); //need?
	void parse_SetOperator();
	void parse_AggregateOperator();
	void parse_Comparison(); //need?

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

	void quit() //need?

bool parse_attributes();
bool parse_tables();
bool parse_schema(const dataType data);

//quit function
void quit(string error);

#endif
