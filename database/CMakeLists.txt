
ADD_ODB_INTERFACE(ODB_GENERATED ${CMAKE_CURRENT_SOURCE_DIR}/pooldb.hxx ${CMAKE_CURRENT_BINARY_DIR})

if (ODB_USE_MYSQL)
add_definitions(-DODB_USE_MYSQL=1)
include_directories(../3rd/libodb-mysql-2.4.0/)
else()
include_directories(../3rd/libodb-pgsql-2.4.0/)
endif()

include_directories(../3rd/libodb-2.4.0)


add_library(database-interface STATIC ${ODB_GENERATED}
	database.cpp database.hpp my-traits.hxx)

target_link_libraries(database-interface utility beautylog Boost::system Boost::thread Boost::program_options Boost::regex Boost::locale Boost::exception
	libodb ${CMAKE_THREAD_LIBS_INIT})
if (ODB_USE_MYSQL)
target_link_libraries(database-interface libodb-mysql mysqlclient)
else()
target_link_libraries(database-interface libodb-pgsql ${POSTGRESQL_LIBRARIES})
endif()

target_include_directories(database-interface PUBLIC ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(database-interface  PROPERTIES FOLDER "libs")
