#pragma once
/* ==========================================================================
 * @(#)File: TSUnit.hpp
 * Created: 2023-02-01
 * --------------------------------------------------------------------------
 *  (c)1982-2024 Tangerine-Software
 *
 *       Hans-Peter Beständig
 *       Kühbachstr. 8
 *       81543 München
 *       GERMANY
 *
 *       mailto:hdusel@tangerine-soft.de
 *       http://hdusel.tangerine-soft.de
 * --------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * ========================================================================== */
#include <list>
#include <string>

namespace tsunit {

const char* const kVersionString = "TSUnit V2.3.3";

void _cntAssertionDone();
void _cntAssertionFailed();
void _markFailed();
void _cntRun();

class Statistics {
public:
    Statistics() = default;

    // ==================================================================
    // Query the counters
    // ==================================================================
    bool allPassed() const {return 0 == _failedTestsCnt;}

    void clear()
    {
        _runTestsCnt = 0;
        _failedTestsCnt = 0;
        _assertionsCnt = 0;
        _assertionsFailedCnt = 0;
    }

    unsigned int failedTestsCnt() const
    {
        return _failedTestsCnt;
    }

    unsigned int passedTestsCnt() const
    {
        return _runTestsCnt -  _failedTestsCnt;
    }

    unsigned int runTestsCnt() const
    {
        return _runTestsCnt;
    }

    unsigned int assertionsCnt() const
    {
        return _assertionsCnt;
    }

    unsigned int assertionsFailedCnt() const
    {
        return _assertionsFailedCnt;
    }

    // ==================================================================
    // Increase the counters
    // ==================================================================
    void incRunTestsCnt()
    {
        ++_runTestsCnt;
    }

    void incFailedTestsCnt()
    {
        ++_failedTestsCnt;
    }

    void incAssertionsCnt()
    {
        ++_assertionsCnt;
    }

    void incAssertionFailedCnt()
    {
        ++_assertionsFailedCnt;
    }

private:
    unsigned int _runTestsCnt = 0;
    unsigned int _failedTestsCnt = 0;
    unsigned int _assertionsCnt = 0;
    unsigned int _assertionsFailedCnt = 0;

    friend void _cntAssertionDone();
    friend void _cntAssertionFailed();
    friend void _markFailed();
    friend void _cntRun();

}; // class Statistics

extern Statistics _totalStatistics;

static Statistics& totalStatistics()
{
    return _totalStatistics;
}

#if defined(UT_USE_COLORED_OUTPUT)
	#define ESC_COLOR_RED   "\x1b[31m"
	#define ESC_COLOR_GREEN "\x1b[32m"
	#define ESC_COLOR_RESET "\x1b[0m"
#else
	#define ESC_COLOR_RED   ""
	#define ESC_COLOR_GREEN ""
	#define ESC_COLOR_RESET ""
#endif


struct TestListEntry {
    const char* const groupName;
    const char* const testCaseName;
    void(*testFunct)(void);
};

using TestList = std::list<TestListEntry>;

class TestCaseRegistrar
{
public:
    void push(const TestListEntry& inEntry);
    const TestList& unittests() const {
        return _unittests; }
    static TestCaseRegistrar& sharedInstance() {
        static TestCaseRegistrar singleton;
        return singleton;
    }

private:
    TestCaseRegistrar() = default;
    TestList _unittests;
}; // class TestCaseRegistrar

#define TESTNAME(groupname,testcase) groupname##_TC_##testcase

/*
 * A test Fixture
 */
class Test
{
public:
    Test() = default;
    virtual ~Test() noexcept = default;

    virtual void SetUp() {}
    virtual void TearDown() {}

protected:
    virtual void _runTest() = 0;
}; // class Test

class TestFixture
{
public:
    TestFixture(const char* const inGroupName, const char* const inTestCaseName, void(*inTestFunction)(void))
    {
        TestCaseRegistrar::sharedInstance().push(TestListEntry{inGroupName, inTestCaseName, inTestFunction});
    }
}; // class TestFixture

#define TSUNIT_TESTF(FixtureClass,Testname)\
    class Ext_##FixtureClass_##Testname : public FixtureClass {\
    public:\
        Ext_##FixtureClass_##Testname() = default;\
        virtual ~Ext_##FixtureClass_##Testname() = default;\
        static void runTest() {\
            Ext_##FixtureClass_##Testname testcase;\
            testcase.SetUp();\
            testcase._runTest();\
            testcase.TearDown();\
            \
        }\
    protected:\
        virtual void _runTest() override;\
    };\
    tsunit::TestFixture testCase_##Testname(#FixtureClass, #Testname, Ext_##FixtureClass_##Testname::runTest);\
    void Ext_##FixtureClass_##Testname::_runTest()

