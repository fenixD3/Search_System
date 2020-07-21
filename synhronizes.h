#pragma once

#include <numeric>
#include <future>

using namespace std;

template<typename T>
class Synchronized
{
public:
	explicit Synchronized(T initial = T())
		: value(move(initial))
	{}

	struct Access
	{
		T& ref_to_value;
		lock_guard<mutex> g;
	};

	Access GetAccess()
	{
		return {value, lock_guard(m)};
	}
private:
	T		value;
	mutex	m;
};
