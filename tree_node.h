#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <string>

#include <QDebug>

#include "filepath_utils.h"

template <class T>
class tree_node_t
{
public:
	class listener_t;
private:

	const tree_node_t	*parent;
	std::string			name;
	bool				attached;

	typedef std::map<std::string, T *>					children_map_t;
	typedef typename children_map_t::value_type			child_item_t;
	children_map_t										children;

	typedef std::set<listener_t *>						listeners_t;
	listeners_t											listeners;
	listeners_t											recursive_listeners;

	bool				is_empty		() const;
	void				set_name		(std::string name);

	void				set_parent		(const tree_node_t *parent);

	T					*get			(std::string path, bool create);
	void				insert			(std::string name, T *obj);

public:
	typedef std::vector<std::string>	ls_list_t;

public:
	/*constructor*/		tree_node_t		(const tree_node_t *parent = NULL);
	virtual /*destructor*/~tree_node_t	();

	T					*append			(std::string path);
	T					*attach			(std::string path, T *obj, bool append = true);
	T					*at				(std::string path);
	int					remove			(std::string path, bool recursive = false);
	//int					get				(std::string path, const T *&node) const;

	ls_list_t			ls				() const;

	T					*operator []	(std::string path);

	virtual void		print			(std::string name = "") const;

	void				add_listener	(listener_t *, bool recursive = false);

	std::string			get_name		() const;
	std::string			get_path		() const;


	class listener_t
	{
	public:
		/*constructor*/				listener_t					()
		{
			//
		}

		virtual /*destructor*/		~listener_t					()
		{
			//
		}

		virtual void				child_added								(T *)
		{
			//
		}

		virtual void				child_removed							(T *, std::string name)
		{
			//
		}

		virtual void				on_remove								(T *)
		{
			//
		}
	};
};












template <class T>
/*constructor*/ tree_node_t<T>::tree_node_t(const tree_node_t<T> *parent)
{
	this->parent = parent;
	attached = false;
}

template <class T>
/*destructor*/ tree_node_t<T>::~tree_node_t()
{
	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->on_remove(dynamic_cast<T *>(this));
	}
	for(typename children_map_t::iterator it = children.begin() ; it != children.end() ; ++it)
	{
		if(it->second->attached == false)
		{
			delete it->second;
		}
		else
		{
			it->second->set_parent(NULL);
			// should we report detached to child?
		}
	}
}

template <class T>
T *tree_node_t<T>::append(std::string path)
{
	return get(path, true);
}

template <class T>
void tree_node_t<T>::insert(std::string name, T *obj)
{
	children[name] = obj;
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
	return get(path, true);
}

template <class T>
T *tree_node_t<T>::get(std::string path, bool create)
{
	if(path[0] == '/')
	{
		path = path.substr(1);
	}

	if(path == "/" || path.size() == 0)
	{
		return dynamic_cast<T*>(this);
	}

	std::string name, rest_of_path;
	extract_next_level_name(path, name, rest_of_path);

	typename children_map_t::const_iterator it = children.find(name);
	if(it == children.end())
	{
		if(create == false)
		{
			return NULL;
		}

		insert(name, new T(this));
	}
	return children[name]->operator[](rest_of_path);
}

template <class T>
int tree_node_t<T>::remove(std::string path, bool recursive)
{
	if(path[0] == '/')
	{
		path = path.substr(1);
	}

	std::string name, rest_of_path;
	extract_next_level_name(path, name, rest_of_path);

	// если нет такого потомка, то ошибка
	if(children.find(name) == children.end())
	{
		return -1;
	}

	// если потомок есть, и остальной путь пуст - то удалять потомка
	if(rest_of_path.size() == 0)
	{
		// если удаление рекурсивное, или потомок пустой - то удаляем, иначе ошибка
		if(recursive || (children[name]->is_empty() == true))
		{
			delete children[name];
			children.erase(name);
			for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
			{
				(*it)->child_removed(dynamic_cast<T *>(this), name);
			}
			return 0;
		}
	}

	return children[name]->remove(rest_of_path, recursive);
}

template <class T>
void tree_node_t<T>::print(std::string name) const
{
	for(typename children_map_t::const_iterator it = children.begin() ; it != children.end() ; ++it)
	{
		it->second->print(name + '/' + it->first);
	}
}

template <class T>
T *tree_node_t<T>::operator [] (std::string path)
{
	return get(path, true);
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

	for(typename children_map_t::const_iterator it = children.begin() ; it != children.end() ; ++it)
	{
		list.push_back(it->first);
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
	for(typename children_map_t::iterator it = children.begin() ; it != children.end() ; ++it)
	{
		l->child_added(it->second);
		if(recursive == true)
		{
			it->second->add_listener(l, recursive);
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
	return "";//"/";
}

template <class T>
void tree_node_t<T>::set_parent(const tree_node_t *parent)
{
	this->parent = parent;
}
