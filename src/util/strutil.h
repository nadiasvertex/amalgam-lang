/*
 * strutil.h
 *
 *  Created on: May 11, 2012
 *      Author: cnelson
 */

#ifndef STRUTIL_H_
#define STRUTIL_H_

#include <algorithm>
#include <string>

namespace amalgam {

bool ends_with(const std::string& s, const std::string& pattern) {
    return std::mismatch(pattern.rbegin(), pattern.rend(), s.rbegin()).first != pattern.rend();
}

std::pair<std::string, std::string> split_literal_int(const std::string& token) {
    auto sep = token.find_first_not_of("0123456789");
    
    // If it's just a number, return the number and an empty string.
    if (sep == std::string::npos) {
        return std::make_pair(token, std::string());
    }
    
    auto number = token.substr(0, sep);
    auto specifier = token.substr(sep);
    
    return std::make_pair(number, specifier);
}

} // end namespace amalgam

#endif /* STRUTIL_H_ */
