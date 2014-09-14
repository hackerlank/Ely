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
 * \file /Ely/src/PhysicsComponents/Ghost.cpp
 *
 * \date 13/ago/2014 (11:36:18)
 * \author consultit
 */

#include "PhysicsComponents/Ghost.h"
#include "PhysicsComponents/GhostTemplate.h"
#include "ObjectModel/ObjectTemplateManager.h"
#include "PhysicsComponents/RigidBody.h"
#include "SceneComponents/Model.h"
#include "SceneComponents/InstanceOf.h"
#include "SceneComponents/Terrain.h"
#include <throw_event.h>

namespace ely
{

Ghost::Ghost()
{
	// TODO Auto-generated constructor stub
}

Ghost::Ghost(SMARTPTR(GhostTemplate)tmpl)
{
	CHECK_EXISTENCE_DEBUG(GamePhysicsManager::GetSingletonPtr(),
			"Ghost::Ghost: invalid GamePhysicsManager")

	mTmpl = tmpl;
	reset();
}

Ghost::~Ghost()
{
}

ComponentFamilyType Ghost::familyType() const
{
	return mTmpl->familyType();
}

ComponentType Ghost::componentType() const
{
	return mTmpl->componentType();
}

bool Ghost::initialize()
{
	bool result = true;
	//ghost type
	std::string ghostType = mTmpl->parameter(std::string("ghost_type"));
	ghostType == std::string("dynamic") ?
			mGhostType = DYNAMIC :
			mGhostType = STATIC;
	//
	float value;
	//body friction
	value = strtof(mTmpl->parameter(std::string("ghost_friction")).c_str(),
	NULL);
	mGhostFriction = (value >= 0.0 ? value : -value);
	//body restitution
	value = strtof(mTmpl->parameter(std::string("ghost_restitution")).c_str(),
	NULL);
	mGhostRestitution = (value >= 0.0 ? value : -value);
	//shape type
	std::string shapeType = mTmpl->parameter(std::string("shape_type"));
	//shape size
	std::string shapeSize = mTmpl->parameter(std::string("shape_size"));
	if (shapeSize == std::string("minimum"))
	{
		mShapeSize = GamePhysicsManager::MINIMUN;
	}
	else if (shapeSize == std::string("maximum"))
	{
		mShapeSize = GamePhysicsManager::MAXIMUM;
	}
	else
	{
		mShapeSize = GamePhysicsManager::MEDIUM;
	}
	//default auto shaping
	mAutomaticShaping = true;
	if (shapeType == std::string("sphere"))
	{
		mShapeType = GamePhysicsManager::SPHERE;
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		if (not radius.empty())
		{
			mDim1 = strtof(radius.c_str(), NULL);
			if (mDim1 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == std::string("plane"))
	{
		mShapeType = GamePhysicsManager::PLANE;
		std::string norm_x = mTmpl->parameter(std::string("shape_norm_x"));
		std::string norm_y = mTmpl->parameter(std::string("shape_norm_y"));
		std::string norm_z = mTmpl->parameter(std::string("shape_norm_z"));
		std::string d = mTmpl->parameter(std::string("shape_d"));
		if ((not norm_x.empty()) and (not norm_y.empty())
				and (not norm_z.empty()))
		{
			LVector3f normal(strtof(norm_x.c_str(), NULL),
					strtof(norm_y.c_str(), NULL), strtof(norm_z.c_str(), NULL));
			normal.normalize();
			mDim1 = normal.get_x();
			mDim2 = normal.get_y();
			mDim3 = normal.get_z();
			mDim4 = strtof(d.c_str(), NULL);
			if (normal.length() > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == std::string("box"))
	{
		mShapeType = GamePhysicsManager::BOX;
		std::string half_x = mTmpl->parameter(std::string("shape_half_x"));
		std::string half_y = mTmpl->parameter(std::string("shape_half_y"));
		std::string half_z = mTmpl->parameter(std::string("shape_half_z"));
		if ((not half_x.empty()) and (not half_y.empty())
				and (not half_z.empty()))
		{
			mDim1 = strtof(half_x.c_str(), NULL);
			mDim2 = strtof(half_y.c_str(), NULL);
			mDim3 = strtof(half_z.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == std::string("cylinder")
			or shapeType == std::string("capsule")
			or shapeType == std::string("cone"))
	{
		if (shapeType == std::string("cylinder"))
		{
			mShapeType = GamePhysicsManager::CYLINDER;
		}
		else if (shapeType == std::string("capsule"))
		{
			mShapeType = GamePhysicsManager::CAPSULE;
		}
		else
		{
			mShapeType = GamePhysicsManager::CONE;
		}
		std::string radius = mTmpl->parameter(std::string("shape_radius"));
		std::string height = mTmpl->parameter(std::string("shape_height"));
		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
		if ((not radius.empty()) and (not height.empty()))
		{
			mDim1 = strtof(radius.c_str(), NULL);
			mDim2 = strtof(height.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
		if (upAxis == std::string("x"))
		{
			mUpAxis = X_up;
		}
		else if (upAxis == std::string("y"))
		{
			mUpAxis = Y_up;
		}
		else
		{
			mUpAxis = Z_up;
		}
	}
	else if (shapeType == std::string("heightfield"))
	{
		mShapeType = GamePhysicsManager::HEIGHTFIELD;
		std::string heightfield_file = mTmpl->parameter(
				std::string("shape_heightfield_file"));
		mHeightfieldFile = Filename(heightfield_file);
		std::string height = mTmpl->parameter(std::string("shape_height"));
		std::string scale_w = mTmpl->parameter(std::string("shape_scale_w"));
		std::string scale_d = mTmpl->parameter(std::string("shape_scale_d"));
		std::string upAxis = mTmpl->parameter(std::string("shape_up"));
		if ((not height.empty()) and (not scale_w.empty())
				and (not scale_d.empty()))
		{
			mDim1 = strtof(height.c_str(), NULL);
			mDim2 = strtof(scale_w.c_str(), NULL);
			mDim3 = strtof(scale_d.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
		if (upAxis == std::string("x"))
		{
			mUpAxis = X_up;
		}
		else if (upAxis == std::string("y"))
		{
			mUpAxis = Y_up;
		}
		else
		{
			mUpAxis = Z_up;
		}
	}
	else if (shapeType == std::string("triangle_mesh"))
	{
		mShapeType = GamePhysicsManager::TRIANGLEMESH;
	}
	else
	{
		//default a sphere (with auto shaping)
		mShapeType = GamePhysicsManager::SPHERE;
	}
	//collide mask
	std::string collideMask = mTmpl->parameter(std::string("collide_mask"));
	if (collideMask == std::string("all_on"))
	{
		mCollideMask = BitMask32::all_on();
	}
	else if (collideMask == std::string("all_off"))
	{
		mCollideMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(collideMask.c_str(), NULL, 0);
		mCollideMask.set_word(mask);
#ifdef ELY_DEBUG
		mCollideMask.write(std::cout, 0);
#endif
	}
	//ccd settings: enabled if both are greater than zero (> 0.0)
	//ccd motion threshold
	value = strtof(
			mTmpl->parameter(std::string("ccd_motion_threshold")).c_str(),
			NULL);
	mCcdMotionThreshold = (value >= 0.0 ? value : -value);
	//ccd swept sphere radius
	value = strtof(
			mTmpl->parameter(std::string("ccd_swept_sphere_radius")).c_str(),
			NULL);
	mCcdSweptSphereRadius = (value >= 0.0 ? value : -value);
	((mCcdMotionThreshold > 0.0) and (mCcdSweptSphereRadius > 0.0)) ?
			mCcdEnabled = true : mCcdEnabled = false;
	//use shape of (another object)
	mUseShapeOfId = ObjectId(mTmpl->parameter(std::string("use_shape_of")));
	//
	return result;
}

void Ghost::onAddToObjectSetup()
{
	//create a Ghost Node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
	std::string name = COMPONENT_STANDARD_NAME;
	mGhostNode = new BulletGhostNode(name.c_str());
	//add to table of all physics components indexed by
	//(underlying) Bullet PandaNodes
	GamePhysicsManager::GetSingletonPtr()->setPhysicsComponentByPandaNode(
			mGhostNode.p(), this);
	//set the physics parameters
	doSetPhysicalParameters();

	//At this point a Scene component (Model, InstanceOf ...) should have
	//been already created and added to the object, so its node path should
	//be the same as the object's one.
	//Note: scaling is applied to a Scene component, so the object node path
	//has scaling already applied.

	//create and add a Collision Shape
	mGhostNode->add_shape(doCreateShape(mShapeType));
	//set ghost type settings
	doSwitchGhostType(mGhostType);

	//create a node path for the rigid body
	mNodePath = NodePath(mGhostNode);

	//attach to Bullet World
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(
			mGhostNode);

	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	NodePath ownerNodePath = mOwnerObject->getNodePath();
	if (not ownerNodePath.is_empty())
	{
		//reparent the object node path as a child of the rigid body's one
		ownerNodePath.reparent_to(mNodePath);
		if (mShapeType != GamePhysicsManager::TRIANGLEMESH) //Hack
		{
			//correct (or possibly reset to zero) pos and hpr of the object node path
			ownerNodePath.set_pos_hpr(mModelDeltaCenter, LVecBase3::zero());
		}

		//BulletShape::set_local_scale doesn't work anymore
		//see: https://www.panda3d.org/forums/viewtopic.php?f=9&t=10231&start=690#p93583
		if (mShapeType == GamePhysicsManager::HEIGHTFIELD)	//Hack
		{
			//check if there is already a scene component
			SMARTPTR(Component)sceneComp = mOwnerObject->getComponent(ComponentFamilyType("Scene"));
			if (sceneComp)
			{
				//check if the scene component is a Terrain
				if (sceneComp->componentType() == ComponentType("Terrain"))
				{
					float widthScale = DCAST(Terrain, sceneComp)->getWidthScale();
					float heightScale = DCAST(Terrain, sceneComp)->getHeightScale();
					if (mUpAxis == X_up)
					{
						mNodePath.set_scale(LVecBase3f(
								heightScale, widthScale, widthScale));
					}
					else if (mUpAxis == Y_up)
					{
						mNodePath.set_scale(LVecBase3f(
								widthScale, heightScale, widthScale));
					}
					else //mUpAxis == Z_up
					{
						mNodePath.set_scale(LVecBase3f(
								widthScale, widthScale, heightScale));
					}
				}
			}
			else
			{
				//no scene component: scale manually if requested
				if (not mAutomaticShaping)
				{
					if (mUpAxis == X_up)
					{
						mNodePath.set_scale(mDim1, mDim2, mDim3);
					}
					else if (mUpAxis == Y_up)
					{
						mNodePath.set_scale(mDim3, mDim1, mDim2);
					}
					else
					{
						mNodePath.set_scale(mDim2, mDim3, mDim1);
					}
				}
			}
		}
		//optimize
		mNodePath.flatten_strong();
	}
	else
	{
		//when ownerNodePath is empty: every rigid body has a shape but
		//HEIGHTFIELD, which should have a chance to scale
		if (mShapeType == GamePhysicsManager::HEIGHTFIELD)	//Hack
		{
			if (not mAutomaticShaping)
			{
				if (mUpAxis == X_up)
				{
					mNodePath.set_scale(mDim1, mDim2, mDim3);
				}
				else if (mUpAxis == Y_up)
				{
					mNodePath.set_scale(mDim3, mDim1, mDim2);
				}
				else
				{
					mNodePath.set_scale(mDim2, mDim3, mDim1);
				}
			}
		}
	}

	//ghost objects (and the entire subgraph below them) are usually hidden
	mNodePath.hide();

	//set this rigid body node path as the object's one
	mOwnerObject->setNodePath(mNodePath);

	//set thrown events if any
	std::string param;
	unsigned int idx1, valueNum1;
	std::vector<std::string> paramValuesStr1, paramValuesStr2;
	param = mTmpl->parameter(std::string("thrown_events"));
	if (param != std::string(""))
	{
		//events specified
		//event1@[event_name1]@[frequency1][:...[:eventN@[event_nameN]@[frequencyN]]]
		paramValuesStr1 = parseCompoundString(param, ':');
		valueNum1 = paramValuesStr1.size();
		for (idx1 = 0; idx1 < valueNum1; ++idx1)
		{
			//eventX@[event_nameX]@[frequencyX]
			paramValuesStr2 = parseCompoundString(paramValuesStr1[idx1], '@');
			if (paramValuesStr2.size() >= 3)
			{
				EventThrown event;
				ThrowEventData eventData;
				//get default name prefix
				std::string objectType = std::string(
						mOwnerObject->objectTmpl()->objectType());
				//get name
				std::string name = paramValuesStr2[1];
				//get frequency
				float frequency = strtof(paramValuesStr2[2].c_str(), NULL);
				if (frequency <= 0.0)
				{
					frequency = 30.0;
				}
				//get event
				if (paramValuesStr2[0] == "overlap")
				{
					event = OVERLAP;
					//set default suffix name
					if (name == "")
					{
						//set default suffix name
						name = objectType + "_Overlap";
					}
				}
				else
				{
					//paramValuesStr2[0] is not a suitable event:
					//continue with the next event
					continue;
				}
				//set event data
				eventData.mEnable = true;
				eventData.mEventName = name;
				eventData.mTimeElapsed = 0;
				eventData.mFrequency = frequency;
				//enable the event
				doEnableGhostEvent(event, eventData);
			}
		}
	}
}

void Ghost::onRemoveFromObjectCleanup()
{
	NodePath oldObjectNodePath;
	//set the object node path to the first child of rigid body's one (if any)
	if (mNodePath.get_num_children() > 0)
	{
		oldObjectNodePath = mNodePath.get_child(0);
		//detach the object node path from the rigid body's one
		oldObjectNodePath.detach_node();
	}
	else
	{
		oldObjectNodePath = NodePath();
	}
	//set the object node path to the old one
	mOwnerObject->setNodePath(oldObjectNodePath);

	//remove from table of all physics components indexed by
	//(underlying) Bullet PandaNodes
	GamePhysicsManager::GetSingletonPtr()->setPhysicsComponentByPandaNode(
			mGhostNode.p(), NULL);

	//remove rigid body from the physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
			mGhostNode);

	//Remove node path
	mNodePath.remove_node();
	//
	reset();
}

void Ghost::onAddToSceneSetup()
{
	//switch the ghost type (take precedence over mass)
	//force this component to static if owner object is static
	if (mOwnerObject->isSteady())
	{
		mGhostType = STATIC;
	}
	//
	doSwitchGhostType(mGhostType);
	//Add to the physics manager update
	GamePhysicsManager::GetSingletonPtr()->addToPhysicsUpdate(this);
}

void Ghost::onRemoveFromSceneCleanup()
{
	//remove from the physics manager update
	GamePhysicsManager::GetSingletonPtr()->removeFromPhysicsUpdate(this);
}

void Ghost::switchType(GhostType ghostType)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying,)

	doSwitchGhostType(ghostType);
}

void Ghost::doSwitchGhostType(GhostType ghostType)
{
	switch (ghostType)
	{
	case DYNAMIC:
		mGhostNode->set_static(false);
		mGhostNode->set_deactivation_enabled(true);
		mGhostNode->set_active(true);
		break;
	case STATIC:
		mGhostNode->set_static(true);
		mGhostNode->set_deactivation_enabled(true);
		mGhostNode->set_active(false);
		break;
	default:
		break;
	}
}
void Ghost::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//handle events
	if (mOverlap.mEnable)
	{
		//update general count:
		//only actual overlapping objects have their count updated,
		//while just gone out objects will be erased from the set
		++mOverlap.mCount;

		//elaborate current overlapping object list
		if (mGhostNode->get_num_overlapping_nodes() > 0)
		{
			//update elapsed time
			mOverlap.mTimeElapsed += ClockObject::get_global_clock()->get_dt();
			for (int i = 0; i < mGhostNode->get_num_overlapping_nodes(); ++i)
			{
				SMARTPTR(Component)physicsComponent = GamePhysicsManager::GetSingletonPtr()->getPhysicsComponentByPandaNode(
						mGhostNode->get_overlapping_node(i));
				//insert a default: check of equality is done only on OverlapNodeData::mPnode member
				pair<std::set<OverlappingNode>::iterator, bool> res =
				mOverlappingNodes.insert(
						OverlappingNode(
								mGhostNode->get_overlapping_node(i)));
				if (res.second)
				{
					//this is a "new" overlapping object
					//event name: <OverlappingObjectType>_<GhostObjectType>_Overlap
					(res.first)->mOverlappingNodeData->mEventName =
						physicsComponent->getOwnerObject()->objectTmpl()->objectType()
						+ "_" + mOverlap.mEventName;
					//throw the event
					throw_event((res.first)->mOverlappingNodeData->mEventName,
							EventParameter(physicsComponent), EventParameter(this));
				}
				else
				{
					//this is an "old" overlapping object
					if (mOverlap.mTimeElapsed >= mOverlap.mPeriod)
					{
						//throw the event
						throw_event((res.first)->mOverlappingNodeData->mEventName,
								EventParameter(physicsComponent), EventParameter(this));
					}
				}
				//update count flag
				(res.first)->mOverlappingNodeData->mCount = mOverlap.mCount;
			}
			//update elapsed time
			if (mOverlap.mTimeElapsed >= mOverlap.mPeriod)
			{
				mOverlap.mTimeElapsed -= mOverlap.mPeriod;
			}
		}
		else
		{
			mOverlap.mTimeElapsed = 0.0;
		}

		//erase gone "out" objects (which have not the count flag updated)
		for (std::set<OverlappingNode>::iterator i = mOverlappingNodes.begin(); i != mOverlappingNodes.end();)
		{
			//check if it has a previous count
			if (i->mOverlappingNodeData->mCount != mOverlap.mCount)
			{
				SMARTPTR(Component)physicsComponent = GamePhysicsManager::GetSingletonPtr()->getPhysicsComponentByPandaNode(
						i->mOverlappingNodeData->mPnode);
				//throw the "off" event
				throw_event(i->mOverlappingNodeData->mEventName + "Off",
						EventParameter(physicsComponent), EventParameter(this));
				//erase the object
				mOverlappingNodes.erase(i++);
			}
			else
			{
				++i;
			}
		}
	}
}

SMARTPTR(BulletShape)Ghost::doCreateShape(GamePhysicsManager::ShapeType shapeType)
{
	//check if it should use shape of another (already) created object
	if (not mUseShapeOfId.empty())
	{
		SMARTPTR(Object)createdObject =
		ObjectTemplateManager::GetSingleton().getCreatedObject(
				mUseShapeOfId);
		if (createdObject)
		{
			//object already exists
			SMARTPTR(Component) physicsComp =
			createdObject->getComponent(ComponentFamilyType("Physics"));
			if(physicsComp)
			{
				if (physicsComp->is_of_type(Ghost::get_class_type()))
				{
					//physics component is a ghost:
					//return a reference to its (first and only) shape
					return DCAST(Ghost, physicsComp)->getBulletGhostNode().get_shape(0);
				}
				else if (physicsComp->is_of_type(RigidBody::get_class_type()))
				{
					//physics component is a rigid body:
					//return a reference to its (first and only) shape
					return DCAST(RigidBody, physicsComp)->getBulletRigidBodyNode().get_shape(0);
				}
			}
		}
	}

	// create and return the current shape: dimensions are wrt the
	//Model or InstanceOf component (if any)
	NodePath shapeNodePath = mOwnerObject->getNodePath();//default
	SMARTPTR(Component) sceneComp = mOwnerObject->getComponent(ComponentFamilyType("Scene"));
	if (sceneComp)
	{
		if (sceneComp->componentType() == ComponentType("Model"))
		{
			shapeNodePath = NodePath(DCAST(Model, sceneComp)->getNodePath().node());
		}
		if (sceneComp->componentType() == ComponentType("InstanceOf"))
		{
			shapeNodePath = NodePath(DCAST(InstanceOf, sceneComp)->getNodePath().node());
		}
	}
	return GamePhysicsManager::GetSingletonPtr()->createShape(
			shapeNodePath, mShapeType, mShapeSize,
			mModelDims, mModelDeltaCenter, mModelRadius, mDim1, mDim2,
			mDim3, mDim4, mAutomaticShaping, mUpAxis,
			mHeightfieldFile, mGhostType == DYNAMIC);
}

void Ghost::doSetPhysicalParameters()
{
	mGhostNode->set_friction(mGhostFriction);
	mGhostNode->set_restitution(mGhostRestitution);
	if (mCcdEnabled)
	{
		mGhostNode->set_ccd_motion_threshold(mCcdMotionThreshold);
		mGhostNode->set_ccd_swept_sphere_radius(mCcdSweptSphereRadius);
	}
}

void Ghost::doEnableGhostEvent(EventThrown event, ThrowEventData eventData)
{
	//some checks
	RETURN_ON_COND(eventData.mEventName == std::string(""),)
	if (eventData.mFrequency <= 0.0)
	{
		eventData.mFrequency = 30.0;
	}

	switch (event)
	{
	case OVERLAP:
		if(mOverlap.mEnable != eventData.mEnable)
		{
			mOverlap = eventData;
			mOverlap.mTimeElapsed = 0;
		}
		break;
	default:
		break;
	}
}

//TypedObject semantics: hardcoded
TypeHandle Ghost::_type_handle;

} /* namespace ely */
