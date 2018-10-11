// file      : odb/mysql/details/config.hxx
// copyright : Copyright (c) 2005-2015 Code Synthesis Tools CC
// license   : GNU GPL v2; see accompanying LICENSE file

#ifndef ODB_MYSQL_DETAILS_CONFIG_HXX
#define ODB_MYSQL_DETAILS_CONFIG_HXX

// no pre

#ifdef _MSC_VER
#  if !defined(LIBODB_MYSQL_INCLUDE_SHORT) && !defined (LIBODB_MYSQL_INCLUDE_LONG)
#    define LIBODB_MYSQL_INCLUDE_SHORT 1
#  endif
#elif defined(ODB_COMPILER)
#  error libodb-mysql header included in odb-compiled header
#else
#define LIBODB_MYSQL_STATIC_LIB
#define LIBODB_MYSQL_INCLUDE_LONG
#      define ODB_CXX11_DELETED_FUNCTION
#      define ODB_CXX11_EXPLICIT_CONVERSION_OPERATOR
#      define ODB_CXX11_FUNCTION_TEMPLATE_DEFAULT_ARGUMENT
#      define ODB_CXX11_VARIADIC_TEMPLATE
#      define ODB_CXX11_INITIALIZER_LIST
#endif

// no post

#endif // ODB_MYSQL_DETAILS_CONFIG_HXX
