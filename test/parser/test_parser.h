/*
 * test_parser.h
 *
 *  Created on: May 10, 2012
 *      Author: cnelson
 */

#ifndef TEST_PARSER_H_
#define TEST_PARSER_H_

#include "parser/parser.h"

TEST(ParserTest, CanCreateParser) {
   ASSERT_NO_THROW(new amalgam::parser::parser());
}

TEST(ParserTest, Integer) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("10"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, HexInteger) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("10h"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, OctInteger) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("10o"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, BinaryInteger) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("10b"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, IntegerExpression) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("10+5"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, IntegerParentheticalExpression) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("5+(6*10)"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, IntegerAllOperatorExpression) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("1+2*3/4&5|6^7%8<<9>>10"));
   EXPECT_TRUE(m!=nullptr);
}

TEST(ParserTest, Identifier) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("an_ident"));
   EXPECT_TRUE(m!=nullptr);
}


#endif /* TEST_PARSER_H_ */
