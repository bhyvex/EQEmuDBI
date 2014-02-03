#include <stdio.h>
#include <string.h>
#include "../dbi/dbi.h"
#include "../dbi/dbh.h"
#include "../dbi/sth.h"
#include "../dbi/rs.h"

#define PrintErr(x, ...) printf("Error at line %d: " x, __LINE__, __VA_ARGS__)

int main() {
	DBI::DatabaseAttributes attr;
	
	auto dbh = DBI::DatabaseInterface::Instance()->Connect("sqlite", "test.db", "", "", "", attr);
	if(!dbh) {
		PrintErr("Failure to connect.\n");
		return 0;
	}

	auto rs = dbh->Do("DROP TABLE IF EXISTS db_test");
	if(!rs) {
		PrintErr("Failure to drop table.\n");
	}

	rs = dbh->Do("CREATE TABLE db_test ("
		"id INTEGER,"
		"int_value INTEGER,"
		"real_value REAL,"
		"text_value TEXT,"
		"blob_value BLOB)");

	if(!rs) {
		PrintErr("Failure to get result from create table. Error: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	if(!dbh->Ping()) {
		PrintErr("Failed to ping.\n");
		return 0;
	}

	if(!dbh->Begin()) {
		PrintErr("Failed to begin.\n");
		return 0;
	}

	rs = dbh->Do("INSERT INTO db_test (id, int_value, real_value, text_value, blob_value) VALUES(?, ?, ?, ?, ?)",
		1,
		nullptr,
		nullptr,
		nullptr,
		nullptr);

	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to insert value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}
	} else {
		PrintErr("Failure to get result from insert: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	std::string blob_value;
	blob_value.assign("hello\0world\0", 12);
	rs = dbh->Do("INSERT INTO db_test (id, int_value, real_value, text_value, blob_value) VALUES(?, ?, ?, ?, ?)",
		2,
		(uint8_t)5,
		(float)125.90,
		std::string("A test value"),
		blob_value);

	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to insert value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}
	} else {
		PrintErr("Failure to get result from insert: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = dbh->Do("INSERT INTO db_test (id, int_value, real_value, text_value, blob_value) VALUES(?, ?, ?, ?, ?)",
		3,
		(uint16_t)556,
		(double)125.90,
		nullptr,
		nullptr);

	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to insert value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}
	} else {
		PrintErr("Failure to get result from insert: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = dbh->Do("INSERT INTO db_test (id, int_value, real_value, text_value, blob_value) VALUES(?, ?, ?, ?, ?)",
		4,
		(uint32_t)518012,
		nullptr,
		nullptr,
		nullptr);

	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to insert value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}
	} else {
		PrintErr("Failure to get result from insert: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = dbh->Do("INSERT INTO db_test (id, int_value, real_value, text_value, blob_value) VALUES(?, ?, ?, ?, ?)",
		5,
		(uint64_t)42949672960,
		nullptr,
		nullptr,
		nullptr);

	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to insert value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}
	} else {
		PrintErr("Failure to get result from insert: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	if(!dbh->Commit()) {
		PrintErr("Failed to commit.\n");
		return 0;
	}

	auto sth = dbh->Prepare("SELECT int_value, real_value, text_value, blob_value FROM db_test WHERE id = ?");
	if(!sth) {
		PrintErr("Failed to create prepared statement.\n");
		return 0;
	}

	rs = sth->Execute(1);
	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to select value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}

		auto row = (*rs->Rows().begin());
		if(!row["int_value"].is_null) {
			PrintErr("Row int_value was incorrect value in row 1\n");
			return 0;
		}

		if(!row["real_value"].is_null) {
			PrintErr("Row real_value was incorrect value in row 1\n");
			return 0;
		}

		if(!row["text_value"].is_null) {
			PrintErr("Row text_value was incorrect value in row 1\n");
			return 0;
		}

		if(!row["blob_value"].is_null) {
			PrintErr("Row blob_value was incorrect value in row 1\n");
			return 0;
		}
	} else {
		PrintErr("Failure to get result from select: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = sth->Execute(2);
	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to select value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}

		auto row = (*rs->Rows().begin());
		if(row["int_value"].is_null) {
			PrintErr("Row int_value was incorrect value in row 2\n");
			return 0;
		} else {
			uint64_t val = std::stoll(row["int_value"].value);
			if(val != 5) {
				PrintErr("Row int_value was incorrect value in row 2\n");
				return 0;
			}
		}

		if(row["real_value"].is_null) {
			PrintErr("Row real_value was incorrect value in row 2\n");
			return 0;
		} else {
			double val = std::stod(row["real_value"].value);
			if(val > 126.0 || val < 125.8) {
				PrintErr("Row real_value was incorrect value in row 2\n");
				return 0;
			}
		}

		if(row["text_value"].is_null) {
			PrintErr("Row text_value was incorrect value in row 2\n");
			return 0;
		} else {
			if(row["text_value"].value.compare("A test value") != 0) {
				PrintErr("Row text_value was incorrect value in row 2\n");
				return 0;
			}
		}

		if(row["blob_value"].is_null) {
			PrintErr("Row blob_value was incorrect value in row 2\n");
			return 0;
		} else {
			if(row["blob_value"].value.length() != 12) {
				PrintErr("Row blob_value was incorrect length in row 2\n");
				return 0;
			}

			if(memcmp(row["blob_value"].value.c_str(), "hello\0world\0", 12) != 0) {
				PrintErr("Row blob_value was incorrect value in row 2\n");
				return 0;
			}
		}
	} else {
		PrintErr("Failure to get result from select: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = sth->Execute(3);
	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to select value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}

		auto row = (*rs->Rows().begin());
		if(row["int_value"].is_null) {
			PrintErr("Row int_value was incorrect value in row 3\n");
			return 0;
		} else {
			uint64_t val = std::stoll(row["int_value"].value);
			if(val != 556) {
				PrintErr("Row int_value was incorrect value in row 3\n");
				return 0;
			}
		}

		if(row["real_value"].is_null) {
			PrintErr("Row real_value was incorrect value in row 3\n");
			return 0;
		} else {
			double val = std::stod(row["real_value"].value);
			if(val > 126.0 || val < 125.8) {
				PrintErr("Row real_value was incorrect value in row 3\n");
				return 0;
			}
		}

		if(!row["text_value"].is_null) {
			PrintErr("Row text_value was incorrect value in row 3\n");
			return 0;
		}

		if(!row["blob_value"].is_null) {
			PrintErr("Row blob_value was incorrect value in row 3\n");
			return 0;
		}
	} else {
		PrintErr("Failure to get result from select: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = sth->Execute(4);
	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to select value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}

		auto row = (*rs->Rows().begin());
		if(row["int_value"].is_null) {
			PrintErr("Row int_value was incorrect value in row 4\n");
			return 0;
		} else {
			uint64_t val = std::stoll(row["int_value"].value);
			if(val != 518012) {
				PrintErr("Row int_value was incorrect value in row 4\n");
				return 0;
			}
		}

		if(!row["real_value"].is_null) {
			PrintErr("Row real_value was incorrect value in row 4\n");
			return 0;
		}

		if(!row["text_value"].is_null) {
			PrintErr("Row text_value was incorrect value in row 4\n");
			return 0;
		}

		if(!row["blob_value"].is_null) {
			PrintErr("Row blob_value was incorrect value in row 4\n");
			return 0;
		}
	} else {
		PrintErr("Failure to get result from select: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	rs = sth->Execute(5);
	if(rs) {
		if(rs->AffectedRows() != 1) {
			PrintErr("Failure to select value: %s\n", dbh->ErrorMessage().c_str());
			return 0;
		}

		auto row = (*rs->Rows().begin());
		if(row["int_value"].is_null) {
			PrintErr("Row int_value was incorrect value in row 5\n");
			return 0;
		} else {
			uint64_t val = std::stoll(row["int_value"].value);
			if(val != 42949672960) {
				PrintErr("Row int_value was incorrect value in row 5\n");
				return 0;
			}
		}

		if(!row["real_value"].is_null) {
			PrintErr("Row real_value was incorrect value in row 5\n");
			return 0;
		}

		if(!row["text_value"].is_null) {
			PrintErr("Row text_value was incorrect value in row 5\n");
			return 0;
		}

		if(!row["blob_value"].is_null) {
			PrintErr("Row blob_value was incorrect value in row 5\n");
			return 0;
		}
	} else {
		PrintErr("Failure to get result from select: %s\n", dbh->ErrorMessage().c_str());
		return 0;
	}

	printf("Tests passed\n");
	return 0;
}
