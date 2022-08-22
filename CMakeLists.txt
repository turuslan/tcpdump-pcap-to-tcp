cmake_minimum_required(VERSION 3.15)

project(tcpdump-pcap-to-tcp)

set(CMAKE_CXX_STANDARD 17)

include(pcapxx.cmake)

add_executable(tcpdump-pcap-to-tcp main.cpp)
target_link_libraries(tcpdump-pcap-to-tcp pcapxx)
