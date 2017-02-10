#include "widget.h"
#include <QApplication>

#include "tree_node.h"

#include "property_system.h"

class node : public resource<node>, public tree_node_t<node>
{
	double		lat;
	double		lon;

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

	/*constructor*/			node			() : latitude("latitude", this, &node::get_lat, &node::set_lat)
												, longitude("longitude", this, &node::lon)
	{
		add_property(&latitude);
		add_property(&longitude);
		latitude = 56.92335;
		longitude = 92.6565;
		printf("constructor %08X\n", this);
	}

	/*constructor*/			node			(const node &n) : latitude("latitude", this, &node::get_lat, &node::set_lat)
															, longitude("longitude", this, &node::lon)
															, lat(n.latitude.get_value())
															, lon(n.longitude.get_value())
	{
		add_property(&latitude);
		add_property(&longitude);
		printf("copy constructor %08X <- %08X\n", this, &n);
	}

	/*destructor*/			~node			()
	{
		//
	}

	using tree_node_t<node>::add_listener;

	void					print			(std::string name = "") const
	{
		props_t props = get_properties();
		for(props_t::size_type i = 0 ; i < props.size() ; i += 1)
		{
			property<double> *prop = dynamic_cast<property<double>* >(props[i]);
			if(prop != NULL)
			{
				printf("%s.%s = %f (%s)\n", name.c_str(), prop->get_name().c_str(), prop->get_value(), prop->get_type().c_str());
			}
		}
		tree_node_t<node>::print(name);
		/*printf("%s.%s = %f\n", name.c_str(), latitude.get_name().c_str(), latitude.get_value());
		printf("%s.%s = %f\n", name.c_str(), longitude.get_name().c_str(), longitude.get_value());
		tree_node_t<node>::print(name);*/
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

	//node n;
	//root["carrier"] = n;

	root["carrier"].latitude.add_listener(&lat_listener);
	root["carrier"].longitude.add_listener(&lon_listener);
	root["carrier"].add_property(new property_value<double>("altitude"));

	root["carrier"].latitude = 32.1;
	root["carrier"].longitude = 84.55;

	node &dummy = root["dummy"];
	dummy.add_property(new property_value<double>("test"));

	dummy.latitude = 7;
	dummy.longitude = 99;

	root["foo"].latitude = 45;
	root["foo"].longitude = -5;

	root["foo/bar"].latitude = 666;
	root["foo/bar"].longitude = 667;

	root.print("/root");

	return 0;

	/*QApplication a(argc, argv);
	Widget w;
	w.show();

	return a.exec();*/
}
