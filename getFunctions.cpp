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

/*  Reads the tokens until the first SELECT
 *  Builds a schemaLL of the database schema
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

      table->m_next = new schemaLL();
      table = table->m_next;
    }

    getToken();
  }
}

/*  Reads tokens from cin
 *  converts all tokens to uppercase to avoid any case sensitivity
 */
void getToken() {
  prevToken = token;
  cin >> token;

  if(!failure)
    getRelationalAlgebra();
  
  for (int i = 0; i < token.length(); i++)
    token[i] = toupper(token[i],loc);
  return;
}

/*  if token isAlias() and not checkAlias() then this will add the alias to schemaLL and 
 *    pop the alias from the potentailAlias vector
 *  setAlias must be passed a string of the table the alias is associated with
 */
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

/*  prints out which query failed and where it failed
 *  clears the relational algebra vectors, relational algebra string, and query tree string
 *  increments queryNum to indicate that the parser is moving to the next query
 *  it will then call getToken until the end of the query ";" is found while constantly checking for the end
 *  calls parse_Query()
 */
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

/*  Checks if any aliases go undeclared and will call fail if there are
 *  prints relational algebra dn query tree
 *  clears relational algebra vectors, relational algebra string, and query tree
 *  calls parse_Query() if it is not at the end of the input
 */
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

/*  checks if the prevToken == token, indicating that the input has reached the end
 *  it then deallocated all the memory used by the linked list and exits the program
 */
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

/*  runs through a state machine based of which current SQL statment is active
 *  adds the  clause of the statement to the respective vector:
 *    SELECT -> project
 *    FROM -> cartesianProduct
 *    WHERE -> select
 *  if UNION | INTERSECT | EXCEPT | CONTAINS
 *    buildRelationalAlgebra and clear vectors
 */
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

/*  constructs the relational algrebra string that will be printed to the console by
 *  reading all the vectors into their respective function calls
 */
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

/*  constructs the query tree string that will be printed to the console by
 *  reading all the vectors into their respective function calls and drawing proper lines
 */
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