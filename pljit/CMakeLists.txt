set(PLJIT_SOURCES
    # add your source files here

        CodeManager.cpp TokenStream.cpp ParseTree.cpp CodeReference.h CodeReference.cpp AST.h AST.cpp ParseTreeVisitor.h PrintParseTreeVisitor.h PrintParseTreeVisitor.cpp ASTVisitor.h PrintASTVistor.cpp PrintASTVistor.h OptimizationASTVisitor.cpp OptimizationASTVisitor.h EvaluationContext.cpp EvaluationContext.h)

add_library(pljit_core ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core)
