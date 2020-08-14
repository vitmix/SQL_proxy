#pragma once

#include <thread>

#include "acceptor.h"
#include "logger.h"

namespace proxy {

	class Connector : public std::enable_shared_from_this<Connector> {
	public:
		

		Connector(const std::string & host, unsigned short port,
					socket_type cl_sock, socket_type sr_sock);

		Connector(const Connector & other) = delete;
		Connector& operator=(const Connector & other) = delete;

		void start_connection();

		void stop();

	private:
		
		void handle_connection(const boost::system::error_code & err);

		// --------------------------------------------------------------
		// BEGIN : Server -> Proxy -> Client

		// Callback that writes data from the server to the client socket
		void handle_read_from_server(const boost::system::error_code & err,
										const size_t & bytes_transferred);


		// Callback that reads data from a server socket
		void handle_write_to_client(const boost::system::error_code& err);

		// END : Server -> Proxy -> Client
		// --------------------------------------------------------------

		// --------------------------------------------------------------
		// BEGIN : Client -> Proxy -> Server

		// Callback that writes data from the client to the server socket
		void handle_read_from_client(const boost::system::error_code & err,
										const size_t & bytes_transferred);
		
		// Callback that reads data from a client socket
		void handle_write_to_server(const boost::system::error_code& err);

		// Method that check sql statements
		void handle_sql_statement(char * stmnt, size_t size);

		// END : Client -> Proxy -> Server
		// --------------------------------------------------------------

	private:
		std::string _server_host;
		unsigned short _server_port;

		socket_type _client_socket;
		socket_type _server_socket;
		
		// Data between client and server is exchanged in packets of max 16 Mb size
		// see : https://dev.mysql.com/doc/dev/mysql-server/latest/page_protocol_basic_packets.html
		// For our purpose 4194304 b is enough
		enum { 
			max_data_length = 4194304,
			header_size = 4
		};

		char _server_data[max_data_length];
		char _client_data[max_data_length];
		
		std::string _sql_statement;
		size_t _payload_size;
		bool _is_set_payload;

		Logger & _logger;

		std::mutex _mutex;
	};	
}