#pragma once

#include <set>

#include <mutex>

class property_base;

class property_listener
{
	typedef std::set<property_base *>		properties_t;
	properties_t							properties;

	std::mutex						mutex;

public:
	/*constructor*/					property_listener			();
	virtual /*destructor*/			~property_listener			();

	void							add_property				(property_base *p);
	void							remove_property				(property_base *p);
	virtual void					updated						(property_base *prop) = 0;
};
