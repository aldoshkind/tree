#pragma once

#include <typeinfo>

#include "tree_node.h"

std::string demangle(const char *name);

template <class T>
class tree_node_inherited : public tree_node, public T
{
public:
	template <class ... types>
	tree_node_inherited(types ... args) : tree_node(), T(args ...)
	{
		tree_node::set_type(demangle(typeid(T).name()));
	}
	
	using T::operator =;
};
