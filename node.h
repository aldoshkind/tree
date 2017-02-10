#pragma once

#include "resource.h"
#include "tree_node.h"

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

	/*constructor*/			node			(const node &n) : latitude("latitude", this, &node::get_lat, &node::set_lat)
															, longitude("longitude", this, &node::lon)
															, lat(n.latitude.get_value())
															, lon(n.longitude.get_value())
	{
		add_property(&latitude);
		add_property(&longitude);
		printf("copy constructor %08X <- %08X\n", this, &n);
	}

public:
	property_get_set<node, double> latitude;
	property_value_ptr<node, double> longitude;

	/*constructor*/			node			(const tree_node_t<node> *parent = NULL) : tree_node_t<node>(parent)
																				, latitude("latitude", this, &node::get_lat, &node::set_lat)
																				, longitude("longitude", this, &node::lon)
	{
		add_property(&latitude);
		add_property(&longitude);
		latitude = 56.92335;
		longitude = 92.6565;
		printf("constructor %08X\n", this);
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
	}
};
