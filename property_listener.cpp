#include "property_listener.h"

#include "property.h"

namespace tree
{

/*constructor*/ observable_listener::observable_listener()
{
	//
}

/*destructor*/ observable_listener::~observable_listener()
{
	mutex.lock();
	for(observables_t::iterator it = observables.begin() ; it != observables.end() ; )
	{
		auto *prop = *it;
		auto next = std::next(it);
		observables.erase(it);
		it = next;
		mutex.unlock();
		prop->remove_listener(this);
		mutex.lock();
	}
	mutex.unlock();
}

void observable_listener::add_observable(observable *p)
{
	std::lock_guard<decltype(mutex)> lock(mutex);
	observables.insert(p);
}

void observable_listener::remove_observable(observable *p)
{
	std::lock_guard<decltype(mutex)> lock(mutex);
	if(p == NULL || observables.find(p) == observables.end())
	{
		return;
	}
	observables.erase(p);
}

}

void property_listener::updated(tree::observable *obs)
{
	auto prop = dynamic_cast<property_base *>(obs);
	if(prop == nullptr)
	{
		return;
	}
	updated(prop);
}

void property_listener::add_property(property_base *p)
{
	add_observable(p);
}

void property_listener::remove_property(property_base *p)
{
	remove_observable(p);
}
