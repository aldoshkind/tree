#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <string>
#include <limits>
#include <vector>

#include "filepath_utils.h"

class tree_node
{
public:
	class listener_t;
	typedef std::vector<std::string>			ls_list_t;
	typedef std::vector<tree_node *>			children_t;

private:
	const tree_node	*parent;
	std::string			name;
	bool				attached;	// if true - do not destruct

	children_t											children;

	typedef std::set<listener_t *>						listeners_t;
	listeners_t											listeners;
	listeners_t											recursive_listeners;

	bool				is_empty		() const;
	void				set_name		(std::string name);

	bool				destructed;

	std::string			type;

protected:
	void				destruct		();
	void				set_parent		(const tree_node *parent);
	virtual tree_node			*generate		();

public:
	/*constructor*/		tree_node		(const tree_node *parent = NULL);
	virtual /*destructor*/~tree_node	();

	virtual tree_node			*get			(std::string path, bool create);
	virtual const tree_node		*get			(std::string path) const;
	//virtual tree_node			*generate		(std::string path);
	virtual tree_node			*attach			(std::string path, tree_node *obj, bool append = true);
	virtual tree_node			*at				(std::string path);
	virtual const tree_node		*at				(std::string path) const;

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


	virtual int			remove			(std::string path, bool recursive = false);
	virtual typename children_t::size_type		insert			(std::string name, tree_node *obj, typename children_t::size_type after = std::numeric_limits<typename children_t::size_type>::max());
	virtual typename children_t::size_type		insert			(std::string name, tree_node *obj, std::string after, bool generate = true);

	virtual ls_list_t	ls				() const;

	void				add_listener	(listener_t *, bool recursive = false);

	std::string			get_name		() const;
	std::string			get_path		() const;

	const tree_node	*get_parent		() const;

	virtual children_t	get_children	() const;

	// refactor to (bool find(std::string name, size_type id);)
	typename children_t::size_type	find		(std::string name) const;


	class listener_t
	{
	public:
		/*constructor*/				listener_t					() {}
		virtual /*destructor*/		~listener_t					() {}
		virtual void				child_added								(tree_node *) {}
		virtual void				child_removed							(tree_node *, std::string/* name*/) {}
		virtual void				on_remove								(tree_node *) {}
	};
};









