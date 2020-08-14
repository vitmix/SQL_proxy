#include "logger.h"

Logger::Logger()
{
	_out_log.open("log.txt", std::ios::app);
	_worker = Worker::create_worker();
}

// This is the key method that returns the singleton reference
Logger & Logger::get_instance()
{
	static Logger instance{};
	return instance;
}

void Logger::make_log(std::shared_ptr<std::string> info)
{
	std::lock_guard<std::mutex> lock(_mutex);
	Callback work = std::bind(&Logger::write_log, this, info);
	_worker->send_work(work);
}

void Logger::write_log(std::shared_ptr<std::string> info)
{
	_out_log << *info.get() << "\n";
	_out_log.flush();
}

Logger::~Logger()
{	
	_out_log.close();
}