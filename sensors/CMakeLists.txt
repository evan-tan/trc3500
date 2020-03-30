cmake_minimum_required (VERSION 2.8) 
project (sensors) 
find_package(OpenCV REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

include_directories (include src) 
add_executable (tuning src/tuning.cpp) 
target_link_libraries(tuning ${OpenCV_LIBS}) 
