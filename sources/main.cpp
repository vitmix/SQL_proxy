#include "proxy.h"

//const unsigned DEFAULT_THREAD_POOL_SIZE = 2;

int main(int argc, char* argv[])
{
	if (argc != 5) {
		std::cerr << "Usage : <name> <local host ip> <local port> <remote host ip> <remote port>\n";
		system("pause");
		return 1;
	}

	proxy::Proxy::config conf;

	conf.local_host = argv[1];
	conf.remote_host = argv[3];
	
	conf.local_port = static_cast<unsigned short>(::atoi(argv[2]));
	conf.remote_port = static_cast<unsigned short>(::atoi(argv[4]));

	proxy::Proxy::set_config(conf);

	proxy::asio::io_service ios;

	try
	{
		proxy::Proxy my_proxy(ios);
		
		//unsigned thread_pool_size = std::thread::hardware_concurrency() * 2;
		//if (thread_pool_size == 0) thread_pool_size = DEFAULT_THREAD_POOL_SIZE;

		my_proxy.start_proxy(/*thread_pool_size*/);
		ios.run();
		my_proxy.stop_proxy();
	}
	catch (const std::exception & ex)
	{
		std::cerr << "Error : " << ex.what() << "\n";
		system("pause");
		return 1;
	}

	system("pause");
	return 0;
}