#ifndef PROTOTYPES_H
#define PROTOTYPES_H
#include "objects.h"

//// Function Prototypes: /////////////////////////////////////////

void getSchema();

//function to use std input to set a value for token
void getToken();

// parsing functions
bool parse_query();
bool parse_attributes();
bool parse_tables();
bool parse_schema(const dataType data);

//quit function
void quit(string error);

#endif