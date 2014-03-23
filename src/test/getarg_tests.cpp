#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-WOOT");
    BOOST_CHECK(GetBoolArg("-WOOT"));
    BOOST_CHECK(GetBoolArg("-WOOT", false));
    BOOST_CHECK(GetBoolArg("-WOOT", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-WOOTo"));
    BOOST_CHECK(!GetBoolArg("-WOOTo", false));
    BOOST_CHECK(GetBoolArg("-WOOTo", true));

    ResetArgs("-WOOT=0");
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));

    ResetArgs("-WOOT=1");
    BOOST_CHECK(GetBoolArg("-WOOT"));
    BOOST_CHECK(GetBoolArg("-WOOT", false));
    BOOST_CHECK(GetBoolArg("-WOOT", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noWOOT");
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));

    ResetArgs("-noWOOT=1");
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));

    ResetArgs("-WOOT -noWOOT");  // -WOOT should win
    BOOST_CHECK(GetBoolArg("-WOOT"));
    BOOST_CHECK(GetBoolArg("-WOOT", false));
    BOOST_CHECK(GetBoolArg("-WOOT", true));

    ResetArgs("-WOOT=1 -noWOOT=1");  // -WOOT should win
    BOOST_CHECK(GetBoolArg("-WOOT"));
    BOOST_CHECK(GetBoolArg("-WOOT", false));
    BOOST_CHECK(GetBoolArg("-WOOT", true));

    ResetArgs("-WOOT=0 -noWOOT=0");  // -WOOT should win
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--WOOT=1");
    BOOST_CHECK(GetBoolArg("-WOOT"));
    BOOST_CHECK(GetBoolArg("-WOOT", false));
    BOOST_CHECK(GetBoolArg("-WOOT", true));

    ResetArgs("--noWOOT=1");
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", "eleven"), "eleven");

    ResetArgs("-WOOT -bar");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", "eleven"), "");

    ResetArgs("-WOOT=");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", "eleven"), "");

    ResetArgs("-WOOT=11");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", "eleven"), "11");

    ResetArgs("-WOOT=eleven");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-WOOT", 0), 0);

    ResetArgs("-WOOT -bar");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-WOOT=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-WOOT=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--WOOT");
    BOOST_CHECK_EQUAL(GetBoolArg("-WOOT"), true);

    ResetArgs("--WOOT=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-WOOT", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noWOOT");
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));

    ResetArgs("-noWOOT=1");
    BOOST_CHECK(!GetBoolArg("-WOOT"));
    BOOST_CHECK(!GetBoolArg("-WOOT", true));
    BOOST_CHECK(!GetBoolArg("-WOOT", false));

    ResetArgs("-noWOOT=0");
    BOOST_CHECK(GetBoolArg("-WOOT"));
    BOOST_CHECK(GetBoolArg("-WOOT", true));
    BOOST_CHECK(GetBoolArg("-WOOT", false));

    ResetArgs("-WOOT --noWOOT");
    BOOST_CHECK(GetBoolArg("-WOOT"));

    ResetArgs("-noWOOT -WOOT"); // WOOT always wins:
    BOOST_CHECK(GetBoolArg("-WOOT"));
}

BOOST_AUTO_TEST_SUITE_END()
