/*
 * SoftBody_test.cpp
 *
 *  Created on: 08/ott/2013
 *      Author: marco
 */

#include "PhysicsSuiteFixture.h"

struct SoftBodyTestCaseFixture
{
	SoftBodyTestCaseFixture()
	{
	}

	~SoftBodyTestCaseFixture()
	{
	}
};

/// Physics suite
BOOST_FIXTURE_TEST_SUITE(Physics, PhysicsSuiteFixture)

/// Test cases
BOOST_AUTO_TEST_CASE(TEST)
{
	BOOST_TEST_MESSAGE("TESTING");
	BOOST_CHECK(true);
}

BOOST_AUTO_TEST_SUITE_END() // Physics suite
