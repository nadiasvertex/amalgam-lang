/*
 * test_module.h
 *
 *  Created on: May 10, 2012
 *      Author: Christopher Nelson
 */

#ifndef TEST_MODULE_H_
#define TEST_MODULE_H_

#include "parser/module.h"

TEST(ModuleTest, CanCreate) {
   ASSERT_NO_THROW(new amalgam::parser::module("test_module"));
}

TEST(ModuleTest, CanGetModuleName) {
   auto m_name = "test_module";
   auto m = new amalgam::parser::module(m_name);

   EXPECT_EQ(m_name, m->get_name());
}


#endif /* TEST_MODULE_H_ */
