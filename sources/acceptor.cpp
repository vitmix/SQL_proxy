#include "acceptor.h"

proxy::Acceptor::Acceptor(asio::io_service & service, 
							const std::string & host, 
							unsigned short port, 
							socket_handler handler)
	:	_io_serv(service), 
		_acceptor(_io_serv, asio::ip::tcp::endpoint(
			asio::ip::address_v4::from_string(host), port)),
		_is_stopped(false),
		_sock_handler(handler)
{}

void proxy::Acceptor::start()
{
	_acceptor.listen();
	run_accept();
}

void proxy::Acceptor::stop()
{
	_is_stopped.store(true);
}

void proxy::Acceptor::run_accept()
{
	auto sock = std::make_shared<asio::ip::tcp::socket>(_io_serv);

	_acceptor.async_accept(
		*sock.get(),
		[this, sock](const boost::system::error_code & err) {
			handle_accept(err, sock);
	});
}

void proxy::Acceptor::handle_accept(const boost::system::error_code & err, socket_type cl_socket)
{
	if (!err) {
		auto srv_socket = std::make_shared<asio::ip::tcp::socket>(_io_serv);
		_sock_handler(cl_socket, srv_socket);
	} else {
		std::cout << "Error occured ! Error code = " << err.value() 
					<< ". Message : " << err.message() << "\n";
	}

	if (!_is_stopped.load()) {
		run_accept();
	} else {
		_acceptor.close();
	}
}
