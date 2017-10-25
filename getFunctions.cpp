#include "objects.h"
#include "global.h"
#include "prototypes.h"

/*  This reads the schema and populates a linked list of attributes
 *  Currently not being used in the rest of the program, but it works. So, yay.
 *  Schema must be input with a space after the table name, and a space after each comma
 */
void getSchema() {
  schemaLL *table = schema;
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
        token.erase(0, 1);  // erase leading parenthesis
      }

      wholeToken = token;
      for (int i = 0; i < strlen(token.c_str()); i++) {
        if(token[i] == ':') {
          split = i;  // element that includes the ':'
        }
      }

      token.erase(split, strlen(token.c_str()));    // gets only the attribute name
      table->m_attributeName = token;
      
      wholeToken.erase(0, split + 1);   // gets the attribute type
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
          split = i;  // element that includes the ':'
        }
      }

      token.erase(split, token.find(")") + 1);    // gets only the attribute name
      table->m_attributeName = token;
      
      wholeToken.erase(0, split + 1);   // gets the attribute type
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

void setAlias(string aliasTable){
  schemaLL *runner = schema;

  for(int i = 0; i < potentialAlias.size(); i++)
    if(potentialAlias[i] == token){
      potentialAlias.erase(potentialAlias.begin()+i);
    }

  while(runner != NULL){
    if(aliasTable == runner->m_tableName){
      runner->m_alias = token;
    }
    runner = runner->m_next;
  }
}

void fail(string error) {
  tableToken = "";    // reset tableToken variable;
  potentialAlias.clear();
  cout << error << endl;

  //go to the end of the query
  while(token != ";"){
    getToken();
    checkEnd();
  }

  if(token == ";"){
    getToken();
    checkEnd();
  }
  parse_Query();      
}

void success() {
  tableToken = "";
  if(potentialAlias.size() > 0){
    for(int i = 0; i < potentialAlias.size(); i++){
      cout << "Error: " << potentialAlias[i] << " not declared\n";
    }
    fail("");
  }
  cout << "SUCCESS!\n";
  
  printRelationalAlgebra();
  printQueryTree();
  select.clear();
  project.clear();
  cartesianProduct.clear();

  getToken();
  checkEnd();

  parse_Query();
}

void checkEnd() {
  if(prevToken == token) {
    if(schema != NULL){
      schemaLL *del;
      while(schema != NULL){
        del = schema;
        schema = schema->m_next;
        delete del;
      }
    }
    exit(0);
  }
}

void printRelationalAlgebra(){
  /*int i = 0;

  cout << "(PROJECT(";
  for(i = 0; i < project.size(); i++){
    cout << project[i];
    if(i+1 < project.size())
      cout << ", ";
    else
      cout << ")";
  }

  cout << "(SELECT(";
  for(i = 0; i < select.size(); i++){
    cout << select[i];
    if(i+1 < select.size())
      cout << ", ";
    else
      cout << ")";
  }

  cout << "(";
  for(i = 0; i < cartesianProduct.size(); i++){
    cout << cartesianProduct[i];
    if(i+1 < cartesianProduct.size())
      cout << " x ";
    else
      cout << ")";
  }

  cout << "))\n";*/
}

void printQueryTree(){

}