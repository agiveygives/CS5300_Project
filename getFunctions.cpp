/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: getFunctions.cpp
 *  Includes:
 *    getSchema()     Success()                     buildQueryTree()
 *    getToken()      checkEnd()                    optimizeQueryTree()
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
  while(token != "SELECT" && token != "(SELECT") { // loops until it reads the first token of the queries
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
      checkEnd();
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
    checkEnd();
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
  groupBy.clear();
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

  buildRelationalAlgebra();
  cout << "Relational Algebra:\n\n" << relationalAlgebra << endl;
  cout << endl << "Query Tree:\n\n" << queryTree << endl << endl;
  cout << endl << "Optimized Query Tree:\n\n" << optimizedTree << endl << endl;

  queryNum++;

  // reset vectors and strings
  select.clear();
  project.clear();
  cartesianProduct.clear();
  groupBy.clear();
  relationalAlgebra = "";
  queryTree = "";
  optimizedTree = "";

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
  bool hasComma = false;
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
    currentStatement = GROUP;
  }
  else if(token == "BY" && currentStatement == GROUP){
    start = false;
    currentStatement = GROUP_BY;
  }
  else if(token == "HAVING"){
    start = false;
    currentStatement = HAVING;
  }
  else if(token == "UNION"){
    start = false;

    buildRelationalAlgebra();
    select.clear();
    project.clear();
    cartesianProduct.clear();
    groupBy.clear();

    secondary = true;
    relationalAlgebra += "UNION";
    queryTree += "\n\t\t^\n\t\t|\n\t\tUNION\n\t\t|\n\t\tv\n";
    optimizedTree += "\n\t\t^\n\t\t|\n\t\tUNION\n\t\t|\n\t\tv\n";

    currentStatement = UNION;
  }
  else if(token == "INTERSECT"){
    start = false;

    buildRelationalAlgebra();
    select.clear();
    project.clear();
    cartesianProduct.clear();
    groupBy.clear();

    secondary = true;
    relationalAlgebra += "INTERSECT";
    queryTree += "\n\t\t^\n\t\t|\n\t\tINTERSECT\n\t\t|\n\t\tv\n";
    optimizedTree += "\n\t\t^\n\t\t|\n\t\tINTERSECT\n\t\t|\n\t\tv\n";

    currentStatement = INTERSECT;
  }
  else if(token == "EXCEPT"){
    start = false;

    buildRelationalAlgebra();
    select.clear();
    project.clear();
    cartesianProduct.clear();
    groupBy.clear();

    secondary = true;
    relationalAlgebra += " - ";
    queryTree += "\n\t\t^\n\t\t|\n\t\tDIFFERENCE\n\t\t|\n\t\tv\n";
    optimizedTree += "\n\t\t^\n\t\t|\n\t\tDIFFERENCE\n\t\t|\n\t\tv\n";

    currentStatement = EXCEPT;
  }
  else if(token == "NOT"){
    start = false;
    currentStatement = IN_NOT;
  }
  else if(token == "IN" /*|| token == "EXISTS"*/){
    start = false;
    if(currentStatement == IN_NOT)
      select.push_back("!=");
    else
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
          if(token[strlen(token.c_str()) - 1] == ','){
            hasComma = true;
            token.erase(token.find(","), 1);
          }
          else
            hasComma = false;
          string lower = token;
          for (int i = 0; i < token.length(); i++)
            token[i] = toupper(token[i],loc);
          if(isTable()){
            cartesianProduct.push_back(lower);
          }
          if(hasComma)
            token += ",";
        }
        break;

      case WHERE:
        select.push_back(token);
        break;
      case GROUP_BY:
        groupBy.push_back(token);
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

  if(groupBy.size() > 0){
    relationalAlgebra += "(GROUP BY(";
    for(i = 0; i < groupBy.size(); i++){
      relationalAlgebra += groupBy[i];
      if(i + 1 < groupBy.size())
        relationalAlgebra += " ";
    }

    relationalAlgebra += ")";
  }

  
  for(i = 0; i < cartesianProduct.size(); i++){
    relationalAlgebra +=  "(";
    relationalAlgebra +=  cartesianProduct[i];
    if(i+1 < cartesianProduct.size())
      relationalAlgebra +=  " x ";
    else
      for(int j = 0; j < cartesianProduct.size() - 1; j++)
        relationalAlgebra +=  ")";
  }

  relationalAlgebra +=  "))";
  if(groupBy.size() > 0){
    relationalAlgebra += ")";
  }

  buildQueryTree();
}

