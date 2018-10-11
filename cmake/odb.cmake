

function(ADD_ODB_INTERFACE ODB_HEADERS)

if (ODB_USE_MYSQL)
	set(DBDRIVER mysql)
	set(ODB_EXTRA_FLAGS --mysql-engine default -DODB_USE_MYSQL=1)
else()
	set(DBDRIVER pgsql)
	set(ODB_EXTRA_FLAGS --pgsql-server-version 9.6)
endif()

if (NOT ARGN)
	message(SEND_ERROR "no header file")
endif()

get_filename_component(ODB_HEADER_BASE ${ARGV1} NAME_WE)

set(ODB_HEADER ${ARGV1})
set(OUT_DIR ${ARGV2})
set(EXTRA_ARGS ${ARG3})

set(include_params "")
foreach (include_dir ${ODB_EXTRA_INCLUDE_DIRS})
set(include_params -I ${include_dir} ${include_params})
endforeach()

if (NOT ${ODB_CROSS})

add_custom_command(OUTPUT ${OUT_DIR}/${ODB_HEADER_BASE}.sql
                          ${OUT_DIR}/${ODB_HEADER_BASE}-odb.cxx
                          ${OUT_DIR}/${ODB_HEADER_BASE}-odb.hxx
                          ${OUT_DIR}/${ODB_HEADER_BASE}-odb.ixx
                   COMMAND ${ODB_COMPILER} ARGS
                              ${EXTRA_ARGS}
                              -d ${DBDRIVER} -p boost --generate-schema --generate-query --generate-schema-only --std c++14 ${ODB_EXTRA_FLAGS}
                              ${ODB_HEADER}
                              ${include_params}
                              -I ${ODB_LIB_DIR}
                              -I ${ODB_DRIVER_LIB_DIR}
                              -I ${BOOST_INCLUDE_DIRS}
                   COMMAND ${ODB_COMPILER} ARGS ${EXTRA_ARGS} -d ${DBDRIVER} -p boost --generate-query --generate-schema
                              --schema-format embedded --std c++14 ${ODB_EXTRA_FLAGS}
                              ${ODB_HEADER}
                              ${include_params}
                              -I ${ODB_LIB_DIR}
                              -I ${ODB_DRIVER_LIB_DIR}
                              -I ${BOOST_INCLUDE_DIRS}

                   MAIN_DEPENDENCY ${ODB_HEADER}
                   DEPENDS ${ODB_HEADER}
                   WORKING_DIRECTORY ${OUT_DIR}
                   COMMENT "generating ODB bindings for ${ODB_HEADER_BASE}"
                   BYPRODUCTS
                         ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HEADER_BASE}.sql
                         ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HEADER_BASE}-odb.cxx
                         ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HEADER_BASE}-odb.hxx
                         ${CMAKE_CURRENT_BINARY_DIR}/${ODB_HEADER_BASE}-odb.ixx
                   VERBATIM)
endif()

set(${ODB_HEADERS})

set(${ODB_HEADERS}
	${OUT_DIR}/${ODB_HEADER_BASE}-odb.cxx
	${OUT_DIR}/${ODB_HEADER_BASE}-odb.hxx
	${OUT_DIR}/${ODB_HEADER_BASE}-odb.ixx PARENT_SCOPE)


endfunction(ADD_ODB_INTERFACE)

