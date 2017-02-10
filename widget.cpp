#include "widget.h"

#include <stdio.h>

#include "filepath_utils.h"

Widget::Widget(QWidget *parent)
	: QWidget(parent)
{
	layout_main = new QVBoxLayout(this);

	tree = new QTreeWidget(this);

	layout_main->addWidget(tree);
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
	QTreeWidgetItem *parent = NULL;
	for(int i = 0 ; i < pts.size() ; i += 1)
	{
		cur_path += "/" + pts[i];
		if(path_to_item_map.find(cur_path) != path_to_item_map.end())
		{
			parent = path_to_item_map[cur_path];
			if(i == (pts.size() - 1))
			{
				delete parent;
				/*if(parent->parent() != NULL)
				{
					parent->parent()->removeChild(parent);
				}
				else
				{
					delete
				}*/
			}

			continue;
		}
	}

}
