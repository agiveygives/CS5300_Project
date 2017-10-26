/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: getFunctions.cpp
 *  Includes:
 *    getSchema()     Success()                     buildQueryTree()
 *    getToken()      checkEnd()
 *    setAlias()      getRelationalAlgebra()
 *    fail()          buildRelationalAlgebra()
 */


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

// Reads in the next token and converts all the characters to uppercase
void getToken() {
  prevToken = token;
  cin >> token;

  if(!failure)
    getRelationalAlgebra();
  
  for (int i = 0; i < token.length(); i++)
    token[i] = toupper(token[i],loc);
  //cout << "token: " << token << endl;
  return;
}

// sets the alias of a specified table
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

// When a query is found invalid, print an error message and getToken until the next query
void fail(string error) {
  failure = true;
  tableToken = "";    // reset tableToken variable;
  potentialAlias.clear();
  cout << "\nQuery " << queryNum << " failed\n";
  cout << "\t" << error << endl;

  select.clear();
  project.clear();
  cartesianProduct.clear();
  relationalAlgebra = "";
  queryTree = "";
  queryNum++;

  //go to the end of the query
  while(token != ";"){
    getToken();
    checkEnd();
  }

  if(token == ";"){
    getToken();
    checkEnd();
  }
  failure = false;
  parse_Query();      
}

// When a query is valid, print the relational algebra and query tree
void success() {
  tableToken = "";

  // if any aliases went undeclared
  if(potentialAlias.size() > 0){
    string error = "";
    for(int i = 0; i < potentialAlias.size(); i++){
      error += "Error: " + potentialAlias[i] + " not declared\n";
    }
    fail(error);
  }
  cout << "\nQuery " << queryNum << " was successful\n";
  queryNum++;

  buildRelationalAlgebra();
  cout << relationalAlgebra << endl;
  cout << endl << queryTree << endl << endl;

  // reset vectors and strings
  select.clear();
  project.clear();
  cartesianProduct.clear();
  relationalAlgebra = "";
  queryTree = "";

  getToken();
  checkEnd();

  parse_Query();
}

// check for the end of file
void checkEnd() {
  if(prevToken == token) {      // fix because ') )' causes this to return true
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

// gets tokens needed to build the relational algebra expression
void getRelationalAlgebra(){
  bool start = true;
  bool open = true;

  if((token == "SELECT" || token == "(SELECT") && currentStatement != IN_EXISTS){
    start = false;
    currentStatement = SELECT;
  }
  else if(token == "FROM"){
    start = false;
    currentStatement = FROM;
  }
  else if(token == "WHERE"){
    start = false;
    currentStatement = WHERE;
  }
  else if(token == "GROUP"){
    start = false;
    currentStatement = NONE;
  }
  else if(token == "UNION"){
    start = false;

    buildRelationalAlgebra();
    select.clear();
    project.clear();
    cartesianProduct.clear();
    relationalAlgebra += "UNION";
    queryTree += "\n\t\t^\n\t\t|\n\t\tUNION\n\t\t|\n\t\tv\n";

    currentStatement = UNION;
  }
  else if(token == "INTERSECT"){
    start = false;

    buildRelationalAlgebra();
    select.clear();
    project.clear();
    cartesianProduct.clear();
    relationalAlgebra += "INTERSECT";
    queryTree += "\n\t\t^\n\t\t|\n\t\tINTERSECT\n\t\t|\n\t\tv\n";

    currentStatement = INTERSECT;
  }
  else if(token == "EXCEPT"){
    start = false;

    buildRelationalAlgebra();
    select.clear();
    project.clear();
    cartesianProduct.clear();
    relationalAlgebra += " - ";
    queryTree += "\n\t\t^\n\t\t|\n\t\tDIFFERENCE\n\t\t|\n\t\tv\n";

    currentStatement = EXCEPT;
  }
  else if(token == "IN" || token == "EXISTS"){
    start = false;
    select.push_back("=");
    currentStatement = IN_EXISTS;
  }

  if(start && token != ";"){
    switch(currentStatement){
      case NONE:
        break;
      case SELECT:
        for(int i = 0; i < project.size(); i++){
          if(project[i] == token){
            open = false;
            break;
          }
        }
        if(open)
          project.push_back(token);
        break;

      case FROM:
        for(int i = 0; i < cartesianProduct.size(); i++){
          if(cartesianProduct[i] == token){
            open = false;
            break;
          }
        }
        if(open){
          string lower = token;
          for (int i = 0; i < token.length(); i++)
            token[i] = toupper(token[i],loc);
          if(isTable())
            cartesianProduct.push_back(lower);
        }
        break;

      case WHERE:
        select.push_back(token);
        break;
      case HAVING:
        select.push_back(token);
        break;
      case IN_EXISTS:
        if(token != "SELECT" && token != "(SELECT" && token != "("){
          select.push_back(token);
          select.push_back("AND");
        }
        break;
      default:
        break;
    }
  }
}

// builds the relational algebra string that will be printed to the console
void buildRelationalAlgebra(){
  int i = 0;

  relationalAlgebra +=  "(PROJECT(";
  for(i = 0; i < project.size(); i++){
    relationalAlgebra +=  project[i];
    if(i+1 < project.size())
      relationalAlgebra +=  " ";
  }
  relationalAlgebra +=  ")";

  relationalAlgebra +=  "(SELECT(";
  for(i = 0; i < select.size(); i++){
    relationalAlgebra +=  select[i];
    if(i+1 < select.size())
      relationalAlgebra +=  " ";
  }
  relationalAlgebra +=  ")";

  relationalAlgebra +=  "(";
  for(i = 0; i < cartesianProduct.size(); i++){
    relationalAlgebra +=  cartesianProduct[i];
    if(i+1 < cartesianProduct.size())
      relationalAlgebra +=  " x ";
    else
      relationalAlgebra +=  ")";
  }

  relationalAlgebra +=  "))";

  buildQueryTree();
}

// builds the query tree string that will be printed to the console
void buildQueryTree(){
  int i = 0;

  if(cartesianProduct.size() >= 1){
    queryTree += cartesianProduct[0];
    queryTree += "\n  |\n  |\n  V\n";
    for( i = 1; i < cartesianProduct.size(); i++){
      queryTree += "  X <------" + cartesianProduct[i];
      queryTree += "\n  |\n  |\n  V\n";
    }
  } 

  queryTree += "SELECT(";
  for(i = 0; i < select.size(); i++){
    queryTree +=  select[i];
    if(i+1 < select.size())
      queryTree +=  " ";
  }
  queryTree +=  ")\n  |\n  |\n  V\n";

  queryTree += "PROJECT(";
  for(i = 0; i < project.size(); i++){
    queryTree +=  project[i];
    if(i+1 < project.size())
      queryTree +=  " ";
  }
  queryTree +=  ")\n\n";
}