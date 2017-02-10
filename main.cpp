#include "widget.h"
#include <QApplication>

#include "event_printer.h"

class prop_change_printer : public property_listener<double>
{
public:
	/*constructor*/			prop_change_printer			()
	{
		//
	}

	/*destructor*/			~prop_change_printer		()
	{
		//
	}

	void					updated						()
	{
		printf("value %s changed %f\n", get_property()->get_name().c_str(), get_property()->get_value());
	}
};

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Widget w;
	w.show();

	event_printer l;

	QObject::connect(&l, SIGNAL(signal_child_added(QString)), &w, SLOT(slot_add_item(QString)));
	QObject::connect(&l, SIGNAL(signal_child_removed(QString)), &w, SLOT(slot_remove_item(QString)));

	node root;

	root.add_listener(&l);

	node *a = root["a"];

	a->add_listener(&l);

	node *b = (*a)["b"];
	b->add_listener(&l);
	node *c = (*((*a)["c"]))["/d/e/f"];

	b->append("bb");

	root.print();

	a->remove("b", true);
	a->remove("c", true);
	root.remove("a");

	root.print();

	prop_change_printer lat_listener;
	prop_change_printer lon_listener;

	root["carrier"]->latitude.add_listener(&lat_listener);
	root["carrier"]->longitude.add_listener(&lon_listener);
	root["carrier"]->add_property(new property_value<double>("altitude"));

	root["carrier"]->latitude = 32.1;
	root["carrier"]->longitude = 84.55;

	node *dummy = root["dummy"];
	dummy->add_property(new property_value<double>("test"));

	dummy->latitude = 7;
	dummy->longitude = 99;

	root["foo"]->latitude = 45;
	root["foo"]->longitude = -5;

	root["foo/bar"]->latitude = 666;
	root["foo/bar"]->longitude = 667;

	root.print("/");

	return app.exec();
}
