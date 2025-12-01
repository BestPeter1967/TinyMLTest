/* ==========================================================================
 * @(#)File: TSUnit.cpp
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
#include "tsunit/TSUnit.hpp"
#include <cstring>
#include <stdarg.h>
#include <cstdlib>
#include <cstdio>
#include <cstdint>

#if defined(CROSS_BUILD) && defined(__ARM_EABI__)
    #include "infrastructure/target/arm/SEGGER_RTT/RTT/SEGGER_RTT.h"
#endif

namespace tsunit {
    Statistics _totalStatistics;

static const char* const _repeatString(unsigned int inRepeatCount, char inRepeatChar)
{
    static char sBuffer[80];
    if (inRepeatCount > sizeof(sBuffer) - 1)
    {
        inRepeatCount = sizeof(sBuffer) - 1;
    }

    for (unsigned int i = 0; i < inRepeatCount; ++i)
    {
        sBuffer[i] = inRepeatChar;
    }
    sBuffer[inRepeatCount] = '\0';

    return sBuffer;
}

class CCommonConsoleLogging : public ILogger
{
private:
    enum struct TestResult
    {
        PASSED, FAILED, RUNNING
    };

    TestResult _testResult = TestResult::FAILED;
public:
    CCommonConsoleLogging() = default;
    virtual ~CCommonConsoleLogging() = default;

    virtual void reportIntro() override
    {
        log("%s\n", _repeatString(80, '=') );
        log("Report of %s\n", tsunit::kVersionString);
        log("%s\n", _repeatString(80, '=') );
    }

    virtual void issueTestRun(const TestListEntry& inTestListEntry) override
    {
        _testResult = TestResult::RUNNING;
        signed int fillerStringSize = 59 - int(strlen(inTestListEntry.groupName)) -
            int(strlen(inTestListEntry.testCaseName));
        if (fillerStringSize < 3)
        {
            fillerStringSize = 3;
        }

        log("Running %s::%s %s ", inTestListEntry.groupName, inTestListEntry.testCaseName,  _repeatString(fillerStringSize, '.'));
    }

    virtual void reportPassed() override
    {
        if ( TestResult::RUNNING == _testResult )
        {
            log(ESC_COLOR_GREEN "[PASSED]" ESC_COLOR_RESET "\n");
            _testResult = TestResult::PASSED;
        }
    }

    virtual void reportFailed() override
    {
        if ( TestResult::RUNNING == _testResult )
        {
            log(ESC_COLOR_RED "[FAILED]" ESC_COLOR_RESET "\n");
            _testResult = TestResult::FAILED;
        }
    }

    virtual void reportResults() override
    {
        log("%s\n", _repeatString(80, '=') );
        log("= Finished all Tests: Run %d Tests, %d passed, %d failed.\n"
            , _totalStatistics.runTestsCnt()
            , _totalStatistics.passedTestsCnt()
            , _totalStatistics.failedTestsCnt()
            );
        log("= %d assertions in total, %d failed of these.\n"
            , _totalStatistics.assertionsCnt()
            , _totalStatistics.assertionsFailedCnt()
            );
        log("%s\n", _repeatString(80, '=') );
    }
}; // class CCommonConsoleLogging : public ILogger

#if defined(CROSS_BUILD) && defined(__ARM_EABI__)
class SeggerRttLogger : public CCommonConsoleLogging
{
private:
public:
    SeggerRttLogger() = default;
    virtual ~SeggerRttLogger() = default;

    virtual void log(const char* fmt, ...) override
    {
        va_list list;
        va_start(list, fmt);
        SEGGER_RTT_vprintf(0, fmt, &list);
    }

    virtual void reportResults() override
    {
        CCommonConsoleLogging::reportResults();
        SEGGER_RTT_printf(0, "*STOP*\n");
    }
}; // class SeggerRttLogger : public CCommonConsoleLogging
#else
class CPrintfLogger : public CCommonConsoleLogging
{
public:
    CPrintfLogger() = default;
    virtual ~CPrintfLogger() = default;

    virtual void log(const char* fmt, ...) override
    {
        va_list list;
        va_start(list, fmt);
        vfprintf(stderr, fmt, list);
    }
}; // class CPrintfLogger : public ILogger
#endif

ILogger* pLogger = nullptr;
const TestListEntry* pCurrentEntry;

// class TestCaseRegistrar - public
void TestCaseRegistrar::push(const TestListEntry& inEntry)
{
    _unittests.push_back(inEntry);
}

static void _runTests()
{
    for (const TestListEntry& entry : TestCaseRegistrar::sharedInstance().unittests())
    {
        pCurrentEntry = &entry;

        _totalStatistics.incRunTestsCnt();
        const auto oldFailCnt = _totalStatistics.assertionsFailedCnt();
        pLogger->issueTestRun(entry);
        entry.testFunct();
        if (oldFailCnt == _totalStatistics.assertionsFailedCnt())
        {
            pLogger->reportPassed();
        }
        else
        {
            _totalStatistics.incFailedTestsCnt();
            pLogger->reportFailed();
        }
    }
}
} // namespace tsunit


int tsunit::runUnitTests(int argc, char* argv[])
{

#if defined(CROSS_BUILD) && defined(__ARM_EABI__)
    SEGGER_RTT_ConfigUpBuffer(0, NULL, NULL, 0, SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL);
    SEGGER_RTT_SetTerminal(0);
#endif

    const bool ownLogger = (nullptr == tsunit::pLogger);
    if (ownLogger)
    {
    #if defined(CROSS_BUILD) && defined(__ARM_EABI__)
        static tsunit::SeggerRttLogger logger;
    #else
        static tsunit::CPrintfLogger logger;
    #endif
        tsunit::pLogger = &logger;
    }

    /* Clear the statistic collected so far... */
    _totalStatistics.clear();

    if (tsunit::pLogger)
    {
        tsunit::pLogger->reportIntro();
        tsunit::_runTests();
    }
    tsunit::pLogger->reportResults();
    return tsunit::_totalStatistics.failedTestsCnt() ? EXIT_FAILURE : EXIT_SUCCESS;
}

#if !defined(UNITTEST_AS_LIBCALL)
/* ========================================================================== *
 * Main entry
 * ========================================================================== */
int main(int argc, char* argv[])
{
    return tsunit::runUnitTests(argc, argv);
}
#endif
