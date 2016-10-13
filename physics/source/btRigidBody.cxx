/**
 * \file btRigidBody.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "btRigidBody.h"
#include "gamePhysicsManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include "py_panda.h"
extern Dtool_PyTypedObject Dtool_BTRigidBody;
#endif //PYTHON_BUILD

/**
 *
 */
BTRigidBody::BTRigidBody(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
BTRigidBody::~BTRigidBody()
{
}

/**
 * Initializes the BTRigidBody with starting settings.
 * \note Internal use only.
 */
void BTRigidBody::do_initialize()
{
	WPT(GamePhysicsManager)mTmpl = GamePhysicsManager::get_global_ptr();
	//body type
	string bodyType = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("body_type"));
	if (bodyType == string("static"))
	{
		mBodyType = STATIC;
	}
	else if (bodyType == string("kinematic"))
	{
		mBodyType = KINEMATIC;
	}
	else
	{
		mBodyType = DYNAMIC;
	}
	//
	float value;
	//body mass (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("body_mass")).c_str(), NULL);
	mBodyMass = (value >= 0.0 ? value : -value);
	//body friction (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("body_friction")).c_str(),
			NULL);
	mBodyFriction = (value >= 0.0 ? value : -value);
	//body restitution (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("body_restitution")).c_str(),
			NULL);
	mBodyRestitution = (value >= 0.0 ? value : -value);
	//shape type
	string shapeType = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_type"));
	//shape size
	string shapeSize = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_size"));
	if (shapeSize == string("minimum"))
	{
		mShapeSize = GamePhysicsManager::MINIMUN;
	}
	else if (shapeSize == string("maximum"))
	{
		mShapeSize = GamePhysicsManager::MAXIMUM;
	}
	else
	{
		mShapeSize = GamePhysicsManager::MEDIUM;
	}
	//default auto shaping
	mAutomaticShaping = true;
	if (shapeType == string("sphere"))
	{
		mShapeType = GamePhysicsManager::SPHERE;
		string radius = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_radius"));
		if (not radius.empty())
		{
			mDim1 = STRTOF(radius.c_str(), NULL);
			if (mDim1 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == string("plane"))
	{
		mShapeType = GamePhysicsManager::PLANE;
		string norm_x = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_norm_x"));
		string norm_y = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_norm_y"));
		string norm_z = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_norm_z"));
		string d = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_d"));
		if ((not norm_x.empty()) and (not norm_y.empty())
				and (not norm_z.empty()))
		{
			LVector3f normal(STRTOF(norm_x.c_str(), NULL),
					STRTOF(norm_y.c_str(), NULL), STRTOF(norm_z.c_str(), NULL));
			normal.normalize();
			mDim1 = normal.get_x();
			mDim2 = normal.get_y();
			mDim3 = normal.get_z();
			mDim4 = STRTOF(d.c_str(), NULL);
			if (normal.length() > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == string("box"))
	{
		mShapeType = GamePhysicsManager::BOX;
		string half_x = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_half_x"));
		string half_y = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_half_y"));
		string half_z = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_half_z"));
		if ((not half_x.empty()) and (not half_y.empty())
				and (not half_z.empty()))
		{
			mDim1 = STRTOF(half_x.c_str(), NULL);
			mDim2 = STRTOF(half_y.c_str(), NULL);
			mDim3 = STRTOF(half_z.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == string("cylinder")
			or shapeType == string("capsule")
			or shapeType == string("cone"))
	{
		if (shapeType == string("cylinder"))
		{
			mShapeType = GamePhysicsManager::CYLINDER;
		}
		else if (shapeType == string("capsule"))
		{
			mShapeType = GamePhysicsManager::CAPSULE;
		}
		else
		{
			mShapeType = GamePhysicsManager::CONE;
		}
		string radius = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_radius"));
		string height = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_height"));
		string upAxis = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_up"));
		if ((not radius.empty()) and (not height.empty()))
		{
			mDim1 = STRTOF(radius.c_str(), NULL);
			mDim2 = STRTOF(height.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
		if (upAxis == string("x"))
		{
			mUpAxis = X_up;
		}
		else if (upAxis == string("y"))
		{
			mUpAxis = Y_up;
		}
		else
		{
			mUpAxis = Z_up;
		}
	}
	else if (shapeType == string("heightfield"))
	{
		mShapeType = GamePhysicsManager::HEIGHTFIELD;
		string heightfield_file = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY,
				string("shape_heightfield_file"));
		mHeightfieldFile = Filename(heightfield_file);
		string height = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_height"));
		string scale_w = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_scale_w"));
		string scale_d = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_scale_d"));
		string upAxis = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("shape_up"));
		if ((not height.empty()) and (not scale_w.empty())
				and (not scale_d.empty()))
		{
			mDim1 = STRTOF(height.c_str(), NULL);
			mDim2 = STRTOF(scale_w.c_str(), NULL);
			mDim3 = STRTOF(scale_d.c_str(), NULL);
			if (mDim1 > 0.0 and mDim2 > 0.0 and mDim3 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
		if (upAxis == string("x"))
		{
			mUpAxis = X_up;
		}
		else if (upAxis == string("y"))
		{
			mUpAxis = Y_up;
		}
		else
		{
			mUpAxis = Z_up;
		}
	}
	else if (shapeType == string("triangle_mesh"))
	{
		mShapeType = GamePhysicsManager::TRIANGLEMESH;
	}
	else
	{
		//default a sphere (with auto shaping)
		mShapeType = GamePhysicsManager::SPHERE;
	}
	//collide mask
	string collideMask = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("collide_mask"));
	if (collideMask == string("all_on"))
	{
		mCollideMask = BitMask32::all_on();
	}
	else if (collideMask == string("all_off"))
	{
		mCollideMask = BitMask32::all_off();
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(collideMask.c_str(), NULL, 0);
		mCollideMask.set_word(mask);
#ifdef ELY_DEBUG
		mCollideMask.write(cout, 0);
#endif
	}
	//ccd settings: enabled if both are greater than zero (> 0.0)
	//ccd motion threshold (>=0.0)
	value = STRTOF(
			mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("ccd_motion_threshold")).c_str(),
			NULL);
	mCcdMotionThreshold = (value >= 0.0 ? value : -value);
	//ccd swept sphere radius (>=0.0)
	value = STRTOF(
			mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("ccd_swept_sphere_radius")).c_str(),
			NULL);
	mCcdSweptSphereRadius = (value >= 0.0 ? value : -value);
	((mCcdMotionThreshold > 0.0) and (mCcdSweptSphereRadius > 0.0)) ?
			mCcdEnabled = true : mCcdEnabled = false;
	//use shape of (another object)
//	mUseShapeOfId = ObjectId(mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY, string("use_shape_of"))); xxx
	//
	//setup setting
	if((
			mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY,
					string("setup")) == string("false") ? false : true))
	{
		setup();
	}
	//
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_BTRigidBody, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

void BTRigidBody::setup()
{
	RETURN_ON_COND(mSetup,)

	//create a Rigid Body Node
	//Component standard name: ObjectId_ObjectType_ComponentId_ComponentType
//	string name = COMPONENT_STANDARD_NAME; xxx
	mRigidBodyNode = new BulletRigidBodyNode(get_name().c_str());
	//add to table of all physics components indexed by
	//(underlying) Bullet PandaNodes
	GamePhysicsManager::get_global_ptr()->setPhysicsComponentByPandaNode(
			mRigidBodyNode.p(), this);
	//set the physics parameters
	doSetPhysicalParameters();

	//At this point a Scene component (Model, InstanceOf ...) should have
	//been already created and added to the object, so its node path should
	//be the same as the object's one.
	//Note: scaling is applied to a Scene component, so the object node path
	//has scaling already applied.

	//create and add a Collision Shape
	mRigidBodyNode->add_shape(doCreateShape(mShapeType));
	//set mass and other body type settings
	switchType(mBodyType);

	//create a node path for the rigid body
	mNodePath = NodePath(mRigidBodyNode);

	//attach to Bullet World
	GamePhysicsManager::get_global_ptr()->bulletWorld()->attach(mRigidBodyNode);

	//<BUG: if you want to switch the body type (e.g. dynamic to static, static to
	//dynamic, etc...) after it has been attached to the world, you must first
	//attach it as a dynamic body and then switch its type:
	//		mRigidBodyNode->set_mass(1.0);
	//		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->attach(mRigidBodyNode);
	//		switchType(mBodyType);
	///BUG>

	//set collide mask
	mNodePath.set_collide_mask(mCollideMask);

	NodePath ownerNodePath = NodePath::any_path(this); //xxx
//	if (not ownerNodePath.is_empty())
//	{
		//reparent the object node path as a child of the rigid body's one
		ownerNodePath.reparent_to(mNodePath);
		if (mShapeType != GamePhysicsManager::TRIANGLEMESH) //Hack
		{
			//correct (or possibly reset to zero) pos and hpr of the object node path
			ownerNodePath.set_pos_hpr(mModelDeltaCenter, LVecBase3::zero());
		}

//		//BulletShape::set_local_scale doesn't work anymore xxx
//		//see: https://www.panda3d.org/forums/viewtopic.php?f=9&t=10231&start=690#p93583
//		if (mShapeType == GamePhysicsManager::HEIGHTFIELD)	//Hack
//		{
//			//check if there is already a scene component
//			PT (Component)
//			sceneComp = mOwnerObject->getComponent(
//					ComponentFamilyType("Scene"));
//			if (sceneComp)
//			{
//				//check if the scene component is a Terrain
//				if (sceneComp->componentType() == ComponentType("Terrain"))
//				{
//					float widthScale =
//							DCAST(Terrain, sceneComp)->getWidthScale();
//					float heightScale =
//							DCAST(Terrain, sceneComp)->getHeightScale();
//					if (mUpAxis == X_up)
//					{
//						mNodePath.set_scale(
//								LVecBase3f(heightScale, widthScale,
//										widthScale));
//					}
//					else if (mUpAxis == Y_up)
//					{
//						mNodePath.set_scale(
//								LVecBase3f(widthScale, heightScale,
//										widthScale));
//					}
//					else //mUpAxis == Z_up
//					{
//						mNodePath.set_scale(
//								LVecBase3f(widthScale, widthScale,
//										heightScale));
//					}
//				}
//			}
//			else
//			{
//				//no scene component: scale manually if requested
//				if (not mAutomaticShaping)
//				{
//					if (mUpAxis == X_up)
//					{
//						mNodePath.set_scale(mDim1, mDim2, mDim3);
//					}
//					else if (mUpAxis == Y_up)
//					{
//						mNodePath.set_scale(mDim3, mDim1, mDim2);
//					}
//					else
//					{
//						mNodePath.set_scale(mDim2, mDim3, mDim1);
//					}
//				}
//			}
//		}
		//optimize
		mNodePath.flatten_strong();
//	}
//	else
//	{
//		//when ownerNodePath is empty: every rigid body has a shape but xxx
//		//HEIGHTFIELD, which should have a chance to scale
//		if (mShapeType == GamePhysicsManager::HEIGHTFIELD)	//Hack
//		{
//			if (not mAutomaticShaping)
//			{
//				if (mUpAxis == X_up)
//				{
//					mNodePath.set_scale(mDim1, mDim2, mDim3);
//				}
//				else if (mUpAxis == Y_up)
//				{
//					mNodePath.set_scale(mDim3, mDim1, mDim2);
//				}
//				else
//				{
//					mNodePath.set_scale(mDim2, mDim3, mDim1);
//				}
//			}
//		}
//	}

//	//set this rigid body node path as the object's one xxx
//	mOwnerObject->setNodePath(mNodePath);
	mSetup = true;
}

/**
 * On destruction cleanup.
 * Gives an BTRigidBody the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void BTRigidBody::do_finalize()
{
	//actual cleanup
	cleanup();
#ifdef PYTHON_BUILD
	//Python callback
	Py_DECREF(mSelf);
	Py_XDECREF(mUpdateCallback);
	Py_XDECREF(mUpdateArgList);
#endif //PYTHON_BUILD
	do_reset();
	//
	return;
}

void BTRigidBody::cleanup()
{
	RETURN_ON_COND(!mSetup,)

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
//	//set the object node path to the old one xxx
//	mOwnerObject->setNodePath(oldObjectNodePath);

	//remove from table of all physics components indexed by
	//(underlying) Bullet PandaNodes
	GamePhysicsManager::GetSingletonPtr()->setPhysicsComponentByPandaNode(
			mRigidBodyNode.p(), NULL);

	//remove rigid body from the physics world
	GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
			mRigidBodyNode);

	//Remove node path
	mNodePath.remove_node();
}

void BTRigidBody::switchType(BodyType bodyType)
{
	switch (bodyType)
	{
	case DYNAMIC:
		mRigidBodyNode->set_mass(mBodyMass);
		mRigidBodyNode->set_kinematic(false);
		mRigidBodyNode->set_static(false);
		mRigidBodyNode->set_deactivation_enabled(true);
		mRigidBodyNode->set_active(true);
		break;
	case STATIC:
		mRigidBodyNode->set_mass(0.0);
		mRigidBodyNode->set_kinematic(false);
		mRigidBodyNode->set_static(true);
		mRigidBodyNode->set_deactivation_enabled(true);
		mRigidBodyNode->set_active(false);
		break;
	case KINEMATIC:
		mRigidBodyNode->set_mass(0.0);
		mRigidBodyNode->set_kinematic(true);
		mRigidBodyNode->set_static(false);
		mRigidBodyNode->set_deactivation_enabled(false);
		mRigidBodyNode->set_active(false);
		break;
	default:
		break;
	}
}

PT(BulletShape)BTRigidBody::doCreateShape(GamePhysicsManager::ShapeType shapeType)
{
//	//check if it should use shape of another (already) created object xxx
//	if (not mUseShapeOfId.empty())
//	{
//		SMARTPTR(Object)createdObject =
//		ObjectTemplateManager::GetSingleton().getCreatedObject(
//				mUseShapeOfId);
//		if (createdObject)
//		{
//			//object already exists
//			SMARTPTR(Component) physicsComp =
//			createdObject->getComponent(ComponentFamilyType("Physics"));
//			if (physicsComp)
//			{
//				if (physicsComp->is_of_type(RigidBody::get_class_type()))
//				{
//					//physics component is a rigid body:
//					//return a reference to its (first and only) shape
//					return DCAST(RigidBody, physicsComp)->getBulletRigidBodyNode().get_shape(0);
//				}
//				else if (physicsComp->is_of_type(Ghost::get_class_type()))
//				{
//					//physics component is a ghost:
//					//return a reference to its (first and only) shape
//					return DCAST(Ghost, physicsComp)->getBulletGhostNode().get_shape(0);
//				}
//			}
//		}
//	}

	// create and return the current shape: dimensions are wrt the
	//Model or InstanceOf component (if any)
	NodePath shapeNodePath = NodePath::any_path(this);//default
//	SMARTPTR(Component) sceneComp = mOwnerObject->getComponent(ComponentFamilyType("Scene"));xxx
//	if (sceneComp)
//	{
//		if (sceneComp->componentType() == ComponentType("Model"))
//		{
//			shapeNodePath = NodePath(DCAST(Model, sceneComp)->getNodePath().node());
//		}
//		if (sceneComp->componentType() == ComponentType("InstanceOf"))
//		{
//			shapeNodePath = NodePath(DCAST(InstanceOf, sceneComp)->getNodePath().node());
//		}
//	}
	return GamePhysicsManager::GetSingletonPtr()->createShape(
			shapeNodePath, mShapeType, mShapeSize,
			mModelDims, mModelDeltaCenter, mModelRadius, mDim1, mDim2,
			mDim3, mDim4, mAutomaticShaping, mUpAxis,
			mHeightfieldFile, not (mBodyType == STATIC));
}

void BTRigidBody::doSetPhysicalParameters()
{
	mRigidBodyNode->set_friction(mBodyFriction);
	mRigidBodyNode->set_restitution(mBodyRestitution);
	if (mCcdEnabled)
	{
		mRigidBodyNode->set_ccd_motion_threshold(mCcdMotionThreshold);
		mRigidBodyNode->set_ccd_swept_sphere_radius(mCcdSweptSphereRadius);
	}
}

/**
 * Updates the BTRigidBody state.
 */
void BTRigidBody::update(float dt)
{
	RETURN_ON_COND(!mSetup,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif
	//
#ifdef PYTHON_BUILD
	// execute python callback (if any)
	if (mUpdateCallback && (mUpdateCallback != Py_None))
	{
		PyObject *result;
		result = PyObject_CallObject(mUpdateCallback, mUpdateArgList);
		if (result == NULL)
		{
			string errStr = get_name() +
					string(": Error calling callback function");
			PyErr_SetString(PyExc_TypeError, errStr.c_str());
			return;
		}
		Py_DECREF(result);
	}
#else
	// execute c++ callback (if any)
	if (mUpdateCallback)
	{
		mUpdateCallback(this);
	}
#endif //PYTHON_BUILD
}

/**
 * Writes a sensible description of the BTRigidBody to the indicated output
 * stream.
 */
void BTRigidBody::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this BTRigidBody as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void BTRigidBody::set_update_callback(PyObject *value)
{
	if ((!PyCallable_Check(value)) && (value != Py_None))
	{
		PyErr_SetString(PyExc_TypeError,
				"Error: the argument must be callable or None");
		return;
	}

	if (mUpdateArgList == NULL)
	{
		mUpdateArgList = Py_BuildValue("(O)", mSelf);
		if (mUpdateArgList == NULL)
		{
			return;
		}
	}
	Py_DECREF(mSelf);

	Py_XDECREF(mUpdateCallback);
	Py_INCREF(value);
	mUpdateCallback = value;
}
#else
/**
 * Sets the update callback as a c++ function taking this BTRigidBody as
 * an argument, or NULL.
 * \note C++ only.
 */
void BTRigidBody::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD

//TypedWritable API
/**
 * Tells the BamReader how to create objects of type BTRigidBody.
 */
void BTRigidBody::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void BTRigidBody::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this BTRigidBody.
	dg.add_string(get_name());

//	///Enable/disable flag.
//	dg.add_bool(mEnabled);
//
//	///Flags.
//	///@{
//	dg.add_bool(mFixedRelativePosition);
//	dg.add_bool(mBackward);
//	dg.add_bool(mFixedLookAt);
//	dg.add_bool(mHoldLookAt);
//	///@}
//
//	///Kinematic parameters.
//	///@{
//	dg.add_stdfloat(mAbsLookAtDistance);
//	dg.add_stdfloat(mAbsLookAtHeight);
//	dg.add_stdfloat(mAbsMaxDistance);
//	dg.add_stdfloat(mAbsMinDistance);
//	dg.add_stdfloat(mAbsMinHeight);
//	dg.add_stdfloat(mAbsMaxHeight);
//	dg.add_stdfloat(mFriction);
//	///@}
//
//	///Positions.
//	///@{
//	mRigidBodyPosition.write_datagram(dg);
//	mLookAtPosition.write_datagram(dg);
//	///@}
//
//	///Key controls and effective keys.
//	///@{
//	dg.add_bool(mHeadLeft);
//	dg.add_bool(mHeadRight);
//	dg.add_bool(mPitchUp);
//	dg.add_bool(mPitchDown);
//	dg.add_bool(mHeadLeftKey);
//	dg.add_bool(mHeadRightKey);
//	dg.add_bool(mPitchUpKey);
//	dg.add_bool(mPitchDownKey);
//	dg.add_bool(mMouseMoveKey);
//	///@}
//
//	///Key control values.
//	///@{
//	dg.add_bool(mMouseEnabledH);
//	dg.add_bool(mMouseEnabledP);
//	dg.add_bool(mMouseHandled);
//	dg.add_int8(mSignOfMouse);
//	///@}
//
//	/// Sensitivity settings.
//	///@{
//	dg.add_stdfloat(mSensX);
//	dg.add_stdfloat(mSensY);
//	dg.add_stdfloat(mHeadSensX);
//	dg.add_stdfloat(mHeadSensY);
//	///@}
//
//	///The chased object's node path.
//	manager->write_pointer(dg, mChasedNP.node());
//	///Auxiliary node path to track the fixed look at.
//	manager->write_pointer(dg, mFixedLookAtNP.node());
	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int BTRigidBody::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = PandaNode::complete_pointers(p_list, manager);

	/// Pointers
	PT(PandaNode)savedPandaNode;
//	///The chased object's node path.
//	savedPandaNode = DCAST(PandaNode, p_list[pi++]);
//	mChasedNP = NodePath::any_path(savedPandaNode);
//	///Auxiliary node path to track the fixed look at.
//	savedPandaNode = DCAST(PandaNode, p_list[pi++]);
//	mFixedLookAtNP = NodePath::any_path(savedPandaNode);
	///The reference node path.
	savedPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(savedPandaNode);

	return pi;
}

/**
 * This function is called by the BamReader's factory when a new object of
 * type BTRigidBody is encountered in the Bam file.  It should create the
 * BTRigidBody and extract its information from the file.
 */
TypedWritable *BTRigidBody::make_from_bam(const FactoryParams &params)
{
	// continue only if GamePhysicsManager exists
	CONTINUE_IF_ELSE_R(GamePhysicsManager::get_global_ptr(), NULL)

	// create a BTRigidBody with default parameters' values: they'll be restored later
	GamePhysicsManager::get_global_ptr()->set_parameters_defaults(
			GamePhysicsManager::RIGIDBODY);
	BTRigidBody *node = DCAST(BTRigidBody,
			GamePhysicsManager::get_global_ptr()->create_rigid_body(
					"BTRigidBody").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new BTRigidBody.
 */
void BTRigidBody::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this BTRigidBody.
	set_name(scan.get_string());

//	///Enable/disable flag.
//	mEnabled = scan.get_bool();
//
//	///Flags.
//	///@{
//	mFixedRelativePosition = scan.get_bool();
//	mBackward = scan.get_bool();
//	mFixedLookAt = scan.get_bool();
//	mHoldLookAt = scan.get_bool();
//	///@}
//
//	///Kinematic parameters.
//	///@{
//	mAbsLookAtDistance = scan.get_stdfloat();
//	mAbsLookAtHeight = scan.get_stdfloat();
//	mAbsMaxDistance = scan.get_stdfloat();
//	mAbsMinDistance = scan.get_stdfloat();
//	mAbsMinHeight = scan.get_stdfloat();
//	mAbsMaxHeight = scan.get_stdfloat();
//	mFriction = scan.get_stdfloat();
//	///@}
//
//	///Positions.
//	///@{
//	mRigidBodyPosition.read_datagram(scan);
//	mLookAtPosition.read_datagram(scan);
//	///@}
//
//	///Key controls and effective keys.
//	///@{
//	mHeadLeft = scan.get_bool();
//	mHeadRight = scan.get_bool();
//	mPitchUp = scan.get_bool();
//	mPitchDown = scan.get_bool();
//	mHeadLeftKey = scan.get_bool();
//	mHeadRightKey = scan.get_bool();
//	mPitchUpKey = scan.get_bool();
//	mPitchDownKey = scan.get_bool();
//	mMouseMoveKey = scan.get_bool();
//	///@}
//
//	///Key control values.
//	///@{
//	mMouseEnabledH = scan.get_bool();
//	mMouseEnabledP = scan.get_bool();
//	mMouseHandled = scan.get_bool();
//	mSignOfMouse = scan.get_int8();
//	///@}
//
//	/// Sensitivity settings.
//	///@{
//	mSensX = scan.get_stdfloat();
//	mSensY = scan.get_stdfloat();
//	mHeadSensX = scan.get_stdfloat();
//	mHeadSensY = scan.get_stdfloat();
//	///@}

	///The chased object's node path.
	manager->read_pointer(scan);
	///Auxiliary node path to track the fixed look at.
	manager->read_pointer(scan);
	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle BTRigidBody::_type_handle;
