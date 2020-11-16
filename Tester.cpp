/*
 * File: Tester.cpp
 * Project: Assignment3
 * File Created: Friday, 13th November 2020 7:45:08 am
 * Author: Shane Arcaro (sma237@njit.edu)
 */

#include <iostream>
#include <fstream>
#include "parse.h"
#include <map>

int main(int argc, char** argv) {
    if (argc == 2) {
        std::fstream file(argv[1]);
        int lineNumber = 0;

        if (file.is_open()) {
            while (true) {
                bool errorsFound = Prog(file, lineNumber);

                if (errorsFound) {
                     std::cout << "Errors found." << std::endl;
                } 
                else {
                    std::cout << "No Errors found." << std::endl;
                }
                break;
            }
        }
        else {
            std::cout << "CANNOT OPEN FILE " << argv[1] << std::endl;
            std::exit(1);
        }
    }
    else {
        std::cout << (argc > 2 ? "ONLY ONE FILE NAME ALLOWED" : "NO FILE NAME GIVEN") << std::endl;
        std::exit(1);
    }
    return 0;
}

bool Prog(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    bool errorsFound = false;

    if (item.GetToken() != BEGIN) {
        ParseError(line, "No Begin Token");
        errorsFound = true;
    }

    if (StmtList(in, line))
        errorsFound = true;

    item = Parser::GetNextToken(in, line);

    if (item.GetToken() != END) {
        ParseError(line, "No END Token");
        errorsFound = true;
    }

    return errorsFound;   
}

bool StmtList(istream& in, int& line) {
    bool errorsFound = false;
    while (true) {
        LexItem item = Parser::GetNextToken(in, line);

        if (item.GetToken() == END) {
            Parser::PushBackToken(item);
            return errorsFound;
        }
        else if (item.GetToken() == DONE)
            return false;

        Parser::PushBackToken(item);
        if (Stmt(in, line))
            errorsFound = true;
    }
    return errorsFound;
}

bool Stmt(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    std::cout << item << std::endl;
    return false;
}

bool PrintStmt(istream& in, int& line) {
    return false;
}

bool IfStmt(istream& in, int& line) {
    return false;
}

bool AssignStmt(istream& in, int& line) {
    return false; 
}

bool ExprList(istream& in, int& line) {
    return false; 
}

bool Expr(istream& in, int& line) {
    return false; 
}

bool Term(istream& in, int& line) {
    return false; 
}

bool Var(istream& in, int& line) {
    return false;
}

bool Factor(istream& in, int& line) {
    return false; 
}
