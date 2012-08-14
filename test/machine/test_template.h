/*
 * test_template.h
 *
 *  Created on: Aug 14, 2012
 *      Author: cnelson
 */

#ifndef TEST_TEMPLATE_H_
#define TEST_TEMPLATE_H_

#include "machine/template.h"
#include "machine/operation_implementation.h"

TEST(TemplateTest, CanCreateTemplate) {
   ASSERT_NO_THROW(new amalgam::machine::templ("load.uint32"));
}

TEST(TemplateTest, CanAddOperation) {
	amalgam::machine::templ t("load.uint32");
	ASSERT_NO_THROW(t.add_operation(
			amalgam::machine::op::ptr_t(
					new amalgam::machine::load("r0", "r1")
	        )
	));
}

TEST(TemplateTest, CanFindOperation) {
	amalgam::machine::templ t("load.uint32");
	t.add_operation(
			amalgam::machine::op::ptr_t(
					new amalgam::machine::load("r0", "r1")
	        )
	);

	ASSERT_TRUE(t.find_operation_by_output("r0").valid());
}


#endif /* TEST_TEMPLATE_H_ */
