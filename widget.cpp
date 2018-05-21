#include "widget.h"

#include <stdio.h>

#include <cxxabi.h>

#include <QLabel>

#include "filepath_utils.h"

Widget::Widget(QWidget *parent) : QWidget(parent)
{
	layout_main = new QHBoxLayout(this);
	layout_props = new QVBoxLayout();

	tree = new QTreeWidget(this);

	layout_main->addWidget(tree, 1);
	layout_main->addLayout(layout_props, 1);

	connect(this, SIGNAL(signal_child_added(QString)), this, SLOT(slot_add_item(QString)));
	connect(this, SIGNAL(signal_child_removed(QString)), this, SLOT(slot_remove_item(QString)));
	connect(tree, SIGNAL(itemClicked(QTreeWidgetItem*,int)), this, SLOT(slot_item_clicked(QTreeWidgetItem*,int)));
	connect(this, SIGNAL(signal_nondouble_property(QString)), this, SLOT(slot_nondouble_property(QString)));
}

Widget::~Widget()
{
	//
}

void Widget::slot_add_item(QString path)
{
	//printf("%s %s\n", __func__, path.toStdString().c_str());

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
	//printf("%s %s\n", __func__, path.toStdString().c_str());

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
				//printf("remove %s\n", cur_path.toStdString().c_str());
				delete item;
				path_to_item_map.erase(cur_path);
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

	if(p->get_type() == "d")
	{
		double_listener *list = new double_listener;
		connect(list, SIGNAL(signal_value(QString,double)), this, SLOT(slot_property_change(QString,double)));
		p->add_listener(list);
		double_listeners.insert(list);
	}
	else if(p->get_type() == typeid(std::string).name())
	{
		using ::property;
		property<std::string> *pr = dynamic_cast<property<std::string> *>(p);
		if(pr == NULL)
		{
			return;
		}
		emit signal_nondouble_property(QString::fromStdString(p->get_name() + " = " + pr->get_value()));
	}
	else
	{
		emit signal_nondouble_property(QString::fromStdString(p->get_type() + " " + p->get_name()));
	}
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

	double_widgets.clear();

	for(double_listeners_t::iterator it = double_listeners.begin() ; it != double_listeners.end() ; ++it)
	{
		delete *it;
	}
	double_listeners.clear();

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

void Widget::slot_property_change(QString name, double value)
{
	if(double_widgets.find(name) == double_widgets.end())
	{
		QLabel *l = new QLabel(this);
		layout_props->addWidget(l);
		double_widgets[name] = l;
	}

	double_widgets[name]->setText(QString("%0 = %1").arg(name).arg(value));
}

void Widget::slot_nondouble_property(QString text)
{
	QLabel *l = new QLabel(this);
	l->setText(text);
	layout_props->addWidget(l);
}











/*constructor*/ double_listener::double_listener()
{
	//
}

/*destructor*/ double_listener::~double_listener()
{
	//
}

void double_listener::updated(property_base *prop)
{
	using ::property;
	property<double> *pb = dynamic_cast<property<double> *>(prop);
	if(pb != NULL)
	{
		emit signal_value(QString::fromStdString(pb->get_name()), pb->get_value());
	}
}
