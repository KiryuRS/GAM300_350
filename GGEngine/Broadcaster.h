#pragma once
#include "MetaBoilerplates.h"
#include <memory>
#include <functional>
#include <tuple>

using token = std::shared_ptr<void>;

template<class M>
struct Broadcaster
{
	// f is the type of something that can eat our message:
	using f = std::function< void(M) >;
	std::vector< std::weak_ptr<f> > listeners;

	// Registers a function pointer into the broadcaster for execution
	token Register(f target)
	{
		auto sp = std::make_shared<f>(std::move(target));
		listeners.push_back(sp);
		return sp;
	}

	// remove expired listeners
	void Trim()
	{
		auto func = [](auto&& p) { return p.expired(); };
		auto iter = std::remove_if(begin(listeners), end(listeners), func);
		if(iter != listeners.end())
			listeners.erase(iter, listeners.end() );
	}

	// Sends a message M m to every listener who is not dead:
	void Send(M m)
	{
		Trim(); // remove dead listeners
		auto tmp_copy = listeners;
		for (auto w : tmp_copy)
		{
			auto p = w.lock();
			if (p) (*p)(m);
		}
	}
};

template<class ... Ts>
struct MultiChannelBroadcaster
{
	std::tuple<Broadcaster<Ts>...> stations;

	// helper function that gets a broadcaster compatible
	// with a message type M:
	template<class M>
	Broadcaster<M>& Station()
	{
		return std::get<Broadcaster<M>>(stations);
	}

	// register a message of type M.  You should call with M explicit usually:
	template<class M>
	token Register(std::function<void(M)> listener)
	{
		return Station<M>().Register(std::move(listener));
	}

	// send a message of type M.  You should explicitly pass M usually:
	template<class M>
	void Send(M m)
	{
		Station<M>().Send(std::forward<M>(m));
	}
};