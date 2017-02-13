#include "widget.h"

#include <stdio.h>

#include <cxxabi.h>

#include <QLabel>

#include "filepath_utils.h"

static std::string demangle(std::string name)
{
	std::string res = std::string("error demagling type ") + "(" + name + ")";

	int status = -1;
	char *ret = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
	if(ret != NULL)
	{
		res = std::string(ret);
	}
	free(ret);

	return res;
}

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
	connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slot_item_clicked(QTreeWidgetItem*,int)));
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
	root = n;
}

void Widget::child_added(node *n)
{
	emit signal_child_added(QString::fromStdString(n->get_path()));
}

void Widget::child_removed(node *n, std::string name)
{
	emit signal_child_removed(QString::fromStdString(n->get_path() + "/" + name));
}

void Widget::new_property(resource *r, property_base *p)
{
	node *n = dynamic_cast<node *>(r);
	if(n == NULL)
	{
		return;
	}

	QLabel *l = new QLabel(QString::fromStdString(demangle(p->get_type())) + " " + QString::fromStdString(n->get_name() + "/" + p->get_name()) + ";");
	layout_props->addWidget(l);
}

void Widget::slot_item_clicked(QTreeWidgetItem *it, int)
{
	QLayoutItem *item = NULL;
	while((item = layout_props->takeAt(0)) != NULL)
	{
		delete item->widget();
		delete item;
	}
	unsubscribe();

	QString path;
	for( ; ; )
	{
		path = it->text(0) + "/" + path;
		if(it->parent() == NULL)
		{
			break;
		}
		it = it->parent();
	}

	node *n = root->at(path.toStdString());
	if(n == NULL)
	{
		return;
	}
	n->resource::add_listener(this);
}
