#include "dbh-mysql.h"
#include <stdint.h>
#include <stdio.h>

DBI::MySQLDatabaseHandle::MySQLDatabaseHandle() {
	handle = nullptr;
	server_side_prepare = false;
}

DBI::MySQLDatabaseHandle::~MySQLDatabaseHandle() {
	Disconnect();
}

bool DBI::MySQLDatabaseHandle::Connect(std::string dbname, std::string host, std::string username,
									   std::string auth, DatabaseAttributes &attr) {
	if(handle) {
		return false;
	}
	
	handle = mysql_init(handle);
	if(!handle) {
		return false;
	}
	
	unsigned int client_flag = 0;
	auto iter = attr.find("mysql_init_command");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_INIT_COMMAND, iter->second.c_str());
	}
	
	iter = attr.find("mysql_compress");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_OPT_COMPRESS, nullptr);
	}
	
	iter = attr.find("mysql_connect_timeout");
	if(iter != attr.end()) {
		int t = std::stoi(iter->second);
		mysql_options(handle, MYSQL_OPT_CONNECT_TIMEOUT, &t);
	}
	
	iter = attr.find("mysql_local_infile");
	if(iter != attr.end()) {
		unsigned int t = static_cast<unsigned int>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_OPT_LOCAL_INFILE, &t);
	}
	
	iter = attr.find("mysql_named_pipe");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_OPT_RECONNECT, nullptr);
	}
	
	iter = attr.find("mysql_protocol");
	if(iter != attr.end()) {
		unsigned int t = static_cast<unsigned int>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_OPT_PROTOCOL, &t);
	}
	
	iter = attr.find("mysql_read_timeout");
	if(iter != attr.end()) {
		unsigned int t = static_cast<unsigned int>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_OPT_READ_TIMEOUT, &t);
	}
	
	iter = attr.find("mysql_reconnect");
	if(iter != attr.end()) {
		my_bool t = static_cast<my_bool>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_OPT_RECONNECT, &t);
	}
	
	iter = attr.find("mysql_ssl_verify_server_cert");
	if(iter != attr.end()) {
		my_bool t = static_cast<my_bool>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &t);
	}
	
	iter = attr.find("mysql_use_result");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_OPT_USE_RESULT, nullptr);
	}
	
	iter = attr.find("mysql_write_timeout");
	if(iter != attr.end()) {
		unsigned int t = static_cast<unsigned int>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_OPT_WRITE_TIMEOUT, &t);
	}
	
	iter = attr.find("mysql_read_default_file");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_READ_DEFAULT_FILE, iter->second.c_str());
	}
	
	iter = attr.find("mysql_read_default_group");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_READ_DEFAULT_GROUP, iter->second.c_str());
	}
	
	iter = attr.find("mysql_report_data_truncation");
	if(iter != attr.end()) {
		my_bool t = static_cast<my_bool>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_REPORT_DATA_TRUNCATION, &t);
	}
	
	iter = attr.find("mysql_secure_auth");
	if(iter != attr.end()) {
		my_bool t = static_cast<my_bool>(std::stoi(iter->second));
		mysql_options(handle, MYSQL_SECURE_AUTH, &t);
	}
	
	iter = attr.find("mysql_charset_dir");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_SET_CHARSET_DIR, iter->second.c_str());
	}
	
	iter = attr.find("mysql_charset_name");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_SET_CHARSET_NAME, iter->second.c_str());
	}
	
	iter = attr.find("mysql_shared_memory_base_name");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_SHARED_MEMORY_BASE_NAME, iter->second.c_str());
	}
	
	iter = attr.find("mysql_read_default_group");
	if(iter != attr.end()) {
		mysql_options(handle, MYSQL_READ_DEFAULT_GROUP, iter->second.c_str());
	}
	
	iter = attr.find("mysql_ssl");
	if(iter != attr.end()) {
		char t = static_cast<char>(std::stoi(iter->second));
		if(t != 0) {
			std::string client_key;
			std::string client_cert;
			std::string ca_file;
			std::string ca_path;
			std::string cipher;
			char verify_true = 0;
	
			iter = attr.find("mysql_ssl_verify_server_cert");
			if(iter != attr.end()) {
				t = static_cast<char>(std::stoi(iter->second));
				verify_true = t != 0 ? true : false;
			}
	
			iter = attr.find("mysql_ssl_client_key");
			if(iter != attr.end()) {
				client_key = iter->second;
			}
	
			iter = attr.find("mysql_ssl_client_cert");
			if(iter != attr.end()) {
				client_cert = iter->second;
			}
	
			iter = attr.find("mysql_ssl_ca_file");
			if(iter != attr.end()) {
				ca_file = iter->second;
			}
	
			iter = attr.find("mysql_ssl_ca_path");
			if(iter != attr.end()) {
				ca_path = iter->second;
			}
	
			iter = attr.find("mysql_ssl_cipher");
			if(iter != attr.end()) {
				cipher = iter->second;
			}
	
			mysql_ssl_set(handle,
				client_key.empty() ? nullptr : client_key.c_str(),
				client_cert.empty() ? nullptr : client_cert.c_str(),
				ca_file.empty() ? nullptr : ca_file.c_str(),
				ca_path.empty() ? nullptr : ca_path.c_str(),
				cipher.empty() ? nullptr : cipher.c_str());
	
			mysql_options(handle, MYSQL_OPT_SSL_VERIFY_SERVER_CERT, &verify_true);
			client_flag |= CLIENT_SSL;
		}
	}
	
	std::string socket;
	int port = 0;
	iter = attr.find("mysql_unix_socket");
	if(iter != attr.end()) {
		socket = iter->second;
	}
	
	iter = attr.find("mysql_port");
	if(iter != attr.end()) {
		port = static_cast<int>(std::stoi(iter->second));
	}

	iter = attr.find("mysql_server_side_prepare");
	if(iter != attr.end()) {
		server_side_prepare = static_cast<my_bool>(std::stoi(iter->second)) == 0 ? false : true;
	}
	
	MYSQL* result = mysql_real_connect(handle, host.c_str(), username.c_str(), auth.c_str(), dbname.c_str(), port, 
		socket.empty() ? nullptr : socket.c_str(), client_flag);
	
	if(result) {
		return true;
	}
	

	Disconnect();
	return false;
}

