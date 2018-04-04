#include "property_listener.h"

#include "resource.h"

/*constructor*/ property_listener::property_listener()
{
	//
}

/*destructor*/ property_listener::~property_listener()
{
	mutex.lock();
	for(properties_t::iterator it = properties.begin() ; it != properties.end() ; ++it)
	{
		property_base *prop = *it;
		properties.erase(it);
		mutex.unlock();
		prop->remove_listener(this);
		mutex.lock();
	}
	mutex.unlock();
}

void property_listener::add_property(property_base *p)
{
	std::lock_guard<decltype(mutex)> lock(mutex);
	properties.insert(p);
}

void property_listener::remove_property(property_base *p)
{
	std::lock_guard<decltype(mutex)> lock(mutex);
	if(p == NULL || properties.find(p) == properties.end())
	{
		return;
	}
	properties.erase(p);
}
