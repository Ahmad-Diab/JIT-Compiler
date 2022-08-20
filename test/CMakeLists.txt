set(TEST_SOURCES
    # add your source files here
    Tester.cpp
        test_syntax/TestTokenStream.cpp test_syntax/TestParseTree.cpp test_semantic/TestAST.cpp test_semantic/TestEvaluation.cpp test_semantic/TestOptimization.cpp TestPljit.cpp)

add_executable(tester ${TEST_SOURCES})
target_link_libraries(tester PUBLIC
    pljit_core
    GTest::GTest)
