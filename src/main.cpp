/*
 * main.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: Christopher Nelson
 */

#include <readline/readline.h>

#include "parser/parser.h"
#include "codegen/generator.h"

int main(int argc, char **argv) {

    while(true) {
    
        auto input = readline("] ");
        
        // If we got EOF, stop looping.
        if (nullptr == input) break;

        amalgam::parser::parser p;

        auto module = p.parse(input, true);

        amalgam::codegen::generator g;

        g.generate(module, true);
        g.run();
    }
    
    return 0;
}



