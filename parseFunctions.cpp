/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: parseFunctions.cpp
 *  Includes:
 *    parseQuery()                  parse_Expression()          parse_InnerSelect()        parse_OrderByStatement()   
 *                                  parse_binaryExpression()    parse_FromStatement()
 *                                  parse_isExpression()        parse_InnerFrom()
 *    parse_AggregateOperator()     parse_betweenExpression()   parse_WhereStatement()
 *    parse_AggregateFunction()     parse_inExpression()        parse_GroupByStatement()
 *    parse_Member()                parse_SelectStatement()     parse_HavingStatement()
 */



#include "objects.h"
#include "global.h"
#include "prototypes.h"

/*  begins parsing the query by checking for any parenthesis, then checking for token == "SELECT"
 *  if SELECT is found, parse_SelectStatement() is called
 *  When select statement returns, if the token == ";" then the query was completed
 */
void parse_Query() {
  if(token[0]=='('){
    if(strlen(token.c_str()) > 1){
        token.erase(0,1);
    } else {
      getToken();
    }
  }

  if(token=="SELECT") {
    currentStatement = SELECT;
    parse_SelectStatement();
    if(token==";"){
      success();
    }
  } else fail("Error: Expected SELECT but found " + token);
}

/*  calls getToken and returns if token is an aggregate operator
 *    "AVG" || "COUNT" || "MAX" || "MIN" || "SUM"
 *  calls fail function if false
 */
void parse_AggregateOperator(){
  if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    getToken();
  else
    fail("Error: Expecting AggregateOperator");
}

/*  checks for valid aggregate function consisting of "aggregateOperator ([ALL] Expression | *) 
 */
void parse_AggregateFunction(){
  parse_AggregateOperator();
  if(token[0]=='(' && token[strlen(token.c_str())-1] == ')'){
    token.erase(0,1);
    token.erase(strlen(token.c_str())-1, 1);
    if(token == "*")
      getToken();
    else
      parse_Expression();
  }
  else if(token=="("){
    getToken();
    if(token=="ALL"){
      getToken();
      parse_Expression();
    }
    if(token=="*")
      getToken();
    else
      parse_Expression();
    if(token==")"){
      getToken();
    } else if(token=="),"){
      token.erase(0,1);
    } else fail("Error: AggregateFunction: Expecting ')'");
  } else fail("Error: AggregateFunction: Expecting '('");
}

/*  checks if the attribute is an attribute of specified table/alias
 */
void parse_Member(){
  schemaLL *runner = schema;
  string temp = "";
  tableToken = token;

  tableToken.erase(tableToken.find("."), strlen(tableToken.c_str()));   // get table associated with attribute
  token.erase(0, token.find(".") + 1);                        // get attribute

  temp = token;
  token = tableToken;
  if(isTable() || isAlias() || checkAlias()) {
    token = temp;
    if(isAttribute()){
      tableToken = "";
      getToken();
      return;
    } else fail("Error: parse_Member: " + token + " is not an attribute of relation " + tableToken);
  } else fail("Error: parse_Member: " + token + " is not a relation in the schema");
}

/*  checks for an expression of the form:
 *    Expression :=   [integer|string|real|date] | Member | [NOT] Expression | 
 *            BinaryExpression | (Expression {, Expression}) | 
 *              NotLikeExpression | Expression NOT NULL | IsExpression | 
 *            BetweenExpression | InExpression | [NOT][EXISTS](SelectStatement)
 */
