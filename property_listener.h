#pragma once

#include <set>

#include <mutex>

class property_base;

namespace tree
{

class observable;

class observable_listener
{
public:
	observable_listener();
	virtual ~observable_listener();

	void add_observable(observable *p);
	void remove_observable(observable *p);
	virtual void updated(observable *prop) = 0;

private:
	typedef std::set<observable *>		observables_t;
	observables_t						observables;

	std::mutex						mutex;
};

}

class property_listener : public tree::observable_listener
{
public:
	/*constructor*/					property_listener			(){}
	virtual /*destructor*/			~property_listener			(){}

	void							add_property				(property_base *p);
	void							remove_property				(property_base *p);
	virtual void					updated						(property_base *prop) = 0;

private:
	void updated(tree::observable *obs);
};
