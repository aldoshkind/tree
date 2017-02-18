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
		fuel = 0;
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

	root["carrier"]->add_property(new property_value<double>("altitude"));

	root.attach("/test/lol/n", new node);
	root.attach("/test/lol/ext", new extended_node);
	root.attach("/synthetic/sin", new sinus_generator, false);

	node *dummy = root["dummy"];
	dummy->add_property(new property_value<double>("test"));
	property_value<std::string> *str_test = new property_value<std::string>("str_test");
	dummy->add_property(str_test);
	*str_test = "lol";

	root.print("/");

	return app.exec();
}
