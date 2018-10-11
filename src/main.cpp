
#include <iostream>
#include <memory>
#include <numeric>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>
#include <boost/chrono.hpp>
#include <json11.hpp>

#include <boost/beast.hpp>
#include <boost/beast/http.hpp>


#ifdef ODB_USE_MYSQL
#include "odb/mysql/database.hxx"
using odb_database = odb::mysql::database;
using odb_connection_factory = odb::mysql::connection_factory;
#else
#include "odb/pgsql/database.hxx"
using odb_database = odb::pgsql::database;
using odb_connection_factory = odb::pgsql::connection_factory;
#endif
#include "odb/schema-catalog.hxx"

#include <boost/thread.hpp>
#include <boost/program_options.hpp>
#include <odb/transaction.hxx>
namespace po = boost::program_options;

#include "database.hpp"

#include "odb/query.hxx"
#include "pooldb-odb.hxx"
#include "pooldb.hxx"

#include <boost/lambda/lambda.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/algorithm.hpp>

int do_db_setup_or_migrate(odb::database& odb_db)
{
	try
	{
		odb::transaction t(odb_db.begin());

		if (odb_db.schema_version() == 0)
		{
			t.commit();
			t.reset(odb_db.begin());
			odb::schema_catalog::create_schema(odb_db);
			t.commit();
			return 0;
		}
		else
		{
			// doing database migration
			odb::schema_catalog::migrate(odb_db);
		}
		t.commit();
	}
	catch (const odb::unknown_schema_version & e)
	{
		std::cerr << "database schema version " << e.version() << " not supported. recreate database or do manual migrate\n";
		return 1;
	}
	catch (const odb::unknown_schema & e)
	{
		std::cerr << "create database connection failed: " << e.what() << std::endl;
		return 1;
	}
	catch (const std::exception & ec)
	{
		std::cerr << typeid(ec).name() << std::endl;
		std::printf("create database connection failed, error: %s\n", ec.what());
		return 1;
	}
	return 0;
}


static boost::multiprecision::cpp_dec_float_50 integer_round_up(boost::multiprecision::cpp_dec_float_50 reward)
{
	return boost::multiprecision::cpp_dec_float_50(reward.convert_to<int>());
}

int main(int argc, char * argv[])
{
	std::shared_ptr<odb_database> odb_db;

	std::string db_host1, db_user1, db_password1, db_name1;
	int db_port1 = 5432;

	po::options_description desc("options");
	desc.add_options()
		("help,h", "help message")
		("version,v", "current sspay version")
		("host", po::value<std::string>(&db_host1), "database hostname for normal operation")
		("port", po::value<int>(&db_port1), "database port for normal operation")
		("user", po::value<std::string>(&db_user1), "database username for normal operation")
		("password", po::value<std::string>(&db_password1), "database password for normal operation")
		("dbname", po::value<std::string>(&db_name1)->default_value("example"), "database name for normal operation")
		;

	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	if (vm.count("help"))
	{
		std::cout << desc << "\n";
		return 0;
	}

	std::unique_ptr<odb_connection_factory>
		databaseconnectionpool(new odb_connection_pool_factory(1, 1));
#ifdef ODB_USE_MYSQL
	odb_db = std::make_shared<odb_database>(db_user1,
		db_password1, db_name1, db_host1, db_port1, nullptr, "", 0, std::move(databaseconnectionpool));
#else
	odb_db = std::make_shared<odb_database>(db_user1,
		db_password1, db_name1, db_host1, db_port1, "", std::move(databaseconnectionpool));
#endif

	if (do_db_setup_or_migrate(*odb_db))
		return 1;

	// start server.
	boost::asio::io_context io;
	return 0;
}
