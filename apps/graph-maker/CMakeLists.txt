set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(graph-maker main.cpp)
target_link_libraries(graph-maker ${LIBS} ${LOC_LIBS})
