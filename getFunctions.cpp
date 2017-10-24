#include "objects.h"
#include "global.h"
#include "prototypes.h"

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



void quit(string error) {
	tableToken = "";		// reset tableToken variable;
	cout << error << endl;
	while(token != "SELECT") {
		getToken();
		if(prevToken == token) {
			exit(0);
		}
	}				
}