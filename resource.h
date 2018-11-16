#pragma once

#include <vector>
#include <string>
#include <typeinfo>

#include "property_listener.h"

template <class value_t>
class property;

class property_base
{
	typedef std::set<property_listener *>	listeners_t;
	listeners_t								listeners;

	std::string					type;

public:
	/*constructor*/			property_base				()
	{
		//
	}

	virtual /*destructor*/	~property_base				()
	{
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			remove_listener(*it);
		}
	}

	virtual void			add_listener				(property_listener *l)
	{
		listeners.insert(l);
		l->add_property(this);
		l->updated(this);
	}

	virtual void			remove_listener				(property_listener *l)
	{
		listeners.erase(l);
		l->remove_property(this);
	}

	std::string				get_type					() const
	{
		return type;
	}

	void					notify_change				()
	{
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->updated(this);
		}
	}

	virtual void			set_value					(property_base */*prop*/)
	{
		//
	}

protected:
	void					set_type					(std::string type)
	{
		this->type = type;
	}

	const listeners_t		&get_listeners				() const
	{
		return listeners;
	}
};

template <class value_t>
class property : public property_base
{
public:
	/*constructor*/			property					() : property_base()
	{
		set_type(typeid(value_t).name());
	}

	virtual /*destructor*/	~property					()
	{
		//
	}

	virtual value_t			get_value					() const
	{
		return value_t();
	}

	virtual void			set_value					(const value_t &value)
	{
		sync_value(value);
		notify_change();
	}

	virtual void			set_value					(property_base *prop)
	{
		property<value_t> *pop = dynamic_cast<property<value_t> *>(prop);
		if(pop != NULL)
		{
			set_value(*pop);
		}
	}

	virtual void			sync_value					(const value_t &/*value*/)
	{
		//
	}

	virtual void			value_changed				()
	{
		notify_change();
	}

	property				&operator =					(const value_t &v)
	{
		set_value(v);
		return *this;
	}
	/*value_t*/				operator value_t			() const
	{
		return get_value();
	}

	property<value_t>		&operator +=				(const value_t val)
	{
		set_value(get_value() + val);
		return *this;
	}
};

template <class owner_t, class value_t>
class property_get_set : public property<value_t>
{
public:
	typedef void (owner_t::*set_t)(const value_t &_t);
	typedef value_t (owner_t::*get_t)() const;

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
		if(get == NULL)
		{
			return value_t();
		}
		return (owner->*get)();
	}

	void					sync_value					(const value_t &v)
	{
		if(set == NULL)
		{
			return;
		}
		(owner->*set)(v);
	}

	using property<value_t>::operator =;
private:
	get_t					get;
	set_t					set;

	owner_t					*owner;
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

	void					sync_value					(const value_t &v)
	{
		owner->*ptr = v;
	}

	using property<value_t>::operator =;
};





template <class value_t>
class property_value : public property<value_t>
{
	value_t					value;

public:
	/*constructor*/			property_value					() : property<value_t>(), value()
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

	void					sync_value					(const value_t &v)
	{
		value = v;
	}

	using property<value_t>::operator =;
};
