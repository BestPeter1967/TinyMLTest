TESTCASE(CMath)
TESTCASE(CVector)
TESTCASE(CClassicMathDriver)


target_link_libraries(UT_CMath PRIVATE utils ${Accelerate_Fwk})
