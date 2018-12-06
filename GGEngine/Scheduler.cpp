#include "stdafx.h"

Worker::Worker(Scheduler &sc)
	: scheduler{ sc }, threadId{ std::this_thread::get_id() }
{
}

void Worker::operator()()
{
	std::function<void()> fn;
	while (true)
	{
		{
			std::unique_lock<std::mutex> lock{ scheduler.tasks_m };
			while (!scheduler.killThreads && scheduler.tasks.empty())
				scheduler.conVar.wait(lock);

			if (scheduler.killThreads)
				return;

			fn = scheduler.tasks.front().function;
			tracker = std::move(scheduler.tasks.front().tracker);
			scheduler.tasks.erase(scheduler.tasks.begin());
		}
		fn();
		tracker.reset();
	}
}

Scheduler::Scheduler(unsigned tNum)
	: killThreads{ false }
{
	// Print out the hardware capabilities
	unsigned int hardware_num = std::thread::hardware_concurrency();
	CONSOLE_SYSTEM(hardware_num, " concurrent threads are supported.");

	workerThreads.reserve(tNum);
	for (size_t i = 0; i != tNum; ++i)
	{
		workerThreads.push_back(std::thread(Worker{ *this }));
	}
}

void Scheduler::JoinThreads()
{
	killThreads = true;
	conVar.notify_all();
	for (auto& elem : workerThreads)
		elem.join();
}

Tracker Scheduler::QueueThread(std::function<void()> task)
{
	Tracker weak;
	{
		std::unique_lock<std::mutex> lock{ tasks_m };
		tasks.emplace_back(std::function<void()>(task));
		weak = tasks.back().tracker;
	}

	conVar.notify_one();
	return std::move(weak);
}

Scheduler::~Scheduler()
{
	JoinThreads();
}

void Scheduler::PrintSchedulerInfo() const
{
	// Prints out all of the relevant information regarding each thread in the pool
	CONSOLE_LN("Number of running threads (including main): ", workerThreads.size() + 1);
}
