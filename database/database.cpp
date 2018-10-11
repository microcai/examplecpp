
#include <thread>
#include <algorithm>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread/future.hpp>
#include <boost/thread.hpp>

#if defined(_WIN32) || defined(_WIN64) || defined(__MACH__)
#include <boost/uuid/random_generator.hpp>
#else
#include <systemd/sd-id128.h>
#endif

#include <json11.hpp>

#include <boost/stacktrace.hpp>

#ifdef ODB_USE_MYSQL
#include <odb/mysql/database.hxx>
#else
#include <odb/pgsql/database.hxx>
#endif

#include <odb/database.hxx>
#include <odb/transaction.hxx>
#include <odb/result.hxx>

#include "database.hpp"
#include "pooldb-odb.hxx"

#include "beautylog/beautylog.hpp"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/algorithm.hpp>

const unsigned short max_retries = 5;

void database::create_stored_procedures(std::shared_ptr< odb::database > db)
{
}

database::database(boost::asio::io_context& io, std::shared_ptr<odb::database> writedb, std::shared_ptr<odb::database> readdb)
	: m_io_service(io)
	, m_write_db(writedb)
	, m_read_db(readdb)
{
}

database::~database() = default;

bool database::handle_connection_lost(int& retry_count, int max_retries)
{
	log_print(LOG_ERR, "database connection lost, try to recover");
	return true;
}

std::vector< db::example_table > database::load_example(std::string key)
{
	std::vector<db::example_table> ret;

	for (int retry_count (0); retry_count < max_retries ; retry_count++)
	{
		try
		{
			odb::transaction t(m_read_db->begin());

#ifndef ODB_USE_MYSQL
			t.connection().execute("set statement_timeout to 5000;");
#endif
			auto r = m_read_db->query<db::example_table>( odb::query<db::example_table>::key == key);
			for (auto & b : r)
				ret.push_back(b);
			t.commit();
			return ret;
		}
		catch(const odb::recoverable&)
		{
			if (handle_connection_lost(retry_count, max_retries))
				continue;
		}
		catch(const odb::exception& err)
		{
			// std::cerr << boost::stacktrace::stacktrace() << std::endl;
			log_print(LOG_ERR, "database error: %s when query load_user_pps", err.what());
			return ret;
		}
	}
	return ret;
}