// Common Tests
class TestCase
{
public:
    TestCase(const char* const inGroupName, const char* const inTestCaseName, void(*inTestFunction)(void))
    {
        TestCaseRegistrar::sharedInstance().push(TestListEntry{inGroupName, inTestCaseName, inTestFunction});
    }
};

#define TSUNIT_TEST(groupname,testcase)\
extern void groupname##_TC_##testcase();\
tsunit::TestCase TR_##groupname##_TC_##testcase(#groupname, #testcase,groupname##_TC_##testcase);\
void groupname##_TC_##testcase()

class ILogger
{
public:
    ILogger() = default;
    virtual ~ILogger() = default;

    virtual void reportIntro() = 0;
    virtual void issueTestRun(const TestListEntry&) = 0;
    virtual void reportPassed() = 0;
    virtual void reportFailed() = 0;
    virtual void log(const char* fmt, ...) = 0;
    virtual void reportResults() = 0;
};

extern ILogger* pLogger;
extern const TestListEntry* pCurrentEntry;

#define UT_EXPECT_TRUE(arg) do{\
  tsunit::totalStatistics().incAssertionsCnt();\
  if ( ! (arg) ) {\
    tsunit::totalStatistics().incAssertionFailedCnt();\
    if (tsunit::pLogger){\
        tsunit::pLogger->reportFailed();\
        tsunit::pLogger->log(ESC_COLOR_RED "*** Assertion failed in %s::%s @line %d" ESC_COLOR_RESET "\n"\
            , tsunit::pCurrentEntry->groupName\
            , tsunit::pCurrentEntry->testCaseName, __LINE__);\
    }\
  }\
} while(0)

#define UT_EXPECT_FALSE(arg) do{\
  tsunit::totalStatistics().incAssertionsCnt();\
  if ( (arg) ) {\
    tsunit::totalStatistics().incAssertionFailedCnt();\
    if (tsunit::pLogger){\
        tsunit::pLogger->reportFailed();\
        tsunit::pLogger->log(ESC_COLOR_RED "*** Assertion failed in %s::%s @line %d" ESC_COLOR_RESET "\n"\
            , tsunit::pCurrentEntry->groupName\
            , tsunit::pCurrentEntry->testCaseName, __LINE__);\
    }\
  }\
} while(0)

#define UT_EXPECT_EQ(argA,argB) do{\
  tsunit::totalStatistics().incAssertionsCnt();\
  if ( (argA) != (argB) ) {\
    tsunit::totalStatistics().incAssertionFailedCnt();\
    if (tsunit::pLogger) {\
        tsunit::pLogger->reportFailed();\
        tsunit::pLogger->log(ESC_COLOR_RED "*** Assertion failed in %s::%s @line %d" ESC_COLOR_RESET "\n"\
            , tsunit::pCurrentEntry->groupName\
            , tsunit::pCurrentEntry->testCaseName, __LINE__);\
    }\
  }\
} while(0)

#define UT_EXPECT_NE(argA,argB) do{\
  tsunit::totalStatistics().incAssertionsCnt();\
  if ( (argA) == (argB) ) {\
    tsunit::totalStatistics().incAssertionFailedCnt();\
    if (tsunit::pLogger) {\
        tsunit::pLogger->reportFailed();\
        tsunit::pLogger->log(ESC_COLOR_RED "*** Assertion failed in %s::%s @line %d" ESC_COLOR_RESET "\n"\
            , tsunit::pCurrentEntry->groupName\
            , tsunit::pCurrentEntry->testCaseName, __LINE__);\
    }\
  }\
} while(0)

int runUnitTests(int argc, char* argv[]);

} // namespace tsunit
