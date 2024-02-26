cmake_minimum_required (VERSION 3.20.0) 

set(CMAKE_CXX_STANDARD 17)

find_package(Threads REQUIRED)
add_executable (main mymain.cpp)
target_link_libraries(main PRIVATE ${CMAKE_THREAD_LIBS_INIT})