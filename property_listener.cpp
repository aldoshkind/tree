#include "property_listener.h"

#include "resource.h"

/*constructor*/ property_listener::property_listener()
{
	//
}

/*destructor*/ property_listener::~property_listener()
{
	std::lock_guard<decltype(mutex)> lock(mutex);
	for(properties_t::iterator it = properties.begin() ; it != properties.end() ; ++it)
	{
		(*it)->remove_listener(this);
	}
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
