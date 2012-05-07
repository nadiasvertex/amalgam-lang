/*
 * main.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: Christopher Nelson
 */

#include <readline/readline.h>

#include "parser/parser.h"

int main(int argc, char **argv) {

    while(true) {
    
        auto input = readline("] ");
        
        // If we got EOF, stop looping.
        if (nullptr == input) break;

        amalgam::parser::parser p;

        p.parse(input);
    }
    
    return 0;
}



