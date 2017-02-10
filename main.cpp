#include "widget.h"
#include <QApplication>

#include "tree_node.h"

#include "property_system.h"

class tn : public tree_node_t<tn>
{
public:
	/*constructor*/				tn					(const tree_node_t<tn> *parent = NULL) : tree_node_t<tn>(parent)
	{
		//
	}

	/*destructor*/				~tn					()
	{
		//
	}
};

class node : public property_carrier, public tree_node_t<node>
{
	double		lat;
	double		lon;
	double		alt;

	double					get_lat			() const
	{
		return lat;
	}

	void					set_lat			(double lat)
	{
		this->lat = lat;
	}

public:
	property_get_set<node, double> latitude;
	property_value_ptr<node, double> longitude;

	/*constructor*/			node			(const tree_node_t<node> *parent = NULL) : tree_node_t<node>(parent), latitude("latitude", this, &node::get_lat, &node::set_lat), longitude("longitude", this, &node::lon)
	{
		add_property(&latitude);
		latitude = 56.92335;
		longitude = 92.6565;
		printf("constructor %08X\n", this);
	}

	/*constructor*/			node			(const node &n) : latitude("latitude", this, &node::get_lat, &node::set_lat)
															, longitude("longitude", this, &node::lon)
															, lat(n.latitude.get_value())
															, lon(n.longitude.get_value())
	{
		printf("copy constructor %08X <- %08X\n", this, &n);
	}

	/*destructor*/			~node			()
	{
		//
	}

	using tree_node_t<node>::add_listener;

	void					print_payload		() const
	{
		printf("%s = %f\n", latitude.get_name().c_str(), latitude.get_value());
		printf("%s = %f\n", longitude.get_name().c_str(), longitude.get_value());
		printf("payload %08X\n", this);
	}
};

class event_printer : public node::listener_t
{
	void on_remove(tree_node_t<node> *n)
	{
		printf("\"%s\" removed\n", n->get_name().c_str());
	}

	void child_added(tree_node_t<node> *n, std::string name)
	{
		printf("\"%s\" added under \"%s\"\n", name.c_str(), n->get_name().c_str());
	}

	void child_removed(tree_node_t<node> *n, std::string name)
	{
		printf("\"%s\" removed under \"%s\"\n", name.c_str(), n->get_name().c_str());
	}
public:
	/*constructor*/				event_printer		()
	{
		//
	}

	/*destructor*/				~event_printer		()
	{
		//
	}
};

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

int main()
{
	node root;

	/*event_printer l;

	root.add_listener(&l);

	node &a = root["a"];

	a.add_listener(&l);

	node &b = a["b"];
	node &c = a["c"]["/d/e/f"];

	root.print();


	a.remove("b", true);
	a.remove("c", true);
	root.remove("a");

	root.print();*/

	prop_change_printer lat_listener;
	prop_change_printer lon_listener;

	node n;
	root["carrier"] = n;

	root["carrier"].latitude.add_listener(&lat_listener);
	root["carrier"].longitude.add_listener(&lon_listener);

	root["carrier"].latitude = 32.1;
	root["carrier"].longitude = 84.55;

	node &dummy = root["dummy"];

	dummy.latitude = 7;
	dummy.longitude = 99;

	root["foo"].latitude = 45;
	root["foo"].longitude = -5;


	root.print();

	return 0;

	/*QApplication a(argc, argv);
	Widget w;
	w.show();

	return a.exec();*/
}
