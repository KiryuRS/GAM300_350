#pragma once
#include "Broadcaster.h"
#include <mutex>
#include "CoreEngine.h"

template<typename T>
struct ThreadsafeQueue
{
	token messageToken;
	std::vector<T> messageQueue;
	std::mutex messageQueue_m;
	ThreadsafeQueue()
	{
		std::function<void(T)> receiveFunction = [this](T m) {
			ReceiveMessage(m);
		};
		messageToken = CoreEngine::GetCoreEngine().GetMessageSystem().Register<T>(receiveFunction);
	}
	void ReceiveMessage(T msg)
	{
		std::lock_guard<std::mutex> m(messageQueue_m);
		messageQueue.emplace_back(msg);
	}
	std::vector<T> ReadMessages()
	{
		std::lock_guard<std::mutex> m(messageQueue_m);
		auto movedQueue = std::move(messageQueue);
		messageQueue.clear();
		return std::move(movedQueue);
	}
};