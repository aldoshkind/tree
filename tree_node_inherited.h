#pragma once

#include <typeinfo>

#include "tree_node.h"

std::string demangle(const char *name);

template <class T>
class tree_node_inherited : public tree_node, public T
{
public:
	tree_node_inherited() : tree_node(), T()
	{
		tree_node::set_type(demangle(typeid(T).name()));
	}
};
