cmake_minimum_required (VERSION 2.8) 
project (sensors) 
find_package(OpenCV REQUIRED)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories (include src) 
add_executable (tuning src/tuning.cpp) 
target_link_libraries(tuning ${OpenCV_LIBS}) 

include_directories (include src) 
add_executable (blob_statistics src/blob_statistics.cpp) 
target_link_libraries(blob_statistics ${OpenCV_LIBS}) 

include_directories (include src) 
add_executable (bar_code_reader src/bar_code_reader.cpp) 
target_link_libraries(bar_code_reader ${OpenCV_LIBS}) 