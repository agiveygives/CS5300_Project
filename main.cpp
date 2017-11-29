//////////////////////////////////////
// Emma Williston and Andrew Givens //
// CS 5300 - Project 1              //
// SQL Compiler                     //
// 26 October 2017                  //
//////////////////////////////////////


#include "objects.h"
#include "global.h"
#include "prototypes.h"

//// Main Method: ////////////////////////////////////////////

string token = "";
string tableToken = token;
string prevToken = token;
string attributeType = "";
string relationalAlgebra = "";
string queryTree = "";
string optimizedTree = "";

int queryNum = 1;
bool failure = false;
bool secondary = false;

locale loc;
statement currentStatement = NONE;

schemaLL *schema = new schemaLL();
vector<string> potentialAlias;
vector<string> select;
vector<string> project;
vector<string> cartesianProduct;

int main() {
  getSchema();

  while(prevToken != token)
    parse_Query();

  if(schema != NULL){
    schemaLL *del;
    while(schema != NULL){
      del = schema;
      schema = schema->m_next;
      delete del;
    }
  }

  return 0;
}