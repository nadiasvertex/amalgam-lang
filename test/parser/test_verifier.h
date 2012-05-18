/*
 * test_verifier.h
 *
 *  Created on: May 10, 2012
 *      Author: cnelson
 */

#ifndef TEST_VERIFIER_H_
#define TEST_VERIFIER_H_

#include "parser/verifier.h"

TEST(VerifierTest, CanCreate) {
   ASSERT_NO_THROW(new amalgam::parser::verifier());
}

TEST(VerifierTest, Identifier) {
   amalgam::parser::parser p;
   amalgam::parser::module_ptr_t m;

   ASSERT_NO_THROW(m = p.parse("an_ident := 5"));
   EXPECT_TRUE(m!=nullptr);
   EXPECT_TRUE(m->get_method("__default__")->has_variable("an_ident"));
}


#endif /* TEST_VERIFIER_H_ */
