#pragma once

#include <map>

#include <QWidget>
#include <QVBoxLayout>
#include <QTreeWidget>

class Widget : public QWidget
{
	Q_OBJECT

	typedef std::map<QString, QTreeWidgetItem *>path_to_item_map_t;
	path_to_item_map_t							path_to_item_map;

	QVBoxLayout			*layout_main;
	QTreeWidget			*tree;

public:
	Widget(QWidget *parent = 0);
	~Widget();

public slots:
	void				slot_add_item			(QString path);
	void				slot_remove_item		(QString path);
};
