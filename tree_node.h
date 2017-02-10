#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <string>

#include <QDebug>

#include "filepath_utils.h"

/*template <class T>
class tree_node_listener_t;*/

template <class T>
class tree_node_t
{
public:
	class listener_t;
private:

	const tree_node_t	*parent;
	std::string			name;

	typedef std::map<std::string, T *>					children_map_t;
	typedef typename children_map_t::value_type			child_item_t;
	children_map_t										children;

	typedef std::set<listener_t *>						listeners_t;
	listeners_t											listeners;

	bool				is_empty		() const;
	void				set_name		(std::string name);

	void				set_parent		(const tree_node_t *parent);

public:
	typedef std::vector<std::string>	ls_list_t;

public:
	/*constructor*/		tree_node_t		(const tree_node_t *parent = NULL);
	virtual /*destructor*/~tree_node_t	();

	T					*append			(std::string path);
	int					remove			(std::string path, bool recursive = false);
	int					get				(std::string path, const T *&node) const;

	ls_list_t			ls				() const;

	T					*operator []	(std::string path);

	virtual void		print			(std::string name = "") const;

	void				add_listener	(listener_t *);

	std::string			get_name		();


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

		virtual void				child_added								(tree_node_t<T> *, std::string name)
		{
			//
		}

		virtual void				child_removed							(tree_node_t<T> *, std::string name)
		{
			//
		}

		virtual void				on_remove								(tree_node_t<T> *)
		{
			//
		}
	};
};












template <class T>
/*constructor*/ tree_node_t<T>::tree_node_t(const tree_node_t<T> *parent)
{
	//this->parent = parent;
	this->parent = NULL;
}

template <class T>
/*destructor*/ tree_node_t<T>::~tree_node_t()
{
	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->on_remove(this);
	}
}

template <class T>
T *tree_node_t<T>::append(std::string path)
{
	return this->operator [](path);
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
		if(recursive || (children[name].is_empty() == true))
		{
			children.erase(name);
			for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
			{
				(*it)->child_removed(this, name);
			}
			return 0;
		}
	}

	return children[name].remove(rest_of_path, recursive);
}

template <class T>
void tree_node_t<T>::print(std::string name) const
{
	for(typename children_map_t::const_iterator it = children.begin() ; it != children.end() ; ++it)
	{
		//it->second->print(name + '/' + it->first);
	}
}

template <class T>
T *tree_node_t<T>::operator [] (std::string path)
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
		children[name] = new T(this);
		children[name]->set_name(name);

		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->child_added(this, name);
		}
	}
	return children[name]->operator[](rest_of_path);
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
int tree_node_t<T>::get(std::string path, const T *&node) const
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

	// если потомок есть, и остальной путь пуст - то возвращать потомка
	if(rest_of_path.size() == 0)
	{
		node = &children.at(name);
		return 0;
	}

	return children.at(name).get(rest_of_path, node);
}

template <class T>
void tree_node_t<T>::set_name(std::string n)
{
	name = n;
}

template <class T>
void tree_node_t<T>::add_listener(listener_t *l)
{
	listeners.insert(l);
}

template <class T>
std::string tree_node_t<T>::get_name()
{
	return name == "" ? "/" : name;
}

template <class T>
void tree_node_t<T>::set_parent(const tree_node_t *parent)
{
	this->parent = parent;
}
