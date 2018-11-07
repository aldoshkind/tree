#pragma once

#include <cxxabi.h>

#include "tree_node.h"

std::string demangle(const char *name)
{
	int status = 0;
	return abi::__cxa_demangle(name, 0, 0, &status);
}

template <class T>
class tree_node_inherited : public tree_node, public T
{
public:
	tree_node_inherited() : tree_node(), T()
	{
		tree_node::set_type(demangle(typeid(T).name()));
	}
};
