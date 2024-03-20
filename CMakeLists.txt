cmake_minimum_required(VERSION 3.5)
project(Hex)

file(
        GLOB
	source_files
	main.cpp
        src/*
)

set(externLibs
        z
        ncurses
        tinfo
)


#add_definitions(-DNDEBUG)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -O3")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -Wno-unused-result -O3")
#SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1z -Wall -O0 -Wno-unused-result -g")


add_executable (
	Hex
	${source_files}
)


target_link_libraries(Hex
    ${externLibs}
)



