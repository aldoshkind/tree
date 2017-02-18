#ifndef SINUS_UAV_H
#define SINUS_UAV_H

#include <math.h>

#include <QTimer>

#include "node.h"

class sinus_generator : public QObject, public node
{
	Q_OBJECT

	QTimer						*tim;
	double						angle;

public:
	/*constructor*/				sinus_generator				(node *parent = NULL) : node(parent)
	{
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
		latitude = sin(angle);
		longitude = cos(angle);
		angle += 0.001;
	}
};

#endif // SINUS_UAV_H
