//////////////////////////////////////
// Emma Williston and Andrew Givens //
// CS 5300 - Project 1              //
// SQL Compiler                     //
// 26 October 2017                  //
//////////////////////////////////////


#include "objects.h"
#include "global.h"
#include "prototypes.h"

//// Main Method: ////////////////////////////////////////////

string token = "";
string prevToken = token;
schemaLL schema = schemaLL();

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