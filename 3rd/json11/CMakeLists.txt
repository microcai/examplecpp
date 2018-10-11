cmake_minimum_required(VERSION 2.8)

option(JSON11_BUILD_TESTS "Build unit tests" OFF)
option(JSON11_ENABLE_DR1467_CANARY "Enable canary test for DR 1467" OFF)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX /usr)
endif()

add_library(json11 json11.cpp)
target_include_directories(json11 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_options(json11 PRIVATE -fPIC  -Wall)

# Set warning flags, which may vary per platform
include(CheckCXXCompilerFlag)
set(_possible_warnings_flags /W4 /WX -Wextra -Werror)
foreach(_warning_flag in ${_possible_warnings_flags})
  CHECK_CXX_COMPILER_FLAG(_warning_flag _flag_supported)
  if(${_flag_supported})
    target_compile_options(json11 PRIVATE ${_warning_flag})
  endif()
endforeach()

configure_file("json11.pc.in" "json11.pc" @ONLY)

if (JSON11_BUILD_TESTS)

  set(Boost_USE_STATIC_LIBS ON)
  find_package(Boost 1.55 REQUIRED system exception)
  target_link_libraries(json11 Boost::system Boost::exception)

  # enable test for DR1467, described here: https://llvm.org/bugs/show_bug.cgi?id=23812
  if(JSON11_ENABLE_DR1467_CANARY)
    add_definitions(-D JSON11_ENABLE_DR1467_CANARY=1)
  else()
    add_definitions(-D JSON11_ENABLE_DR1467_CANARY=0)
  endif()

  add_executable(json11_test test.cpp)
  target_link_libraries(json11_test json11)
endif()
