#include "objects.h"
#include "global.h"
#include "prototypes.h"

void checkAlias(){ //RETURN TO THIS LATER?
}

void parse_BinaryOperator(){
  if(token=="+" || token=="-" || token=="*" || token=="/" || token=="%"){
    getToken();
  } else
    quit("Error: Expecting BinaryOperator");
}

void parse_BitwiseOperator(){ //need?
}

void parse_LogicalOperator(){ //need?
}

void parse_SetOperator(){
  if(token=="UNION" || token=="INTERSECT")
    getToken();
  else
    quit("Error: Expecting SetOperator");
}

void parse_AggregateOperator(){
  if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    getToken();
  else
    quit("Error: Expecting AggregateOperator");
}

void parse_Comparison(){ 
  if(token=="<"||token==">"||token=="="||token==">="||token=="<="||token=="<>"||
     token=="!="||token=="!>"||token=="!<")
    getToken();
  else
    quit("Error: Expecting Comparison");
}

void parse_AggregateFunction(){
  parse_AggregateOperator();
  if(token=="("){
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
    } else quit("Error: AggregateFunction: Expecting ')'");
  } else quit("Error: AggregateFunction: Expecting '('");
}

void parse_Member(){ //NEEDS CODE! IMPORTANT!
  schemaLL *runner = &schema;
  string temp = "";
  tableToken = token;

  tableToken.erase(tableToken.find("."), strlen(tableToken.c_str()));   // get table associated with attribute
  token.erase(0, token.find(".") + 1);                        // get attribute

  temp = token;
  token = tableToken;
  if(isTable() || isAlias()) {
    token = temp;
    if(isAttribute()){
      tableToken = "";
      return;
    } else quit("Error: " + token + " is not an attribute of relation " + tableToken);
  } else quit("Error: " + token + " is not a relation in the schema");
}

void parse_Expression(){ //needs review
  if(isInteger() || isString())
    getToken();
  else if(token.find(".") < strlen(token.c_str())) // member
    parse_Member();
  else if(token=="("){
    getToken();
    parse_Expression();
    while(token==","){
      getToken();
      parse_Expression();
    }
    if(token==")")
      getToken();
    else quit("Error: Expression: Expecting ')'");
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
          quit("Error: parse_Expression: expecting ')'");
      }
    }else 
      parse_Expression();
  }else{ //EXPRESSION BEFORE OTHER TOKEN(S)
    parse_Expression();
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
    else if(token=="IN")
      parse_InExpression();
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
  } else quit("Error: BetweenExpression: Expecting 'AND'");
}

void parse_InExpression(){
  getToken();
  if(token=="("){
    getToken();
    if(token=="SELECT")
      parse_SelectStatement();
    else
      parse_Expression();
    if(token==")")
      getToken();
    else
      quit("Error: InExpression: Expecting ')'");
  } else if (isTable())
    getToken();
  else quit("Error: InExpression: Expecting Destination of IN");
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
    if(token=="BY")
      parse_GroupByStatement();
    else
      quit("Error: SelectStatement: GROUP: expecting 'BY'");
  }  
  if(token=="HAVING")
    parse_HavingStatement();
  
  while(token=="UNION" || token=="INTERSECT"){
    getToken();
    parse_SelectStatement();
  }
  
  if(token=="ORDER"){
    if(token=="BY")
      parse_OrderByStatement();
    else
      quit("Error: SelectStatement: ORDER: expecting 'BY'");
  }
}

void parse_InnerSelect(){ //NEEDS WORK
  bool isComma = false;

  if(token[strlen(token.c_str())] == ',') {     // checks if there is a comma after the token
    token.erase(token.find("."), 1);            // erases the comma from the token so attributes can properly be found
    isComma = true;
  }

  if(token.find(".") < strlen(token.c_str())) {   // check if attribute includes a '.' indicating that a table is specified
    schemaLL *runner = &schema;
    string table = token;

    table.erase(table.find("."), strlen(table.c_str()));  // get table associated with attribute
    token.erase(0, token.find(".") + 1);                  // get attribute

    if(token != "*") {
      string temp = token;
      token = table;
      if(isTable()){
        token = temp;
        if(isAttribute())
          getToken();
      }
    } else {
      token = table;
      if(isTable())
        getToken();
    }
  } 
  else if(token=="*")
    getToken();
  else if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    parse_AggregateFunction();
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
  }
}

void parse_FromStatement(){
  getToken();
  parse_InnerFrom();
}

void parse_InnerFrom(){
  if(isTable()){
    getToken();
    if(token=="AS"){
      getToken();
      if(isAlias())
        getToken();
    } else quit("Error: FromStatement: InnerFrom: AS: expecting alias");
  }
  else if(token=="("){
    getToken();
    if(token=="SELECT"){
      parse_SelectStatement();
      if(token=="AS"){
        getToken();
        if(isAlias())
          getToken();
      } else quit("Error: FromStatement: InnerFrom: AS: expecting alias");
    }
    else {
      parse_InnerFrom();
      while(token[strlen(token.c_str())] == ','){
        getToken();
        parse_InnerFrom();
      }
    }
    if(token==")")
      getToken();
    else quit("Error: Expecting ')'");
  }
}

void parse_WhereStatement(){
  getToken();
  parse_Expression();
}

void parse_GroupByStatement(){
  getToken();
  parse_Expression();
  while (token[strlen(token.c_str())] == ','){
    getToken();
    parse_Expression();
  }
}

void parse_HavingStatement(){
  getToken();
  if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    parse_AggregateFunction();
  else
    parse_Expression();
  parse_Comparison();
  if(isReal() || isString())
    getToken();
  else
    quit("Error: Expecting real or string");
}

void parse_OrderByStatement(){
  getToken();
  parse_Expression();
}
