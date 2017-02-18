#ifndef SINUS_UAV_H
#define SINUS_UAV_H

#include <math.h>

#include <QTimer>

#include "node.h"

class sinus_generator : public QObject, public node
{
	Q_OBJECT

	QTimer						*tim;

public:
	property_value<double>		angle;
	property_value<double>		sinus;
	/*constructor*/				sinus_generator				(node *parent = NULL) : node(parent), angle("angle"), sinus("sinus")
	{
		add_property(&angle);
		add_property(&sinus);

		angle = 0.0;

		tim = new QTimer(this);

		tim->setInterval(50);
		tim->setSingleShot(false);

		connect(tim, SIGNAL(timeout()), this, SLOT(slot_timeout()));

		tim->start();
	}

	/*destructor*/				~sinus_generator				()
	{
		//
	}

public slots:
	void						slot_timeout				()
	{
		sinus = sin(angle);
		angle += 0.001;
	}
};

#endif // SINUS_UAV_H
