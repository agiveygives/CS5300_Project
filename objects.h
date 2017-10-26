/*  Andrew Givens and Emma Williston
 *  cs 5300 Project 1
 *  File: objects.h
 *  Includes:
 *      class schemaLL
 *    enum statement
 */


#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <locale>

using namespace std;

/*  Linked List data structure to store table name, attribute name, attribute type, and table renaming
 *  the pointer will point to the next attribute in the schema loaded from the input file before the queries
 */
class schemaLL {

  public:
    string m_tableName;
    string m_alias;
    string m_attributeName;
    string m_attributeType;
    schemaLL *m_next;

    schemaLL() {
      m_tableName = "";
      m_alias = "";
      m_attributeName = "";
      m_attributeType = "";
      m_next = NULL;
    }
};

// enumerator to store the current SQL statement we are in.
// Used to build relational algebra
enum statement {
  NONE,
  SELECT,
  FROM,
  WHERE,
  HAVING,
  UNION,
  INTERSECT,
  EXCEPT,
  IN_EXISTS
};

#endif