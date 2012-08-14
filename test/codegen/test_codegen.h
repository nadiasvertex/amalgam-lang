/*
 * test_codegen.h
 *
 *  Created on: May 10, 2012
 *      Author: cnelson
 */

#ifndef TEST_CODEGEN_H_
#define TEST_CODEGEN_H_

#include "parser/parser.h"
#include "codegen/generator.h"

TEST(CodeGenTest, CanCreateGenerator) {
   ASSERT_NO_THROW(new amalgam::codegen::generator());
}

TEST(CodeGenTest, Integer) {
   amalgam::parser::parser p;
   amalgam::codegen::generator g;

   auto m = p.parse("10");
   ASSERT_TRUE(m!=nullptr);

   ASSERT_NO_THROW(g.generate(m));
}

TEST(CodeGenTest, IntegerExpression) {
   amalgam::parser::parser p;
   amalgam::codegen::generator g;

   auto m = p.parse("10+5");
   ASSERT_TRUE(m!=nullptr);

   ASSERT_NO_THROW(g.generate(m));
}

TEST(CodeGenTest, IntegerParentheticalExpression) {
   amalgam::parser::parser p;
   amalgam::codegen::generator g;

   auto m = p.parse("5+(6*10)");
   ASSERT_TRUE(m!=nullptr);

   ASSERT_NO_THROW(g.generate(m));
}

TEST(CodeGenTest, AllOperatorExpression) {
   amalgam::parser::parser p;
   amalgam::codegen::generator g;

   auto m = p.parse("1+2*3/4&5|6^7%8<<9>>10");
   ASSERT_TRUE(m!=nullptr);

   ASSERT_NO_THROW(g.generate(m));
}

/*TEST(CodeGenTest, Identifier) {
   amalgam::parser::parser p;
   amalgam::codegen::generator g;

   auto m = p.parse("the_ident");
   ASSERT_TRUE(m!=nullptr);

   ASSERT_NO_THROW(g.generate(m));
}*/


#endif /* TEST_CODEGEN_H_ */
