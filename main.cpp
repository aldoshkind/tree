#include <QApplication>

#include "tree_node.h"
#include "resource.h"

template <class ... types>
class tree_node_inherited : public tree_node_t, public types...
{
public:
	tree_node_inherited() : tree_node_t(), types()... {}
};


int main(int argc, char *argv[])
{
	tree_node_t root;
	tree_node_t a;
	root.attach("a", &a, false);
	auto tnid = new tree_node_inherited<property_value<double>>;
	root.attach("b", tnid);

	return 0;
}
