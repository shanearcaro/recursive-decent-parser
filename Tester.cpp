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

bool g_valCheck = false;

int main(int argc, char** argv) {
    if (argc == 2) {
        std::fstream file(argv[1]);
        int lineNumber = 1;

        if (file.is_open()) {
            bool errorsFound = Prog(file, lineNumber);
            std::cout << std::endl;
            if (errorsFound) {
                std::cout << "Unsuccessful Parsing" << std::endl;
                std::cout << std::endl;
                std::cout << "Number of Syntax Errors: " << error_count << std::endl;
            }
            else
                std::cout << "Successful Parsing" << std::endl;
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

/**
 * Starts Recursive Decent Parser 
 */
bool Prog(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    bool errorsFound = false;

    if (item.GetToken() != BEGIN) {
        ParseError(line, "No Begin Token");
        Parser::PushBackToken(item);
        errorsFound = true;
    }

    if (StmtList(in, line))
        errorsFound = true;

    return errorsFound;   
}

/**
 * Language is one long Statement List broken up into smaller statements
 */
bool StmtList(istream& in, int& line) {
    LexItem item;
    bool errorsFound = false;

    while (true) {
        item = Parser::GetNextToken(in, line);
        if (item.GetToken() == DONE || item.GetToken() == END)
            return errorsFound;
        else
            Parser::PushBackToken(item);
        if (Stmt(in, line)) {
            // ParseError(line, "Invalid Statement Expression");
            errorsFound = true;
        }
    }
    return errorsFound;
}

/**
 * A statement can be either Print, If, Assign or an error
 */
bool Stmt(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    Parser::PushBackToken(item);

    switch (item.GetToken()) {
        case PRINT:
            return PrintStmt(in, line);
        case IF:
            return IfStmt(in, line);
        case IDENT:
            return AssignStmt(in, line);
        default:
            /**
             * If the input cannot be recognized read in tokens until the next
             * line is reached. If the current line is evaluated after an 
             * unrecognized input AssignStmt tends to be called causing
             * a back-to-back chain of errors.
             */
            const int currentLine = item.GetLinenum();
            while (currentLine == line) {
                item = Parser::GetNextToken(in, line);
            }
            Parser::PushBackToken(item);
            ParseError(currentLine, "Unrecognized Input");
            return true;
    }
    return false;
}

/**
 * Print statements
 * Need to check for variable assignment
 */
bool PrintStmt(istream& in, int& line) {
    const int lineNumber = line;
    g_valCheck = true;
    // Increase line count if print statement is incorrect
    if (ExprList(in, line)) {
        ParseError(lineNumber, "Invalid Print Statement Expression");
        line++;
        return true;
    }
    return false;
}

/**
 * If statements a little more complex
 * Still need to check for variable assignment
 */
bool IfStmt(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    bool errorsFound = false;
    g_valCheck = true;

    if (item.GetToken() != IF) {
        ParseError(line, "Missing If Statement Expression");
        errorsFound = true;
    }

    item = Parser::GetNextToken(in, line);
    if (item.GetToken() != LPAREN) {
        ParseError(line, "Missing Left Parenthesis");
        errorsFound = true;
    }

    if (Expr(in, line)) {
        // ParseError(line, "Invalid if statement Expression");
        errorsFound = true;
    }

    item = Parser::GetNextToken(in, line);
    if (item.GetToken() != RPAREN) {
        ParseError(line, "Missing Right Parenthesis");
        errorsFound = true;
    }

    item = Parser::GetNextToken(in, line);
    if (item.GetToken() != THEN) {
        ParseError(line, "Missing THEN");
        errorsFound = true;
    }

    if (Stmt(in, line)) {
        ParseError(line, "Invalid If Statement Expression");
        errorsFound = true;
    }
    return errorsFound;
}

/**
 * Assign statement just assigns variable
 * Don't need to check for variable assignment because
 * that's what this is doing
 */
bool AssignStmt(istream& in, int& line) {
    const int currentLine = line;
    bool errorsFound = false;
    if (Var(in, line)) {
        ParseError(currentLine, "Invalid Assignment Statement Identifier");
        errorsFound = true;
    }
        
    LexItem item = Parser::GetNextToken(in, line);
    if (item.GetToken() != EQ) {
        ParseError(currentLine, "Missing Assignment Operator =");
        errorsFound = true;
    }

    if (Expr(in, line)) {
        ParseError(currentLine, "Invalid Assignment Statement Expression");
        errorsFound = true;
    }
    item = Parser::GetNextToken(in, line);
    if (item.GetToken() != SCOMA) {
        Parser::PushBackToken(item);
        ParseError(currentLine, "Missing Semicolon");
        errorsFound = true;
    }
    return errorsFound; 
}

/**
 * Expression list can only be called from print statement
 * Breaks into multiple expression statements
 */
bool ExprList(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    bool errorsFound = false;

    if (item.GetToken() != PRINT) {
        // ParseError(line, "Invalid Print Statement Expression");
        errorsFound = true;
    }

    while (true) {
        if (Expr(in, line)) {
            // ParseError(line, "Invalid Print Statement Expression List");
            return true;
        }
        item = Parser::GetNextToken(in, line);

        if (item.GetToken() != COMA) {
            if (item.GetToken() == SCOMA)
                return errorsFound;
            Parser::PushBackToken(item);
            return errorsFound;
        }
    }
    return errorsFound; 
}

/**
 * Basic level of expression with + and - operations
 */
bool Expr(istream& in, int& line) {
    bool errorsFound = false;
    while (true) {
        if (Term(in, line)) {
            // ParseError(line, "Invalid Term Expression");
            return true;
        }

        LexItem item = Parser::GetNextToken(in, line);
        if (item.GetToken() == ERR)
            errorsFound = true;
        else if (item.GetToken() != PLUS && item.GetToken() != MINUS) {
            Parser::PushBackToken(item);
            return errorsFound;
        }
    }
    return errorsFound;  
}

/**
 * Higher Precedence expression with * and / 
 */
bool Term(istream& in, int& line) {
    bool errorsFound = false;
    while (true) {
        if (Factor(in, line)) {
            // ParseError(line, "Invalid Factor Expression");
            return true;
        }

        LexItem item = Parser::GetNextToken(in, line);
        if (item.GetToken() == ERR)
            errorsFound = true;
        else if (item.GetToken() != MULT && item.GetToken() != DIV) {
            Parser::PushBackToken(item);
            return errorsFound;
        }
    }
    return errorsFound; 
}

/**
 * Used to check for variable assignment
 * g_valCheck is used with statements to check
 * if value is defined or not 
 */
bool Var(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    if (item.GetToken() == IDENT) {
        if (defVar.find(item.GetLexeme()) != defVar.end()) {
            defVar[item.GetLexeme()] = false;
            g_valCheck = false;
        }
        else {
            if (g_valCheck) {
                ParseError(line, "Undefined variable used in expression");
                g_valCheck = false;
                return true;
            }
            defVar[item.GetLexeme()] = true;
        }
    }
    else {
        ParseError(line, "Invalid Identifier Expression");
        g_valCheck = false;
        return true;
    }
    return false;
}

/**
 * Most basic level of expression - breaks into basic components
 * of the grammar 
 */
bool Factor(istream& in, int& line) {
    LexItem item = Parser::GetNextToken(in, line);
    bool errorsFound = false;
    const int lineNumber = item.GetLinenum();
    switch (item.GetToken()) {
        case ERR:
            item = Parser::GetNextToken(in, line);
            if (item.GetLinenum() == lineNumber)
                line++;
            Parser::PushBackToken(item);
            g_valCheck = false;
            return true;
        case IDENT:
            Parser::PushBackToken(item);
            return Var(in, line);
        case ICONST:
            g_valCheck = false;
            return false;
        case RCONST:
            g_valCheck = false;
            return false;
        case SCONST:
            g_valCheck = false;
            return false;
        case LPAREN:
            g_valCheck = false;
            if (Expr(in, line)) {
                // ParseError(line, "Invalid Expression Statement");
                errorsFound = true;
            }
            item = Parser::GetNextToken(in, line);
            if (item.GetToken() == RPAREN)
                return errorsFound;
            else {
                Parser::PushBackToken(item);
                ParseError(line, "Missing Right Parenthesis");
                return true;
            }
        default:
            return true;
    }
}
