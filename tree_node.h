#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <list>
#include <string>
#include <limits>
#include <vector>
#include <mutex>

#include "filepath_utils.h"

#include "tree_node_listener.h"








using tree_node_listener = tree::tree_node_listener;

class tree_node
{
public:
	typedef std::list<std::string> string_list_t;
	
	typedef string_list_t children_name_order_t;
	typedef std::map<std::string, tree_node *> children_map_t;
	
	typedef std::set<tree_node *> parent_set_t;
	parent_set_t parent_set;

public:
	/*constructor*/ tree_node(tree_node *owner = NULL);
	virtual /*destructor*/ ~tree_node();
	
	void set_name(std::string name);

	parent_set_t get_parents() const;
	
	virtual tree_node *get(std::string path, bool create);
	virtual const tree_node *get(std::string path) const;

    virtual void clear_listeners();

    virtual tree_node *detach(std::string path);
	virtual tree_node *detach(tree_node *);
    virtual tree_node *attach(const std::string &path, tree_node *obj, bool grant_ownership = true);
    virtual int remove(std::string path, bool recursive = false);

	virtual tree_node *at(std::string path);
	virtual const tree_node *at(std::string path) const;
	
	void set_owner(tree_node *owner);
	
	tree_node *operator[](std::string path);
	
	const tree_node *operator[](std::string path) const;

	template<class type_as>
	const type_as *at_as(const std::string &path) const;
	template<class type_as>
	type_as *at_as(const std::string &path);

	void set_type(const std::string &type);
	std::string get_type() const;
	
	string_list_t get_names_of(const tree_node *) const;

	virtual bool insert(const std::string &name, tree_node *obj, bool grant_ownership);

	virtual string_list_t ls() const;

	void add_listener(tree_node_listener *, bool recursive = false);
	void remove_listener(tree_node_listener *l, bool recursive = false);

	std::string get_name(tree_node *owner = nullptr) const;
	std::string get_path() const;

	tree_node *get_owner() const;

	virtual children_map_t get_children() const;
	virtual children_name_order_t get_children_order() const;

//private:
	tree_node *owner = nullptr;
	typedef std::map<tree_node *, size_t> parents_t;
	parents_t parents;
	std::string name;
	bool owned = false;

	children_name_order_t children_name_order;
	children_map_t children_map;
	
	std::recursive_mutex listeners_mutex;
	typedef std::set<tree_node_listener *> listeners_t;
	listeners_t listeners;
	listeners_t recursive_listeners;
	
	bool destructed;

	bool is_empty() const;

	std::string type;

	void subtree_child_added(tree_node *parent, tree_node *child, const std::string &path);

	virtual void set_notify_subtree_enabled(bool enabled);
	
protected:
	void destruct();
	
	void add_parent(tree_node *owner);
	void remove_parent(tree_node *owner);
	virtual tree_node *generate();
	
private:
	void notify_parents_child_added(tree_node *parent, tree_node *child, const std::string &path);

	bool do_notify_subtree_changes = true;
};








template<class type_as>
const type_as *tree_node::at_as(const std::string &path) const
{
	return dynamic_cast<type_as *>(at(path));
}

template<class type_as>
type_as *tree_node::at_as(const std::string &path)
{
	return dynamic_cast<type_as *>(at(path));
}
