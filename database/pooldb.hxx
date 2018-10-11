#pragma once

#ifdef _MSC_VER
#  pragma warning(disable: 4068)
#endif // _MSC_VER

#include <string>
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <odb/core.hxx>
#include <odb/section.hxx>

#include "my-traits.hxx"

#if ODB_USE_MYSQL

#pragma db value(std::string) type("VARCHAR(255)")
#pragma db value(boost::multiprecision::cpp_dec_float_50) type("decimal(60,20)")

#else
#pragma db map type("numeric") \
               as("TEXT")                    \
               to("(?)::numeric")        \
               from("(?)::TEXT")

#pragma db value(std::string) type("TEXT")
#pragma db value(boost::multiprecision::cpp_dec_float_50) type("numeric")

#endif

#pragma db model version(1, 1, open)

// class database;
namespace db
{
	#pragma db object table("example_table")
	class example_table
	{
		friend class odb::access;

	public:
		#pragma db id
		std::string key;

		#pragma db index
		long value;
	};

} // namespace db
