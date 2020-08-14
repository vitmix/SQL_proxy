// Logger implemented using a Singleton pattern.
// The logger uses an instance of the worker to write 
// to the file (sends write_log(...) to the worker's task queue)
#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <mutex>

#include "worker.h"

class Logger {
public:
	static Logger & get_instance();

	// Sends task to worker ("makes log")
	void make_log(std::shared_ptr<std::string> info);

	// Main method for writing to file
	void write_log(std::shared_ptr<std::string> info);

private:
	Logger(const Logger &) = delete;
	Logger(Logger &&) = delete;
	Logger & operator=(const Logger &) = delete;
	Logger & operator=(Logger &&) = delete;

	Logger();
	~Logger();

private:
	std::ofstream _out_log;
	std::mutex _mutex;
	std::unique_ptr<Worker> _worker;
};