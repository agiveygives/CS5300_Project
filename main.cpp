//////////////////////////////////////
// Emma Williston and Andrew Givens //
// CS 5300 - Project 1              //
// SQL Compiler                     //
// 26 October 2017                  //
//////////////////////////////////////


#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <vector>
#include <sstream>

using namespace std;


//global variables
string token;
string query;
const string END = "END";
const string QUERY_FILE = "query.txt";
const int COMPARE_SIZE = 6;
const string COMPARE[COMPARE_SIZE] = {"=", ">", ">=", "<", "<=", "<>"};
const string TABLES[] = {"Employee", "Department", "Works_on"};
vector<string> tables (TABLES, TABLES + sizeof(TABLES) / sizeof(TABLES[0]) );
const string ATTRIBUTES[] = {"Fname", "Minit", "Lname"};
vector<string> attributes (ATTRIBUTES, ATTRIBUTES + sizeof(ATTRIBUTES) / sizeof(ATTRIBUTES[0]) );

//// Function Prototypes: /////////////////////////////////////////

// function to read in Query input
void readInFile();
void readQuery();

//function to use std input to set a value for token
void getToken();

// parsing functions
bool parse_query();
bool parse_attributes();
bool parse_tables();
bool parse_comparison();
bool parse_nestedQuery();

//quit function
void quit(string error);

//// Main Method: ////////////////////////////////////////////


int main(/*int argc, char* argv[]*/) {
    //readInFile();
    
    getToken();
    if(parse_query())
        cout << "CORRECT" << endl;
    else
        cout << "INVALID" << endl;
    
    return 0;
}

//// Function Declarations: /////////////////////////////////

//void readInFile() {
//    string queryLine;
//    ofstream queryFile;
//    queryFile.open(QUERY_FILE);
//    
//    if (queryFile.is_open()) {
//        while (getline(cin, queryLine)) {
//            if (queryLine == "END") break;
//            queryFile.write(queryLine.c_str(), queryLine.length());
//        }
//    
//        queryFile.write(END.c_str(), END.length());
//        queryFile.close();
//    } else {
//        quit("\n\nERR: " + QUERY_FILE + " could not be open");
//    }
//}

void getToken() {
    cin >> token;
    return;
}

bool parse_query() {
    bool valid = false;
    
    if (token == "SELECT") {
        if(parse_attributes()) {
            getToken();
            
            if (token == "FROM") {
                if (parse_tables()) {
                    getToken();
                    
                    if (token == "WHERE") {
                        if(parse_comparison()) {
                            valid = true;
                        } else if (parse_query()) {
                            
                        } else {
                            cout << "\n\nERR: Invalid WHERE clause\n\n";
                        }
                    } else {
                        cout << "\n\nERR: missing WHERE statement\n\n";
                    }
                } else {
                    cout << "\n\nERR: Invalid FROM clause\n\n";
                }
            } else {
                cout << "\n\nERR: missing FROM statement\n\n";
            }
        } else {
            cout << "\n\nERR: Invalid SELECT clause\n\n";
        }
    } else {
        cout << "\n\nERR: missing SELECT statement\n\n";
    }
    
    return valid;
}

bool parse_attributes() {
    getToken();
    bool valid = false;
    
    valid = for_each(attributes.begin(), attributes.end(),
             [] (const string& attribute)
             {
                 bool valid = false;
                 
                 if (token == attribute) {
                     valid = true;
                 } else if (token == attribute + ",") {
                     if (parse_attributes())
                         valid = true;
                 }
                 
                 return valid;
             });
    
    return valid;
}

bool parse_tables() {
    getToken();
    bool valid = false;
    
    valid = for_each(tables.begin(), tables.end(),
                     [] (const string& table)
                     {
                         bool valid = false;
                         
                         if (token == table) {
                             valid = true;
                         } else if (token == table + ",") {
                             if (parse_tables())
                                 valid = true;
                         }
                         
                         return valid;
                     });

    
    return valid;
}

bool parse_comparison() {
    getToken();
    bool valid = false;
    
    valid = for_each(attributes.begin() , attributes.end(),
                     [] (const string& attribute)
                     {
                         bool valid = false;
                         
                         if (token == attribute) {
                             valid = true;
                         }
                         
                         return valid;
                     });
    if (valid) {
        getToken();
        
        for (int i = 0; i < COMPARE_SIZE; i++) {
            if (token == COMPARE[i]) {
                valid = true;
                break;
            } else
                valid = false;
        }
    }
    
    if (valid) {
        
    } else {
        // WHERE is not a comparison. Check for nested Query. If nested query is there, valid = true
    }
    
    return valid;
}

bool parse_nestedQuery() {
    bool valid = false;

    return valid;
}

void quit(string error) {
    cout << error << endl;
    exit(0);
}
