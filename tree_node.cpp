#include "tree_node.h"

#include <algorithm>

/*constructor*/ tree_node::tree_node(tree_node *parent)
{
	set_parent(parent);
	owned = false;
	destructed = false;
}


/*destructor*/ tree_node::~tree_node()
{
	printf("%s: %s\n", __func__, get_name().c_str());
	destruct();
}

void tree_node::destruct()
{
	printf("%s: a %s\n", __func__, get_name().c_str());
	if(destructed == true)
	{
		return;
	}
	destructed = true;

	//printf("%s: b %s\n", __func__, get_name().c_str());
	if(get_name() == "jetson")
	{
		//printf("jj\n");
	}
	printf("%s: %s has %d children\n", __func__, get_name().c_str(), children_name_order.size());
	for( ; children_map.size() ; )
	{
		const std::string &name = children_map.begin()->first;
		tree_node *child = children_map.begin()->second;
		if(child->owned == true && child->get_parent() == this)
		{
			child->set_parent(NULL);
			printf("%s: delete own child %s\n", __func__, name.c_str());
			delete child;
			printf("%s: %s's children is of size %d now\n", __func__, name.c_str(), children_name_order.size());
		}
		else
		{
			if(child->get_parent() == this)
			{
				printf("%s: detach child %s\n", __func__, name.c_str());
				child->set_parent(NULL);
			}
			else
			{
				printf("%s: remove parent from %s whose parent is\"%s\"\n", __func__, child->get_name().c_str(), child->get_parent()->get_name().c_str());
				child->remove_parent(this);
			}
			// should we report detached to child?
		}
		//printf("%s: e %s\n", __func__, get_name().c_str());
		for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			//printf("%s: f %s\n", __func__, get_name().c_str());
			(*it)->child_removed(this, name, child);
		}
	}
	printf("%s: processing children of %s done\n", __func__, get_name().c_str());
	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		//printf("%s: h %s\n", __func__, get_name().c_str());
		(*it)->on_remove(this);
	}
	for( ; parents.size() != 0 ; )
	{
		tree_node *p = parents.begin()->first;
		printf("%s: detach from '%s'\n", __func__, p->get_name().c_str());
		p->detach(this);
	}
	printf("%s: %s destructed\n", __func__, get_name().c_str());
}

tree_node *tree_node::generate()
{
	return new tree_node(this);
}

bool tree_node::insert(std::string name, tree_node *obj, bool grant_ownership)
{
	if(children_map.find(name) != children_map.end())
	{
		return false;
	}
	
	children_name_order.push_back(name);
	children_map[name] = obj;
	
	// изменяем признак принаджелности объекта только если он нам принаджелит
	if(grant_ownership == true)
	{
		obj->owned = true;
		obj->set_name(name);
		obj->set_parent(this);
	}
	else if(obj->get_parent() == nullptr)
	{
		obj->set_name(name);
		obj->set_parent(this);
	}
	else
	{
		obj->add_parent(this);
	}

	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->child_added(this, name, obj);
	}

	for(typename listeners_t::iterator it = recursive_listeners.begin() ; it != recursive_listeners.end() ; ++it)
	{
		obj->add_listener(*it, true);
	}
	return true;
}

tree_node *tree_node::attach(std::string path, tree_node *obj, bool grant_ownership)
{
	if(obj == NULL)
	{
		return NULL;
	}

	std::string branch, name;
	extract_last_level_name(path, branch, name);
	tree_node *par = get(branch, true);
	if(par == NULL)
	{
		return NULL;
	}

	tree_node *item = par->get(name, false);
	if(item == NULL)
	{
		par->insert(name, obj, grant_ownership);
		item = obj;
	}

	return item;
}

tree_node *tree_node::at(std::string path)
{
	return get(path, false);
}

const tree_node *tree_node::at(std::string path) const
{
	return get(path);
}

tree_node *tree_node::get(std::string path, bool create)
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

	if(children_map.find(name) == children_map.end())
	{
		if(create == false)
		{
			return nullptr;
		}

		bool ok = insert(name, generate(), true);
		if(ok != true)
		{
			return nullptr;
		}
	}
	return children_map[name]->get(rest_of_path, create);
}

const tree_node *tree_node::get(std::string path) const
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

	if(children_map.find(name) == children_map.end())
	{
		return nullptr;
	}
	return children_map.at(name)->get(rest_of_path);
}

void tree_node::clear_listeners()
{
    listeners.clear();
    recursive_listeners.clear();
}

