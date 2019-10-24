#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <list>
#include <string>
#include <limits>
#include <vector>

#include "filepath_utils.h"

class tree_node
{
public:
	class listener_t;
	
	typedef std::list<std::string> string_list_t;
	
	typedef string_list_t children_name_order_t;
	typedef std::map<std::string, tree_node *> children_map_t;

//private:
	tree_node *parent;
	typedef std::map<tree_node *, size_t> parents_t;
	parents_t parents;
	std::string			name;
	bool				owned;	// if true - do destruct

	children_name_order_t								children_name_order;
	children_map_t										children_map;
	
	typedef std::set<listener_t *>						listeners_t;
	listeners_t											listeners;
	listeners_t											recursive_listeners;

	bool				is_empty		() const;

	bool				destructed;

	std::string			type;

protected:
	void				destruct		();
	
	void				add_parent		(tree_node *parent);
	void				remove_parent	(tree_node *parent);
	virtual tree_node			*generate		();

public:
	/*constructor*/		tree_node		(tree_node *parent = NULL);
	virtual /*destructor*/~tree_node	();
	
	void						set_name		(std::string name);

	virtual tree_node			*get			(std::string path, bool create);
	virtual const tree_node		*get			(std::string path) const;

    virtual void                clear_listeners();

    virtual tree_node *          detach			(std::string path);
	virtual tree_node *          detach			(tree_node *);
    virtual tree_node *			attach			(std::string path, tree_node *obj, bool grant_ownership = true);
    virtual int					remove			(std::string path, bool recursive = false);

	virtual tree_node			*at				(std::string path);
	virtual const tree_node		*at				(std::string path) const;
	
	void				set_parent		(tree_node *parent);
	
	tree_node *					operator[]		(std::string path)
	{
		return at(path);
	}
	
	const tree_node *			operator[]		(std::string path) const
	{
		return at(path);
	}

	template<class type_as>
	const type_as		*at_as			(std::string path) const
	{
		return dynamic_cast<type_as *>(at(path));
	}

	template<class type_as>
	type_as		*at_as			(std::string path)
	{
		return dynamic_cast<type_as *>(at(path));
	}

	void set_type(const std::string &type)
	{
		this->type = type;
	}

	std::string get_type() const
	{
		return type;
	}
	
	string_list_t get_names_of(const tree_node *) const;

	virtual bool insert(std::string name, tree_node *obj, bool grant_ownership);

	virtual string_list_t	ls				() const;

	void				add_listener	(listener_t *, bool recursive = false);
	void				remove_listener(listener_t *l, bool recursive = false);

	std::string			get_name		(tree_node *parent = nullptr) const;
	std::string			get_path		() const;

	tree_node *get_parent() const;

	virtual children_map_t get_children() const;

	class listener_t
	{
	public:
		/*constructor*/				listener_t					() {}
		virtual /*destructor*/		~listener_t					() {}
        virtual void				child_added								(tree_node */*parent*/, const std::string &/*name*/, tree_node *) = 0;// {}
		virtual void				child_removed							(tree_node */*parent*/, std::string/* name*/, tree_node */*removed_child*/) = 0; //{}
		virtual void				on_remove								(tree_node *) {}
	};
};









