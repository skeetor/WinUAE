#pragma once

#include "algorithms/observer.h"
#include "algorithms/serialize.h"

template<typename T, typename... U>
class Config
: public Dispatcher<T, U...>
, public Serialize
{
public:
	Config(const wxString &configName)
	: m_configName(configName)
	{}
	virtual ~Config(void) {}

	wxString const &getConfigName(void) const { return m_configName; }

	/**
	 * Update should be used instead of assignment, because it only
	 * copies the values, and not the derived objects. It also notifies
	 * the listeners that the config has changed.
	 */
	void update(const T &, U...);

private:
	wxString m_configName;
};
