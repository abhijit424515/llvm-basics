add_executable (calc calc.cpp codegen.cpp lexer.cpp parser.cpp sema.cpp)
target_link_libraries(calc PRIVATE ${llvm_libs})