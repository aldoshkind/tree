#include "widget.h"

#include <stdio.h>

#include "filepath_utils.h"

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	layout_main = new QHBoxLayout(this);
	layout_props = new QVBoxLayout();

	tree = new QTreeWidget(this);

	layout_main->addWidget(tree, 1);
	layout_main->addLayout(layout_props, 1);

	connect(this, SIGNAL(signal_child_added(QString)), this, SLOT(slot_add_item(QString)));
	connect(this, SIGNAL(signal_child_removed(QString)), this, SLOT(slot_remove_item(QString)));
}

Widget::~Widget()
{
	//
}

void Widget::slot_add_item(QString path)
{
	printf("%s %s\n", __func__, path.toStdString().c_str());

	std::string pstd = path.toStdString();
	char *p = (char *)malloc(pstd.size() + 1);
	strncpy(p, pstd.c_str(), pstd.size());
	p[pstd.size()] = 0;
	clean_path(p);
	path = QString(p);
	free(p);

	QStringList pts = path.split('/', QString::SkipEmptyParts);
	QString cur_path;
	QTreeWidgetItem *parent = NULL;
	for(int i = 0 ; i < pts.size() ; i += 1)
	{
		cur_path += "/" + pts[i];
		if(path_to_item_map.find(cur_path) != path_to_item_map.end())
		{
			parent = path_to_item_map[cur_path];
			continue;
		}

		QTreeWidgetItem *current_item = new QTreeWidgetItem();
		current_item->setText(0, pts[i]);
		path_to_item_map[cur_path] = current_item;

		if(parent == NULL)
		{
			tree->addTopLevelItem(current_item);
			parent = current_item;
		}
		else
		{
			parent->addChild(current_item);
			parent = current_item;
		}
	}
}

void Widget::slot_remove_item(QString path)
{
	printf("%s %s\n", __func__, path.toStdString().c_str());

	std::string pstd = path.toStdString();
	char *p = (char *)malloc(pstd.size() + 1);
	strncpy(p, pstd.c_str(), pstd.size());
	p[pstd.size()] = 0;
	clean_path(p);
	path = QString(p);
	free(p);

	QStringList pts = path.split('/', QString::SkipEmptyParts);
	QString cur_path;
	QTreeWidgetItem *item = NULL;
	for(int i = 0 ; i < pts.size() ; i += 1)
	{
		cur_path += "/" + pts[i];
		if(path_to_item_map.find(cur_path) != path_to_item_map.end())
		{
			item = path_to_item_map[cur_path];
			if(i == (pts.size() - 1))
			{
				delete item;
			}

			continue;
		}
	}

}

void Widget::set_tree(node *n)
{
	n->add_listener(this, true);
}

void Widget::child_added(node *n)
{
	emit signal_child_added(QString::fromStdString(n->get_path()));
}

void Widget::child_removed(node *n, std::string name)
{
	emit signal_child_removed(QString::fromStdString(n->get_path() + "/" + name));
}
