set(PLJIT_SOURCES
    # add your source files here
        management/CodeManager.cpp syntax/TokenStream.cpp syntax/ParseTree.cpp management/CodeReference.cpp semantic/AST.cpp semantic/OptimizationASTVisitor.cpp semantic/EvaluationContext.cpp
        )


add_library(pljit_core ${PLJIT_SOURCES})
target_include_directories(pljit_core PUBLIC ${CMAKE_SOURCE_DIR})

add_clang_tidy_target(lint_pljit_core ${PLJIT_SOURCES})
add_dependencies(lint lint_pljit_core)

add_executable(pljit main.cpp)
target_link_libraries(pljit PUBLIC pljit_core)
