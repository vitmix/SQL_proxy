#include "connector.h"

proxy::Connector::Connector(const std::string & host, unsigned short port,
							socket_type cl_sock, socket_type sr_sock)
	:	_server_host(host), _server_port(port), 
		_client_socket(cl_sock), _server_socket(sr_sock), 
		_is_set_payload(false), _logger(Logger::get_instance())
{}

void proxy::Connector::start_connection()
{
	try
	{
		_server_socket->async_connect(
			asio::ip::tcp::endpoint(
				asio::ip::address::from_string(_server_host), _server_port),
			boost::bind(&Connector::handle_connection,
				shared_from_this(),
				asio::placeholders::error)
		);
	}
	catch (const std::exception & ex)
	{
		std::cerr << "Error (start_connection) : " << ex.what() << "\n";
		stop();
	}
}

void proxy::Connector::stop()
{
	std::lock_guard<std::mutex> lock(_mutex);

	if (_server_socket->is_open()) {
		_server_socket->close();
	}

	if (_client_socket->is_open()) {
		_client_socket->close();
	}
}

void proxy::Connector::handle_connection(const boost::system::error_code & err)
{
	if (!err) {
		_server_socket->async_read_some(
			asio::buffer(_server_data, max_data_length),
			boost::bind(&Connector::handle_read_from_server,
				shared_from_this(),
				asio::placeholders::error,
				asio::placeholders::bytes_transferred)
		);

		_client_socket->async_read_some(
			asio::buffer(_client_data, max_data_length),
			boost::bind(&Connector::handle_read_from_client,
				shared_from_this(),
				asio::placeholders::error,
				asio::placeholders::bytes_transferred)
		);
	}
	else {
		std::cout << "Error occured ! Error code = " << err.value()
			<< ". Message : " << err.message() << "\n";
		stop();
	}
}

// BEGIN : Server -> Proxy -> Client

void proxy::Connector::handle_read_from_server(const boost::system::error_code & err,
												const size_t & bytes_transferred)
{
	if (!err) {
		asio::async_write(*_client_socket.get(),
			asio::buffer(_server_data, bytes_transferred),
			boost::bind(&Connector::handle_write_to_client,
				shared_from_this(),
				asio::placeholders::error)
		);
	}
	else {
		stop();
	}
}

void proxy::Connector::handle_write_to_client(const boost::system::error_code & err)
{
	if (!err) {
		_server_socket->async_read_some(
			asio::buffer(_server_data, max_data_length),
			boost::bind(&Connector::handle_read_from_server,
				shared_from_this(),
				asio::placeholders::error,
				asio::placeholders::bytes_transferred)
		);
	}
	else {
		stop();
	}
}

// END : Server -> Proxy -> Client

// ------------------------------------------------------------------------------------------

// BEGIN : Client -> Proxy -> Server

void proxy::Connector::handle_read_from_client(const boost::system::error_code & err,
												const size_t & bytes_transferred)
{
	if (!err) {
		asio::async_write(*_server_socket.get(),
			asio::buffer(_client_data, bytes_transferred),
			boost::bind(&Connector::handle_write_to_server,
				shared_from_this(),
				asio::placeholders::error)
		);

		handle_sql_statement(_client_data, bytes_transferred);
	}
	else {
		stop();
	}
}

void proxy::Connector::handle_write_to_server(const boost::system::error_code & err)
{
	if (!err) {
		_client_socket->async_read_some(
			asio::buffer(_client_data, max_data_length),
			boost::bind(&Connector::handle_read_from_client,
				shared_from_this(),
				asio::placeholders::error,
				asio::placeholders::bytes_transferred)
		);
	}
	else {
		stop();
	}
}

void proxy::Connector::handle_sql_statement(char * stmnt, size_t size)
{
	// Accumulate read bytes in the string
	_sql_statement.append(stmnt, size);

	// If not read the header - return
	if (_sql_statement.size() < header_size) return;

	// Size of header is 4b, size of payload is 3b (first 3b) :
	// int<3>int<1>*payload*
	if (!_is_set_payload && _sql_statement.size() >= header_size) {
		// int<3> is a fixed-length unsigned integer, it stores its value in a
		// series of bytes with the least significant byte first
		_payload_size = 0u;
		_payload_size |= unsigned char(_sql_statement[0]);
		_payload_size |= unsigned char(_sql_statement[1]) << 8;
		_payload_size |= unsigned char(_sql_statement[2]) << 16;
		
		_is_set_payload = true;
	}

	// Have not read the whole package body yet, so - return
	if (_sql_statement.size() - header_size < _payload_size) return;

	// Create a packet for recording and send it to logger
	if (_sql_statement.size() - header_size >= _payload_size) {
		_sql_statement.erase(0, header_size);
		auto sql_ptr(std::make_shared<std::string>(_sql_statement, 0, _payload_size));
		
		// std::cout << *sql_ptr.get() << "\n";

		_logger.make_log(sql_ptr);
		_sql_statement.erase(0, _payload_size);
		
		_is_set_payload = false;
	}
}

// END : Client -> Proxy -> Server