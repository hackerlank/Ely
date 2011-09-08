/**
 * \file GameObject.h
 * \date 08/set/2011
 * \author Marco Paone
 *
 * From "Game Object Component System" page 393 -
 * 		Chris Stoy - "Game Programming Gems 6" (2006)
 * The Game Object Component System (GOCS) attempts to solve the problem
 * related to class explosion and confusion by forcing functionality
 * out of the game object itself and into components that supply very
 * specific functionality and interfaces.
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

/**
 * \brief The Game Object base class.
 *
 * A game object represents the basic entity that can exists in the
 * game world. At the most general level, it consists of a transform
 * representing the location and orientation of the object, a unique
 * identifier, some information defining the properties of the object,
 * and methods used to modify and query the state.
 * At the core of the GOCS is the Game Object (GO). It consists
 * of a Ogre::Node (i.e., position and orientation
 */
class GameObject
{
public:
	GameObject();
	virtual ~GameObject();
};

#endif /* GAMEOBJECT_H_ */
