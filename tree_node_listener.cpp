#include "tree_node_listener.h"

#include "tree/tree_node.h"

namespace tree
{

/*destructor*/ tree_node_listener::~tree_node_listener()
{
	std::unique_lock<decltype(observables_mutex)> lock(observables_mutex);
	for(auto &o : observables)
	{
		o->remove_listener(this);
	}
}

void tree_node_listener::add_observable(tree_node *o)
{
	std::unique_lock<decltype(observables_mutex)> lock(observables_mutex);
	observables.insert(o);
}

void tree_node_listener::remove_observable(tree_node *o)
{
	std::unique_lock<decltype(observables_mutex)> lock(observables_mutex);
	observables.erase(o);
}

}
