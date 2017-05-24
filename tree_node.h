#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <string>
#include <limits>

#include "filepath_utils.h"

template <class T>
class tree_node_t
{
public:
	class listener_t;
	typedef std::vector<std::string>	ls_list_t;
	typedef std::vector<T *>			children_t;

private:
	const tree_node_t	*parent;
	std::string			name;
	bool				attached;	// if true - do not destruct

	children_t											children;

	typedef std::set<listener_t *>						listeners_t;
	listeners_t											listeners;
	listeners_t											recursive_listeners;

	bool				is_empty		() const;
	void				set_name		(std::string name);

	virtual T			*get			(std::string path, bool create);
	virtual const T		*get			(std::string path) const;

	bool				destructed;

protected:
	void				destruct		();
	void				set_parent		(const tree_node_t *parent);
	virtual T			*generate		();

public:
	/*constructor*/		tree_node_t		(const tree_node_t *parent = NULL);
	virtual /*destructor*/~tree_node_t	();

	virtual T			*generate		(std::string path);
	virtual T			*attach			(std::string path, T *obj, bool append = true);
	virtual T			*at				(std::string path);
	virtual const T		*at				(std::string path) const;
	virtual int			remove			(std::string path, bool recursive = false);
	virtual typename children_t::size_type		insert			(std::string name, T *obj, typename children_t::size_type after = std::numeric_limits<typename children_t::size_type>::max());
	virtual typename children_t::size_type		insert			(std::string name, T *obj, std::string after, bool generate = true);

	virtual ls_list_t	ls				() const;

	void				add_listener	(listener_t *, bool recursive = false);

	std::string			get_name		() const;
	std::string			get_path		() const;

	const tree_node_t	*get_parent		() const;

	virtual children_t	get_children	() const;

	// refactor to (bool find(std::string name, size_type id);)
	typename children_t::size_type	find		(std::string name) const;


	class listener_t
	{
	public:
		/*constructor*/				listener_t					() {}
		virtual /*destructor*/		~listener_t					() {}
		virtual void				child_added								(T *) {}
		virtual void				child_removed							(T *, std::string/* name*/) {}
		virtual void				on_remove								(T *) {}
	};
};












template <class T>
/*constructor*/ tree_node_t<T>::tree_node_t(const tree_node_t<T> *parent)
{
	this->parent = parent;
	attached = false;
	destructed = false;
}

template <class T>
/*destructor*/ tree_node_t<T>::~tree_node_t()
{
	destruct();
}

template <class T>
void tree_node_t<T>::destruct()
{
	if(destructed == true)
	{
		return;
	}
	destructed = true;

	for(typename children_t::size_type i = 0 ; i < children.size() ; i += 1)
	{
		if(children[i]->attached == false)
		{
			delete children[i];
		}
		else
		{
			children[i]->set_parent(NULL);
			// should we report detached to child?
		}
		const std::string &name = children[i]->get_name();
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->child_removed(dynamic_cast<T *>(this), name);
		}
	}
	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->on_remove(dynamic_cast<T *>(this));
	}
}

template <class T>
T *tree_node_t<T>::generate(std::string path)
{
	return get(path, true);
}

template <class T>
T *tree_node_t<T>::generate()
{
	return new T(this);
}

template <class T>
typename tree_node_t<T>::children_t::size_type tree_node_t<T>::insert(std::string name, T *obj, typename children_t::size_type after)
{
	if(after >= children.size())
	{
		children.push_back(obj);
	}
	else
	{
		children.insert(children.begin() + after, obj);
	}
	obj->set_parent(this);
	obj->set_name(name);

	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->child_added(obj);
	}

	for(typename listeners_t::iterator it = recursive_listeners.begin() ; it != recursive_listeners.end() ; ++it)
	{
		obj->add_listener(*it, true);
	}
	return children.size() - 1;
}

/*template <class T>
typename tree_node_t<T>::children_t::size_type tree_node_t<T>::insert(std::string name, T *obj, std::string after)
{
	return insert(name, obj, find(after));
}*/

template <class T>
typename tree_node_t<T>::children_t::size_type tree_node_t<T>::insert(std::string name, T *obj, std::string after, bool append)
{
	obj->attached = !append;
	return insert(name, obj, find(after));
}

template <class T>
T *tree_node_t<T>::attach(std::string path, T *obj, bool append)
{
	if(obj == NULL)
	{
		return NULL;
	}

	std::string branch, name;
	extract_last_level_name(path, branch, name);
	T *par = get(branch, true);
	if(par == NULL)
	{
		return NULL;
	}

	T *item = par->get(name, false);
	if(item == NULL)
	{
		obj->attached = !append;
		par->insert(name, obj);
		item = obj;
	}

	return item;
}

