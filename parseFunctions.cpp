#include "objects.h"
#include "global.h"
#include "prototypes.h"

void parse_Query() {
  if(token[0]=='('){
    if(strlen(token.c_str()) > 1){
        token.erase(0,1);
    } else {
      getToken();
    }
  }

  if(token=="SELECT") {
    parse_SelectStatement();
    if(token==";"){
      success();
    }
  } else fail("Error: Expected SELECT but found " + token);
}

void parse_BinaryOperator(){
  if(token=="+" || token=="-" || token=="*" || token=="/" || token=="%"){
    getToken();
  } else
    fail("Error: Expecting BinaryOperator");
}

void parse_BitwiseOperator(){ //need?
}

void parse_LogicalOperator(){ //need?
}

void parse_SetOperator(){
  if(token=="UNION" || token=="INTERSECT")
    getToken();
  else
    fail("Error: Expecting SetOperator");
}

void parse_AggregateOperator(){
  if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    getToken();
  else
    fail("Error: Expecting AggregateOperator");
}

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
    if(token=="*") //REVIEW this if statement
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

void parse_Member(){ //NEEDS CODE! IMPORTANT!
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

void parse_Expression(){ //needs review
  select.push_back(token);
  int tokenLength = strlen(token.c_str());

  if(isInteger() || isString()){
    getToken();
  }
  else if(token.find(".") < tokenLength){ // member
    parse_Member();
  } 
  else if(isAttribute()){
    getToken();
  } 
  else if(token=="("){
    getToken();
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

  select.push_back(token);
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

void parse_BinaryExpression(){
  getToken();
  parse_Expression();
}

void parse_IsExpression(){
  getToken();
  if(token=="NOT")
    getToken();
  parse_Expression();
}

void parse_BetweenExpression(){
  getToken();
  parse_Expression();
  if(token=="AND"){
    getToken();
    parse_Expression();
  } else fail("Error: BetweenExpression: Expecting 'AND'");
}

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


void parse_SelectStatement(){
  getToken();
  if(token=="DISTINCT")
    getToken();
  parse_InnerSelect();
  parse_FromStatement();
  if(token=="WHERE")
    parse_WhereStatement();
  if(token=="GROUP"){
    getToken();
    if(token=="BY")
      parse_GroupByStatement();
    else
      fail("Error: SelectStatement: GROUP: expecting 'BY'");
  }  
  if(token=="HAVING")
    parse_HavingStatement();
  
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
    else fail("Error: Expecting SELECT bu found " + token);
  }
  
  if(token=="ORDER"){
    if(token=="BY")
      parse_OrderByStatement();
    else
      fail("Error: SelectStatement: ORDER: expecting 'BY'");
  }
}

void parse_InnerSelect(){ //NEEDS WORK
  bool isComma = false;

  if(token[strlen(token.c_str())-1] == ',') {     // checks if there is a comma after the token
    token.erase(token.find(","), 1);            // erases the comma from the token so attributes can properly be found
    isComma = true;
  }

  project.push_back(token);

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

void parse_FromStatement(){
  if(token=="FROM"){
    getToken();
    parse_InnerFrom();
  } else fail("Error: expected FROM, but found " + token);
}

void parse_InnerFrom(){
  bool isComma = false;

  if(token[strlen(token.c_str())-1] == ',') {     // checks if there is a comma after the token
    token.erase(token.find(","), 1);              // erases the comma from the token so table can properly be found
    isComma = true;
  }

  if(isTable()){
    cartesianProduct.push_back(token);
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

void parse_WhereStatement(){
  getToken();
  parse_Expression();
}

void parse_GroupByStatement(){
  getToken();
  parse_Expression();
  while (token[strlen(token.c_str())-1] == ','){
    getToken();
    parse_Expression();
  }
}

void parse_HavingStatement(){
  getToken();

  if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    parse_AggregateFunction();
  else
    parse_Member();

  if(isComparison())
    if(isReal() || isString())
      getToken();
    else
      fail("Error: Expecting real or string");
  else
    fail("Error: HavingStatement: Expecting comparison operator");
}

void parse_OrderByStatement(){
  getToken();
  parse_Expression();
}
