#pragma once

#include <set>
#include <string>
#include <mutex>

class tree_node;

namespace tree
{



class tree_subtree_listener
{
public:
	tree_subtree_listener() = default;
	virtual ~tree_subtree_listener() = default;

	virtual void subtree_child_added(tree_node */*reporter*/
									 , tree_node */*parent*/
									 , tree_node */*newly_added_child*/
									 , const std::string &/*relative_path*/
									 ){};

    virtual void subtree_child_removed(tree_node */*reporter*/
                                     , tree_node */*parent*/
                                     , tree_node */*newly_added_child*/
                                     , const std::string &/*relative_path*/
                                     ){};
};



class tree_node_listener : public tree_subtree_listener
{
public:
	/*constructor*/ tree_node_listener() {}
	virtual /*destructor*/ ~tree_node_listener();

	virtual void child_added(tree_node *parent, const std::string &name, tree_node *) = 0;
	virtual void child_removed(tree_node *parent, std::string name, tree_node *removed_child) = 0;
	virtual void on_remove(tree_node *) {}


	void add_observable(tree_node *o);
	void remove_observable(tree_node *o);

private:
	std::recursive_mutex observables_mutex;

	typedef std::set<tree_node *> observables_t;
	observables_t observables;
};

}
