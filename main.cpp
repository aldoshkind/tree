#include "widget.h"
#include <QApplication>

#include "event_printer.h"

class prop_change_printer : public property_listener
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
		property_base *prop = get_property();
		double val = 0;
		if(prop->get_type() == "d")
		{
			val = dynamic_cast<property<double> *>(prop)->get_value();
		}
		printf("value %s of type %s changed %f\n", prop->get_name().c_str(), prop->get_type().c_str(), val);
	}
};

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Widget w;
	w.show();

	//event_printer l;

	node root;

	w.set_tree(&root);

	//root.add_listener(&l, true);

	node *a = root["a"];

	//a->add_listener(&l, true);

	node *b = (*a)["b"];
	node *c = a->append("c/d/e/f");

	b->append("bb");

	root.print();

	//a->remove("b", true);
	//a->remove("c", true);
	//root.remove("a");

	root.print();

	prop_change_printer lat_listener;
	prop_change_printer lon_listener;

	root["carrier"]->latitude.add_listener(&lat_listener);
	root["carrier"]->longitude.add_listener(&lon_listener);
	root["carrier"]->add_property(new property_value<double>("altitude"));

	root["carrier"]->latitude = 32.1;
	root["carrier"]->longitude = 84.55;
	root["carrier/abc"]->longitude = 84.55;

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
