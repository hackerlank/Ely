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
#include "transformState.h"

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
		BulletRigidBodyNode(name.c_str())
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
	//
	float value;
	//shape type
	string shapeType = mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("shape_type"));
	//shape size
	string shapeSize = mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("shape_size"));
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
		string radius = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_radius"));
		if (! radius.empty())
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
		string norm_x = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_norm_x"));
		string norm_y = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_norm_y"));
		string norm_z = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_norm_z"));
		string d = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_d"));
		if ((! norm_x.empty()) && (! norm_y.empty())
				&& (! norm_z.empty()))
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
		string half_x = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_half_x"));
		string half_y = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_half_y"));
		string half_z = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_half_z"));
		if ((! half_x.empty()) && (! half_y.empty())
				&& (! half_z.empty()))
		{
			mDim1 = STRTOF(half_x.c_str(), NULL);
			mDim2 = STRTOF(half_y.c_str(), NULL);
			mDim3 = STRTOF(half_z.c_str(), NULL);
			if (mDim1 > 0.0 && mDim2 > 0.0 && mDim3 > 0.0)
			{
				mAutomaticShaping = false;
			}
		}
	}
	else if (shapeType == string("cylinder")
			|| shapeType == string("capsule")
			|| shapeType == string("cone"))
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
		string radius = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_radius"));
		string height = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_height"));
		string upAxis = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_up"));
		if ((! radius.empty()) && (! height.empty()))
		{
			mDim1 = STRTOF(radius.c_str(), NULL);
			mDim2 = STRTOF(height.c_str(), NULL);
			if (mDim1 > 0.0 && mDim2 > 0.0)
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
		string heightfield_file = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_heightfield_file"));
		mHeightfieldFile = Filename(heightfield_file);
		string height = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_height"));
		string scale_w = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_scale_w"));
		string scale_d = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_scale_d"));
		string upAxis = mTmpl->get_parameter_value(
				GamePhysicsManager::RIGIDBODY, string("shape_up"));
		if ((! height.empty()) && (! scale_w.empty())
				&& (! scale_d.empty()))
		{
			mDim2 = STRTOF(height.c_str(), NULL);
			mDim3 = STRTOF(scale_w.c_str(), NULL);
			mDim4 = STRTOF(scale_d.c_str(), NULL);
			if (mDim2 > 0.0 && mDim3 > 0.0 && mDim4 > 0.0)
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
	NodePath thisNP = NodePath::any_path(this);
	string collideMask = mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("collide_mask"));
	if (collideMask == string("all_on"))
	{
		thisNP.set_collide_mask(BitMask32::all_on());
	}
	else if (collideMask == string("all_off"))
	{
		thisNP.set_collide_mask(BitMask32::all_off());
	}
	else
	{
		uint32_t mask = (uint32_t) strtol(collideMask.c_str(), NULL, 0);
		BitMask32 mCollideMask;
		mCollideMask.set_word(mask);
		thisNP.set_collide_mask(mCollideMask);
#ifdef ELY_DEBUG
		mCollideMask.write(cout, 0);
#endif
	}
	//body mass (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("body_mass")).c_str(), NULL);
	set_mass(value >= 0.0 ? value : -value);
	//body friction (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("body_friction")).c_str(),
			NULL);
	set_friction(value >= 0.0 ? value : -value);
	//body restitution (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("body_restitution")).c_str(),
			NULL);
	set_restitution(value >= 0.0 ? value : -value);
	//ccd settings: enabled if both are greater than zero (> 0.0)
	//ccd motion threshold (>=0.0)
	value = STRTOF(
			mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY,
					string("ccd_motion_threshold")).c_str(), NULL);
	set_ccd_motion_threshold(value >= 0.0 ? value : -value);
	//ccd swept sphere radius (>=0.0)
	value = STRTOF(
			mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY,
					string("ccd_swept_sphere_radius")).c_str(),	NULL);
	set_ccd_swept_sphere_radius(value >= 0.0 ? value : -value);
	//body type (after setting the other physics parameters)
	string bodyType = mTmpl->get_parameter_value(
			GamePhysicsManager::RIGIDBODY, string("body_type"));
	if (bodyType == string("static"))
	{
		switch_body_type(STATIC);
	}
	else if (bodyType == string("kinematic"))
	{
		switch_body_type(KINEMATIC);
	}
	else
	{
		switch_body_type(DYNAMIC);
	}

	// use shape of (another object)
	mUseShapeOfId = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY,
			string("use_shape_of"));

	//object setting
	string object = mTmpl->get_parameter_value(GamePhysicsManager::RIGIDBODY,
			string("object"));
	if(!object.empty())
	{
		// search object under reference node
		NodePath objectNP = mReferenceNP.find(string("**/") + object);
		if (!objectNP.is_empty())
		{
			setup(objectNP);
		}
	}
	//
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_BTRigidBody, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