/*  constructs the query tree string that will be printed to the console by
 *  reading all the vectors into their respective function calls and drawing proper lines
 */
void buildQueryTree(){
  int i = 0;

  // First Tree
  if(!secondary){
    if(cartesianProduct.size() >= 1){
      queryTree += cartesianProduct[0];
      queryTree += "\n  |\n  |\n  V\n";
      for( i = 1; i < cartesianProduct.size(); i++){
        queryTree += "  X <------" + cartesianProduct[i];
        queryTree += "\n  |\n  |\n  V\n";
      }
    } 

    if(groupBy.size() > 0){
      queryTree += "GROUP BY(";
      for(i=0; i < groupBy.size(); i++){
        queryTree += groupBy[i];
        if(i+1 < groupBy.size())
          queryTree += " ";
      }
      queryTree +=  ")\n  |\n  |\n  V\n";
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

    optimizedQueryTree();
  }

  // Second Tree
  else {
    queryTree += "PROJECT(";
    for(i = 0; i < project.size(); i++){
      queryTree +=  project[i];
      if(i+1 < project.size())
        queryTree +=  " ";
    }
    queryTree +=  ")\n  ^\n  |\n  |\n";

    queryTree += "SELECT(";
    for(i = 0; i < select.size(); i++){
      queryTree +=  select[i];
      if(i+1 < select.size())
        queryTree +=  " ";
    }
    queryTree +=  ")\n  ^\n  |\n  |\n";

    if(groupBy.size() > 0){
      queryTree += "GROUP BY(";
      for(i=0; i < groupBy.size(); i++){
        queryTree += groupBy[i];
        if(i+1 < groupBy.size())
          queryTree += " ";
      }
      queryTree +=  ")\n  ^\n  |\n  |\n";
    }

    if(cartesianProduct.size() >= 1){
      for( i = cartesianProduct.size() - 1; i > 0; i--){
        queryTree += "  X <------" + cartesianProduct[i] + "\n  ^\n  |\n  |\n";
      }
      queryTree += cartesianProduct[0];
    }

    optimizedQueryTree(); 
    secondary = false;
  }
}

/*  Constructs an optimized query tree that will be printed to the console by
 *  reading all the vectors into their respective function calls associated with
 *  the correct table and drawing proper lines
 */
void optimizedQueryTree(){
  vector<string> tableProject;
  vector<string> usedAttributes;
  string prevToken = token;
  string prevTableToken = tableToken;
  string currentTable = "";
  int i = 0;
  bool validAttribute = false;
  string leftSide = "";
  string rightSide = "";
  string compareSign = "";
  bool compare = false;

  if(cartesianProduct.size() > 1){

    // First Tree
    if(!secondary){
      for(i = 0; i < cartesianProduct.size(); i++) {
        tableProject.clear();

        currentTable = cartesianProduct[i];
        for (int k = 0; k < currentTable.length(); k++)
          currentTable[k] = toupper(currentTable[k],loc);

        getAttributes(select, tableProject, currentTable);
        getAttributes(project, tableProject, currentTable);

        if(i == 0){
          optimizedTree += "      " + cartesianProduct[i] + "\n\t|\n\t|\n\tV\n";
        } else {
          optimizedTree += "<---";
        }

        optimizedTree += "PROJECT(";
        for(int j = 0; j < tableProject.size(); j++){
          usedAttributes.push_back(tableProject[j]);
          if(j+1 != tableProject.size())
            optimizedTree += tableProject[j] + ", ";
          else
            optimizedTree += tableProject[j] + ")";
        }

        if(i == 0){
          optimizedTree += "\n\t|\n\t|\n\tV\n";
        } else {
          optimizedTree += "<---" + cartesianProduct[i] + "\n\t|\n\t|\n\tV\n";
        }
          
        if(i + 1 != cartesianProduct.size()){
          optimizedTree += "JOIN(";
          for(int j = 0; j < select.size(); j++){
            if(select[j] == "=" || select[j] == ">" || select[j] == "<"){
              compare = true;
              compareSign = select[j];
            }
            else if(select[j] == "AND" || select[j] == "OR")
              compare = false;
            else if(!compare)
              leftSide = select[j];
            else if(compare){
              rightSide = select[j];
              token = rightSide;
              if((leftSide[0] == cartesianProduct[i][0] || leftSide[0] == cartesianProduct[i+1][0]) &&
                  (rightSide[0] == cartesianProduct[i][0] || rightSide[0] == cartesianProduct[i+1][0] || isString() || isInteger())){
                optimizedTree += " " + leftSide + " " + compareSign + " " + rightSide + " ";
                if(j+1 < select.size())
                  optimizedTree += select[j+1];
              }
            }
          }

          optimizedTree += ")";
        } else {

          if(groupBy.size() > 0){
            optimizedTree += "GROUP BY(";
            for(i=0; i < groupBy.size(); i++){
              optimizedTree += groupBy[i];
              if(i+1 < groupBy.size())
                optimizedTree += " ";
            }
            optimizedTree +=  ")\n  |\n  |\n  V\n";
          }

          optimizedTree += "PROJECT(";
          for(int j = 0; j < project.size(); j++){
            optimizedTree +=  project[j];
            if(i+1 < project.size())
              optimizedTree +=  " ";
          }
          optimizedTree +=  ")\n\n";
        }
      }
    }

    // Second Tree
    else {
      optimizedTree += "PROJECT(";
      for(int j = 0; j < project.size(); j++){
        optimizedTree +=  project[j];
        if(i+1 < project.size())
          optimizedTree +=  " ";
      }
      optimizedTree +=  ")\n\t^\n\t|\n\t|\n";

      if(groupBy.size() > 0){
        optimizedTree += "GROUP BY(";
        for(i=0; i < groupBy.size(); i++){
          optimizedTree += groupBy[i];
          if(i+1 < groupBy.size())
            optimizedTree += " ";
        }
        optimizedTree +=  ")\n  ^\n  |\n  |\n";
      }

      for(i = cartesianProduct.size() - 1; i > 0; i--){
        tableProject.clear();

        currentTable = cartesianProduct[i];
        for (int k = 0; k < currentTable.length(); k++)
          currentTable[k] = toupper(currentTable[k],loc);

        getAttributes(select, tableProject, currentTable);
        getAttributes(project, tableProject, currentTable);

        optimizedTree += "JOIN(";
        for(int j = 0; j < select.size(); j++){
          if(select[j] == "=" || select[j] == ">" || select[j] == "<"){
            compare = true;
            compareSign = select[j];
          }
          else if(select[j] == "AND" || select[j] == "OR")
            compare = false;
          else if(!compare)
            leftSide = select[j];
          else if(compare){
            rightSide = select[j];
            token = rightSide;
            //cout << endl << "LEFTSIDE: " << leftSide << "\tRIGHTSIDE: " << rightSide << endl;
            if((leftSide[0] == cartesianProduct[i][0] || leftSide[0] == cartesianProduct[i-1][0]) &&
                (rightSide[0] == cartesianProduct[i][0] || rightSide[0] == cartesianProduct[i-1][0] || isString() || isInteger())){
              optimizedTree += " " + leftSide + " " + compareSign + " " + rightSide + " ";
              if(j+1 < select.size())
                optimizedTree += select[j+1];
              compare = false;
            }
          }
        }

        optimizedTree += ")<---PROJECT(";
        for(int j = 0; j < tableProject.size(); j++){
          if(j+1 != tableProject.size())
            optimizedTree += tableProject[j] + ", ";
          else
            optimizedTree += tableProject[j] + ")" + "<---" + cartesianProduct[i] + "\n\t^\n\t|\n\t|\n";
        }
      }
      optimizedTree += "      " + cartesianProduct[i];
    }
  }
  else {
    //optimizedTree = queryTree;
    if(!secondary){
      optimizedTree = cartesianProduct[0] + "\n  |\n  |\n  V\n";
      tableProject.clear();

      currentTable = cartesianProduct[0];
      for (int k = 0; k < currentTable.length(); k++)
        currentTable[k] = toupper(currentTable[k],loc);

      getAttributes(select, tableProject, currentTable);
      getAttributes(project, tableProject, currentTable);

      optimizedTree += "PROJECT(";
      for(int j = 0; j < tableProject.size(); j++){
        //usedAttributes.push_back(tableProject[j]);
        if(j+1 != tableProject.size())
          optimizedTree += tableProject[j] + ", ";
        else
          optimizedTree += tableProject[j] + ")\n  |\n  |\n  V\n";
      }

      if(groupBy.size() > 0){
        optimizedTree += "GROUP BY(";
        for(i=0; i < groupBy.size(); i++){
          optimizedTree += groupBy[i];
          if(i+1 < groupBy.size())
            optimizedTree += " ";
        }
        optimizedTree +=  ")\n  |\n  |\n  V\n";
      }

      optimizedTree += "SELECT(";
      for(i = 0; i < select.size(); i++){
        optimizedTree +=  select[i];
        if(i+1 < select.size())
          optimizedTree +=  " ";
      }
      optimizedTree +=  ")\n  |\n  |\n  V\n";

      optimizedTree += "PROJECT(";
      for(i = 0; i < project.size(); i++){
        optimizedTree +=  project[i];
        if(i+1 < project.size())
          optimizedTree +=  " ";
      }
      optimizedTree +=  ")\n\n";

    }
    else {
      optimizedTree += "PROJECT(";
      for(int j = 0; j < project.size(); j++){
        optimizedTree +=  project[j];
        if(i+1 < project.size())
          optimizedTree +=  " ";
      }
      optimizedTree +=  ")\n\t^\n\t|\n\t|\n";

      optimizedTree += "SELECT(";
      for(i = 0; i < select.size(); i++){
        optimizedTree +=  select[i];
        if(i+1 < select.size())
          optimizedTree +=  " ";
      }
      optimizedTree +=  ")\n  ^\n  |\n  |\n";

      if(groupBy.size() > 0){
        optimizedTree += "GROUP BY(";
        for(i=0; i < groupBy.size(); i++){
          optimizedTree += groupBy[i];
          if(i+1 < groupBy.size())
            optimizedTree += " ";
        }
        optimizedTree +=  ")\n  ^\n  |\n  |\n";
      }

      tableProject.clear();

      currentTable = cartesianProduct[0];
      for (int k = 0; k < currentTable.length(); k++)
        currentTable[k] = toupper(currentTable[k],loc);

      getAttributes(select, tableProject, currentTable);
      getAttributes(project, tableProject, currentTable);

      optimizedTree += "PROJECT(";
      for(int j = 0; j < tableProject.size(); j++){
        //usedAttributes.push_back(tableProject[j]);
        if(j+1 != tableProject.size())
          optimizedTree += tableProject[j] + ", ";
        else
          optimizedTree += tableProject[j] + ")\n  ^\n  |\n  |\n";
      }

      optimizedTree += "      " + cartesianProduct[i];
    }

  }

  tableToken = prevTableToken;
  token = prevToken;
}

void getAttributes(vector<string> queryTokens, vector<string> &tableProject, string currentTable){
  string temp = "";
  string tempUpper = "";
  string attribute = "";

  for(int j = 0; j < queryTokens.size(); j++) {
    temp = queryTokens[j];
    if(temp[temp.length() - 1] == ','){
      temp.erase(temp.length() - 1, 1);
    }
    if(temp.find(".") < temp.length() - 1){
      attribute = temp;

      temp.erase(temp.find("."), strlen(temp.c_str()));                   // get table associated with attribute
      attribute.erase(0, attribute.find(".") + 1);                        // get attribute

      tempUpper = temp;
      for (int k = 0; k < tempUpper.length(); k++)
        tempUpper[k] = toupper(tempUpper[k],loc);

      if(currentTable != tempUpper){
        token = temp;
        if(checkAlias() && currentTable == tableToken){
          tableProject.push_back(queryTokens[j]);
        }
      } else {
        tableProject.push_back(queryTokens[j]);
      }
    } else {
      token = temp;
      for(int k = 0; k < token.length(); k++)
        token[k] = toupper(token[k],loc);

      tableToken = currentTable;

      if(isAttribute()){
        tableProject.push_back(queryTokens[j]);
      }
    }
  }
}

string toCaps(string capitalize){
  for (int k = 0; k < capitalize.length(); k++)
    capitalize[k] = toupper(capitalize[k],loc);

  return capitalize;
}

bool checkAttribute(string check, string table){
  check = toCaps(check);
  string aliasToken = check;

  if(check.find(".") < check.length()){
    aliasToken.erase(aliasToken.find("."), strlen(aliasToken.c_str()));   // get table associated with attribute
    check.erase(0, token.find(".") + 1);                                  // get attribute

    token = aliasToken;
    if(isTable() || checkAlias()) {
      token = check;
      if(isAttribute() || isString() || isInteger()){
        return true;
      }
    }
    return false;
  } else return true;
}