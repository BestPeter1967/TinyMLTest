TESTCASE(CLayer)
TESTCASE(CNeuronalNet)

target_link_libraries(UT_CLayer PRIVATE kilib ${EXTRA_LIBS} utils)
target_link_libraries(UT_CNeuronalNet PRIVATE kilib)
