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

