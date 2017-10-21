//////////////////////////////////////
// Emma Williston and Andrew Givens //
// CS 5300 - Project 1              //
// SQL Compiler                     //
// 26 October 2017                  //
//////////////////////////////////////


#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <array>

using namespace std;


//global variables
string token;
string query;
int state = 0;

const int COMPARE_SIZE = 6;
const string COMPARE[COMPARE_SIZE] = {"=", ">", ">=", "<", "<=", "<>"};
const string TABLES[] = {"Sailors", "Boats", "Reserves"};
const string ATTRIBUTES[] = {"sid:integer", "sname:string", "rating:integer", "age:real", "bid:integer", "bname:string", "color:string", "sid:integer", "bid:integer", "day:date"};

// struct schemaLL {
// 	string m_tableName;
//	string alias;
// 	string m_attributeName;
// 	string m_sttributeType;
// 	schemaLL *m_next = NULL;
// };

//// Function Prototypes: /////////////////////////////////////////

// function to read in Query input
void readInFile();
void readQuery();

//function to use std input to set a value for token
void getToken();

// parsing functions
bool parse_query();
bool parse_attributes();
bool parse_tables();

//quit function
bool forUntil(const string checkArr[]);
void quit(string error);

//// Main Method: ////////////////////////////////////////////


int main() {
	//readInFile();
	
	getToken();
	if(parse_query())
		cout << "CORRECT" << endl;
	else
		cout << "INVALID" << endl;
	
	return 0;
}

//// Function Declarations: /////////////////////////////////

void getToken() {
	cin >> token;
	return;
}

bool parse_query() {
	bool valid = false;
	short whereState = 0;
	int size = 0;
	
	while (state != -1) {
		switch(state) {
			case 0:	// SELECT
				if (token == "SELECT") {
					getToken();
					if(parse_attributes())
						state = 1;
				}
				break;
				
			case 1:	// FROM
				if (token == "FROM") {
					getToken();
					if (parse_tables())
						state = 2;
				}
				break;
				
			case 2:	// WHERE
				if (token == "WHERE") {
					getToken();

					// state machine for WHERE clause
					while(whereState >= 0) {
						switch(whereState) {
							case 0:	// check for an attribute
								size = ATTRIBUTES->size();
	
								for (int i = 0; i < size; i++) {
									if(token == ATTRIBUTES[i]) {
										whereState = 1;		// attribute found
										break;
									} else {
										whereState = -1;	// No attribute, invalid WHERE clause
									}
								}

								break;
							case 1:	// check for comparison operator
								getToken();
		
								for (int i = 0; i < COMPARE_SIZE; i++) {
									if (token == COMPARE[i]) {
										state = 2; 	// There is a comparison operator present.
										break;
									} else
										state = 3;	// Token is not a comparison operator. Check if AS, EXISTS, or NOT
								}

								break;
							case 2: // check for valid right side of comparison
								getToken();

								// token must be attribute, constant, or nested query w/ aggregate function.

								// check for an attribute
								size = ATTRIBUTES->size();
	
								for (int i = 0; i < size; i++) {
									if(token == ATTRIBUTES[i]) {
										whereState = 4;		// attribute found
										break;
									}
								}

								if(whereState == 4){
									break;
								} else {
									// check if token is a constant

									// check for query returning an aggregate function
								}

								break;
							case 3: // check NOT, AS, and EXISTS
								if(token == "NOT")
									getToken();

								if(token == "AS" || "EXISTS") {
									getToken();

									if(token == "(") { // check for opening parenthesis
										getToken();

										if(parse_query()) { // check for valid query
											getToken();

											if(token == ")") { // check for closing parenthesis
												whereState = 4;
											} else
												whereState = -1;
										} else
											whereState = -1;
									} else
										whereState = -1;
								}

								break;
							case 4: // check for AND or OR
								getToken();

								if(token == "AND" || token == "OR"){
									whereState = 0;
								} else {
									whereState = -1;
									valid = true;		// query is currently in a valid state
								}

								break;
						}
					}
				}
				break;

			case 3: // CONTAINS

				break;
			case 4:	// GROUP BY

				break;
			case 5:	// HAVING

				break;
			case 6:	// UNION

				break;
			case 7:	// INTERSECT

				break;
			case 8:	// EXCEPT
				break;
		}
	}
	
	return valid;
}

/*	validates that all attributes are present in the schema
 * 	returns true if attribute or attribute list is valid
 */
bool parse_attributes() {
	bool valid = false;
	
	// checks if there is a list of attributes
	if(token[token.length() - 1] == ',' && forUntil(ATTRIBUTES)) {
		valid = parse_attributes();
	} else { // if there isn't a list check if the attribute is in the schema
		valid = forUntil(ATTRIBUTES);

		getToken();

		// checks if a comma is the token after an attribute
		// if so, parse_attributes is recalled on the next token
		if(token == ",") {
			getToken();
			valid = parse_attributes();
		}
	}
	
	return valid;
}

/*	validates that all tables are present in the schema
 *	Handles any renaming to the data structure
 */
bool parse_tables() {
	bool valid = false;
	
	// checks if there is a list of tables
	if(token[token.length() - 1] == ',' && forUntil(TABLES)) {
		valid = parse_tables();
	} else { // if there isn't a list check if the table is in the schema
		valid = forUntil(TABLES);

		getToken();

		// checks if a comma is the token after an table
		// if so, parse_tables is recalled on the next token
		if(token == ",") {
			getToken();
			valid = parse_tables();
		}
	}

	// add renaming using the "AS" keyword
	
	return valid;
}

bool forUntil(const string checkArr[]) {
	bool valid = false;
	int size = checkArr->size();
	
	for (int i = 0; i < size; i++) {
		if (token == checkArr[i] || token == checkArr[i] + ",") {
			valid = true;
			break;
		}
	}
	return valid;
}

void quit(string error) {
	cout << error << endl;
	exit(0);
}
