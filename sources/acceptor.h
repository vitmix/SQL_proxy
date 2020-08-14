#pragma once

#include <boost/asio.hpp>
#include <boost/bind.hpp>

#include <iostream>
#include <atomic>
#include <memory>
#include <functional>


namespace proxy {

	namespace asio = boost::asio;

	typedef std::shared_ptr<asio::ip::tcp::socket> socket_type;

	// A pointer to a function or method that will accept instances 
	// of client-server`s sockets formed by the Acceptor
	typedef void(*socket_handler) (socket_type cl_socket, socket_type srv_socket);

	class Acceptor {
	public:
		
		Acceptor(asio::io_service & service,
					const std::string & host,
					unsigned short port, 
					socket_handler handler);

		Acceptor(const Acceptor & other) = delete;
		Acceptor& operator=(const Acceptor & other) = delete;

		void start();

		void stop();

	private:
		
		void run_accept();
		
		// Will call socket_handler for passing client and server socket`s
		void handle_accept(const boost::system::error_code & err, socket_type cl_socket);

	private:

		asio::io_service & _io_serv;
		asio::ip::tcp::acceptor _acceptor;
		std::atomic<bool> _is_stopped;
		socket_handler _sock_handler;
	};

}