tree_node *tree_node::detach(std::string name)
{
    // если нет такого потомка, то ошибка
    if(children_map.find(name) == children_map.end())
    {
        return nullptr;
    }

    tree_node *child = children_map[name];
    children_map.erase(name);
	children_name_order.remove(name);
	if(child->get_parent() == this)
	{
		child->set_parent(nullptr);
//		child->clear_listeners();
	}
	else
	{
		child->remove_parent(this);
	}
    for (typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
    {
		(*it)->child_removed(this, child->get_name(), child);
    }
	
	return child;
}

#warning Копипаста метода выше?
tree_node *tree_node::detach(tree_node *child)
{
	//auto found = std::find(children.begin(), children.end(), child);
	auto names = get_names_of(child);
	
    // если нет такого потомка, то ошибка
    if(names.size() == 0)
    {
        return nullptr;
    }

	for(auto &name : names)
	{
		children_map.erase(name);
		children_name_order.remove(name);
		if(child->get_parent() == this)
		{
			child->set_parent(nullptr);
//			child->clear_listeners();
		}
		else
		{
			child->remove_parent(this);
		}
		for (typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
		{
			(*it)->child_removed(this, name, child);
		}
	}
	
	return child;
}

int tree_node::remove(std::string path, bool recursive)
{
	std::string::size_type begin = path.find_first_not_of('/');
	if((begin != 0) && (begin != std::string::npos))
	{
		path = path.substr(begin);
	}

	std::string name, rest_of_path;
	extract_next_level_name(path, name, rest_of_path);

	// если нет такого потомка, то ошибка
    if(children_map.find(name) == children_map.end())
    {
        return -1;
    }

	tree_node *child = children_map[name];

	// если потомок есть, и остальной путь пуст - то удалять потомка
	if(rest_of_path.size() == 0)
	{
		// если удаление рекурсивное, или потомок пустой - то удаляем, иначе ошибка
		if(recursive || (child->is_empty() == true))
		{
			// если хозяин должен удалить потомка и этот потомок наш и не является ссылкой (совпадает имя удаляемого с именем объекта) - удаляем
            if(child->owned == true && child->get_parent() == this && child->get_name() == name)
			{
				// тут он сам удалится у остальных родителей
				delete child;
			}
			// если не нужно удалять, либо потомок не наш, либо это ссылка - забываем его
			else
			{
				child->remove_parent(this);
				children_map.erase(name);
				children_name_order.remove(name);
				for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
				{
					(*it)->child_removed(this, name, child);
				}
			}
			return 0;
		}
	}

	return child->remove(rest_of_path, recursive);
}

bool tree_node::is_empty() const
{
	return (children_map.size() == 0);
}

typename tree_node::string_list_t tree_node::ls() const
{
	return children_name_order;
}

void tree_node::set_name(std::string n)
{
	name = n;
}

void tree_node::add_listener(listener_t *l, bool recursive)
{
	listeners.insert(l);
	if(recursive == true)
	{
		recursive_listeners.insert(l);
	}

	for(auto &ch : children_map)
	{
		l->child_added(this, ch.first, ch.second);
		if(recursive == true)
		{
			ch.second->add_listener(l, recursive);
		}
	}
}
void tree_node::remove_listener(listener_t *l, bool recursive)
{
	listeners.erase(l);
	recursive_listeners.erase(l);
	if (recursive)
	{
		for (auto child : children_map)
		{
			child.second->remove_listener(l, recursive);
		}
	}
}


std::string tree_node::get_name(tree_node *parent) const
{
	if(parents.find(parent) == parents.end())
	{
		return name;
	}
	auto names = parent->get_names_of(this);
	return names.size() > 0 ? names.front() : "";
}


std::string tree_node::get_path() const
{
	if(parent != NULL)
	{
		return parent->get_path() + "/" + get_name();
	}
	return "";
}


void tree_node::set_parent(tree_node *parent)
{
	if(parent != nullptr)
	{
		add_parent(const_cast<tree_node *>(parent));
	}
	else
	{
		remove_parent(const_cast<tree_node *>(this->parent));
	}
	this->parent = parent;
}


tree_node *tree_node::get_parent() const
{
	return parent;
}


typename tree_node::children_map_t tree_node::get_children() const
{
	return children_map;
}

void tree_node::add_parent(tree_node *parent)
{
	parents[parent] += 1;
	if(parent == nullptr)
	{
		printf("%s: replace null parent\n", __func__);
		this->parent = parent;
	}
}

void tree_node::remove_parent(tree_node *parent)
{
	auto it = parents.find(parent);
	if(it != parents.end())
	{
		it->second -= 1;
		if(it->second == 0)
		{
			parents.erase(it);
		}
	}
}

tree_node::string_list_t tree_node::get_names_of(const tree_node *n) const
{
	string_list_t names;
	for(auto i : children_map)
	{
		if(i.second == n)
		{
			names.push_back(i.first);
		}
	}
	
	return names;
}

tree_node::children_name_order_t tree_node::get_children_order() const
{
	return children_name_order;
}
