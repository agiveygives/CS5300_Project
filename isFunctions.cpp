/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: isFunctions.cpp
 *  Includes:
 *    isKeyword()         checkAlias()      isReal()
 *    isComparison()      isTable()         isDate()
 *    isInteger()         isAttribute()
 *    isAlias()           isString()
 */

#include "objects.h"
#include "global.h"
#include "prototypes.h"

/*  returns true if token is a reserved word/character
 *  false if the token is not
 */
bool isKeyword(){
  if(token=="SELECT"||token=="FROM"||token=="AS"||token=="WHERE"||token=="NOT"||
     token=="OR"||token=="AND"||token=="GROUP"||token=="BY"||token=="HAVING"||
     token=="IN"||token=="MAX"||token=="MIN"||token=="+"||token=="-"||
     token=="~"||token=="*"||token=="/"||token=="%"||token=="ALL"||token=="ANY"||
     token=="BETWEEN"||token=="EXISTS"||token=="LIKE"||token=="SOME"||token=="UNION"||
     token=="INTERSECT"||token=="EXCEPT"||token=="AVG"||token=="COUNT"||token=="SUM"|| token == "CONTAINS" ||
     token=="<"||token==">"||token=="="||token==">="||token=="<="||token=="<>"||
     token=="!="||token=="!>"||token=="!<")
    return true;
  else
    return false;
}

/*  returns true if token is a comprison operator
 *  false if it is not
 */
bool isComparison(){ 
  if(token=="<"||token==">"||token=="="||token==">="||token=="<="||token=="<>"||
     token=="!="||token=="!>"||token=="!<"){
    getToken();
    return true;
  } 
  return false;
}

/*  returns true if token is a non-empty sequence of digits
 */
