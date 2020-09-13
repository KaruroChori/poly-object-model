add_executable(graph-runtime-checks main.cpp)
target_link_libraries(graph-runtime-checks ${LIBS} ${LOC_LIBS})
add_test(NAME graph-runtime-checks COMMAND graph-runtime-checks)
