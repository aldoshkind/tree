#pragma once

#include <map>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>

#include "node.h"

class Widget : public QWidget, public node::listener_t, public resource::new_property_listener
{
	Q_OBJECT

	typedef std::map<QString, QTreeWidgetItem *>path_to_item_map_t;
	path_to_item_map_t							path_to_item_map;

	QHBoxLayout			*layout_main;
	QVBoxLayout			*layout_props;
	QTreeWidget			*tree;

	void				child_added				(node *);
	void				child_removed			(node *, std::string name);
	void				new_property			(resource *, property_base *);

	node				*root;

public:
	Widget(QWidget *parent = 0);
	~Widget();

	void				set_tree				(node *);

public slots:
	void				slot_add_item			(QString path);
	void				slot_remove_item		(QString path);
	void				slot_item_clicked		(QTreeWidgetItem*, int);

signals:
	void				signal_child_added		(QString path);
	void				signal_child_removed	(QString path);
};
