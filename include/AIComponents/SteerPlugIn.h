/*
 *   This file is part of Ely.
 *
 *   Ely is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   Ely is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Ely.  If not, see <http://www.gnu.org/licenses/>.
 */
/**
 * \file /Ely/include/AIComponents/SteerPlugIn.h
 *
 * \date 04/dic/2013 (09:11:38)
 * \author consultit
 */
#ifndef STEERPLUGIN_H_
#define STEERPLUGIN_H_

#include "ObjectModel/Component.h"
#include "SteerVehicle.h"
#include <OpenSteer/PlugIn.h>
#include <conditionVar.h>

namespace ely
{

class SteerPlugInTemplate;

/**
 * \brief Component implementing OpenSteer PlugIns.
 *
 * \see http://opensteer.sourceforge.net
 *
 * This component could be used alone or in association with
 * other components.\n
 * Each SteerPlugIn component could handle a single pathway and several
 * obstacles.\n
 * The parent node path of this component's object, will be the reference
 * which any SteerVehicle will be reparented to (if necessary) and which any
 * scene computation will be performed wrt.\n
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *type*				|single| *one_turning* | values: one_turning,pedestrian,boid,multiple_pursuit,soccer,capture_the_flag,low_speed_turn,map_drive
 * | *pathway*			|single|"0.0,0.0,0.0:1.0,1.0,1.0$1.0$false" (specified as "p1,py1,pz1:px2,py2,pz2[:...:pxN,pyN,pzN]$r1[:r2:...:rM]$closedCycle" with M,closedCycle=N-1,false,N,true)
 * | *obstacles*  		|multiple| - | each one specified as "objectId1@shape1@seenFromState1[:objectId2@shape2@seenFromState2:...:objectIdN@shapeN@seenFromStateN]"] with shapeX=sphere,box,plane,rectangle and seenFromStateX=outside,inside,both
 *
 * \note parts inside [] are optional.\n
 */
class SteerPlugIn: public Component
{
protected:
	friend class SteerPlugInTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	SteerPlugIn();
	SteerPlugIn(SMARTPTR(SteerPlugInTemplate)tmpl);
	virtual ~SteerPlugIn();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	struct Result: public Component::Result
	{
		Result(int value):Component::Result(value)
		{
		}
		enum
		{
		};
	};

	/**
	 * \brief Adds a SteerVehicle component to the OpenSteer handling
	 * mechanism.
	 *
	 * If SteerVehicle belongs to any SteerPlugIn it is not added.\n
	 * @param steerVehicle The SteerVehicle to add.
	 * @return Result::OK on successful addition, various error conditions otherwise.
	 */
	Result addSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle);

	/**
	 * \brief Removes a SteerVehicle component from the OpenSteer handling
	 * mechanism.
	 *
	 * If SteerVehicle doesn't belong to any SteerPlugIn it is not removed.\n
	 * @param steerVehicle The SteerVehicle to remove.
	 * @return Result::OK on successful removal, various error conditions otherwise.
	 */
	Result removeSteerVehicle(SMARTPTR(SteerVehicle)steerVehicle);

	/**
	 * \brief Sets the pathway of this SteerPlugin.
	 * @param numOfPoints Number of points.
	 * @param points Points' vector.
	 * @param singleRadius Single radius flag.
	 * @param radii Radii' vector.
	 * @param closedCycle Closed cycle flag.
	 */
	void setPathway(int numOfPoints, LPoint3f const points[], bool singleRadius,
			float const radii[], bool closedCycle);

	/**
	 * \brief Adds an OpenSteer obstacle, seen by all SteerPlugins.
	 *
	 * If the object parameter is not NULL,
	 * @param object The Object used as obstacle.
	 * @param type The obstacle type: box, plane, rectangle, sphere.
	 * @param width Obstacle's width (box, rectangle).
	 * @param height Obstacle's height (box, rectangle).
	 * @param depth Obstacle's depth (box).
	 * @param radius Obstacle's radius (sphere).
	 * @param side Obstacle's right side direction.
	 * @param up Obstacle's up direction.
	 * @param forward Obstacle's forward direction.
	 * @param position Obstacle's position.
	 * @param seenFromState Possible values: outside, inside, both.
	 * @return
	 */
	OpenSteer::AbstractObstacle* addObstacle(SMARTPTR(Object) object,
			const std::string& type, const std::string& seenFromState,
			float width = 0.0, float height = 0.0, float depth = 0.0,
			float radius = 0.0, const LVector3f& side = LVector3f::zero(),
			const LVector3f& up = LVector3f::zero(), const LVector3f& = LVector3f::zero(),
			const LPoint3f& position = LPoint3f::zero());

