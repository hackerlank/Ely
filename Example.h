/*
 * Example.h
 *
 *  Created on: Mar 25, 2011
 *      Author: marco
 */

#ifndef EXAMPLE_H_
#define EXAMPLE_H_

#include "ExampleApplication.h"

class Example: public ExampleApplication
{
public:
	Example();
	virtual ~Example();

	virtual void createScene();
	virtual void createCamera();
	virtual void createViewports(void);

protected:

};

#endif /* EXAMPLE_H_ */
