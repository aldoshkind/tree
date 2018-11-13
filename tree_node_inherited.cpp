#include "tree_node_inherited.h"

#include <cxxabi.h>

std::string demangle(const char *name)
{
	int status = 0;
	return abi::__cxa_demangle(name, 0, 0, &status);
}
