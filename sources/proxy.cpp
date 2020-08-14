#include "proxy.h"

proxy::Proxy::config proxy::Proxy::_config;

proxy::Proxy::Proxy(asio::io_service & ios)
	: _io_serv(ios)
{
	_work.reset(new asio::io_service::work(_io_serv));
	
	_acceptor.reset(new Acceptor(_io_serv, 
									_config.local_host, 
									_config.local_port, 
									creator));
}

void proxy::Proxy::start_proxy(/*size_t num_of_threads*/)
{
	// assert(num_of_threads > 0);

	_acceptor->start();

	// Create specified number of threads
	//for (size_t i = 0; i < num_of_threads; ++i) {
	//	std::unique_ptr<std::thread> th(new std::thread([this]() 
	//	{
	//			_io_serv.run();
	//	}));
	//	
	//	// and add them to the thread pool
	//	_thread_pool.push_back(std::move(th));
	//}

}

void proxy::Proxy::stop_proxy()
{
	_acceptor->stop();
	_work.reset(NULL);

	// Joining all threads
	/*for (auto & th : _thread_pool) {
		th->join();
	}*/
}

void proxy::Proxy::set_config(const Proxy::config & conf)
{
	_config = conf;
}

void proxy::Proxy::creator(socket_type cl_sock, socket_type srv_sock)
{
	// Constructing an instance of a class Connector that manages two sockets 
	// and implements input / output and logging operations.
	// Also we run start_connection() for starting an async loop of read/write ops
	(std::make_shared<Connector>(_config.remote_host, _config.remote_port, cl_sock, srv_sock))->start_connection();
}