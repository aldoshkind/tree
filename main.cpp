#include <QApplication>

#include "widget.h"
#include "event_printer.h"
#include "sinus_generator.h"

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

class extended_node : public node
{
	double fuel_val;
public:
	property_value_ptr<extended_node, double> fuel;
	property_value<std::string> message;

	/*constructor*/				extended_node				(node *parent = NULL) : node(parent), fuel("fuel", this, &extended_node::fuel_val), message("message")
	{
		add_property(&fuel);
		add_property(&message);
	}
	/*destructor*/				~extended_node				() {}
};

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	Widget w;
	w.show();

	node root;

	w.set_tree(&root);

	node *a = root["a"];
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

	root.append("/test/lol/n", new node);
	root.append("/test/lol/ext", new extended_node);
	root.append("/synthetic/sin", new sinus_generator);

	node *dummy = root["dummy"];
	dummy->add_property(new property_value<double>("test"));
	property_value<std::string> *str_test = new property_value<std::string>("str_test");
	dummy->add_property(str_test);
	*str_test = "lol";

	dummy->latitude = 7;
	dummy->longitude = 99;

	root["foo"]->latitude = 45;
	root["foo"]->longitude = -5;

	root["foo/bar"]->latitude = 666;
	root["foo/bar"]->longitude = 667;

	root.print("/");

	return app.exec();
}
