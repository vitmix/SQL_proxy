#pragma once

#include <vector>

#include "connector.h"

namespace proxy {

	class Proxy {
	public:
		typedef struct _config {
			std::string remote_host;
			unsigned short remote_port;

			std::string local_host;
			unsigned short local_port;

		} config;

		Proxy(asio::io_service & ios);

		Proxy(const Proxy & other) = delete;
		Proxy & operator=(const Proxy & other) = delete;

		void start_proxy(/*size_t num_of_threads*/);

		void stop_proxy();

		static void set_config(const Proxy::config & conf);

	private:
		// Function that the _acceptor will call to transfer the sockets
		static void creator(socket_type cl_sock, socket_type srv_sock);
		
	private:
		asio::io_service & _io_serv;
		std::unique_ptr<asio::io_service::work> _work;
		std::unique_ptr<Acceptor> _acceptor;

		// std::vector<std::unique_ptr<std::thread>> _thread_pool;

		static config _config;
	};

}