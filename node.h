#pragma once

#include "resource.h"
#include "tree_node.h"

class node : public resource, public tree_node_t<node>
{
	/*constructor*/			node			(const node &n) : resource()
															, tree_node_t<node>()
	{
		//
	}

public:
	/*constructor*/			node			(const tree_node_t<node> *parent = NULL) : tree_node_t<node>(parent)
	{
		//
	}

	/*destructor*/			~node			()
	{
		printf("destructor %s\n", get_path().c_str());
	}

	using tree_node_t<node>::add_listener;
};
