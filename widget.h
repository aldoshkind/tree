#pragma once

#include <map>

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QLabel>

#include "node.h"

class double_listener : public QObject, public property_listener
{
	Q_OBJECT

	void					updated						();

public:
	/*constructor*/			double_listener				();
	/*destructor*/			~double_listener			();

signals:
	void					signal_value				(QString value_name, double value);
};

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

	double_listener		dl_lat;
	double_listener		dl_lon;

	QLabel				*label_lat;
	QLabel				*label_lon;

	node				*root;

public:
	Widget(QWidget *parent = 0);
	~Widget();

	void				set_tree				(node *);

public slots:
	void				slot_add_item			(QString path);
	void				slot_remove_item		(QString path);
	void				slot_item_clicked		(QTreeWidgetItem*, int);
	void				slot_property_change	(QString name, double value);

signals:
	void				signal_child_added		(QString path);
	void				signal_child_removed	(QString path);
};
