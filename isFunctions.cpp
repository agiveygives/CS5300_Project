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

bool isInteger(){         // possibly won't return
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

  return false;     // if none of the conditions are satisfied
}

bool isAlias(){         // possibly won't return
    int i=0, k=0, tok_size, state=1;
  char tok[50];
  
  strncpy(tok, token.c_str(), sizeof(tok)); //turn token (string) into chars, store in char array c[].
  tok[sizeof(tok)-1] = 0;
  tok_size = sizeof(tok);
  
  if(!isKeyword()){
    while(k<tok_size){ //for each character in tok
      switch(state){
        case 1:
          if(tok[k]>='a' && tok[k]<='z')
            state=3;
          else
            state=2;
          break;
        case 3:
          if((tok[k]>='a' && tok[k]<='z')||(tok[k]>='0' && tok[k]<='9'))
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

  return false;     // if none of the conditions are satisfied
}

bool isTable(){ //Need to change to not being hardcoded
  bool valid = false;
  schemaLL *runner = &schema;
  
  while(runner != NULL) {
    if (token == runner->m_tableName) {
      tableToken = token;
      valid = true;
      break;
    }
    runner = runner->m_next;
  }
  
  return valid;

  /*if (token=="Sailors"||token=="Boats"||token=="Reserves"){
    tableToken = token;
    return true;
  }
  else
    return false;*/
}

bool isAttribute(){ //Need to change to not being hardcoded
  bool valid = false;
  schemaLL *runner = &schema;

    while(runner != NULL) {
      if(tableToken == runner->m_tableName) {
        if(token == runner->m_attributeName) {
          valid = true;
          break;
        }
      }
      runner = runner->m_next;
    }

  return valid;

  /*if(tableToken=="Sailors"){
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
    return false;*/
}

bool isString(){         // possibly won't return
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
    if(state==3)
      return true;
    else
      return false;
  }

  return false;     // if none of the conditions are satisfied
}

bool isReal(){         // possibly won't return
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
    if(state==3||state==6)
      return true;
    else
      return false;
  }//if

  return false;     // if none of the conditions are satisfied
}

bool isDate(){         // possibly won't return
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
    if(state==14)
      return true;
    else
      return false;
  }

  return false;     // if none of the conditions are satisfied
}