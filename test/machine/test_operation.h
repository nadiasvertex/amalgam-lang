/*
 * test_operation.h
 *
 *  Created on: Aug 14, 2012
 *      Author: cnelson
 */

#ifndef TEST_OPERATION_H_
#define TEST_OPERATION_H_

#include "machine/operation_implementation.h"

TEST(OperationTest, CanCreateLoad) {
	ASSERT_NO_THROW(amalgam::machine::load("r0", "r1"));
}

TEST(OperationTest, CanCreateConstant) {
	ASSERT_NO_THROW(amalgam::machine::constant_value<int>("r0", 10));
}

TEST(OperationTest, CanPrepareLoad) {
	amalgam::machine::templ t("load.uint32");
	auto o1 = amalgam::machine::op::ptr_t(new amalgam::machine::load("r0", "r1"));

	ASSERT_TRUE(o1->prepare(t).valid());
	ASSERT_TRUE(o1->prepare(t).get());
}

#endif /* TEST_OPERATION_H_ */
