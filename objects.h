#ifndef OBJECTS_H
#define OBJECTS_H

#include <iostream>
#include <string>
#include <cstring>
#include <algorithm>
#include <locale>

using namespace std;

/*	Linked List data structure to store table name, attribute name, attribute type, and table renaming
 *	the pointer will point to the next attribute in the schema loaded from the input file before the queries
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

/*	This class will be used to store items to build our relational algebra expression
 */
class relationalAlgebra {

	public:

};

/*	This class will be used to build and display our query tree
 */
class queryTree {

	public:

};

enum dataType {
	tableName,
	alias,
	attributeName,
	attributeType
};

#endif