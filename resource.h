#pragma once

#include <set>
#include <vector>
#include <string>
#include <typeinfo>

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

template <class owner_t, class value_t>
class property_get_set : public property<value_t>
{
	typedef void (owner_t::*set_t)(value_t);
	typedef value_t (owner_t::*get_t)() const;

	get_t					get;
	set_t					set;

	owner_t					*owner;

public:
	/*constructor*/			property_get_set			(std::string name, owner_t *owner, get_t g, set_t s) : property<value_t>(name)
	{
		get = g;
		set = s;

		this->owner = owner;
	}

	virtual /*destructor*/	~property_get_set			()
	{
		//
	}

	value_t					get_value						() const
	{
		return (owner->*get)();
	}

	void					set_value					(const value_t &v)
	{
		(owner->*set)(v);
		property<value_t>::notify_change();
	}

	using property<value_t>::operator =;
};






template <class owner_t, class value_t>
class property_value_ptr : public property<value_t>
{
	typedef value_t owner_t::* value_ptr;

	value_ptr				ptr;

	owner_t					*owner;

public:
	/*constructor*/			property_value_ptr			(std::string name, owner_t *owner, value_ptr p) : property<value_t>(name)
	{
		ptr = p;
		this->owner = owner;
	}

	virtual /*destructor*/	~property_value_ptr			()
	{
		//
	}

	value_t					get_value						() const
	{
		return owner->*ptr;
	}

	void					set_value					(const value_t &v)
	{
		owner->*ptr = v;
		property<value_t>::notify_change();
	}

	using property<value_t>::operator =;
};





template <class value_t>
class property_value : public property<value_t>
{
	value_t					value;

public:
	/*constructor*/			property_value					(std::string name) : property<value_t>(name)
	{
		//
	}

	virtual /*destructor*/	~property_value					()
	{
		//
	}

	value_t					get_value						() const
	{
		return value;
	}

	void					set_value					(const value_t &v)
	{
		value = v;
		property<value_t>::notify_change();
	}

	using property<value_t>::operator =;
};



template <class base_t>
class resource
{
	/*constructor*/			resource			(const resource &)
	{
		//
	}

public:
	typedef std::vector<property_base *>			props_t;

	/*constructor*/			resource			()
	{
		//
	}

	virtual /*destructor*/	~resource			()
	{
		//
	}

	props_t					get_properties				() const
	{
		return props;
	}

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
