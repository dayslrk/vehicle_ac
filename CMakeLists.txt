PROJECT(vSomeIPExample LANGUAGES CXX C)
CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

set (CMAKE_CXX_FLAGS "-g -pthread -std=c++11")

find_package (vsomeip3 REQUIRED)
set(BOOST_ROOT /home/dayslrk/boost_1_66_0/install)
find_package( Boost 1.55 COMPONENTS system thread log REQUIRED )

include_directories (
    ${Boost_INCLUDE_DIR}
    ${VSOMEIP_INCLUDE_DIRS}
)

add_executable(service-example service-example.cpp ABAC.cpp Attribute.cpp)

add_executable(client-example client-example.cpp ABAC.cpp Attribute.cpp)

target_link_libraries(service-example vsomeip3 ${Boost_LIBRARIES})
target_link_libraries(client-example vsomeip3 ${Boost_LIBRARIES})
