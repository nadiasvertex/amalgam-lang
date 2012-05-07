/*
 * memo.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Christopher Nelson
 */

#ifndef MEMO_H_
#define MEMO_H_

#include <map>

#include "ast.h"

namespace amalgam {
namespace parser {

/** Stores memoized tokens matched from the source data. */
class memo {    
public:
    /** Type that stores in the actual memoized ast data. */
    typedef std::map<uint64_t, ast_ptr_t> memo_map_t;
    
private:
    /** The memo. */
    memo_map_t memo;
    
public:
    /** Finds out if there is a memo associated with this location
     * in the file. 
     * 
     * @param index: The byte index into the file.
     *
     * @returns: true if there is a memo at the given index, false 
     *  otherwise.
     */
    auto has_memo(uint64_t index) -> bool {
        if (memo.find(index) == memo.end()) {
            return false;            
        }
        
        return true;
    }
    
    /** Fetches the memo associated with the requested location. You need to
     * check that there is a memo here before calling this routine in order to
     * avoid the exception that will be thrown if there is none.
     * 
     * @param index: The byte index into the file.
     *
     * @returns: A shared pointer to an ast object if there is
     *  anything memoized at the location.
     */
    inline auto load(uint64_t index) -> ast_ptr_t {
        return memo[index];
    }
    
    /** Associates a memo with the requested location. 
     * 
     * @param index: The byte index into the file.
     * @param ast: The token to store.
     */
    void store(uint64_t index, ast_ptr_t ast) {
        memo[index] = ast;
    }

};
    

} // end parser namespace
} // end fusion namespace



#endif /* MEMO_H_ */
