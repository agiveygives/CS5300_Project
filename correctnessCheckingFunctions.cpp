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

bool isTable(){
  if (token=="Sailors"||token=="Boats"||token=="Reserves"){
    tableToken = token;
    return true;
  }
  else
    return false;
}

bool isAttribute(){
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
}

void checkAlias(){
}


void parse_UnaryOperator(){
}

void parse_BinaryOperator(){
}

void parse_BitwiseOperator(){
}

void parse_LogicalOperator(){
}

void parse_SetOperator(){
}

void parse_AggregateOperator(){
}

void parse_Comparison(){
}


void parse_AliasAssignment(){
}

void parse_AggregateFunction(){
}

void parse_Member(){
}


void parse_Expression(){
}

void parse_BinaryExpression(){
}

void parse_NotLikeExpression(){
}

void parse_IsExpression(){
}

void parse_BetweenExpression(){
}

void parse_InExpression(){
}

void parse_SelectStatement(){
}

void parse_InnerSelect(){
}

void parse_FromStatement(){
}

void parse_InnerFrom(){
}

void parse_WhereStatement(){
}

void parse_GroupByStatement(){
}

void parse_HavingStatement(){
}

void parse_OrderByStatement(){
}
