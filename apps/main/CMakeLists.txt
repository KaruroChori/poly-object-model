set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(apply-program main.cpp)
target_link_libraries(apply-program ${LIBS} ${LOC_LIBS})
