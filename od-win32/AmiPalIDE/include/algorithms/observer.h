#pragma once

/*******************************************************************************
 *
 * SupportLib (c) by Gerhard W. Gruber in 2014
 *
 ******************************************************************************/

// The variadic template mechanism requires at least a C++11 compiler.

#include <vector>
#include "framework.h"

#define ListenerList		std::vector

#ifndef UNUSED
#define UNUSED
#endif

template <typename T, typename... U> class Dispatcher;

template <typename T, typename... U>
class Listener
{
public:
	Listener(void)
	{
	}

	virtual ~Listener(void)
	{
	}

	virtual void handleNotification(T &oSource, U... /* oEvent */)
	{
		// Default implementation does nothing
		// which can be used as a null listener where
		// a listener is expected but doesn't have
		// a meaningful implementation.
		UNUSED(oSource);
	}

	/**
	 * The invalidateDispatcher() call is sent when the dispatcher
	 * should no longer be accessed anymore. After this call, the listener
	 * will no longer receive any notifications and the dispatcher is
	 * destroyed, so the listener should not unregister with
	 * removeListener().
	 */
	virtual void invalidateDispatcher(Dispatcher<T, U...> const *oDispatcher)
	{
		UNUSED(oDispatcher);
	}
};

template <typename T, typename... U>
class Dispatcher
{
public:
	Dispatcher(void)
	{
		mAllowDuplicates = false;
	}

	virtual ~Dispatcher(void)
	{
		invalidate();
	}

	void allowDuplicates(bool bAllowDuplicates = true)
	{
		mAllowDuplicates = bAllowDuplicates;
	}

	/**
	 * After the invalidate() message is sent to the listeners,
	 * they will no longer receive any notifications and they should
	 * no longer access the dispatcher pointer as the object became invalid.
	 * When this call is sent, the listener also shouldn't
	 * unregister via removeListener().
	 */
	virtual void invalidate(void)
	{
		for(Listener<T, U...> * &listener : mListeners)
			listener->invalidateDispatcher(this);
	}

	virtual void notify(U... oEvent)
	{
		for(Listener<T, U...> * &listener : mListeners)
			listener->handleNotification((T &)(*this), oEvent...);
	}

	/**
	 * Adds a listener to the dispatcher. A listener
	 * can attach itself multiple times, in which case
	 * it will receive as many notifications as it
	 * is registered. When the listener is removed
	 * it will remove all instances with a single call
	 * so there is no need to balance the addListener()
	 * with removeListener() calls.
	 */
	virtual void addListener(Listener<T, U...> *oListener)
	{
		if(!mAllowDuplicates)
		{
			if(listenerIndex(oListener) != -1)
				return;
		}

		mListeners.push_back(oListener);
	}

	virtual void removeListener(Listener<T, U...> *oListener)
	{
		// The listener may have registered multiple times
		// so we must remove all instances.
		int i;
		while((i = listenerIndex(oListener)) != -1)
			mListeners.erase(mListeners.begin() + i);
	}

protected:
	ListenerList<Listener<T, U...> *> &getListeners(void) const
	{
		return mListeners;
	}

	virtual int listenerIndex(Listener<T, U...> const *oListener) const
	{
		int i = -1;
		for(Listener<T, U...> * const &listener : mListeners)
		{
			i++;
			if(listener == oListener)
				return i;
		}

		return -1;
	}

 private:
	ListenerList<Listener<T, U...> *> mListeners;
	bool mAllowDuplicates;
};