void parse_Expression(){
  int tokenLength = strlen(token.c_str());

  if(isInteger() || isString() || isDate() || isReal()){
    getToken();
  }
  else if(token.find(".") < tokenLength){ // member
    parse_Member();
  } 
  else if(isAttribute()){
    getToken();
  } 
  else if(token[0]=='('){
    if(strlen(token.c_str()) > 1)
      token.erase(0,1);
    else
      getToken();

    if(token == "SELECT"){
      parse_SelectStatement();
    }
    else
      parse_Expression();
    if(token==")")
      getToken();
    else fail("Error: Expression: Expecting ')'");
  }
  else if(token=="NOT"){
    getToken();
    if(token=="EXISTS"){
      getToken();
      if(token=="("){
        getToken();
        parse_SelectStatement();
        if(token==")")
          getToken();
        else
          fail("Error: parse_Expression: expecting ')'");
      }
    }else 
      parse_Expression();
  }

  if(token=="NOT"){
    getToken();
    if(token=="NULL") //NOT NULL
      getToken();
    else if(token=="LIKE") //NOT LIKE
      parse_Expression();
    else if(token=="IN"){ //NOT IN
      parse_InExpression();
    } 
    else if(token=="BETWEEN")
      parse_BetweenExpression();
  } //END NOT
  else if(token=="+" || token=="-" || token=="*" || token=="/" || token=="%")
    parse_BinaryExpression();
  else if(token=="IS")
    parse_IsExpression();
  else if(token=="BETWEEN")
    parse_BetweenExpression();
  else if(token=="IN" || token=="EXISTS"){
    getToken();
    parse_InExpression();
  }
  else if(isComparison())
    parse_Expression();
  else if(token=="AND" || token=="OR"){
    getToken();
    parse_Expression();
  }
}

/*  checks for binary expression of the form:
 *    BinaryExpression := Expression [ + | - | * | / | % ] Expression
 */
void parse_BinaryExpression(){
  getToken();
  parse_Expression();
}

/*  checks for expression of the form:
 *    Expression IS [NOT] Expression
 */
void parse_IsExpression(){
  getToken();
  if(token=="NOT")
    getToken();
  parse_Expression();
}

/*  checks for expression of the form:
 *    Expression [NOT] BETWEEN Expression AND Expression
 */
void parse_BetweenExpression(){
  getToken();
  parse_Expression();
  if(token=="AND"){
    getToken();
    parse_Expression();
  } else fail("Error: BetweenExpression: Expecting 'AND'");
}

/*  checks for expression of the form:
 *    Expression [NOT] IN [[(SelectStatement | Expression[, Expression])] | table]
 */
void parse_InExpression(){
  if(token[0]=='('){

    if(strlen(token.c_str()) > 1)
      token.erase(0,1);
    else
      getToken();

    if(token[0]=='(')
      parse_InExpression();

    if(token=="SELECT")
      parse_SelectStatement();
    else
      parse_Expression();
    if(token==")" || token[strlen(token.c_str())-1]==')')
      getToken();
    else
      fail("Error: InExpression: Expecting ')' found " + token);
  } else if (isTable())
    getToken();
  else fail("Error: InExpression: Expecting Destination of IN");
}

/*  checks if token == "DISTINCT"
 *    calls getToken()
 *  calls parse_InnerSelect()
 *  calls parse_FromStatement()
 *  checks for where statment, group by statement, having statement, union, intersect, except, or contains
 *    will call respective function for found statements
 */
void parse_SelectStatement(){
  getToken();
  if(token=="DISTINCT")
    getToken();
  parse_InnerSelect();
  parse_FromStatement();
  if(token=="WHERE")
    parse_WhereStatement();
  if(token=="GROUP"){
    currentStatement = NONE;
    getToken();
    if(token=="BY")
      parse_GroupByStatement();
    else
      fail("Error: SelectStatement: GROUP: expecting 'BY'");
  }  
  if(token=="HAVING"){
    currentStatement = HAVING;
    parse_HavingStatement();
  }

  if(token == ")")
    return;
  
  while(token=="UNION" || token=="INTERSECT" || token == "EXCEPT" || token == "CONTAINS"){
    getToken();
    if(token[0] == '('){
      if(strlen(token.c_str()) > 1)
        token.erase(0,1);
      else
        getToken();
    }
    if(token == "SELECT"){
      parse_SelectStatement();
    } 
    else fail("Error: Expecting SELECT but found " + token);
  }
  
  if(token=="ORDER"){
    if(token=="BY")
      parse_OrderByStatement();
    else
      fail("Error: SelectStatement: ORDER: expecting 'BY'");
  }
}

/*  checks for valid select clause consisting of attributes and/or aggregate functions
 */
