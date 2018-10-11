cmake_minimum_required(VERSION 3.3)

set(LIBODB_PGSQL_SOURCES
	auto-handle.cxx
	connection.cxx
	connection-factory.cxx
	database.cxx
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
	details/endian-traits.cxx
	details/options.cxx
)

if (WIN32)
	list(APPEND LIBODB_PGSQL_SOURCES
		details/win32/init.cxx
		details/win32/once.cxx
		details/win32/tls.cxx
		details/win32/condition.cxx
		details/win32/thread.cxx
		details/win32/exceptions.cxx
		details/win32/dll.cxx)
else()
	list(APPEND LIBODB_PGSQL_SOURCES
		details/posix/exceptions.cxx
		details/posix/thread.cxx)
endif()

include_directories(${PostgreSQL_INCLUDE_DIR})
add_library(libodb-pgsql STATIC ${LIBODB_PGSQL_SOURCES})
target_include_directories(libodb-pgsql PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../..)
target_link_libraries(libodb-pgsql libodb ${PostgreSQL_LIBRARIES})
set_target_properties(libodb-pgsql PROPERTIES FOLDER "third_party")