	/**
	 * \brief Removes an OpenSteer obstacle, seen by all plugins.
	 * @param obstacle The obstacle to remove.
	 */
	void removeObstacle(OpenSteer::AbstractObstacle* obstacle);

	/**
	 * \brief Gets the obstacles, seen by all plugins.
	 * @return The obstacles.
	 */
	OpenSteer::ObstacleGroup getObstacles();

	/**
	 * \brief Updates OpenSteer underlying component.
	 *
	 * Will be called automatically by an ai manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \name AbstractPlugIn reference getter & conversion function.
	 */
	///@{
	OpenSteer::AbstractPlugIn& getAbstractPlugIn();
	operator OpenSteer::AbstractPlugIn&();
	///@}

#ifdef ELY_DEBUG
	/**
	 * \brief Gets a reference to the OpenSteer Drawer3d Debug node path.
	 * @return The OpenSteer Debug node.
	 */
	NodePath getDrawer3dDebugNodePath() const;
	/**
	 * \brief Gets a reference to the OpenSteer Drawer2d Debug node path.
	 * @return The OpenSteer Debug node.
	 */
	NodePath getDrawer2dDebugNodePath() const;
	/**
	 * \brief Enables/disables debugging.
	 * @param enable True to enable, false to disable.
	 */
	Result debug(bool enable);
#endif

#ifdef ELY_THREAD
	/**
	 * \brief Get the Obstacles member reference mutex.
	 * @return The Obstacles mutex.
	 */
	Mutex& getObstaclesMutex();
#endif

private:
	///Current underlying AbstractPlugIn.
	OpenSteer::AbstractPlugIn* mPlugIn;
	///The PlugIn type.
	std::string mPlugInTypeParam;

	///The reference node path (read only after creation).
	NodePath mReferenceNP;

	///Current time.
	float mCurrentTime;

	///The SteerVehicle components handled by this SteerPlugIn.
	std::set<SMARTPTR(SteerVehicle)> mSteerVehicles;

	///The global obstacles handled by all SteerPlugIns.
	static OpenSteer::ObstacleGroup mObstacles;
	///The local obstacles handled by this SteerPlugIn.
	OpenSteer::ObstacleGroup mLocalObstacles;

	/**
	 * \name Helpers variables/functions.
	 */
	///@{
	std::string mPathwayParam;
	void doBuildPathway();
	std::list<std::string> mObstacleListParam;
	void doAddObstacles();
	///@}

#ifdef ELY_DEBUG
	///OpenSteer debug node paths.
	NodePath mDrawer3dNP, mDrawer2dNP;
	///OpenSteer debug camera.
	NodePath mDebugCamera;
	///OpenSteer DebugDrawers.
	DrawMeshDrawer *mDrawer3d, *mDrawer2d;
	///Enable Debug Draw update.
	bool mEnableDebugDrawUpdate;
#endif

#ifdef ELY_THREAD
	///Protect Obstacles reference members (mObstacles, mLocalObstacles).
	///During updates Obstacles are only read, so add/removeObstacle
	///must wait until no update is active.
	static Mutex mObstaclesMutex;
	static ConditionVar mObstaclesVar;
	static unsigned int mUpdateCounter;
#endif

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "SteerPlugIn", Component::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}

private:
	static TypeHandle _type_handle;

};

///inline definitions

inline void SteerPlugIn::reset()
{
	//
	mPlugIn = NULL;
	mPlugInTypeParam.clear();
	mReferenceNP = NodePath();
	mCurrentTime = 0.0;
	mSteerVehicles.clear();
	mPathwayParam.clear();
	mObstacleListParam.clear();
#ifdef ELY_DEBUG
	mDrawer3dNP = NodePath();
	mDrawer2dNP = NodePath();
	mDebugCamera = NodePath();
	mDrawer3d = mDrawer2d = NULL;
	mEnableDebugDrawUpdate = false;
#endif
}

inline OpenSteer::ObstacleGroup SteerPlugIn::getObstacles()
{
	//lock (guard) the obstacles' mutex
	HOLD_MUTEX(mObstaclesMutex)

	return mObstacles;
}

inline OpenSteer::AbstractPlugIn& SteerPlugIn::getAbstractPlugIn()
{
	return *mPlugIn;
}

inline SteerPlugIn::operator OpenSteer::AbstractPlugIn&()
{
	return *mPlugIn;
}

#ifdef ELY_DEBUG
inline NodePath SteerPlugIn::getDrawer3dDebugNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mDrawer3dNP;
}

inline NodePath SteerPlugIn::getDrawer2dDebugNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mDrawer2dNP;
}
#endif

#ifdef ELY_THREAD
inline Mutex& SteerPlugIn::getObstaclesMutex()
{
	return mObstaclesMutex;
}
#endif

} /* namespace ely */

#endif /* STEERPLUGIN_H_ */
