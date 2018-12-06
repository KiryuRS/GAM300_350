#pragma once
#include <functional>
#include <atomic>
#include <condition_variable>
#include <utility>
#include <memory>

using Tracker = std::weak_ptr<char>;

class Scheduler;
struct Task
{
	std::function<void()> function;
	std::shared_ptr<char> tracker;
	Task(std::function<void()> f) :
		function(f), tracker(std::make_shared<char>())
	{}
};

class Worker
{
	Scheduler& scheduler;
	std::string taskName;
	std::thread::id threadId;
	std::shared_ptr<char> tracker;
public:

	// Create our thread
	Worker(Scheduler&);
	// Looking for available tasks in queue
	void operator()();
	void operator=(Worker&) = delete;
};

class Scheduler
{
	std::vector<std::thread> workerThreads;
public:
	std::condition_variable conVar;
	std::vector<Task> tasks;
	std::atomic<bool> killThreads;
	std::mutex tasks_m;

	Scheduler(unsigned num);
	void JoinThreads();
	Tracker QueueThread(std::function<void()>);
	~Scheduler();

	void PrintSchedulerInfo() const;
};