#pragma once

#include <vector>
#include <string>
#include <typeinfo>

#include "property_listener.h"

template <class value_t>
class property;

class resource;

class property_base
{
	typedef std::set<property_listener *>	listeners_t;
	listeners_t								listeners;

	std::string					name;
	std::string					type;

	resource					*res;

	void						set_resource			(resource *res)
	{
		this->res = res;
	}

	friend class resource;

public:
	/*constructor*/			property_base				(std::string n) : name(n)
	{
		res = NULL;
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

	std::string				get_name					() const
	{
		return name;
	}

	std::string				get_type					() const
	{
		return type;
	}

	resource				*get_resource				() const
	{
		return res;
	}

	void					notify_change				()
	{
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->updated(this);
		}
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
	/*constructor*/			property					(std::string n) : property_base(n)
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

	void					sync_value					(const value_t &v)
	{
		(owner->*set)(v);
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
	/*constructor*/			property_value					(std::string name) : property<value_t>(name), value()
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



class resource
{
public:
	class new_property_listener;
private:

	typedef std::set<new_property_listener *>	listeners_t;
	listeners_t									listeners;

	/*constructor*/			resource			(const resource &)
	{
		//
	}

public:
	typedef std::vector<property_base *>		props_t;

	class new_property_listener
	{
		typedef std::set<resource *>		resources_t;
		resources_t							resources;
	public:
		/*constructor*/			new_property_listener			()
		{
			//
		}

		virtual /*destructor*/	~new_property_listener			()
		{
			for(resources_t::iterator it = resources.begin() ; it != resources.end() ; ++it)
			{
				(*it)->remove_listener(this);
			}
		}

		void					remove_resource					(resource *r)
		{
			resources.erase(r);
		}

		void					add_resource					(resource *r)
		{
			resources.insert(r);
		}

		virtual void			new_property					(resource *r, property_base *) = 0;
		void					unsubscribe						(resource *r = NULL)
		{
			if(r == NULL)
			{
				for(resources_t::iterator it = resources.begin() ; it != resources.end() ; ++it)
				{
					(*it)->remove_listener(this);
				}
				resources.clear();
			}
			else
			{
				r->remove_listener(this);
				resources.erase(r);
			}
		}
	};

	/*constructor*/			resource			()
	{
		//
	}

	virtual /*destructor*/	~resource			()
	{
		for(listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->remove_resource(this);
		}
	}

	void					remove_listener		(new_property_listener *l)
	{
		listeners.erase(l);
	}

	void					add_listener		(new_property_listener *l)
	{
		listeners.insert(l);
		for(props_t::size_type i = 0 ; i < props.size() ; i += 1)
		{
			l->new_property(this, props[i]);
		}
	}

	props_t					get_properties				() const
	{
		return props;
	}

	virtual property_base	*add_property				(property_base *p)
	{
		props.push_back(p);
		p->set_resource(this);
		for(listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->new_property(this, p);
		}
		return p;
	}

	const property_base		*get_property				(const std::string &name) const
	{
		for(props_t::size_type i = 0 ; i < props.size() ; i += 1)
		{
			const property_base * const &prop = props[i];
			if(prop->get_name() == name)
			{
				return prop;
			}
		}
		return nullptr;
	}

	property_base			*get_property				(const std::string &name)
	{
		for(props_t::size_type i = 0 ; i < props.size() ; i += 1)
		{
			property_base *&prop = props[i];
			if(prop->get_name() == name)
			{
				return prop;
			}
		}
		return nullptr;
	}

private:
	props_t					props;
};
