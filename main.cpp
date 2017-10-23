//////////////////////////////////////
// Emma Williston and Andrew Givens //
// CS 5300 - Project 1              //
// SQL Compiler                     //
// 26 October 2017                  //
//////////////////////////////////////


#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>

using namespace std;


//global variables
string token = "";
string prevToken = token;

const int COMPARE_SIZE = 6;
const string COMPARE[COMPARE_SIZE] = {"=", ">", ">=", "<", "<=", "<>"};

/*	Linked List data structure to store table name, attribute name, attribute type, and table renaming
 *	the pointer will point to the next attribute in the schema loaded from the input file before the queries
 */
class schemaLL {

	public:
		string m_tableName;
		string m_alias;
		string m_attributeName;
		string m_attributeType;
		schemaLL *m_next;

		schemaLL() {
			m_tableName = "";
			m_alias = "";
			m_attributeName = "";
			m_attributeType = "";
			m_next = NULL;
		}
};

schemaLL schema = schemaLL();		// creates an empty schema

/*	This class will be used to store items to build our relational algebra expression
 */
class relationalAlgebra {

	public:

};

/*	This class will be used to build and display our query tree
 */
class queryTree {

	public:

};

enum dataType {
	tableName,
	alias,
	attributeName,
	attributeType
};

//dataType dataSearch;

//// Function Prototypes: /////////////////////////////////////////

void getSchema();

//function to use std input to set a value for token
void getToken();

// parsing functions
bool parse_query();
bool parse_attributes();
bool parse_tables();
bool parse_schema(const dataType data);

//quit function
void quit(string error);

//// Main Method: ////////////////////////////////////////////


int main() {
	getSchema();

	while (prevToken != token) {					// loop while previous token does not equal the current token.
		if(parse_query())
			cout << "CORRECT" << endl;
		else {
			cout << "INVALID" << endl;
		}
	}

	return 0;
}

//// Function Declarations: /////////////////////////////////

/*	This reads the schema and populates a linked list of attributes
 *	Currently not being used in the rest of the program, but it works. So, yay.
 *	Schema must be input with a space after the table name, and a space after each comma
 */
void getSchema() {
	schemaLL *table = &schema;
	string wholeToken = "";
	string currentTable = "";
	int split = 0;

	getToken();
	while(token != "SELECT") { // loops until it reads the first token of the queries
		currentTable = token;

		getToken();
		if(token == "(")
			getToken();

		while(token[strlen(token.c_str()) - 1] != ')') {
			table->m_tableName = currentTable;

			if(token[0] == '(') {
				token.erase(0, 1); 	// erase leading parenthesis
			}

			wholeToken = token;
			for (int i = 0; i < strlen(token.c_str()); i++) {
				if(token[i] == ':') {
					split = i;	// element that includes the ':'
				}
			}

			token.erase(split, strlen(token.c_str()));		// gets only the attribute name
			table->m_attributeName = token;
			
			wholeToken.erase(0, split + 1);		// gets the attribute type
			if (wholeToken[strlen(wholeToken.c_str()) - 1] == ',') {
				wholeToken.erase(wholeToken.find(","), 1);
			}
			table->m_attributeType = wholeToken;

			/*cout << "Table: " << table->m_tableName << endl
			 	 << "Attribute: " << table->m_attributeName << endl
			 	 << "Type: " << table->m_attributeType << endl << endl;*/

			table->m_next = new schemaLL();
			table = table->m_next;

			getToken();
		}

		if(token != ")") {
			// process the last attribute
			table->m_tableName = currentTable;
			wholeToken = token;

			for (int i = 0; i < strlen(token.c_str()); i++) {
				if(token[i] == ':') {
					split = i;	// element that includes the ':'
				}
			}

			token.erase(split, token.find(")") + 1);		// gets only the attribute name
			table->m_attributeName = token;
			
			wholeToken.erase(0, split + 1);		// gets the attribute type
			wholeToken.erase(wholeToken.find(")"), 1);
			table->m_attributeType = wholeToken;

			/*cout << "Table: " << table->m_tableName << endl
				 << "Attribute: " << table->m_attributeName << endl
				 << "Type: " << table->m_attributeType << endl << endl;*/

			table->m_next = new schemaLL();
			table = table->m_next;
		}

		getToken();
	}
}

void getToken() {
	prevToken = token;
	cin >> token;
	//cout << "token: " << token << endl;
	return;
}