bool isInteger(){
  int i=0, k=0, tok_size, state=1;
  
  tok_size = strlen(token.c_str());  
  
  while(k<tok_size){
    switch(state){
      case 1:
        if(token[k]=='+' || token[k]=='-')
          state=2;
        else if(token[k]>='0' && token[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 2:
        if(token[k]>='0' && token[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 3:
        if(token[k]>='0' && token[k]<='9')
          state=3;
        else
          state=4;
      break;
      default:        //if in state 4, failure
        return false;
    }//switch
    k++;
  }//while
  if(k==tok_size && state == 3)
    if(attributeType == "INTEGER")
      return true;
    else {
      fail("Error: Expected " + attributeType);
      return false;
    }
  else 
    return false;

  return false;     // if none of the conditions are satisfied
}

/*  returns true if token is a non-empty sequence of characters beginning with a letter
 *    and if the token is not a keyword or a table
 *  The function will add the alias to the potentialAliases vector if it satisfied the conditions
 */
bool isAlias(){
  int i=0, k=0, tok_size, state=1;
  bool comma = false;

  tok_size = strlen(token.c_str());

  if(token[tok_size-1] == ','){
    comma = true;
    token.erase(tok_size-1, 1);
    tok_size--;
  }
  
  if(!isKeyword() && !isTable()){
    tableToken = "";
    while(k<tok_size){ //for each character in tok
      switch(state){
        case 1:
          if((token[k]>='a' && token[k]<='z')||(token[k]>='A' && token[k]<='Z'))
            state=3;
          else
            state=2;
          break;
        case 3:
          if((token[k]>='a' && token[k]<='z')||(token[k]>='A' && token[k]<='Z')||(token[k]>='0' && token[k]<='9'))
            state=3;
          else
            state=2;
          break;
        default: //state 2=failure
          if(comma)
            token += ",";
          return false;
      }//switch
      k++;
    }//while
    if(k==tok_size){
      switch(state){
        case 3: 
          if(!checkAlias()){
            for(int i = 0 ; i < potentialAlias.size(); i++){
              if(potentialAlias[i] == token){
                if(comma)
                  token += ",";
                return true;
              }
            }
            potentialAlias.push_back(token);
          }
          if(comma)
            token += ",";
          return true;
        default: 
          if(comma)
            token += ",";
          return false;
      }//switch
    }//if
  }
  tableToken = "";

  if(comma)
    token += ",";
  return false;     // if none of the conditions are satisfied
}

/* returns true if the alias exists in the schemaLL data structure
 */
bool checkAlias(){
  schemaLL *runner = schema;

  while(runner != NULL){
    if(runner->m_alias == token){
      tableToken = runner->m_tableName;
      return true;
    }
    runner = runner->m_next;
  }

  return false;
}

/*  returns true if the table exists in the schemaLL data structure
 */
bool isTable(){
  bool valid = false;
  schemaLL *runner = schema;
  
  while(runner != NULL) {
    if (token == runner->m_tableName) {
      tableToken = token;
      valid = true;
      break;
    }
    runner = runner->m_next;
  }
  
  return valid;
}

/*  returns true if the attribute exists in the schemaLL data structure
 *    if a table is specified, it will check if the attribute exists in that table
 */
bool isAttribute(){
  bool valid = false;
  schemaLL *runner = schema;

    while(runner != NULL) {
      if(tableToken == "") {
        if(token == runner->m_attributeName) {
          attributeType = runner->m_attributeType;
          valid = true;
          break;
        }
      } else if(tableToken == runner->m_tableName) {
        if(token == runner->m_attributeName) {
          attributeType = runner->m_attributeType;
          valid = true;
          break;
        }
      }
      runner = runner->m_next;
    }
  if(!valid)
    attributeType = "";

  return valid;
}

/*  returns true if token is a non empty set of characters beginning an ending with single quotes (')
 */
bool isString(){
  int i=0, k=0, tok_size, state=0;
  
  tok_size = strlen(token.c_str());  
  
  while(k<tok_size){
    switch(state){
      case 0:
        if(token[k]=='\'')
          state=1;
        else
          state=4;
      break;
      case 1:
        if((token[k]>='A' && token[k]<='Z') || (token[k]>='a' && token[k]<='z') || (token[k]>='0' && token[k]<='9'))
          state=2;
        else
          state=4;
      break;
      case 2:
        if((token[k]>='A' && token[k]<='Z') || (token[k]>='a' && token[k]<='z') || (token[k]>='0' && token[k]<='9'))
          state=2;
        else if (token[k]=='\'')
          state=3;
        else
          state=4;
      break;
      case 3:
        if(token[k])
          state=4;
      break;
      default:
        return false;
    }
    k++;
  }
  if(k==tok_size && state==3)
    if(attributeType == "STRING")
      return true;
    else {
      fail("Error: Expected " + attributeType);
      return false;
    }
  else
    return false;

  cout << "return false\n";
  return false;     // if none of the conditions are satisfied
}

/*  returns true for an integer, negative number, or decimal
 *    checks for an integer that can be proceeded by a '-' or followed by a '.' and another integer
 */
bool isReal(){
  int i=0, k=0, tok_size, state=0;
  
  tok_size = strlen(token.c_str());  
  
  if(isInteger())
    return true;
  
  while(k<tok_size){
    switch(state){
      case 1:
        if(token[k]=='+' || token[k]=='-')
          state=2;
        else if(token[k]>='0' || token[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 2:
        if(token[k]>='0' || token[k]<='9')
          state=3;
        else
          state=4;
      break;
      case 3:
        if(token[k]>='0' || token[k]<='9')
          state=3;
        else if(token[k]=='.')
          state=5;
        else
          state=4;
      break;
      case 5:
        if(token[k]>='0' || token[k]<='9')
          state=6;
        else 
          state=4;
      break;
      case 6:
        if(token[k]>='0' || token[k]<='9')
          state=6;
        else
          state=4;
      break;
      default:        //if in state 4, failure
        return false;
    }//switch
    k++;
  }//while
  if(k==tok_size && (state==3||state==6)){
    if(attributeType == "REAL")
      return true;
    else {
      fail("Error: Expected " + attributeType);
      return false;
    }
  } else
    return false;

  return false;     // if none of the conditions are satisfied
}

/*  returns true if token is a date enclosed in quotes of the following formats
 *      'MM/DD/YY' || 'M/D/YY' || 'M/DD/YY'
 */
bool isDate(){
  int i=0, k=0, tok_size, state=0;
  
  tok_size = strlen(token.c_str());   
  
  while(k<tok_size){
    switch(state){
      case 0:
        if(token[k]=='\'')
          state=1;
        else
          state=4;
      break;
      case 1:
        if(token[k]=='0')
          state=2;
        else if (token[k]=='1')
          state=3;
        else if (token[k]>'1' && token[k]<='9')
          state=5;
        else
          state=4;
      break;
      case 2:
        if(token[k]>='1' && token[k]<='9')
          state=5;
        else
          state=4;
      break;
      case 3:
        if(token[k]=='/')
          state=6;
        else if(token[k]>='0' && token[k]<='2')
          state=5;
        else
          state=4;
      break;
      case 5:
        if(token[k]=='/')
          state=6;
        else
          state=4;
      break;
      case 6:
        if(token[k]>='4' && token[k]<='9')
          state=10;
        else if(token[k]=='0')
          state=7;
        else if(token[k]=='1' || token[k]=='2')
          state=8;
        else if(token[k]=='3')
          state=9;
        else
          state=4;
      break;
      case 7:
        if(token[k]>='1' && token[k]<='9')
          state=10;
        else
          state=4;
      break;
      case 8:
        if(token[k]>='0' && token[k]<='9')
          state=10;
        else if(token[k]=='/')
          state=11;
        else
          state=4;
      break;
      case 9:
        if(token[k]>='0' && token[k]<='1')
          state=10;
        else if(token[k]=='/')
          state=11;
        else
          state=4;
      break;
      case 10:
        if(token[k]=='/')
          state=11;
        else
          state=4;
      break;
      case 11:
        if(token[k]>='0' && token[k]<='9')
          state=12;
        else
          state=4;
      break;
      case 12:
        if(token[k]>='0' && token[k]<='9')
          state=13;
        else
          state=4;
      break;
      case 13:
        if(token[k]=='\'')
          state=14;
        else
          state=4;
      break;
      case 14:
        if(token[k])
          state=4;
      break;
      default:
        return false;
    }
    k++;
  }
  if(k==tok_size && state == 14)
    if(attributeType == "DATE")
      return true;
    else {
      fail("Error: Expected " + attributeType);
      return false;
    }
  else
    return false;

  return false;     // if none of the conditions are satisfied
}