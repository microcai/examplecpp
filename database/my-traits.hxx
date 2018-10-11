
#pragma once

#include <string>
#include <boost/multiprecision/cpp_dec_float.hpp>

#include <json11.hpp>

using json_value = json11::Json;

#ifndef ODB_USE_MYSQL
#pragma db map type("jsonb") \
               as("TEXT")                    \
               to("(?)::jsonb")        \
               from("(?)::json")

#pragma db map type("INTEGER *\\[(\\d*)\\]") \
               as("TEXT")                    \
               to("(?)::INTEGER[$1]")        \
               from("(?)::TEXT")
#endif

#ifndef ODB_COMPILER

#ifndef ODB_USE_MYSQL
#include <odb/pgsql/traits.hxx>

namespace odb
{
	namespace pgsql
	{
		template <>
		class value_traits<json_value, id_string>
		{
		public:
			typedef json_value value_type;
			typedef value_type query_type;
			typedef details::buffer image_type;

			static void set_value(json_value& v, const details::buffer& b, std::size_t n, bool is_null)
			{
				if (!is_null)
				{
					std::string err;
					v = json11::Json::parse(std::string(b.data(), n), err);
				}
			}

			static void set_image(details::buffer& b, std::size_t& n, bool& is_null, const json_value& v)
			{
				std::string i = v.dump();
				n = i.length();
				b.capacity(i.length());
				std::copy(i.begin(), i.end(), b.data());
				is_null = false;
			}
		};

		template <>
		class value_traits<std::vector<int>, id_string>
		{
		public:
			typedef std::vector<int> value_type;
			typedef value_type query_type;
			typedef details::buffer image_type;

			static void
				set_value(value_type& v,
					const details::buffer& b,
					std::size_t n,
					bool is_null)
			{
				v.clear();

				if (!is_null)
				{
					char c;
					std::istringstream is(std::string(b.data(), n));

					is >> c; // '{'

					for (c = static_cast<char> (is.peek()); c != '}'; is >> c)
					{
						v.push_back(int());
						is >> v.back();
					}
				}
			}

			static void
				set_image(details::buffer& b,
					std::size_t& n,
					bool& is_null,
					const value_type& v)
			{
				is_null = false;
				std::ostringstream os;

				os << '{';

				for (value_type::const_iterator i(v.begin()), e(v.end());
					i != e;)
				{
					os << *i;

					if (++i != e)
						os << ',';
				}

				os << '}';

				const std::string& s(os.str());
				n = s.size();

				if (n > b.capacity())
					b.capacity(n);

				std::memcpy(b.data(), s.c_str(), n);
			}
		};

		template <>
		class value_traits<::boost::multiprecision::cpp_dec_float_50, id_string>
		{
		public:
			typedef ::boost::multiprecision::cpp_dec_float_50 value_type;
			typedef value_type query_type;
			typedef details::buffer image_type;

			static void
				set_value(value_type& v,
					const details::buffer& b,
					std::size_t n,
					bool is_null)
			{
				if (!is_null)
				{
					std::string numeric_number(b.data(), n);
					v = value_type(numeric_number);
				}
			}

			static void
				set_image(details::buffer& b,
					std::size_t& n,
					bool& is_null,
					const value_type& v)
			{
				is_null = false;
				std::ostringstream os;
				os.precision(std::numeric_limits<value_type>::max_digits10);
				os << v;
				std::string s = os.str();
				n = s.size();
				if (n > b.capacity())
					b.capacity(n);
				std::memcpy(b.data(), s.c_str(), n);
			}
		};
	}
}
#else

#include <odb/mysql/traits.hxx>

namespace odb
{
	namespace mysql
	{
		template <>
		class value_traits<json_value, id_string>
		{
		public:
			typedef json_value value_type;
			typedef value_type query_type;
			typedef details::buffer image_type;

			static void set_value(json_value& v, const details::buffer& b, std::size_t n, bool is_null)
			{
				if (!is_null)
				{
					std::string err;
					v = json11::Json::parse(std::string(b.data(), n), err);
				}
			}

			static void set_image(details::buffer& b, std::size_t& n, bool& is_null, const json_value& v)
			{
				std::string i = v.dump();
				n = i.length();
				b.capacity(i.length());
				std::copy(i.begin(), i.end(), b.data());
				is_null = false;
			}
		};

		template <>
		class value_traits<std::vector<int>, id_string>
		{
		public:
			typedef std::vector<int> value_type;
			typedef value_type query_type;
			typedef details::buffer image_type;

			static void
				set_value(value_type& v,
					const details::buffer& b,
					std::size_t n,
					bool is_null)
			{
				v.clear();

				if (!is_null)
				{
					char c;
					std::istringstream is(std::string(b.data(), n));

					is >> c; // '{'

					for (c = static_cast<char> (is.peek()); c != '}'; is >> c)
					{
						v.push_back(int());
						is >> v.back();
					}
				}
			}

			static void
				set_image(details::buffer& b,
					std::size_t& n,
					bool& is_null,
					const value_type& v)
			{
				is_null = false;
				std::ostringstream os;

				os << '{';

				for (value_type::const_iterator i(v.begin()), e(v.end());
					i != e;)
				{
					os << *i;

					if (++i != e)
						os << ',';
				}

				os << '}';

				const std::string& s(os.str());
				n = s.size();

				if (n > b.capacity())
					b.capacity(n);

				std::memcpy(b.data(), s.c_str(), n);
			}
		};

		template <>
		class value_traits<::boost::multiprecision::cpp_dec_float_50, id_decimal>
		{
		public:
			typedef ::boost::multiprecision::cpp_dec_float_50 value_type;
			typedef value_type query_type;
			typedef details::buffer image_type;

			static void
				set_value(value_type& v,
					const details::buffer& b,
					std::size_t n,
					bool is_null)
			{
				if (!is_null)
				{
					std::string numeric_number(b.data(), n);
					v = value_type(numeric_number);
				}
			}

			static void
				set_image(details::buffer& b,
					std::size_t& n,
					bool& is_null,
					const value_type& v)
			{
				is_null = false;
				std::ostringstream os;
				os.precision(std::numeric_limits<value_type>::max_digits10);
				os << v;
				std::string s = os.str();
				n = s.size();
				if (n > b.capacity())
					b.capacity(n);
				std::memcpy(b.data(), s.c_str(), n);
			}
		};
	}
}
#endif

#endif

