cmake_minimum_required(VERSION 3.10)

project(cool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_program(RE2C_EXECUTABLE
  NAMES re2c
  DOC "Path to re2c executable"
)

if(RE2C_EXECUTABLE)
  message(STATUS "Found re2c: ${RE2C_EXECUTABLE}")
else()
  message(FATAL_ERROR "re2c not found - please install re2c")
endif()

set(RE2C_COOL_SCANNER_OUTPUT_SOURCE ${CMAKE_SOURCE_DIR}/src/cool-lex.gen.cc)

add_custom_command(
  OUTPUT ${RE2C_COOL_SCANNER_OUTPUT_SOURCE}
  COMMAND ${RE2C_EXECUTABLE} -o ${RE2C_COOL_SCANNER_OUTPUT_SOURCE} ${CMAKE_SOURCE_DIR}/src/cool-lex.re
  DEPENDS ${CMAKE_SOURCE_DIR}/src/cool-lex.re
  COMMENT "Generate cool-lex.gen.cc with re2c"
)

add_library(cool-core STATIC
  ${CMAKE_SOURCE_DIR}/src/cool-lex.cc
  ${CMAKE_SOURCE_DIR}/src/cool-lex.h
  ${CMAKE_SOURCE_DIR}/src/strtab.cc
  ${CMAKE_SOURCE_DIR}/src/strtab.h
  ${CMAKE_SOURCE_DIR}/src/utilities.cc
  ${CMAKE_SOURCE_DIR}/src/utilities.h
  ${RE2C_COOL_SCANNER_OUTPUT_SOURCE}
)
target_include_directories(cool-core PUBLIC ${CMAKE_SOURCE_DIR}/src)

add_executable(coolc ${CMAKE_SOURCE_DIR}/src/main.cc)
target_link_libraries(coolc PRIVATE cool-core)

find_package(GTest QUIET)

if(GTEST_FOUND)
  enable_testing()

  add_executable(tests
    ${CMAKE_SOURCE_DIR}/tests/test_lexer.cc
  )
  target_link_libraries(tests PRIVATE GTest::GTest GTest::Main cool-core)

  include(GoogleTest)
  gtest_discover_tests(tests)
endif()
