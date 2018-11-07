#include <QApplication>

#include "tree_node_inherited.h"
#include "resource.h"

int main(/*int argc, char *argv[]*/)
{
	tree_node root;
	tree_node a;
	root.attach("a", &a, false);
	auto tnid = new tree_node_inherited<property_value<double>>;
	root.attach("b", tnid);

	return 0;
}
