#include "objects.h"
#include "global.h"
#include "prototypes.h"


bool isKeyword(){
  if(token=="SELECT"||token=="FROM"||token=="AS"||token=="WHERE"||token=="NOT"||
     token=="OR"||token=="AND"||token=="GROUP"||token=="BY"||token=="HAVING"||
     token=="IN"||token=="MAX"||token=="MIN"||token=="+"||token=="-"||
     token=="~"||token=="*"||token=="/"||token=="%"||token=="ALL"||token=="ANY"||
     token=="BETWEEN"||token=="EXISTS"||token=="LIKE"||token=="SOME"||token=="UNION"||
     token=="INTERSECT"||token=="EXCEPT"||token=="AVG"||token=="COUNT"||token=="SUM"||
     token=="<"||token==">"||token=="="||token==">="||token=="<="||token=="<>"||
     token=="!="||token=="!>"||token=="!<")
    return true;
  else
    return false;
}

bool isInteger(){
  int i=0, k=0, tok_size, state=1;
  char tok[50];
  
  strncpy(tok, token.c_str(), sizeof(tok)); //turn token (string) into chars, store in char array c[].
  tok[sizeof(tok)-1] = 0;
  tok_size = sizeof(tok);  
  
  while(k<tok_size){
    switch(state){
      case 1:
        if(tok[k]=='+' || tok[k]=='-')
          state=2;
        else if(tok[k]>='0' || tok[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 2:
        if(tok[k]>='0' || tok[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 3:
        if(tok[k]>='0' || tok[k]<='9')
          state=3;
        else
          state=4;
      break;
      default:        //if in state 4, failure
        return false;
    }//switch
  }//while
  if(k==tok_size){
    switch(state){
      case 3: return true;
      default: return false;
    }//switch
  }//if
}

bool isAlias(){
    int i=0, k=0, tok_size, state=1;
  char tok[50];
  
  strncpy(tok, token.c_str(), sizeof(tok)); //turn token (string) into chars, store in char array c[].
  tok[sizeof(tok)-1] = 0;
  tok_size = sizeof(tok);
  
  if(!isKeyword(token)){
    while(k<tok_size){ //for each character in tok
      switch(state){
        case 1:
          if(tok[k]>='a' && tok[k]<='z')
            state=3;
          else
            state=2;
          break;
        case 3:
          if((tok[k]>='a' && tok[k]<='z')||(tok[k]>='0' && tok[k]<='9')
            state=3;
          else
            state=2;
          break;
        default: //state 2=failure
          return false;
      }//switch
      k++;
    }//while
    if(k==tok_size){
      switch(state){
        case 3: return true;
        default: return false;
      }//switch
    }//if
  }
}

bool isTable(){ //Need to change to not being hardcoded
  if (token=="Sailors"||token=="Boats"||token=="Reserves"){
    tableToken = token;
    return true;
  }
  else
    return false;
}

bool isAttribute(){ //Need to change to not being hardcoded
  if(tableToken=="Sailors"){
    if(token=="sid"||token=="sname"||token=="rating"||token=="age")
      return true;
    else
      return false;
  } else if(tableToken=="Boats"){
    if(token=="bid"||token=="bname"||token=="color")
      return true;
    else
      return false;
  } else if(tableToken=="Reserves"){
    if(token=="sid"||token=="bid"||token=="day")
      return true;
    else
      return false;
  } else
    return false;
}

bool isString(){
  int i=0, k=0, tok_size, state=0;
  char tok[50];
  
  strncpy(tok, token.c_str(), sizeof(tok));
  tok[sizeof(tok)-1] = 0;
  tok_size = sizeof(tok);
  
  while(k<tok_size){
    switch(state){
      case 0:
        if(tok[k]=='\'')
          state=1;
        else
          state=4;
      break;
      case 1:
        if((tok[k]>='A' && tok[k]<='Z') || (tok[k]>='a' && tok[k]<='z') || (tok[k]>='0' && tok[k]<='9'))
          state=2;
        else
          state=4;
      break;
      case 2:
        if((tok[k]>='A' && tok[k]<='Z') || (tok[k]>='a' && tok[k]<='z') || (tok[k]>='0' && tok[k]<='9'))
          state=2;
        else if (tok[k]=='\'')
          state=3;
        else
          state=4;
      break;
      case 3:
        if(tok[k])
          state=4;
      break;
      default:
        return false;
    }
  }
  if(k==tok_size){
    if(state==3){
      return true;
    else
      return false;
    }
  }
}

bool isReal(){
  int i=0, k=0, tok_size, state=0;
  char tok[50];
  
  strncpy(tok, token.c_str(), sizeof(tok));
  tok[sizeof(tok)-1] = 0;
  tok_size = sizeof(tok);
  
  if(isInteger())
    return true;
  
  while(k<tok_size){
    switch(state){
      case 1:
        if(tok[k]=='+' || tok[k]=='-')
          state=2;
        else if(tok[k]>='0' || tok[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 2:
        if(tok[k]>='0' || tok[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 3:
        if(tok[k]>='0' || tok[k]<='9')
          state=3;
        else if(tok[k]=='.')
          state=5;
        else
          state=4;
      break;
      case 5:
        if(tok[k]>='0' || tok[k]<='9')
          state=6;
        else 
          state=4;
      break;
      case 6:
        if(tok[k]>='0' || tok[k]<='9')
          state=6;
        else
          state=4;
      break;
      default:        //if in state 4, failure
        return false;
    }//switch
  }//while
  if(k==tok_size){
    if(state==3||state==6){
      return true;
    else
      return false;
    }
  }//if
}

bool isDate(){
  int i=0, k=0, tok_size, state=1;
  char tok[50];
  
  strncpy(tok, token.c_str(), sizeof(tok)); //turn token (string) into chars, store in char array c[].
  tok[sizeof(tok)-1] = 0;
  tok_size = sizeof(tok);  
  
  while(k<tok_size){
    switch(state){
      case 0:
        if(tok[k]=='\'')
          state=1;
        else
          state=4;
      break;
      case 1:
        if(tok[k]=='0')
          state=2;
        else if (tok[k]=='1')
          state=3;
        else if (tok[k]>'1' && tok[k]<='9')
          state=5;
        else
          state=4;
      break;
      case 2:
        if(tok[k]>='1' && tok[k]<='9')
          state=5;
        else
          state=4;
      break;
      case 3:
        if(tok[k]=='/')
          state=6;
        else if(tok[k]>='0' && tok[k]<='2')
          state=5;
        else
          state=4;
      break;
      case 5:
        if(tok[k]=='/')
          state=6;
        else
          state=4;
      break;
      case 6:
        if(tok[k]>='4' && tok[k]<='9')
          state=10;
        else if(tok[k]=='0')
          state=7;
        else if(tok[k]=='1' || tok[k]=='2')
          state=8;
        else if(tok[k]=='3')
          state=9;
        else
          state=4;
      break;
      case 7:
        if(tok[k]>='1' && tok[k]<='9')
          state=10;
        else
          state=4;
      break;
      case 8:
        if(tok[k]>='0' && tok[k]<='9')
          state=10;
        else if(tok[k]=='/')
          state=11;
        else
          state=4;
      break;
      case 9:
        if(tok[k]>='0' && tok[k]<='1')
          state=10;
        else if(tok[k]=='/')
          state=11;
        else
          state=4;
      break;
      case 10:
        if(tok[k]=='/')
          state=11;
        else
          state=4;
      break;
      case 11:
        if(tok[k]>='0' && tok[k]<='9')
          state=12;
        else
          state=4;
      break;
      case 12:
        if(tok[k]>='0' && tok[k]<='9')
          state=13;
        else
          state=4;
      break;
      case 13:
        if(tok[k]=='\'')
          state=14;
        else
          state=4;
      break;
      case 14:
        if(tok[k])
          state=4;
      break;
      default:
        return false;
    }
  }
  if(k==tok_size){
    if(state==14){
      return true;
    else
      return false;
    }
  }
}

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
    } else quit("Error: AggregateFunction: Expecting ')'")
  } else quit("Error: AggregateFunction: Expecting '('")
}

void parse_Member(){ //NEEDS CODE! IMPORTANT!
}

void parse_Expression(){ //needs review
  if(isInteger() || isString())
    getToken();
  else if(/*Member cond*/) 
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
      parse_GroupByStatement()
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
  if(isTable()){ //!! table.*
  } //NEEDS CODE
  else if(token=="*")
    getToken();
  else if(token=="AVG" || token=="COUNT" || token=="MAX" || token=="MIN" || token=="SUM")
    AggregateFunction();
  else {
    parse_Expression();
    if(token=="AS"){
      getToken();
      if(isAlias)
        getToken();
    }
  }
  if(/*THERE'S A COMMA*/){
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
      while(/*THERE'S A COMMA*/){
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
  while (/*THERE'S A COMMA*/){
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
