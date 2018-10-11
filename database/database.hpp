
#pragma once

#include <atomic>
#include <string>
#include <list>
#include <vector>

#include <boost/atomic.hpp>
#include <boost/thread/shared_mutex.hpp>

#include <boost/noncopyable.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/optional.hpp>
#include <boost/logic/tribool.hpp>
#include <json11.hpp>

#include "steady_clock.hpp"

#include "pooldb.hxx"

#include "odb/database.hxx"
#include "odb/connection.hxx"
#ifdef ODB_USE_MYSQL
#include "odb/mysql/database.hxx"
#include "odb/mysql/connection-factory.hxx"
using odb_connection_pool_factory = odb::mysql::connection_pool_factory;
#else
#include "odb/pgsql/database.hxx"
#include "odb/pgsql/connection-factory.hxx"
using odb_connection_pool_factory = odb::pgsql::connection_pool_factory;
#endif
template<typename policytable>
struct db_cache_trait;

class database
	: public boost::noncopyable
{
public:
	static void create_stored_procedures(std::shared_ptr<odb::database> db);

	explicit database(boost::asio::io_context& io, std::shared_ptr<odb::database> writedb,
		std::shared_ptr<odb::database> readdb = std::shared_ptr<odb::database>());
	~database();

public:
	std::vector< db::example_table > load_example(std::string key);

private:
	bool  handle_connection_lost(int & retry_count, int max_retries);

private:
	boost::asio::io_context& m_io_service;

	std::shared_ptr<odb_connection_pool_factory> db_pool_factory;
	std::shared_ptr<odb::database> m_write_db, m_read_db;
};
