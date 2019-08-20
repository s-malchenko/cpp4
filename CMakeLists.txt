cmake_minimum_required (VERSION 3.0.0)

set(CMAKE_CXX_FLAGS "-std=c++1z -O2")

include_directories(include)
#include_directories(stats_aggregator)
#include_directories(expression)
#include_directories(textures)
#include_directories(cache)
#include_directories(raii_booking)
include_directories(transport)
include_directories(transport/test)

project (course)
add_executable(	course

                #lib/xml.cpp
                #lib/json.cpp
                #lib/ini.cpp
                #lib/geo2d.cpp

                #hash_set.cpp)
                #set_iterator_next.cpp)
                #hash_point.cpp)
                #hash_person.cpp)
                #secondary_index.cpp)
                #spendings_xml.cpp)
                #spendings_json.cpp)
                #test_ini.cpp)
                #refactoring.cpp)

                #stats_aggregator/main.cpp
                #stats_aggregator/stats_aggregator.cpp
                #stats_aggregator/stats_aggregator_test.cpp)

                #collide.cpp)
                #comment_server.cpp)
                #print_stats_2.cpp)
                #lazy_value.cpp)
                #synchronized_2.cpp)
                #concurrent_map_2.cpp)
                #polynomial.cpp)
                #object_pool.cpp)
                #zoo.cpp)
                #pipeline.cpp)

                #expression/main.cpp
                #expression/expression.cpp)

                #textures/main.cpp
                #textures/Textures.cpp
                #textures/Solution.cpp)

                #cache/main.cpp
                #cache/Solution.cpp)

                #unique_ptr.cpp)
                #raii_booking/test.cpp)
                #print_stats_testing.cpp)
                #domains.cpp)
                #test_domains.cpp)
                #budget.cpp)
                #budget_mobile.cpp)
                #budget_mobile_legacy.cpp)

                transport/main.cpp
                transport/bus_stop.cpp
                transport/input_parser.cpp
                transport/transport_processor.cpp
                transport/test/bus_route_test.cpp
                transport/test/bus_stop_test.cpp
                transport/test/input_parser_test.cpp
                transport/test/transport_processor_test.cpp
                transport/test/transport_test.cpp)
