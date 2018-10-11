cmake_minimum_required(VERSION 3.10)
project(templateproject)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ODB_USE_MYSQL OFF)

option(Boost_USE_STATIC_LIBS "use static Boost" ON)
option(ODB_CROSS "enable this to not call odb. usefull on arm cross compile." OFF)

set(DBNAME "drpool" CACHE STRING "the db that used to store shares")

if(WIN32)
	add_definitions(-DBOOST_ALL_STATIC_LINK -DBOOST_AUTO_LINK_NOMANGLE)
	add_definitions(-DBOOST_THREAD_USE_LIB)
	add_definitions(-DBOOST_FILESYSTEM_STATIC_LINK)
	add_definitions(-DWIN32_LEAN_AND_MEAN)
	add_definitions(-DNOMINMAX)
endif()

if(WIN32)
add_subdirectory(3rd/openssl-cmake)
set(OPENSSL_LIBRARIES crypto_static eay32_static)
else()
find_package(OpenSSL REQUIRED)

add_definitions(-DHAVE_DECL_EVP_MD_CTX_NEW)

endif()

add_subdirectory(3rd/boost EXCLUDE_FROM_ALL)
include_directories(3rd/boost)
set(Boost_LIBRARIES Boost::filesystem Boost::system Boost::thread Boost::date_time Boost::program_options Boost::regex Boost::locale)
set(BOOST_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/boost)

add_subdirectory(3rd/libodb-2.4.0)

include_directories(${BOOST_INCLUDE_DIRS})

if (ODB_USE_MYSQL)
add_subdirectory(3rd/libodb-mysql-2.4.0)
set(ODB_DRIVER_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/libodb-mysql-2.4.0/)
else()
 if (WIN32)
 add_subdirectory(3rd/libpq)
 set(POSTGRESQL_LIBRARIES libpq)
 set(PostgreSQL_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/libpq/include)
 else()
 set(PostgreSQL_ADDITIONAL_VERSIONS "9.6" "9.5" "10.3" "10.4" "10.2" "10.1")
 find_package(PostgreSQL REQUIRED)
 link_directories(SYSTEM ${PostgreSQL_LIBRARY_DIR})
 endif()

set(ODB_DRIVER_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/libpq/include)
add_subdirectory(3rd/libodb-pgsql-2.4.0)
endif()


if(WIN32)
set(ODB_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/3rd/odb-2.4.0-i686-windows/bin/odb.exe)
elseif(APPLE)
set(ODB_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/3rd/odb-2.4.0-i686-macosx/bin/odb)
else()
set(ODB_COMPILER ${CMAKE_CURRENT_SOURCE_DIR}/3rd/odb-2.4.0-x86_64-linux-gnu/bin/odb)
endif()

set(ODB_EXTRA_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/3rd/json11/)
set(ODB_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/libodb-2.4.0/)

include(cmake/odb.cmake)

if(ODB_USE_MYSQL)
add_definitions(-DODB_USE_MYSQL)
endif()

add_subdirectory(3rd/json11)
add_subdirectory(beautylog)
add_subdirectory(utility)

add_subdirectory(database)

add_executable(example src/main.cpp)

target_link_libraries(example Boost::date_time Boost::fiber Boost::system Boost::thread Boost::program_options)
target_link_libraries(example database-interface)
