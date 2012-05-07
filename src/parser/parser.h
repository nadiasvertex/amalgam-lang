/*
 * parser.h
 *
 *  Created on: Mar 30, 2012
 *      Author: Christopher Nelson
 */

#ifndef PARSER_H_
#define PARSER_H_

#include <functional>

#include "source.h"

namespace fusion {
namespace parser {

/** Parses fusion code into ast objects. */
class parser {
   struct context {
      bool parsed;

      /** The memo object for this context. */
      memo m;

      /** The source file for this context. */
      source s;

      /** The AST tree as of now. */
      ast_ptr_t tree;
   };

   template<typename F>
   static void literal_digits(context &ctx, F next) {
      // Parse out a literal integer.

      string number;
      string::value_type c;

      auto start = ctx.s.mark();

      while(true) {
         bool cont = true;
         c = ctx.s.peek();

         switch(c) {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
               ctx.s.consume();
               number.push_back(c);
               break;
            default:
               cont = false;
               break;
         }

         if (!cont) break;
      }

      if (number.size() > 0) {
         auto end = ctx.s.mark();

         auto node = ast_ptr_t();
         node->type = node_type::literal_int;
         node->data = number;
         node->start_pos = start;
         node->end_pos = end;

         ctx.tree = node;
         ctx.m.store(start, node);
      }

      // Handle the next parse operation.
      next(ctx);
   }

   template<typename F>
   static void literal_spec(context &ctx, F next) {
      next(ctx);
   }

   template<typename F>
   static void literal_int(context &ctx, F next) {
      auto literal_digits_tail = [&next] (context &_ctx) {
         literal_spec(_ctx, next);
      };

      literal_digits(ctx, literal_digits_tail);
   }

   template<typename F>
   static void start(context &ctx, F next) {
      literal_int(ctx, next);
   }

   static void endmark(context &ctx) {
      ctx.parsed = true;
   }

   void parse() {
      context ctx;

      start(ctx, endmark);
   }
    
};
    

} // end parser namespace
} // end fusion namespace

#endif /* PARSER_H_ */
