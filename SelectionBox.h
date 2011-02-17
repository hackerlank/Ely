/*
 * SelectionBox.h
 *
 *  Created on: 17/feb/2011
 *      Author: marco
 */

#ifndef SELECTIONBOX_H_
#define SELECTIONBOX_H_

#include "OgreManualObject.h"

class SelectionBox: public Ogre::ManualObject
{
public:
	SelectionBox(const Ogre::String& name);
	~SelectionBox(void);

	void setCorners(float left, float top, float right, float bottom);
	void
	setCorners(const Ogre::Vector2& topLeft, const Ogre::Vector2& topRight);

};

#endif /* SELECTIONBOX_H_ */
