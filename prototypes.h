/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: prototypes.h
 *  Includes:
 *    all function prototypes
 */

#ifndef PROTOTYPES_H
#define PROTOTYPES_H
#include "objects.h"

//// Function Prototypes: /////////////////////////////////////////

/*	Reads the tokens until the first SELECT
 *	Builds a schemaLL of the database schema
 */
void getSchema();

/*	Reads tokens from cin
 *	converts all tokens to uppercase to avoid any case sensitivity
 */
void getToken();

//------checking functions--------------------------------

/*	returns true if token is a reserved word/character
 *	false if the token is not
 */
bool isKeyword();
/*	returns true if token is a comprison operator
 *	false if it is not
 */
bool isComparison();
/*	returns true if token is a non-empty sequence of digits
 */
bool isInteger();
/*	returns true if token is a non-empty sequence of characters beginning with a letter
 *		and if the token is not a keyword or a table
 *	The function will add the alias to the potentialAliases vector if it satisfied the conditions
 */
bool isAlias();
/*	returns true if the table exists in the schemaLL data structure
 */
bool isTable();
/*	returns true if the attribute exists in the schemaLL data structure
 *		if a table is specified, it will check if the attribute exists in that table
 */
bool isAttribute();
/*	returns true if token is a non empty set of characters beginning an ending with single quotes (')
 */
bool isString();
/*	returns true for an integer, negative number, or decimal
 *		checks for an integer that can be proceeded by a '-' or followed by a '.' and another integer
 */
bool isReal();
/*	returns true if token is a date enclosed in quotes of the following formats
 *			'MM/DD/YY' || 'M/D/YY' || 'M/DD/YY'
 */
bool isDate();

/* returns true if the alias exists in the schemaLL data structure
 */
bool checkAlias();
/*	if token isAlias() and not checkAlias() then this will add the alias to schemaLL and 
 *		pop the alias from the potentailAlias vector
 *	setAlias must be passed a string of the table the alias is associated with
 */
void setAlias(string);

/*	begins parsing the query by checking for any parenthesis, then checking for token == "SELECT"
 *	if SELECT is found, parse_SelectStatement() is called
 *	When select statement returns, if the token == ";" then the query was completed
 */
void parse_Query();
/*	calls getToken and returns if token is an aggregate operator
 *		"AVG" || "COUNT" || "MAX" || "MIN" || "SUM"
 *	calls fail function if false
 */
void parse_AggregateOperator();
/*	checks for valid aggregate function consisting of "aggregateOperator ([ALL] Expression | *) 
 */
void parse_AggregateFunction();
/*	checks if the attribute is an attribute of specified table/alias
 */
void parse_Member();
/*	checks for an expression of the form:
 *		Expression := 	[integer|string|real|date] | Member | [NOT] Expression | 
 * 						BinaryExpression | (Expression {, Expression}) | 
 *		   				NotLikeExpression | Expression NOT NULL | IsExpression | 
 *  					BetweenExpression | InExpression | [NOT][EXISTS](SelectStatement)
 */
void parse_Expression();
/*	checks for binary expression of the form:
 *		BinaryExpression := Expression [ + | - | * | / | % ] Expression
 */
void parse_BinaryExpression();
/*	checks for expression of the form:
 *		Expression IS [NOT] Expression
 */
void parse_IsExpression();
/*	checks for expression of the form:
 *		Expression [NOT] BETWEEN Expression AND Expression
 */
void parse_BetweenExpression();
/*	checks for expression of the form:
 *		Expression [NOT] IN [[(SelectStatement | Expression[, Expression])] | table]
 */
void parse_InExpression();
/*	checks if token == "DISTINCT"
 *		calls getToken()
 *	calls parse_InnerSelect()
 *	calls parse_FromStatement()
 *	checks for where statment, group by statement, having statement, union, intersect, except, or contains
 *		will call respective function for found statements
 */
void parse_SelectStatement();
/*	checks for valid select clause consisting of attributes and/or aggregate functions
 */
void parse_InnerSelect();
/*	checks if token=="FROM"
 *		calls parse_InnerFrom()
 *	else fail
 */
void parse_FromStatement();
/*	checks for valid from statement consisting of tables and possible renaming using the AS keyword
 */
void parse_InnerFrom();
/*	calls getToken() and parse_Expression()
 */
void parse_WhereStatement();
/*	checks for valid GROUP BY clause by calling parse_Expression() until a comma is not present
 */
void parse_GroupByStatement();
/*	checks for valud HAVING statement consisting of:
 *		Expression | AggregateFunction Comparison [real | string]
 */
void parse_HavingStatement();
/*	checks for valid ORDER BY statement consisting of an Expression
 */
void parse_OrderByStatement();

//ending functions
/*	prints out which query failed and where it failed
 * 	clears the relational algebra vectors, relational algebra string, and query tree string
 * 	increments queryNum to indicate that the parser is moving to the next query
 *	it will then call getToken until the end of the query ";" is found while constantly checking for the end
 *	calls parse_Query()
 */
void fail(string error);
/*	Checks if any aliases go undeclared and will call fail if there are
 *	prints relational algebra dn query tree
 *	clears relational algebra vectors, relational algebra string, and query tree
 *	calls parse_Query() if it is not at the end of the input
 */
void success();
/* 	checks if the prevToken == token, indicating that the input has reached the end
 * 	it then deallocated all the memory used by the linked list and exits the program
 */
void checkEnd();

/*	constructs the relational algrebra string that will be printed to the console by
 *	reading all the vectors into their respective function calls
 */
void buildRelationalAlgebra();
/*	runs through a state machine based of which current SQL statment is active
 *	adds the  clause of the statement to the respective vector:
 *		SELECT -> project
 *		FROM -> cartesianProduct
 *		WHERE -> select
 *	if UNION | INTERSECT | EXCEPT | CONTAINS
 *		buildRelationalAlgebra and clear vectors
 */
void getRelationalAlgebra();
/*	constructs the query tree string that will be printed to the console by
 *	reading all the vectors into their respective function calls and drawing proper lines
 */
void buildQueryTree();
/*	Constructs an optimized query tree that will be printed to the console by
 *	reading all the vectors into their respective function calls associated with
 *	the correct table and drawing proper lines
 */
void optimizedQueryTree();
 
/*	Used by optimizedQueryTree() to get all the attributes of a table that a query needs
 */
void getAttributes(vector<string> queryTokens, vector<string> &tableProject, string currentTable);

string toCaps(string capitalize);

bool checkAttribute(string check, string table);

#endif
