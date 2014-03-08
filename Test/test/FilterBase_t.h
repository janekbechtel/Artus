/* Copyright (c) 2013 - All Rights Reserved
 *   Thomas Hauth  <Thomas.Hauth@cern.ch>
 *   Joram Berger  <Joram.Berger@cern.ch>
 *   Dominik Haitz <Dominik.Haitz@kit.edu>
 */

#pragma once

#include <boost/test/included/unit_test.hpp>

#include "Artus/Core/interface/FilterBase.h"
#include "Artus/Core/interface/FilterResult.h"

BOOST_AUTO_TEST_CASE( test_filter_result )
{
	FilterResult fres;

	fres.SetFilterDecision("filter1",true);
	fres.SetFilterDecision("filter_too", false);

	BOOST_CHECK( fres.GetFilterDecision("filter1") == true );
	BOOST_CHECK( fres.GetFilterDecision("filter_too") == false );

	BOOST_CHECK( fres.HasPassed() == false );
	BOOST_CHECK( fres.HasPassedIfExcludingFilter("filter_too") == true );
}
