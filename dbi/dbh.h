#ifndef DBI__DBH_H
#define DBI__DBH_H

#include "dbi.h"
#include "dbi-error.h"

namespace DBI
{

class StatementHandle;
class ResultSet;

class DatabaseHandle : public ErrorHandler
{
public:
	DatabaseHandle() { }
	virtual ~DatabaseHandle() { }
	
	virtual bool Connect(std::string dbname, std::string host, std::string username,
		std::string auth, DatabaseAttributes &attr) = 0;
	virtual bool Disconnect() = 0;
	
	virtual std::unique_ptr<ResultSet> Do(std::string stmt) = 0;
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, StatementArguments &args) = 0;
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3, DBI::Any arg4);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3, DBI::Any arg4, DBI::Any arg5);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3, DBI::Any arg4, DBI::Any arg5,
		DBI::Any arg6);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3, DBI::Any arg4, DBI::Any arg5,
		DBI::Any arg6, DBI::Any arg7);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3, DBI::Any arg4, DBI::Any arg5,
		DBI::Any arg6, DBI::Any arg7, DBI::Any arg8);
	virtual std::unique_ptr<ResultSet> Do(std::string stmt, DBI::Any arg0, DBI::Any arg1, DBI::Any arg2, DBI::Any arg3, DBI::Any arg4, DBI::Any arg5,
		DBI::Any arg6, DBI::Any arg7, DBI::Any arg8, DBI::Any arg9);

	virtual std::unique_ptr<StatementHandle> Prepare(std::string stmt, std::string name) = 0;

	virtual bool Ping() = 0;
	virtual bool Begin() = 0;
	virtual bool Commit() = 0;
	virtual bool Rollback() = 0;
};

}

#endif
