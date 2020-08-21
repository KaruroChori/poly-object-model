set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(merge main.cpp)
target_link_libraries(merge ${LIBS} ${LOC_LIBS})
