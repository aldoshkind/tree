#pragma once

#include <set>
#include <vector>
#include <string>
#include <typeinfo>

class property_carrier;

template <class value_t>
class property;

template <class value_t>
class property_listener
{
	typedef property<value_t>		prop_t;
	prop_t							*prop;

public:
	/*constructor*/			property_listener			();
	virtual /*destructor*/	~property_listener			();

	void					set_property				(prop_t *p)
	{
		prop = p;
	}

	virtual void			updated						() = 0;

	prop_t					*get_property				() const
	{
		return prop;
	}
};

class property_base
{
public:
	/*constructor*/			property_base				()
	{
		//
	}

	virtual /*destructor*/	~property_base				()
	{
		//
	}
};

template <class value_t>
class property : public property_base
{
	typedef property_listener<value_t>		listener_t;
	typedef std::set<listener_t *>			listeners_t;
	listeners_t								listeners;

	std::string					name;

public:
	/*constructor*/			property					(std::string n) : name(n)
	{
		//
	}

	virtual /*destructor*/	~property					()
	{
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			remove_listener(*it);
		}
	}

	void					add_listener				(listener_t *l)
	{
		listeners.insert(l);
		l->set_property(this);
		l->updated();
	}

	void					remove_listener				(listener_t *l)
	{
		listeners.erase(l);
		l->set_property(NULL);
	}

	virtual value_t			get_value						() const
	{
		return value_t();
	}

	virtual void			set_value					(const value_t &value)
	{
		notify_change();
	}

	std::string				get_name					() const
	{
		return name;
	}

	virtual void			value_changed				()
	{
		notify_change();
	}

	std::string				get_type					()
	{
		return typeid(value_t).name();
	}

	property				&operator =					(const value_t &v)
	{
		set_value(v);
		return *this;
	}

protected:
	void					notify_change				()
	{
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->updated();
		}
	}
};

template <class base_t, class value_t>
class property_get_set : public property<value_t>
{
	typedef void (base_t::*set_t)(value_t);
	typedef value_t (base_t::*get_t)() const;

	get_t get;
	set_t set;

	base_t					*base;

public:
	/*constructor*/			property_get_set			(std::string name, base_t *base, get_t g, set_t s) : property<value_t>(name)
	{
		get = g;
		set = s;

		this->base = base;
	}

	virtual /*destructor*/	~property_get_set			()
	{
		//
	}

	value_t					get_value						() const
	{
		return (base->*get)();
	}

	void					set_value					(const value_t &v)
	{
		(base->*set)(v);
		property<value_t>::notify_change();
	}

	using property<value_t>::operator =;
};






template <class base_t, class value_t>
class property_value_ptr : public property<value_t>
{
	typedef value_t base_t::* value_ptr;

	value_ptr				ptr;

	base_t					*base;

public:
	/*constructor*/			property_value_ptr			(std::string name, base_t *base, value_ptr p) : property<value_t>(name)
	{
		ptr = p;
		this->base = base;
	}

	virtual /*destructor*/	~property_value_ptr			()
	{
		//
	}

	value_t					get_value						() const
	{
		return base->*ptr;
	}

	void					set_value					(const value_t &v)
	{
		base->*ptr = v;
		property<value_t>::notify_change();
	}

	using property<value_t>::operator =;
};








class property_carrier
{
public:
	typedef std::vector<property_base *>			props_t;

	/*constructor*/			property_carrier			()
	{
		//
	}

	virtual /*destructor*/	~property_carrier			()
	{
		//
	}

	props_t					get_properties				() const
	{
		return props;
	}

protected:
	void					add_property				(property_base *p)
	{
		props.push_back(p);
	}

private:
	props_t					props;
};


template <class value_t>
/*constructor*/ property_listener<value_t>::property_listener() : prop(NULL)
{
	//
}

template <class value_t>
/*destructor*/ property_listener<value_t>::~property_listener()
{
	if(prop != NULL)
	{
		prop->remove_listener(this);
	}
}
