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
 * \file /Ely/include/PhysicsComponents/Ghost.h
 *
 * \date 13/ago/2014 (11:36:18)
 * \author consultit
 */

#ifndef GHOST_H_
#define GHOST_H_

#include <nodePath.h>
#include "PhysicsComponents/BulletLocal/bulletGhostNode.h"
#include "ObjectModel/Component.h"
#include "ObjectModel/Object.h"
#include "Game/GamePhysicsManager.h"

namespace ely
{
class GhostTemplate;

/**
 * \brief Component representing a ghost object attached to an object.
 *
 * It constructs a ghost object with the single specified collision shape_type
 * along with relevant parameters.\n
 * Collision shapes are:
 * - "sphere"
 * - "plane"
 * - "box"
 * - "cylinder"
 * - "capsule"
 * - "cone"
 * - "heightfield"
 * - "triangle mesh"
 * In case of "sphere", "box", "cylinder", "capsule", "cone", if any of
 * the relevant parameters is missing, the shape is automatically
 * constructed by guessing them through calculation of a tight bounding volume
 * of object geometry (supposedly specified by the model component).\n
 * For "plane" shape, in case of missing parameters, the default is
 * a plane with normal = (0,0,1) and d = 0.\n
 * If specified in "thrown_events", this component throws:
 * - when an object overlaps it, the event "<OverlappingObjectType>_<SUFFIX>",
 * with default <SUFFIX> = <GhostObjectType>_Overlap; this event is thrown
 * continuously at a frequency which is the minimum between the fps and
 * the frequency specified (which defaults to 30 times per seconds) until
 * the object keeps overlapping
 * - when the object stops overlapping it, the event
 * "<OverlappingObjectType>_<SUFFIX>Off"; this event is thrown only once\n
 * The first argument of each event is a reference of the overlapping object's
 * Physics component, the second argument is a reference to this component.\n
 * \note: "event_name" xml parameter is used to specify a <SUFFIX> of the event
 * names.\n
 *
 * XML Param(s):
 * - "thrown_events"			|single|"overlap" (specified as "event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]" with eventX = overlap)
 * - "ghost_type"  				|single|"static" (values: static|dynamic)
 * - "ghost_friction"  			|single|"0.8"
 * - "ghost_restitution"  		|single|"0.1"
 * - "collide_mask"  			|single|"all_on"
 * - "shape_type"  				|single|"sphere" (values: sphere|plane|box|cylinder|capsule|cone|heightfield|triangle mesh)
 * - "shape_size"  				|single|"medium"  (values: minimum|medium|maximum)
 * - "use_shape_of"				|single|no default
 * - "shape_radius"  			|single|no default (for sphere,cylinder,capsule,cone)
 * - "shape_norm_x"  			|single|no default (for plane)
 * - "shape_norm_y"  			|single|no default (for plane)
 * - "shape_norm_z"  			|single|no default (for plane)
 * - "shape_d"  				|single|no default (for plane)
 * - "shape_half_x"  			|single|no default (for box)
 * - "shape_half_y"  			|single|no default (for box)
 * - "shape_half_z"  			|single|no default (for box)
 * - "shape_height"  			|single|"1.0" (for cylinder,capsule,cone,heightfield)
 * - "shape_up"  				|single|"z" (values: x|y|z for cylinder,capsule,cone,heightfield)
 * - "shape_heightfield_file" 	|single|no default (for heightfield)
 * - "shape_scale_w"  			|single|"1.0" (for heightfield)
 * - "shape_scale_d"  			|single|"1.0" (for heightfield)
 * - "ccd_motion_threshold"  	|single|no default
 * - "ccd_swept_sphere_radius" 	|single|no default
 *
 * \note parts inside [] are optional.\n
 */
class Ghost: public Component
{
protected:
	friend class GhostTemplate;

	virtual void reset();
	virtual bool initialize();
	virtual void onAddToObjectSetup();
	virtual void onRemoveFromObjectCleanup();
	virtual void onAddToSceneSetup();
	virtual void onRemoveFromSceneCleanup();

public:
	Ghost();
	Ghost(SMARTPTR(GhostTemplate)tmpl);
	virtual ~Ghost();

	virtual ComponentFamilyType familyType() const;
	virtual ComponentType componentType() const;

	/**
	 * \brief Updates the controlled object.
	 *
	 * Will be called automatically by an physics manager update.
	 * @param data The custom data.
	 */
	virtual void update(void* data);

	/**
	 * \brief The current component's type.
	 *
	 * It may change during the component's lifetime.
	 */
	enum GhostType
	{
		STATIC,
		DYNAMIC
	};

	/**
	 * \brief Switches the current component's type.
	 *
	 * @param ghostType The new component's type.
	 */
	void switchType(GhostType ghostType);

	/**
	 * \brief Gets/sets the node path of this rigid body.
	 */
	///@{
	NodePath getNodePath() const;
	void setNodePath(const NodePath& nodePath);
	///@}

	/**
	 * \name BulletGhostNode reference getter & conversion function.
	 */
	///@{
	BulletGhostNode& getBulletGhostNode();
	operator BulletGhostNode&();
	///@}

	///Ghost thrown events.
	enum EventThrown
	{
		OVERLAP
	};

