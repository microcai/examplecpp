cmake_minimum_required(VERSION 3.3)

set(LIBODB_SOURCES
	callback.cxx
	exceptions.cxx
	database.cxx
	vector-impl.cxx
	connection.cxx
	lazy-ptr-impl.cxx
	prepared-query.cxx
	query-dynamic.cxx
	result.cxx
	schema-catalog.cxx
	section.cxx
	session.cxx
	statement.cxx
	statement-processing.cxx
	tracer.cxx
	transaction.cxx
	details/buffer.cxx
	details/condition.cxx
	details/lock.cxx
	details/mutex.cxx
	details/shared-ptr/base.cxx
)

if (WIN32)
	list(APPEND LIBODB_SOURCES
		details/win32/init.cxx
		details/win32/once.cxx
		details/win32/tls.cxx
		details/win32/condition.cxx
		details/win32/thread.cxx
		details/win32/exceptions.cxx
		details/win32/dll.cxx)
else()
	list(APPEND LIBODB_SOURCES
		details/posix/exceptions.cxx
		details/posix/thread.cxx)
endif()

list(APPEND LIBODB_SOURCES
	boost/date-time/exceptions.cxx)

add_library(libodb STATIC ${LIBODB_SOURCES})

target_include_directories(libodb PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_compile_definitions(libodb PUBLIC -DLIBODB_STATIC_LIB -DLIBODB_BOOST_STATIC_LIB)
target_link_libraries(libodb Boost::date_time)
set_target_properties(libodb PROPERTIES FOLDER "third_party")
