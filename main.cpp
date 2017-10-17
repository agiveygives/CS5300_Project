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
#include <array>

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
const string ATTRIBUTES[] = {"Fname", "Minit", "Lname"};

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

//quit function
bool forUntil(const string checkArr[]);
void quit(string error);

//// Main Method: ////////////////////////////////////////////


int main() {
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
            if (token == "SELECT") {
                getToken();
                if(parse_attributes())
                    state = 1;
            }
            
        case 1:
            if (token == "FROM") {
                getToken();
                if (parse_tables())
                    state = 2;
            }
            
        case 2:
            if (token == "WHERE") {
                getToken();
                if(parse_comparison() || parse_query()) {
                   valid = true;
                } 
            }
    }
    
    return valid;
}

bool parse_attributes() {
    bool valid = false;
    
    valid = forUntil(ATTRIBUTES);
    
    return valid;
}

bool parse_tables() {
    bool valid = false;
    
    valid = forUntil(TABLES);
    
    return valid;
}

bool parse_comparison() {
    bool valid = false;
    
    int size = ATTRIBUTES->size();
    
    for (int i = 0; i < size; i++) {
        if(token == ATTRIBUTES[i]) {
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

bool forUntil(const string checkArr[]) {
    bool valid = false;
    int size = checkArr->size();
    
    for (int i = 0; i < size; i++) {
        if (token == checkArr[i] || token == checkArr[i] + ",") {
            valid = true;
            break;
        }
    }
    return valid;
}

void quit(string error) {
    cout << error << endl;
    exit(0);
}