bool DBI::MySQLDatabaseHandle::Disconnect() {
	if(!handle)
		return false;

	mysql_close(handle);
	handle = nullptr;
	return true;
}

bool DBI::MySQLDatabaseHandle::Do(std::string stmt) {
	StatementArguments args;
	return Do(stmt, args);
}

bool DBI::MySQLDatabaseHandle::Do(std::string stmt, DBI::StatementArguments &args) {
	MYSQL_STMT *my_stmt = nullptr;
	MYSQL_BIND *my_bind = nullptr;

	if(server_side_prepare) {
		return false;
	} else {
		return _basic_execute(stmt, args);
	}
}

bool DBI::MySQLDatabaseHandle::_basic_execute(std::string stmt, DBI::StatementArguments &args) {
	std::string final_string;
	size_t current_arg = 0;
	size_t stmt_sz = stmt.size();

	bool escaped = false;
	for(size_t i = 0; i < stmt_sz; ++i) {
		char t = stmt[i];
		if(escaped) {
			final_string.push_back('\\');
			final_string.push_back(t);
			escaped = false;
		} else if(t == '\\') {
			escaped = true;
		} else if(t == '?') {
			if(current_arg < args.size()) {
				DBI::Any t = args[current_arg];
				if(t.empty()) {
					final_string += "NULL";
				} else if(t.type() == typeid(bool)) {
					try {
						bool v = DBI::any_cast<bool>(t);
						if(v) {
							final_string += "1";
						} else {
							final_string += "0";
						}
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from bool arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(int8_t)) {
					try {
						int8_t v = DBI::any_cast<int8_t>(t);
						final_string += std::to_string(static_cast<long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from int8_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(uint8_t)) {
					try {
						uint8_t v = DBI::any_cast<uint8_t>(t);
						final_string += std::to_string(static_cast<unsigned long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from uint8_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(int16_t)) {
					try {
						int16_t v = DBI::any_cast<int16_t>(t);
						final_string += std::to_string(static_cast<long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from int16_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(uint16_t)) {
					try {
						uint16_t v = DBI::any_cast<uint16_t>(t);
						final_string += std::to_string(static_cast<unsigned long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from uint16_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(int32_t)) {
					try {
						int32_t v = DBI::any_cast<int32_t>(t);
						final_string += std::to_string(static_cast<long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from int32_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(uint32_t)) {
					try {
						uint32_t v = DBI::any_cast<uint32_t>(t);
						final_string += std::to_string(static_cast<unsigned long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from uint32_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(int64_t)) {
					try {
						int64_t v = DBI::any_cast<int64_t>(t);
						final_string += std::to_string(static_cast<long long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from int8_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(uint64_t)) {
					try {
						uint64_t v = DBI::any_cast<uint64_t>(t);
						final_string += std::to_string(static_cast<unsigned long long>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from uint8_t arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(float)) {
					try {
						float v = DBI::any_cast<float>(t);
						final_string += std::to_string(static_cast<double>(v));
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from float arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(double)) {
					try {
						double v = DBI::any_cast<double>(t);
						final_string += std::to_string(v);
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from double arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(const char*)) {
					try {
						const char *v = DBI::any_cast<const char*>(t);
						final_string += _quote(v);
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from const char* arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(std::string)) {
					try {
						std::string v = DBI::any_cast<std::string>(t);
						final_string += _quote(v);
					} catch(DBI::bad_any_cast) {
						SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from std::string arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
						return false;
					}
				} else if(t.type() == typeid(std::nullptr_t)) {
					final_string += "NULL";
				} else {
					SetError(DBH_ERROR_INVALID_ARGS, "Could not convert from unknown arg in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
					return false;
				}
				++current_arg;
			} else {
				SetError(DBH_ERROR_INVALID_ARGS, "Invalid arg count in DBI::MySQLDatabaseHandle::_basic_execute(stmt, args).");
				return false;
			}
		} else {
			final_string.push_back(t);
		}
	}

	int ret = mysql_real_query(handle, final_string.c_str(), static_cast<unsigned long>(final_string.length()));
	switch(ret) {
	case CR_COMMANDS_OUT_OF_SYNC:
		SetError(DBH_ERROR_QUERY, "DBI::MySQLDatabaseHandle::_basic_execute(stmt, args) returned a command out of sync error.");
		return false;
		break;
	case CR_SERVER_GONE_ERROR:
		SetError(DBH_ERROR_QUERY, "DBI::MySQLDatabaseHandle::_basic_execute(stmt, args) returned a server gone error.");
		return false;
		break;
	case CR_SERVER_LOST:
		SetError(DBH_ERROR_QUERY, "DBI::MySQLDatabaseHandle::_basic_execute(stmt, args) returned a server lost error.");
		return false;
		break;
	case CR_UNKNOWN_ERROR:
		SetError(DBH_ERROR_QUERY, "DBI::MySQLDatabaseHandle::_basic_execute(stmt, args) returned an unknown error.");
		return false;
		break;
	case 0:
		break;
	default:
		std::string error_buffer = "DBI::MySQLDatabaseHandle::_basic_execute(stmt, args) Generic Error: #";
		error_buffer += std::to_string(static_cast<unsigned long>(mysql_errno(handle)));
		error_buffer += " ";
		error_buffer += mysql_error(handle);
		SetError(DBH_ERROR_QUERY, error_buffer.c_str());
		return false;
	}

	return true;
}

std::string DBI::MySQLDatabaseHandle::_quote(std::string v) {
	std::string ret = "'";

	if(!v.empty()) {
		size_t nb_sz = v.size() * 2 + 1;
		char *escaped = new char[nb_sz];
		mysql_real_escape_string(handle, escaped, v.c_str(), static_cast<unsigned long>(v.size()));
		ret += escaped;
	}

	ret.push_back('\'');
	return ret;
}