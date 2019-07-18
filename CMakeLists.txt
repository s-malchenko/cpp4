cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_FLAGS "-std=c++1z -O2")

include_directories(./include)

project (course)
add_executable(	course
                #lib/xml.cpp
                lib/json.cpp
                #hash_set.cpp)
                #set_iterator_next.cpp)
                #hash_point.cpp)
                #hash_person.cpp)
                #secondary_index.cpp)
                #spendings_xml.cpp)
                spendings_json.cpp)