template <class T>
T *tree_node_t<T>::at(std::string path)
{
	return get(path, false);
}

template <class T>
const T *tree_node_t<T>::at(std::string path) const
{
	return get(path);
}

template <class T>
T *tree_node_t<T>::get(std::string path, bool create)
{
	std::string::size_type begin = path.find_first_not_of('/');
	if((begin != 0) && (begin != std::string::npos))
	{
		path = path.substr(begin);
	}

	if(path == "/" || path.size() == 0)
	{
		return dynamic_cast<T*>(this);
	}

	std::string name, rest_of_path;
	extract_next_level_name(path, name, rest_of_path);

	typename children_t::size_type child_id = find(name);
	if(child_id >= children.size())
	{
		if(create == false)
		{
			return NULL;
		}

		child_id = insert(name, generate());
	}
	return children[child_id]->get(rest_of_path, create);
}

template <class T>
const T *tree_node_t<T>::get(std::string path) const
{
	std::string::size_type begin = path.find_first_not_of('/');
	if((begin != 0) && (begin != std::string::npos))
	{
		path = path.substr(begin);
	}

	if(path == "/" || path.size() == 0)
	{
		return dynamic_cast<const T*>(this);
	}

	std::string name, rest_of_path;
	extract_next_level_name(path, name, rest_of_path);

	typename children_t::size_type child_id = find(name);
	if(child_id >= children.size())
	{
		return NULL;
	}
	return children[child_id]->get(rest_of_path);
}

template <class T>
int tree_node_t<T>::remove(std::string path, bool recursive)
{
	std::string::size_type begin = path.find_first_not_of('/');
	if((begin != 0) && (begin != std::string::npos))
	{
		path = path.substr(begin);
	}

	std::string name, rest_of_path;
	extract_next_level_name(path, name, rest_of_path);

	typename children_t::size_type child_id = find(name);

	// если нет такого потомка, то ошибка
	if(child_id >= children.size())
	{
		return -1;
	}

	T *child = children[child_id];

	// если потомок есть, и остальной путь пуст - то удалять потомка
	if(rest_of_path.size() == 0)
	{
		// если удаление рекурсивное, или потомок пустой - то удаляем, иначе ошибка
		if(recursive || (child->is_empty() == true))
		{
			if(child->attached == false)
			{
				delete child;
			}
			children.erase(children.begin() + child_id);
			for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
			{
				(*it)->child_removed(dynamic_cast<T *>(this), name);
			}
			return 0;
		}
	}

	return child->remove(rest_of_path, recursive);
}

template <class T>
bool tree_node_t<T>::is_empty() const
{
	return (children.size() == 0);
}

template <class T>
typename tree_node_t<T>::ls_list_t tree_node_t<T>::ls() const
{
	ls_list_t list;

	for(typename children_t::const_iterator it = children.begin() ; it != children.end() ; ++it)
	{
		list.push_back((*it)->get_name());
	}

	return list;
}

template <class T>
void tree_node_t<T>::set_name(std::string n)
{
	name = n;
}

template <class T>
void tree_node_t<T>::add_listener(listener_t *l, bool recursive)
{
	listeners.insert(l);
	if(recursive == true)
	{
		recursive_listeners.insert(l);
	}
	for(typename children_t::iterator it = children.begin() ; it != children.end() ; ++it)
	{
		l->child_added(*it);
		if(recursive == true)
		{
			(*it)->add_listener(l, recursive);
		}
	}
}

template <class T>
std::string tree_node_t<T>::get_name() const
{
	return name;
}

template <class T>
std::string tree_node_t<T>::get_path() const
{
	if(parent != NULL)
	{
		return parent->get_path() + "/" + get_name();
	}
	return "";
}

template <class T>
void tree_node_t<T>::set_parent(const tree_node_t *parent)
{
	this->parent = parent;
}

template <class T>
const tree_node_t<T> *tree_node_t<T>::get_parent() const
{
	return parent;
}

template <class T>
typename tree_node_t<T>::children_t::size_type tree_node_t<T>::find(std::string name) const
{
	for(typename children_t::size_type i = 0 ; i < children.size() ; i += 1)
	{
		if(children[i]->get_name() == name)
		{
			return i;
		}
	}
	return std::numeric_limits<typename children_t::size_type>::max();
}

template <class T>
typename tree_node_t<T>::children_t tree_node_t<T>::get_children() const
{
	return children;
}
