#include "worker.h"

Worker::Worker() : _done(false) {}

Worker::~Worker()
{
	Callback quit_token = std::bind(&Worker::set_done, this);
	
	// Tell thread to exit
	send_work(quit_token);

	// Waiting for a thread
	_thread.join();
}

void Worker::send_work(Callback work)
{
	_work_queue.push(work);
}

// Safe construction and thread start
std::unique_ptr<Worker> Worker::create_worker()
{
	std::unique_ptr<Worker> w_ptr(new Worker());
	
	// Creating of "background" thread
	w_ptr->_thread = std::thread(&Worker::start, w_ptr.get());
	return w_ptr;
}

void Worker::start()
{
	while (!_done) {
		Callback work;

		// Wait till job is available, then retrieve it 
		_work_queue.wait_and_pop(work);

		// and execute the retrieved job in this "background" thread
		work();
	}
}
