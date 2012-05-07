/*
 * source.h
 *
 *  Created on: Mar 30, 2012
 *      Author: cnelson
 */

#ifndef SOURCE_H_
#define SOURCE_H_

#include "memo.h"

#include <fstream>
#include <map>
#include <sstream>
#include <tuple>

namespace amalgam {
namespace parser {

/** Associates all the aspects of a source file together. */
class source {
    /** The type that associates line and column information together. */
    typedef std::tuple<uint32_t, uint32_t> location_t;

    /** The type that associates file index with line number and column information. */
    typedef std::map<uint64_t, location_t > location_map_t;

    /** The type that keeps track of the input stream. */
    typedef std::unique_ptr<std::istream> stream_ptr_t;

    location_map_t loc;

    /** The filename of the source file. */
    string filename;

    /** The current line number. */
    uint32_t line;

    /** The current column. */
    uint32_t col;

    /** An stream object for source input. */
    stream_ptr_t input;
    
public:
    source():line(1), col(1) {}

    /** Opens the source file.
     *
     * @param filename: The filename to open. Must exist! */
    void open(string _filename) {
       input = stream_ptr_t(new std::ifstream(_filename));
       filename = _filename;
    }

    /** Converts the given buffer to a stream and sets it as the input.
     *
     * @param filename: The string containing the source buffer. */
    void load(string buffer) {
       input = stream_ptr_t(new std::stringstream(buffer, std::stringstream::in));
       filename = "<string>";
    }

    /** Sets a mark at the current location and returns the location.
     *
     * @returns: A 64-bit value indicating where we are in the file. */
    auto mark() -> uint64_t {
       auto pos = input->tellg();

       loc[pos] = std::make_tuple(line, col);

       return pos;
    }

    /** Provides the location information that corresponds to an absolute position in the file.
     *
     * @param pos: The absolute position in the file.
     * @returns: The line and column data associated with this position, or an empty tuple. */
    auto get_location(uint64_t pos) -> location_t {
       if (loc.find(pos) == loc.end()) {
          return location_t();
       }

       return loc[pos];
    }

    /** Consumes a character from the input stream.
     *
     * @returns: A tuple of (worked, character). The character may or may not be an
     * 8-bit entity. On versions of the compiler supporting wide characters it will
     * very likely not be.
     * */
    auto consume() -> std::tuple<bool, string::value_type> {
       string::value_type c;

       if (!input->get(c)) {
          return std::make_tuple(false, c);
       }

       // Handle detection of line and column.
       if (c=='\n') {
          ++line;
          col=1;
       } else {
          ++col;
       }

       return std::make_tuple(true, c);
    }

    /** Provides a peek at what the next character in the stream will be if it is consumed.
     *
     * @returns: If the EOF is reached, this function returns istream::traits::eof(), otherwise it
     * returns the next character that would be read. */
    auto peek() -> int {
       return input->peek();
    }

    /** Resets the input stream to an earlier spot. This spot must have been recorded by using "mark()".
     * If mark() was not called, this function will fail.
     *
     * @param pos: The absolute position in the file to seek to.
     * @returns: true on success, false on failure.
     */
    auto backtrack(uint64_t pos) -> bool {
       if (loc.find(pos) == loc.end()) {
          return false;
       }

       input->seekg(pos);

       std::tie(line, col) = loc[pos];

       return true;
    }

};
    

} // end parser namespace
} // end fusion namespace


#endif /* SOURCE_H_ */
