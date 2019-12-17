cmake_minimum_required(VERSION 3.0.0)

project(GenIR VERSION 0.1.0)

SET(CMAKE_CXX_STANDARD 17)

FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

BISON_TARGET(
    LimcParser
    src/2-parser.y 
    ${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
)

FLEX_TARGET(
    LimcScanner
    src/1-scanner.l 
    ${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp
)

ADD_FLEX_BISON_DEPENDENCY(LimcScanner LimcParser)

INCLUDE_DIRECTORIES(
    src/
    lib/
    ${CMAKE_CURRENT_BINARY_DIR}
)

file(GLOB Src src/*.h src/*.cc)
file(GLOB Lib lib/*.hpp)

add_executable(
    GenIR
    ${Src}
    ${Lib}
    ${BISON_LimcParser_OUTPUTS}
    ${FLEX_LimcScanner_OUTPUTS}
)

include(CPack)