void BTRigidBody::do_check_auto_shaping()
{
	// default
	mAutomaticShaping = true;
	//check
	if (mShapeType == GamePhysicsManager::SPHERE)
	{
		if (mDim1 > 0.0)
		{
			mAutomaticShaping = false;
		}
	}
	else if (mShapeType == GamePhysicsManager::PLANE)
	{
		if (LVector3f(mDim1, mDim2, mDim3).length() > 0.0)
		{
			mAutomaticShaping = false;
		}
	}
	else if (mShapeType == GamePhysicsManager::BOX)
	{
		if (mDim1 > 0.0 && mDim2 > 0.0 && mDim3 > 0.0)
		{
			mAutomaticShaping = false;
		}
	}
	else if ((mShapeType == GamePhysicsManager::CYLINDER)
			|| (mShapeType == GamePhysicsManager::CAPSULE)
			|| (mShapeType == GamePhysicsManager::CONE))
	{
		if (mDim1 > 0.0 && mDim2 > 0.0)
		{
			mAutomaticShaping = false;
		}
	}
	else if (mShapeType == GamePhysicsManager::HEIGHTFIELD)
	{
		if (mDim2 > 0.0 && mDim3 > 0.0 && mDim4 > 0.0)
		{
			mAutomaticShaping = false;
		}
	}
}

void BTRigidBody::setup(NodePath& objectNP)
{
	RETURN_ON_COND(mSetup,)

	// get this node path (if !empty)
	NodePath thisNP = NodePath::any_path(this);
	if (!objectNP.is_empty())
	{
		// TRIANGLEMESH should preserve its transform
		if (mShapeType != GamePhysicsManager::TRIANGLEMESH) //Hack
		{
			// inherit the TrasformState from the object
			set_transform(objectNP.node()->get_transform());
			// reset object's TrasformState
			objectNP.set_transform(TransformState::make_identity());
			// recompute objectNP mModelDeltaCenter
			LVecBase3f modelDims;
			GamePhysicsManager::get_global_ptr()->get_bounding_dimensions(
					objectNP, modelDims, mModelDeltaCenter);
		}
		// reparent the object node path to this
		objectNP.reparent_to(thisNP);
		// correct (or possibly reset to zero) transform of the object node path
		if (mShapeType != GamePhysicsManager::TRIANGLEMESH) //Hack
		{
			objectNP.set_pos_hpr(mModelDeltaCenter, LVecBase3::zero());
		}
		//optimize
		thisNP.flatten_strong();
	}
	// Note: the object NodePath (if !empty) has scaling already applied, and
	// it is is taken into account for the construction of the shape
	// add a Collision Shape
	add_shape(do_create_shape(mShapeType, objectNP));

	// attach this to Bullet World
	GamePhysicsManager::get_global_ptr()->get_bullet_world()->attach(this);

	// set the flag
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
	//cleanup (if needed)
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

	// remove rigid body from the physics world
	GamePhysicsManager::GetSingletonPtr()->get_bullet_world()->remove(this);

	// remove all rigid body's shapes
	for (int i = 0; i < get_num_shapes(); ++i)
	{
		remove_shape(get_shape(i));
	}

	NodePath thisNP = NodePath::any_path(this);
	// detach this rigid body's children
	for (int i = 0; i < thisNP.get_num_children(); ++i)
	{
		NodePath childNP = thisNP.get_child(i);
		// TRIANGLEMESH preserved its transform
		if (mShapeType != GamePhysicsManager::TRIANGLEMESH)
		{
			// restore child's transform to the
			// current rigid body' one
			childNP.set_transform(get_transform());
		}
		// detach childNP
		thisNP.get_child(i).detach_node();
	}

	// set the flag
	mSetup = false;
}

/**
 * Switches the current body type of this BTRigidBody.
 * For DYNAMIC type mass must be >0.0 for type to be changed. For STATIC and
 * KINEMATIC mass is reset to 0.0.
 */
