/*
 * token.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Christopher Nelson
 */

#ifndef TOKEN_H_
#define TOKEN_H_

#include <memory>

#include "types.h"

namespace fusion {
namespace parser {

/** Stores token information. */
struct token {
    /** The token type. */
    unsigned char type;
    
    /** Where the token ends in the input stream. */    
    unsigned long long end_index;
                
    /** The token data (if any). */
    string data; 
};

typedef std::shared_ptr<token> token_ptr_t;   

} // end parser namespace
} // end fusion namespace


#endif /* TOKEN_H_ */
