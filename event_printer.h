#pragma once

#include <QObject>

#include "node.h"

class event_printer : public QObject, public node::listener_t
{
	Q_OBJECT

	void on_remove(tree_node_t<node> *n)
	{
		printf("\"%s\" removed under\n", n->get_path().c_str());
	}

	void child_added(tree_node_t<node> *n)
	{
		std::string path = n->get_path();
		printf("\"%s\" added under \"%s\"\n", n->get_name().c_str(), path.c_str());
		emit signal_child_added(QString::fromStdString(path));
	}

	void child_removed(tree_node_t<node> *n, std::string name)
	{
		std::string path = n->get_path();
		printf("\"%s\" removed under \"%s\"\n", name.c_str(), path.c_str());
		emit signal_child_removed(QString::fromStdString(path + "/" + name));
	}
public:
	/*constructor*/				event_printer		()
	{
		//
	}

	/*destructor*/				~event_printer		()
	{
		//
	}

signals:
	void						signal_child_added	(QString path);
	void						signal_child_removed(QString path);
};