	/**
	 * \brief Enables/disables the Ghost event to be thrown.
	 * @param event The Ghost event.
	 * @param eventData The Ghost event data. ThrowEventData::mEnable
	 * will enable/disable the event.
	 */
	void enableGhostEvent(EventThrown event, ThrowEventData eventData);

private:
	///The NodePath associated to this rigid body.
	NodePath mNodePath;
	///The underlying BulletGhostNode (read-only after creation & before destruction).
	SMARTPTR(BulletGhostNode) mGhostNode;
	///@{
	///Physical parameters.
	float mGhostFriction, mGhostRestitution;
	GhostType mGhostType;
	GamePhysicsManager::ShapeType mShapeType;
	GamePhysicsManager::ShapeSize mShapeSize;
	BitMask32 mCollideMask;
	//ccd stuff
	float mCcdMotionThreshold, mCcdSweptSphereRadius;
	bool mCcdEnabled;
	/**
	 * \brief Sets physical parameters of a bullet rigid body node (helper function).
	 */
	void doSetPhysicalParameters();
	///@}

	/**
	 * \brief Sets ghost type.
	 * @param ghostType The body type.
	 */
	void doSwitchGhostType(GhostType ghostType);

	///Geometric functions and parameters.
	///@{
	/**
	 * \brief Create a shape given its type.
	 * @param shapeType The shape type.
	 * @return The created shape.
	 */
	SMARTPTR(BulletShape) doCreateShape(GamePhysicsManager::ShapeType shapeType);
	LVecBase3f mModelDims;
	float mModelRadius;
	//use shape of (another object).
	ObjectId mUseShapeOfId;
	//any model has a local frame and the tight bounding box is computed
	//wrt it; so mModelDeltaCenter represents a transform (translation) to
	//be applied to the model node path so that the middle point of the
	//bounding box will overlap the frame center of the parent's node path .
	LVector3f mModelDeltaCenter;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	Filename mHeightfieldFile;
	BulletUpAxis mUpAxis;
	///@}

	/**
	 * \name Throwing Ghost events.
	 */
	///@{
	struct OverlappingNode
	{
		struct OverlappingNodeData
		{
			PandaNode* mPnode;
			std::string mEventName;
			int mCount;
		};
		//main constructors
		OverlappingNode(PandaNode* _pnode) :
				mOverlappingNodeData(new OverlappingNodeData), mRefCount(new int)
		{
			mOverlappingNodeData->mPnode = _pnode;
			*mRefCount = 1;
		}
		OverlappingNode(PandaNode* _pnode, const std::string& _name, int _count) :
				mOverlappingNodeData(new OverlappingNodeData), mRefCount(new int)
		{
			mOverlappingNodeData->mPnode = _pnode;
			mOverlappingNodeData->mEventName = _name;
			mOverlappingNodeData->mCount = _count;
			*mRefCount = 1;
		}
		//copy constructor
		OverlappingNode(const OverlappingNode& on) :
				mOverlappingNodeData(on.mOverlappingNodeData), mRefCount(on.mRefCount)
		{
			++(*mRefCount);
		}
		//destructor
		~OverlappingNode()
		{
			if (--(*mRefCount) == 0)
			{
				delete mOverlappingNodeData;
				delete mRefCount;
			}
		}
		//assignment operator
		OverlappingNode& operator=(const OverlappingNode& on)
		{
			mOverlappingNodeData = on.mOverlappingNodeData;
			++(*mRefCount);
			return *this;
		}
		//comparison operator
		bool operator<(const OverlappingNode& on) const
		{
			return mOverlappingNodeData->mPnode < on.mOverlappingNodeData->mPnode;
		}
		//owned resource
		OverlappingNodeData* mOverlappingNodeData;
	private:
		int* mRefCount;
	};
	ThrowEventData mOverlap;
	std::set<OverlappingNode> mOverlappingNodes;
	///Helper.
	void doEnableGhostEvent(EventThrown event, ThrowEventData eventData);
	///@}

	///TypedObject semantics: hardcoded
public:
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		Component::init_type();
		register_type(_type_handle, "Ghost", Component::get_class_type());
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

inline void Ghost::reset()
{
	//
	mNodePath = NodePath();
	mGhostNode.clear();
	mGhostFriction = mGhostRestitution = 0.0;
	mGhostType = STATIC;
	mShapeType = GamePhysicsManager::SPHERE;
	mShapeSize = GamePhysicsManager::MEDIUM;
	mCollideMask = BitMask32::all_off();
	mCcdMotionThreshold = mCcdSweptSphereRadius = 0.0;
	mCcdEnabled = false;
	mModelDims = LVector3f::zero();
	mModelRadius = 0.0;
	mUseShapeOfId = ObjectId();
	mModelDeltaCenter = LVector3f::zero();
	mAutomaticShaping = false;
	mDim1 = mDim2 = mDim3 = mDim4 = 0.0;
	mHeightfieldFile = Filename();
	mUpAxis = Z_up;
	mOverlap = ThrowEventData();
	mOverlappingNodes.clear();
}

inline NodePath Ghost::getNodePath() const
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	return mNodePath;
}

inline void Ghost::setNodePath(const NodePath& nodePath)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	mNodePath = nodePath;
}

inline BulletGhostNode& Ghost::getBulletGhostNode()
{
	return *mGhostNode;
}

inline Ghost::operator BulletGhostNode&()
{
	return *mGhostNode;
}

inline void Ghost::enableGhostEvent(EventThrown event,
		ThrowEventData eventData)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	doEnableGhostEvent(event, eventData);
}

} /* namespace ely */

#endif /* GHOST_H_ */
