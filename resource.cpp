#include "resource.h"

/*constructor*/ property_listener::property_listener()
{
	//
}

/*destructor*/ property_listener::~property_listener()
{
	for(properties_t::iterator it = properties.begin() ; it != properties.end() ; ++it)
	{
		(*it)->remove_listener(this);
	}
}

void property_listener::add_property(property_base *p)
{
	properties.insert(p);
}

void property_listener::remove_property(property_base *p)
{
	if(p == NULL || properties.find(p) == properties.end())
	{
		return;
	}
	properties.erase(p);
}