void parse_InnerSelect(){
  bool isComma = false;

  if(token[strlen(token.c_str())-1] == ',') {     // checks if there is a comma after the token
    token.erase(token.find(","), 1);            // erases the comma from the token so attributes can properly be found
    isComma = true;
  }


  if(isAttribute())                                       // SELECT attribute
    getToken();
  else if(token.find(".") < strlen(token.c_str())){       // SELECT table.[attribute|*]
    if(token[strlen(token.c_str())] == '*'){              // SELECT table.*
      token.erase(token.find("."), 2);
      if(isTable())
        getToken();
      else {
        bool open = true;
        if(!checkAlias())
            for(int i = 0 ; i < potentialAlias.size(); i++)
              if(potentialAlias[i] == token)
                open = false;
            if(open)
              potentialAlias.push_back(token);
      }
    } else {
      parse_Member();                                // SELECT table.attribute
    }
  } else if(token=="*")                                   // SELECT *
    getToken();
  else if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM"){
    parse_AggregateFunction();
    if(token == "AS"){
      getToken();
      if(isAlias()){
        for(int i = 0; i < potentialAlias.size(); i++)
          if(potentialAlias[i] == token){
            potentialAlias.erase(potentialAlias.begin()+i);
          }
        getToken();
      }
    }
  }
  else {
    parse_Expression();
    if(token=="AS"){
      getToken();
      if(isAlias())
        getToken();
    }
  }

  if(isComma){
    parse_InnerSelect();
  } else if(token == ",") {
    getToken();
    parse_InnerSelect();
  }
}

/*  checks if token=="FROM"
 *    calls parse_InnerFrom()
 *  else fail
 */
void parse_FromStatement(){
  if(token=="FROM"){
    currentStatement = FROM;
    getToken();
    parse_InnerFrom();
  } else fail("Error: expected FROM but found " + token);
}

/*  checks for valid from statement consisting of tables and possible renaming using the AS keyword
 */
void parse_InnerFrom(){
  bool isComma = false;

  if(token[strlen(token.c_str())-1] == ',') {     // checks if there is a comma after the token
    token.erase(token.find(","), 1);              // erases the comma from the token so table can properly be found
    isComma = true;
  }

  if(isTable()){
    tableToken = token;
    string aliasTable = token;
    getToken();   
    if(token=="AS"){
      getToken();
      if(token[strlen(token.c_str())-1] == ',') {     // checks if there is a comma after the token
        token.erase(token.find(","), 1);
        isComma = true;
      }
      if(isAlias()){
        setAlias(aliasTable);
        getToken();
      }
      else fail("Error: FromStatement: InnerFrom: AS: expecting alias");
    }
    else if(isAlias()) fail("Error: Found alias [ " + token + " ] after a table without AS statement");
  }
  else if(token[0]=='('){

    if(strlen(token.c_str()) > 1)
      token.erase(0,1);
    else
      getToken();

    if(token=="SELECT"){
      parse_SelectStatement();
      if(token=="AS"){
        getToken();
        if(isAlias()){
          getToken();
        }
      } else fail("Error: FromStatement: InnerFrom: AS: expecting alias");
    }
    else {
      parse_InnerFrom();
      while(token[strlen(token.c_str())-1] == ','){
        getToken();
        parse_InnerFrom();
      }
    }
    if(token==")")
      getToken();
    else fail("Error: Expecting ')'");
  } else if(!isTable()) fail("Error: InnerFrom: " + token + " is not a valid relation");

  if(isComma){
    parse_InnerFrom();
  } else if(token == ",") {
    getToken();
    parse_InnerFrom();
  }
}

/*  calls getToken() and parse_Expression()
 */
void parse_WhereStatement(){
  currentStatement = WHERE;
  getToken();
  parse_Expression();
}

/*  checks for valid GROUP BY clause by calling parse_Expression() until a comma is not present
 */
void parse_GroupByStatement(){
  getToken();
  parse_Expression();
  while (token[strlen(token.c_str())-1] == ','){
    getToken();
    parse_Expression();
  }
}

/*  checks for valud HAVING statement consisting of:
 *    Expression | AggregateFunction Comparison [real | string]
 */
void parse_HavingStatement(){
  getToken();

  if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    parse_AggregateFunction();
    if(isComparison())
      if(isReal() || isString())
        getToken();
      else
        fail("Error: Expecting real or string");
    else
      fail("Error: HavingStatement: Expecting comparison operator");
  else
    parse_Expression();
}

/*  checks for valid ORDER BY statement consisting of an Expression
 */
void parse_OrderByStatement(){
  getToken();
  parse_Expression();
}