void BTRigidBody::switch_body_type(BodyType bodyType)
{
	// return if DYNAMIC && mass==0.0
	RETURN_ON_COND((bodyType == DYNAMIC) && (get_mass() == 0.0),)

	switch (bodyType)
	{
	case DYNAMIC:
		set_kinematic(false);
		set_static(false);
		set_deactivation_enabled(true);
		set_active(true);
		break;
	case STATIC:
		set_mass(0.0);
		set_kinematic(false);
		set_static(true);
		set_deactivation_enabled(true);
		set_active(false);
		break;
	case KINEMATIC:
		set_mass(0.0);
		set_kinematic(true);
		set_static(false);
		set_deactivation_enabled(false);
		set_active(false);
		break;
	default:
		break;
	}
	mBodyType = bodyType;
}

/**
 * Creates a shape given its type.
 * \note Internal use only.
 */
PT(BulletShape)BTRigidBody::do_create_shape(GamePhysicsManager::ShapeType shapeType,
		const NodePath& objectNP)
{
	NodePath objectToShapeNP = objectNP;
	//check if it should use shape of another (already) created object
	if(!mUseShapeOfId.empty())
	{
		// search object under reference node
		NodePath objectUseShapeOfNP = mReferenceNP.find(string("**/") +
				mUseShapeOfId);
		if (!objectUseShapeOfNP.is_empty())
		{
			objectToShapeNP = objectUseShapeOfNP;
		}
	}
	// create and return the current shape
	return GamePhysicsManager::GetSingletonPtr()->create_shape(
			objectToShapeNP, mShapeType, mShapeSize,
			mModelDims, mModelDeltaCenter, mModelRadius, mDim1, mDim2,
			mDim3, mDim4, mAutomaticShaping, mUpAxis,
			mHeightfieldFile, ! (mBodyType == STATIC));
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
	BulletRigidBodyNode::write_datagram(manager, dg);

	///The setup flag.
	dg.add_bool(mSetup);

	///@{
	///Physical parameters.
	dg.add_uint8((uint8_t) mBodyType);
	dg.add_uint8((uint8_t) mShapeType);
	dg.add_uint8((uint8_t) mShapeSize);
	///@}

	///@{
	///Geometric functions and parameters.
	mModelDims.write_datagram(dg);
	dg.add_stdfloat(mModelRadius);
	dg.add_string(mUseShapeOfId);
	mModelDeltaCenter.write_datagram(dg);
	dg.add_bool(mAutomaticShaping);
	dg.add_stdfloat(mDim1);
	dg.add_stdfloat(mDim2);
	dg.add_stdfloat(mDim3);
	dg.add_stdfloat(mDim4);
	dg.add_string(mHeightfieldFile);
	dg.add_uint8((uint8_t) mUpAxis);
	///@}

	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int BTRigidBody::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = BulletRigidBodyNode::complete_pointers(p_list, manager);

	/// Pointers
	///The reference node path.
	PT(PandaNode)referenceNPPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(referenceNPPandaNode);

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
	BulletRigidBodyNode::fillin(scan, manager);

	///The setup flag.
	mSetup = scan.get_bool();

	///@{
	///Physical parameters.
	mBodyType = (BodyType) scan.get_uint8();
	mShapeType = (GamePhysicsManager::ShapeType) scan.get_uint8();
	mShapeSize = (GamePhysicsManager::ShapeSize) scan.get_uint8();
	///@}

	///@{
	///Geometric functions and parameters.
	mModelDims.read_datagram(scan);
	mModelRadius = scan.get_stdfloat();
	mUseShapeOfId = scan.get_string();
	mModelDeltaCenter.read_datagram(scan);
	mAutomaticShaping = scan.get_bool();
	mDim1 = scan.get_stdfloat();
	mDim2 = scan.get_stdfloat();
	mDim3 = scan.get_stdfloat();
	mDim4 = scan.get_stdfloat();
	mHeightfieldFile = scan.get_string();
	mUpAxis = (BulletUpAxis) scan.get_uint8();
	///@}

	///The reference node path.
	manager->read_pointer(scan);

	/// attach this BTRigidBody to Bullet World if needed
	/// Note: here is the right place to complete restoring,
	/// because BulletRigidBodyNode is complete at this point.
	if (mSetup)
	{
		GamePhysicsManager::get_global_ptr()->get_bullet_world()->attach(this);
	}
}

//TypedObject semantics: hardcoded
TypeHandle BTRigidBody::_type_handle;
