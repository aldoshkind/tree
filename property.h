#pragma once

#include <vector>
#include <string>
#include <typeinfo>

#include "property_listener.h"

template <class value_t>
class property;

namespace tree
{

class observable
{
public:
	observable(){}
	virtual ~observable()
	{
		std::lock_guard<std::recursive_mutex> lock(listeners_mutex);
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			remove_listener(*it);
		}
	}

	virtual void			add_listener				(observable_listener *l)
	{
		std::lock_guard<std::recursive_mutex> lock(listeners_mutex);
		listeners.insert(l);
		l->add_observable(this);
		l->updated(this);
	}

	virtual void			remove_listener				(observable_listener *l)
	{
		std::lock_guard<std::recursive_mutex> lock(listeners_mutex);
		listeners.erase(l);
		l->remove_observable(this);
	}

	void					notify_change				()
	{
		std::unique_lock<std::mutex> notification_lock(notification_mutex);
		if(is_notification_in_process == true)
		{
			return;
		}
		is_notification_in_process = true;

		std::lock_guard<std::recursive_mutex> lock(listeners_mutex);

		notification_lock.unlock();

		for(const auto listener : listeners)
		{
			listener->updated(this);
		}
		is_notification_in_process = false;
	}

private:
	typedef std::set<observable_listener *>	listeners_t;

protected:
	const listeners_t		&get_listeners				() const
	{
		return listeners;
	}

private:
	listeners_t								listeners;
	std::recursive_mutex					listeners_mutex;

	bool						is_notification_in_process = false;
	std::mutex					notification_mutex;
};

}

class property_base : public tree::observable
{
	std::string					type;

public:
	/*constructor*/			property_base				()
	{
		//
	}

	virtual /*destructor*/	~property_base				()
	{

	}

	std::string				get_type					() const
	{
		return type;
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
};

template <class value_t>
class property : virtual public property_base
{
public:
	typedef value_t value_type;
	
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
class property_get_set : virtual public property<value_t>
{
public:
	typedef void (owner_t::*set_t)(const value_t &_t);
	typedef value_t (owner_t::*get_t)() const;

	/*constructor*/			property_get_set			(owner_t *owner, get_t g, set_t s) : property<value_t>()
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
class property_value_ptr : virtual public property<value_t>
{
	typedef value_t owner_t::* value_ptr;

	value_ptr				ptr;

	owner_t					*owner;

public:
	/*constructor*/			property_value_ptr			(owner_t *owner, value_ptr p) : property<value_t>()
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
class property_value : virtual public property<value_t>
{
	value_t					value;

public:
	/*constructor*/			property_value					(const value_t &val = value_t()) : property<value_t>(), value(val)
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



class enum_base
{

};

template <class base_t>
class enumeration : public enum_base, public base_t
{
public:
	typedef std::set<typename base_t::value_type> allowed_values_t;
	allowed_values_t allowed_values;
	typedef typename base_t::value_type value_type;
	
	template<class ... types>
	enumeration(types ... args) : base_t(args ...)
	{
		//
	}
	
	void set_value(const value_type &v) override
	{
		if(is_acceptable(v))
		{
			base_t::set_value(v);
		}
	}
	
	void			sync_value					(const value_type &v)
	{
		if(is_acceptable(v))
		{
			base_t::sync_value(v);
		}
	}
	
	allowed_values_t get_options() const
	{
		return allowed_values;
	}
	
	void add_option(const value_type &v)
	{
		allowed_values.insert(v);
	}
	
	bool is_acceptable(const value_type &v)
	{
		return allowed_values.find(v) != allowed_values.end();
	}
	
	using base_t::operator =;
};


template <class T>
class property_clamped : public T
{
public:
	typedef typename T::value_type value_type;

	template <class ... types>
	property_clamped(types ... args) : T(args ...)
	{
		//
	}

	using T::operator =;

	void set_value(const value_type &value) override
	{
		value_type tmp = value;
		if(min_set)
		{
			tmp = std::max(tmp, min);
		}
		if(max_set)
		{
			tmp = std::min(tmp, max);
		}
		T::set_value(tmp);
	}

	void set_min(const value_type &m)
	{
		min_set = true;
		min = m;
	}

	void set_max(const value_type &m)
	{
		max_set = true;
		max = m;
	}

private:
	bool min_set = false;
	bool max_set = false;

	value_type min;
	value_type max;
};
