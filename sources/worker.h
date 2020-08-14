// "Active object pattern" by Herb Sutter
// https://sites.google.com/site/kjellhedstrom2/active-object-with-cpp0x

#pragma once

#include <thread>
#include <functional>
#include <memory>

#include "shared_queue.h"

typedef std::function<void()> Callback;

class Worker {
public:
	virtual ~Worker();

	void send_work(Callback work);

	static std::unique_ptr<Worker> create_worker();

private:
	// Construction only through factory create_worker()
	Worker(); 

	Worker(const Worker &) = delete;
	Worker & operator=(const Worker &) = delete;

	void set_done() { _done = true; }

	void start();

private:
	shared_queue<Callback> _work_queue;
	std::thread _thread;
	bool _done;
};