#include "tree_node.h"

#include <algorithm>

/*constructor*/ tree_node::tree_node(const tree_node *parent)
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
	printf("%s: %s has %d children\n", __func__, get_name().c_str(), children.size());
	for(typename children_t::size_type i = 0 ; i < children.size() ; i += 1)
	{
		tree_node *child = children[i];
		if(child->owned == true && child->get_parent() == this)
		{
			child->set_parent(NULL);
			printf("%s: delete own child %s\n", __func__, child->get_name().c_str());
			delete child;
			printf("%s: %s's children is of size %d now\n", __func__, get_name().c_str(), children.size());
		}
		else
		{
			if(child->get_parent() == this)
			{
				printf("%s: detach child %s\n", __func__, get_name().c_str());
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
		const std::string &name = child->get_name();
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
		tree_node *p = *parents.begin();
		printf("%s: detach from %s\n", __func__, p->get_name().c_str());
		p->detach(this);
	}
	printf("%s: %s destructed\n", __func__, get_name().c_str());
}

/*tree_node *tree_node::generate(std::string path)
{
	return get(path, true);
}*/

tree_node *tree_node::generate()
{
#warning
	return new tree_node(this);
}

typename tree_node::children_t::size_type tree_node::insert(std::string name, tree_node *obj, typename children_t::size_type after)
{
	if(after >= children.size())
	{
		children.push_back(obj);
	}
	else
	{
		children.insert(children.begin() + after, obj);
	}
	if(obj->get_parent() == nullptr)
	{
		obj->set_parent(this);
	}
	
	if(obj->get_parent() == this)
	{
		obj->set_name(name);
	}
	
	if(obj->get_parent() != nullptr && obj->get_parent() != this)
	{
		obj->add_parent(this);
	}

	for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
	{
		(*it)->child_added(this, obj);
	}

	for(typename listeners_t::iterator it = recursive_listeners.begin() ; it != recursive_listeners.end() ; ++it)
	{
		obj->add_listener(*it, true);
	}
	return children.size() - 1;
}

typename tree_node::children_t::size_type tree_node::insert(std::string name, tree_node *obj, std::string after, bool grant_ownership)
{
	obj->owned = grant_ownership;
	return insert(name, obj, find(after));
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
		obj->owned = grant_ownership;
		par->insert(name, obj);
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

	typename children_t::size_type child_id = find(name);
	if(child_id >= children.size())
	{
		return NULL;
	}
	return children[child_id]->get(rest_of_path);
}

void tree_node::clear_listeners()
{
    listeners.clear();
    recursive_listeners.clear();
}

tree_node *tree_node::detach(std::string name)
{
    typename children_t::size_type child_id = find(name);

    // если нет такого потомка, то ошибка
    if(child_id >= children.size())
    {
        return nullptr;
    }

    tree_node* child = children[child_id];
    children.erase(children.begin() + child_id);
	if(child->get_parent() == this)
	{
		child->set_parent(nullptr);
#warning Почему?
		child->clear_listeners();
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
	auto found = std::find(children.begin(), children.end(), child);
	
    // если нет такого потомка, то ошибка
    if(found == children.end())
    {
        return nullptr;
    }

    children.erase(found, found + 1);
	if(child->get_parent() == this)
	{
		child->set_parent(nullptr);
		child->clear_listeners();
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

int tree_node::remove(std::string path, bool recursive)
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

	tree_node *child = children[child_id];

	// если потомок есть, и остальной путь пуст - то удалять потомка
	if(rest_of_path.size() == 0)
	{
		// если удаление рекурсивное, или потомок пустой - то удаляем, иначе ошибка
		if(recursive || (child->is_empty() == true))
		{
            if(child->owned == true && child->get_parent() == this)
			{
				delete child;
			}
			else if(child->get_parent() != this)
			{
				child->remove_parent(this);
				children.erase(children.begin() + child_id);
			}
			for(typename listeners_t::iterator it = listeners.begin() ; it != listeners.end() ; ++it)
			{
				(*it)->child_removed(this, name, child);
			}
			return 0;
		}
	}

	return child->remove(rest_of_path, recursive);
}

bool tree_node::is_empty() const
{
	return (children.size() == 0);
}

typename tree_node::ls_list_t tree_node::ls() const
{
	ls_list_t list;

	for(typename children_t::const_iterator it = children.begin() ; it != children.end() ; ++it)
	{
		list.push_back((*it)->get_name());
	}

	return list;
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
	for(typename children_t::iterator it = children.begin() ; it != children.end() ; ++it)
	{
		l->child_added(this, *it);
		if(recursive == true)
		{
			(*it)->add_listener(l, recursive);
		}
	}
}
void tree_node::remove_listener(listener_t *l, bool recursive)
{
	listeners.erase(l);
	recursive_listeners.erase(l);
	if (recursive)
	{
		for (tree_node* child : children)
		{
			child->remove_listener(l, recursive);
		}
	}
}


std::string tree_node::get_name() const
{
	return name;
}


std::string tree_node::get_path() const
{
	if(parent != NULL)
	{
		return parent->get_path() + "/" + get_name();
	}
	return "";
}


void tree_node::set_parent(const tree_node *parent)
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


const tree_node *tree_node::get_parent() const
{
	return parent;
}


typename tree_node::children_t::size_type tree_node::find(std::string name) const
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

tree_node *tree_node::find(tree_node *child) const
{
	auto found = std::find(children.begin(), children.end(), child);
	
    // если нет такого потомка, то ошибка
    if(found == children.end())
    {
        return nullptr;
    }
	
	return *found;
}


typename tree_node::children_t tree_node::get_children() const
{
	return children;
}

void tree_node::add_parent(tree_node *parent)
{
	parents.insert(parent);
	if(parent == nullptr)
	{
		printf("%s: replace null parent\n", __func__);
		this->parent = parent;
	}
}

void tree_node::remove_parent(tree_node *parent)
{
	parents.erase(parent);
}
