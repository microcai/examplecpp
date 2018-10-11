cmake_minimum_required(VERSION 3.3)

set(LIBODB_MYSQL_SOURCES
	connection.cxx
	connection-factory.cxx
	database.cxx
	enum.cxx
	error.cxx
	exceptions.cxx
	prepared-query.cxx
	query.cxx
	query-dynamic.cxx
	query-const-expr.cxx
	simple-object-statements.cxx
	statement.cxx
	statements-base.cxx
	tracer.cxx
	traits.cxx
	transaction.cxx
	transaction-impl.cxx
	details/options.cxx
)

include_directories(${MYSQL_INCLUDE_DIR})
add_library(libodb-mysql STATIC ${LIBODB_MYSQL_SOURCES})
target_include_directories(libodb-mysql PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(libodb-mysql libodb ${MYSQL_LIBRARIES} -lmysqlclient)
target_compile_definitions(libodb-mysql PRIVATE -DLIBODB_MYSQL_DYNAMIC_LIB)
set_target_properties(libodb-mysql PROPERTIES FOLDER "third_party")
