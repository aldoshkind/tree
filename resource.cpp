#include "resource.h"

/*constructor*/ property_listener::property_listener() : prop(NULL)
{
	//
}

/*destructor*/ property_listener::~property_listener()
{
	if(prop != NULL)
	{
		prop->remove_listener(this);
	}
}

void property_listener::set_property(property_base *p)
{
	if(prop != NULL)
	{
		prop->remove_listener(this);
	}
	prop = p;
}
