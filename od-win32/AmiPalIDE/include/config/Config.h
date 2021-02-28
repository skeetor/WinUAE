#pragma once

#include "algorithms/observer.h"

template<typename T, typename... U>
class Config
: public Dispatcher<T, U...>
{
public:
	Config(void) {};
	virtual ~Config(void) {}

	/**
	 * Update should be used instead of assignment, because it only
	 * copies the values, and not the derived objects. It also notifies
	 * the listeners that the config has changed.
	 */
	virtual void update(const T &, U...) = 0;
};
