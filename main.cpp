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

using namespace std;


//global variables
string token;
string query;
int state = 0;
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
    
    switch(state) {
        case 0:
            if (token == "SELECT")
                if(parse_attributes())
                    state = 1;
            
        case 1:
            if (token == "FROM")
                if (parse_tables())
                    state = 2;
            
        case 2:
            if (token == "WHERE")
                if(parse_comparison())
                   valid = true;
                else if (parse_query())
                    valid = true;
    }
    
    return valid;
}

bool parse_attributes() {
    getToken();
    bool valid = false;
    
    forUntil(attributes);
    
    return valid;
}

bool parse_tables() {
    getToken();
    bool valid = false;
    
    forUntil(tables);

    
    return valid;
}

bool parse_comparison() {
    getToken();
    bool valid = false;
    
    int size = static_cast<int>(attributes.size());
    
    for (int i = 0; i < size; i++) {
        if(token == attributes.at(i)) {
            valid = true;
            break;
        }
    }
    
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

void forUntil(vector<int> &checkVector) {
    int size = static_cast<int>(checkVector.size());
    
    for (int i = 0; i < size; i++) {
        if (token == checkVector.at(i) || token == checkVector.at(i) + ",") {
            valid = true;
            break;
        }
    }
}

void quit(string error) {
    cout << error << endl;
    exit(0);
}