bool parse_query() {
	int state = 0;
	bool valid = false;
	short whereState = 0;
	
	while (state != -1) {
		switch(state) {
			case 0:	// SELECT
				if (token == "SELECT") {
					getToken();
					if(parse_attributes()) {	// Save tokens for project relational algebra
						state = 1;
					}
				} else
					quit("ERR: Expected 'SELECT' but got " + token);
				break;
				
			case 1:	// FROM
				getToken();
				if (token == "FROM") {			// save tokens for relations in relational algebra/leaves in query tree
					getToken();
					if (parse_tables()) {
						state = 2;
					}
				} else
					quit("ERR: Expected 'FROM' but got " + token);
				break;
				
			case 2:	// WHERE
				if (token == "WHERE") {			// save inequalities for select relational algebra
					//cout << "\n\nWHERE\n\n";
					

					// state machine for WHERE clause
					// This could probably be put in it's own function, considering WHERE 
					// is a shit show in the state diagram
					while(whereState >= 0) {
						switch(whereState) {
							schemaLL *runner;
							case 0:	// check for an attribute
								getToken();
									
								runner = &schema;
								if(token.find(".") < strlen(token.c_str())) {		// check if attribute includes a '.'
									schemaLL *runner = &schema;
									string table = token;

									table.erase(table.find("."), strlen(table.c_str()));	// get table associated with attribute
									token.erase(0, token.find(".") + 1);					// get attribute

									while(runner != NULL) {
										if(table == runner->m_tableName) {
											if(token == runner->m_attributeName) {
												whereState = 1;
												break;
											} else {
												whereState = -1;
											}
										} else
											whereState = -1;
										runner = runner->m_next;
									}
								} else {		// No table is specified
									while(runner != NULL) {
										if (token == runner->m_attributeName) {
											whereState = 1;
											break;
										} 
										whereState = -1;
										runner = runner->m_next;
									}
								}

								break;
							case 1:	// check for comparison operator
								getToken();
		
								for (int i = 0; i < COMPARE_SIZE; i++) {
									if (token == COMPARE[i]) {
										whereState = 2; 	// There is a comparison operator present.
										break;
									} else {
										whereState = 3;	// Token is not a comparison operator. Check if AS, EXISTS, or NOT
									}
								}

								break;
							case 2: // check for valid right side of comparison
								getToken();

								// token must be attribute, constant, or nested query w/ aggregate function.

								// check for an attribute
	
								runner = &schema;
								if(token.find(".") < strlen(token.c_str())) {		// check if attribute includes a '.'
									schemaLL *runner = &schema;
									string table = token;

									table.erase(table.find("."), strlen(table.c_str()));	// get table associated with attribute
									token.erase(0, token.find(".") + 1);					// get attribute

									runner = &schema;
									while(runner != NULL) {
										if(table == runner->m_tableName) {
											if(token == runner->m_attributeName) {
												whereState = 4;
												break;
											} else
												whereState = -1;
										} else {
											whereState = -1;
										}
										runner = runner->m_next;
									}
								} else {
									while(runner != NULL) {
										if (token == runner->m_attributeName) {
											whereState = 4;
											break;
										} 
										whereState = -1;
										runner = runner->m_next;
									}
								}

								if(whereState == 4){
									break;
								} else {
									// check if token is a constant of matching type

									// check for query returning an aggregate function
								}

								break;
							case 3: // check NOT, AS, and EXISTS
								if(token == "NOT")
									getToken();

								if(token == "IN" || "EXISTS") {
									getToken();

									if(token == "(") { // check for opening parenthesis
										getToken();

										if(parse_query()) { // check for valid query
											//getToken();

											if(token == ")") { // check for closing parenthesis
												whereState = 4;
											} else
												whereState = -1;
										} else
											whereState = -1;
									} else
										whereState = -1;
								} else
									whereState = -1;

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
					if(valid){
						state = -1;		// this exits the while loop if the WHERE ends in a valid state
										// it will have to be taken out in order to check for the rest
										// couldn't think of a way to determine if the query is over since
										//	his test data doesn't include semi colons and it's 4 am
					}
				} else 
					state = -1;
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
	string wholeToken = "";
	bool valid = false;
	
	// checks if there is a list of attributes
	if(token.find(".") < strlen(token.c_str())) {		// check if attribute includes a '.'
		schemaLL *runner = &schema;
		string table = token;

		table.erase(table.find("."), strlen(table.c_str()));	// get table associated with attribute
		token.erase(0, token.find(".") + 1);					// get attribute

		while(runner != NULL) {
			if(table == runner->m_tableName) {
				if(token == runner->m_attributeName) {
					valid = true;
					break;
				}
				if(token == runner->m_attributeName + ",") {
					getToken();
					valid = parse_attributes();
					break;
				}
			}
			runner = runner->m_next;
		}
	} else {
		if(token[strlen(token.c_str()) - 1] == ',' && parse_schema(attributeName)) {
			getToken();
			valid = parse_attributes();
		} else { // if there isn't a list check if the attribute is in the schema
			valid = parse_schema(attributeName);

			//getToken();

			// checks if a comma is the token after an attribute
			// if so, parse_attributes is recalled on the next token
			// if(token == ",") {
			// 	getToken();
			// 	valid = parse_attributes();
			// }
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
	if(token[strlen(token.c_str()) - 1] == ',' && parse_schema(tableName)) {
		getToken();
		valid = parse_tables();
	} else { // if there isn't a list check if the table is in the schema
		valid = parse_schema(tableName);

		getToken();

		// checks if a comma is the token after an table
		// if so, parse_tables is recalled on the next token
		if(token == ",") {
			getToken();
			valid = parse_tables();
		}
	}

	// add renaming using the "AS" keyword 
	// rename will be assigned to table->m_alias
	
	return valid;
}

/*	pass it a value of the enumeration dataType
 * 	the value you will determine which variable of the LL you run through
 */
bool parse_schema(const dataType data) {
	schemaLL *runner = &schema;
	bool valid = false;
	
	switch(data) {
		case tableName:
			while(runner != NULL) {
				if (token == runner->m_tableName || token == runner->m_tableName + ",") {
					valid = true;
					break;
				}
				runner = runner->m_next;
			}
			break;
		case alias:
			while(runner != NULL) {
				if (token == runner->m_alias || token == runner->m_alias + ",") {
					valid = true;
					break;
				}
				runner = runner->m_next;
			}
			break;
		case attributeName:
			while(runner != NULL) {
				if (token == runner->m_attributeName || token == runner->m_attributeName + ",") {
					valid = true;
					break;
				}
				runner = runner->m_next;
			}
			break;
		case attributeType:
			while(runner != NULL) {
				if (token == runner->m_attributeType) {
					valid = true;
					break;
				}
				runner = runner->m_next;
			}
			break;
	}
	
	return valid;
}

void quit(string error) {
	cout << error << endl;
	while(token != "SELECT") {
		getToken();
		if(prevToken == token) {
			exit(0);
		}
	}				
}
