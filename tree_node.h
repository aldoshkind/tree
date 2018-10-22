#pragma once

#include <stdio.h>

#include <map>
#include <set>
#include <string>
#include <limits>
#include <vector>

#include "filepath_utils.h"

class tree_node_t
{
public:
	class listener_t;
	typedef std::vector<std::string>	ls_list_t;
	typedef std::vector<tree_node_t *>			children_t;

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

	virtual tree_node_t			*get			(std::string path, bool create);
	virtual const tree_node_t		*get			(std::string path) const;

	bool				destructed;

protected:
	void				destruct		();
	void				set_parent		(const tree_node_t *parent);
	virtual tree_node_t			*generate		();

public:
	/*constructor*/		tree_node_t		(const tree_node_t *parent = NULL);
	virtual /*destructor*/~tree_node_t	();

	virtual tree_node_t			*generate		(std::string path);
	virtual tree_node_t			*attach			(std::string path, tree_node_t *obj, bool append = true);
	virtual tree_node_t			*at				(std::string path);
	virtual const tree_node_t		*at				(std::string path) const;

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


	virtual int			remove			(std::string path, bool recursive = false);
	virtual typename children_t::size_type		insert			(std::string name, tree_node_t *obj, typename children_t::size_type after = std::numeric_limits<typename children_t::size_type>::max());
	virtual typename children_t::size_type		insert			(std::string name, tree_node_t *obj, std::string after, bool generate = true);

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
		virtual void				child_added								(tree_node_t *) {}
		virtual void				child_removed							(tree_node_t *, std::string/* name*/) {}
		virtual void				on_remove								(tree_node_t *) {}
	};
};









