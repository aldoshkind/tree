#include "tree_node.h"

/*constructor*/ tree_node_t::tree_node_t(const tree_node_t *parent)
{
	this->parent = parent;
	attached = false;
	destructed = false;
}


/*destructor*/ tree_node_t::~tree_node_t()
{
	destruct();
}

void tree_node_t::destruct()
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
			(*it)->child_removed(this, name);
		}
	}
	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->on_remove(this);
	}
}

tree_node_t *tree_node_t::generate(std::string path)
{
	return get(path, true);
}

tree_node_t *tree_node_t::generate()
{
#warning
	return new tree_node_t(this);
}

typename tree_node_t::children_t::size_type tree_node_t::insert(std::string name, tree_node_t *obj, typename children_t::size_type after)
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

typename tree_node_t::children_t::size_type tree_node_t::insert(std::string name, tree_node_t *obj, std::string after, bool append)
{
	obj->attached = !append;
	return insert(name, obj, find(after));
}

tree_node_t *tree_node_t::attach(std::string path, tree_node_t *obj, bool append)
{
	if(obj == NULL)
	{
		return NULL;
	}

	std::string branch, name;
	extract_last_level_name(path, branch, name);
	tree_node_t *par = get(branch, true);
	if(par == NULL)
	{
		return NULL;
	}

	tree_node_t *item = par->get(name, false);
	if(item == NULL)
	{
		obj->attached = !append;
		par->insert(name, obj);
		item = obj;
	}

	return item;
}

tree_node_t *tree_node_t::at(std::string path)
{
	return get(path, false);
}

const tree_node_t *tree_node_t::at(std::string path) const
{
	return get(path);
}

tree_node_t *tree_node_t::get(std::string path, bool create)
{
	std::string::size_type begin = path.find_first_not_of('/');
	if((begin != 0) && (begin != std::string::npos))
	{
		path = path.substr(begin);
	}

	if(path == "/" || path.size() == 0)
	{
		return this;
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

const tree_node_t *tree_node_t::get(std::string path) const
{
	std::string::size_type begin = path.find_first_not_of('/');
	if((begin != 0) && (begin != std::string::npos))
	{
		path = path.substr(begin);
	}

	if(path == "/" || path.size() == 0)
	{
		return this;
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

int tree_node_t::remove(std::string path, bool recursive)
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

	tree_node_t *child = children[child_id];

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
				(*it)->child_removed(this, name);
			}
			return 0;
		}
	}

	return child->remove(rest_of_path, recursive);
}

bool tree_node_t::is_empty() const
{
	return (children.size() == 0);
}

typename tree_node_t::ls_list_t tree_node_t::ls() const
{
	ls_list_t list;

	for(typename children_t::const_iterator it = children.begin() ; it != children.end() ; ++it)
	{
		list.push_back((*it)->get_name());
	}

	return list;
}

void tree_node_t::set_name(std::string n)
{
	name = n;
}


void tree_node_t::add_listener(listener_t *l, bool recursive)
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


std::string tree_node_t::get_name() const
{
	return name;
}


std::string tree_node_t::get_path() const
{
	if(parent != NULL)
	{
		return parent->get_path() + "/" + get_name();
	}
	return "";
}


void tree_node_t::set_parent(const tree_node_t *parent)
{
	this->parent = parent;
}


const tree_node_t *tree_node_t::get_parent() const
{
	return parent;
}


typename tree_node_t::children_t::size_type tree_node_t::find(std::string name) const
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


typename tree_node_t::children_t tree_node_t::get_children() const
{
	return children;
}
