/**
 * \file btSoftBody.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "btSoftBody.h"
#include "gamePhysicsManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include "py_panda.h"
extern Dtool_PyTypedObject Dtool_BTSoftBody;
#endif //PYTHON_BUILD

/**
 *
 */
BTSoftBody::BTSoftBody(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
BTSoftBody::~BTSoftBody()
{
}

/**
 * Initializes the BTSoftBody with starting settings.
 * \note Internal use only.
 */
void BTSoftBody::do_initialize()
{
	WPT(GamePhysicsManager)mTmpl = GamePhysicsManager::get_global_ptr();
	//body type
	string bodyType = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY,
			string("body_type"));
	if (bodyType == string("patch"))
	{
		mBodyType = PATCH;
	}
	else if (bodyType == string("ellipsoid"))
	{
		mBodyType = ELLIPSOID;
	}
	else if (bodyType == string("tri_mesh"))
	{
		mBodyType = TRIMESH;
	}
	else if (bodyType == string("tetra_mesh"))
	{
		mBodyType = TETRAMESH;
	}
	else
	{
		mBodyType = ROPE;
	}
	//collide mask
	NodePath thisNP = NodePath::any_path(this);
	string collideMask = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY,
			string("collide_mask"));
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
	//
	string param;
	unsigned int idx, valueNum;
	vector<string> paramValuesStr;
	float value;
	int valueInt;
	//body total mass
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("body_total_mass")).c_str(),
	NULL);
	mBodyTotalMass = (value >= 0.0 ? value : -value);
	//body mass from faces
	mBodyMassFromFaces = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("body_mass_from_faces"))
					== string("true") ? true : false);
	//air density
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("air_density")).c_str(),
	NULL);
	mAirDensity = (value >= 0.0 ? value : -value);
	//water density
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("water_density")).c_str(),
	NULL);
	mWaterDensity = (value >= 0.0 ? value : -value);
	//water offset
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("water_offset")).c_str(),
	NULL);
	mWaterOffset = (value >= 0.0 ? value : -value);
	//water normal
	param = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("water_normal"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mWaterNormal[idx] = STRTOF(paramValuesStr[idx].c_str(), NULL);
	}
	//show model
	mShowModel = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("show_model")) == string("true") ?
					true : false);
	//points
	param = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("points"));
	paramValuesStr = parseCompoundString(param, ':');
	valueNum = paramValuesStr.size();
	for (idx = 0; (idx < valueNum) && (idx < 4); ++idx)
	{
		vector<string> paramValuesStrExt = parseCompoundString(
				paramValuesStr[idx], ',');
		if (paramValuesStrExt.size() < 3)
		{
			paramValuesStrExt.resize(3, "0.0");
		}
		LPoint3f point;
		for (unsigned int i = 0; i < 3; ++i)
		{
			point[i] = STRTOF(paramValuesStrExt[i].c_str(), NULL);
		}
		mPoints.push_back(point);
	}
	//res
	param = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("res"));
	paramValuesStr = parseCompoundString(param, ':');
	valueNum = paramValuesStr.size();
	for (idx = 0; (idx < valueNum) && (idx < 2); ++idx)
	{
		valueInt = strtol(paramValuesStr[idx].c_str(), NULL, 0);
		mRes.push_back(valueInt >= 0 ? valueInt : -valueInt);
	}
	//fixeds
	mFixeds = strtol(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("fixeds")).c_str(), NULL, 0);
	//gendiags
	mGendiags = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("gendiags")) == string("false") ?
					false : true);
	//radius
	param = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("radius"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mRadius[idx] = STRTOF(paramValuesStr[idx].c_str(), NULL);
	}
	//randomize constraints
	mRandomizeConstraints = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("randomize_constraints"))
					== string("false") ? false : true);
	//tetra data files
	param = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("tetra_data_files"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum >= 3)
	{
		mTetraDataFileNames["elems"] = paramValuesStr[0];
		mTetraDataFileNames["faces"] = paramValuesStr[1];
		mTetraDataFileNames["nodes"] = paramValuesStr[2];
	}
	//enabling setting
//	if ((mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY,xxx
//			string("enabled")) == string("false") ? false : true))
//	{
//		do_enable();
//	}
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_BTSoftBody, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}


void BTSoftBody::setup(NodePath& objectNP)
{
	RETURN_ON_COND(mSetup,)

	//At this point a Scene component (Model) should have been already xxx
	//created and added to the object, so its node path should
	//be the same as the object's one.

	//Soft body world information
	BulletSoftBodyWorldInfo info = GamePhysicsManager::get_global_ptr()->
			get_bullet_world()->get_world_info();
	info.set_air_density(mAirDensity);
	info.set_water_density(mWaterDensity);
	info.set_water_offset(mWaterOffset);
	info.set_water_normal(mWaterNormal);
	// reset temporary nodes
	PT(PandaNode)pandaNode;
	PT(Geom)geom;
	pandaNode.clear();
	geom.clear();
	//create a Soft Body Node
	if (mBodyType == PATCH)
	{
		//get points
		if (mPoints.size() < 4)
		{
			mPoints.resize(4, LPoint3f::zero());
		}
		//get res
		if (mRes.size() < 2)
		{
			mRes.resize(2, 0);
		}
		//create patch
		this = BulletSoftBodyNode::make_patch(info, mPoints[0],
				mPoints[1], mPoints[2], mPoints[3], mRes[0], mRes[1], mFixeds,
				mGendiags);
		//visualize
		if (!objectNP.is_empty())
		{
			pandaNode =
					objectNP.find_all_matches("**/+GeomNode").get_path(0).node();
		}
		if (mShowModel)
		{
			//visualize with model GeomNode (if any)
			if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
			{
				geom = DCAST(GeomNode, pandaNode)->modify_geom(0).p();
			}
			else
			{
				CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3n3t2();
				geom = BulletHelper::make_geom_from_faces(this, format, true).p();
				//make texcoords for patch.
				BulletHelper::make_texcoords_for_patch(geom, mRes[0], mRes[1]);
			}
		}
		else
		{
			CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3n3t2();
			geom = BulletHelper::make_geom_from_faces(this, format, true).p();
			//make texcoords for patch.
			BulletHelper::make_texcoords_for_patch(geom, mRes[0], mRes[1]);
			//visualize with GeomNode (if any)
			if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
			{
				DCAST(GeomNode, pandaNode)->add_geom(geom);
			}
		}
		//link with Geom
		this->link_geom(geom);
	}
	else if (mBodyType == ELLIPSOID)
	{
		//get points
		if (mPoints.size() < 1)
		{
			mPoints.resize(1, LPoint3f::zero());
		}
		//get res
		if (mRes.size() < 1)
		{
			mRes.resize(1, 0);
		}
		//create ellipsoid
		this = BulletSoftBodyNode::make_ellipsoid(info, mPoints[0],
				mRadius, mRes[0]);
		//visualize
		if (!objectNP.is_empty())
		{
			pandaNode =
					objectNP.find_all_matches("**/+GeomNode").get_path(0).node();
		}
		if (mShowModel)
		{
			//visualize with model GeomNode (if any)
			if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
			{
				geom = DCAST(GeomNode, pandaNode)->modify_geom(0).p();
			}
			else
			{
				CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3n3t2();
				geom = BulletHelper::make_geom_from_faces(this, format).p();
				//make texcoords for ellipsoid: to be written!!!
///				BulletHelper::make_texcoords_for_ellipsoid(geom, radius, mRes[0]);
			}
		}
		else
		{
			CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3n3t2();
			geom = BulletHelper::make_geom_from_faces(this, format).p();
			//make texcoords for ellipsoid: to be written!!!
///			BulletHelper::make_texcoords_for_ellipsoid(geom, radius, mRes[0]);
			//visualize with GeomNode (if any)
			if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
			{
				DCAST(GeomNode, pandaNode)->add_geom(geom);
			}
		}
		//link with Geom
		this->link_geom(geom);
	}
	else if (mBodyType == TRIMESH)
	{
		//get and visualize with model GeomNode (if any)
		if (!objectNP.is_empty())
		{
			pandaNode =
					objectNP.find_all_matches("**/+GeomNode").get_path(0).node();
		}
		if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
		{
			geom = DCAST(GeomNode, pandaNode)->modify_geom(0).p();
		}
		else
		{
			//default trimesh
			LPoint3f point[] =
			{
				LPoint3f(0.0,0.0,0.0),
				LPoint3f(1.0,0.0,0.0),
				LPoint3f(0.0,1.0,0.0),
				LPoint3f(0.0,0.0,1.0)
			};
			int index[]=
			{
				0,1,2,
				0,1,3,
				1,2,3,
				0,3,2
			};
			CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3();
			PT(GeomVertexData) vdata;
			vdata = new GeomVertexData("defaultTriMesh", format, Geom::UH_static);
			GeomVertexWriter vertex;
			for (int i = 0; i < 4; ++i)
			{
				vertex.add_data3(point[i]);
			}
			PT(GeomTriangles) prim = new GeomTriangles(Geom::UH_static);
			for (int i = 0; i < 4; ++i)
			{
				prim->add_vertex(index[3*i + 0]);
				prim->add_vertex(index[3*i + 1]);
				prim->add_vertex(index[3*i + 2]);
				prim->close_primitive();
			}
			geom = new Geom(vdata);
			geom->add_primitive(prim);
		}
		//create trimesh
		this = BulletSoftBodyNode::make_tri_mesh(info, geom, mRandomizeConstraints);
		//link with Geom
		this->link_geom(geom);
	}
	else if (mBodyType == TETRAMESH)
	{
		bool goodness = true;
		if (mTetraDataFileNames.size() == 3)
		{
			map<string, string>::const_iterator iter;
			//get files
			map<string, ifstream*> files;
			map<string, int> lengths;
			map<string, char*> buffers;
			for (iter = mTetraDataFileNames.begin();
					iter != mTetraDataFileNames.end(); ++iter)
			{
				files[iter->first] = new ifstream();
				files[iter->first]->open(iter->second.c_str(),
						ifstream::in);
				if (! files[iter->first]->good())
				{
					goodness = false;
					continue;
				}
				//get file's length
				files[iter->first]->seekg(0, files[iter->first]->end);
				lengths[iter->first] = files[iter->first]->tellg();
				files[iter->first]->seekg(0, files[iter->first]->beg);
			}
			//check files' goodness
			if (goodness)
			{
				//setup files' buffers and read data as blocks
				for (iter = mTetraDataFileNames.begin();
						iter != mTetraDataFileNames.end(); ++iter)
				{
					buffers[iter->first] = new char[lengths[iter->first]];
					files[iter->first]->read(buffers[iter->first],
							lengths[iter->first]);
				}
				//create tetra mesh
				this = BulletSoftBodyNode::make_tet_mesh(info,
						buffers["elems"], buffers["faces"], buffers["nodes"]);
				//visualize
				if (!objectNP.is_empty())
				{
					pandaNode =
							objectNP.find_all_matches("**/+GeomNode").get_path(
									0).node();
				}
				if (mShowModel)
				{
					//visualize with model GeomNode (if any)
					if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
					{
						geom = DCAST(GeomNode, pandaNode)->modify_geom(0).p();
					}
					else
					{
						CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3n3t2();
						geom = BulletHelper::make_geom_from_faces(this, format).p();
						//make texcoords for tetramesh: to be written!!!
///						BulletHelper::make_texcoords_for_tetramesh(geom, mRes[0], mRes[1]);
					}
				}
				else
				{
					CPT(GeomVertexFormat)format = GeomVertexFormat::get_v3n3t2();
					geom = BulletHelper::make_geom_from_faces(this, format).p();
					//make texcoords for tetramesh: to be written!!!
///					BulletHelper::make_texcoords_for_tetramesh(geom, mRes[0], mRes[1]);
					//visualize with GeomNode (if any)
					if (pandaNode && pandaNode->is_of_type(GeomNode::get_class_type()))
					{
///						geomNode = DCAST(GeomNode, pandaNode);
						DCAST(GeomNode, pandaNode)->add_geom(geom);
					}
				}
				//link with Geom
				this->link_geom(geom);
			}
			//clear data
			for (iter = mTetraDataFileNames.begin();
					iter != mTetraDataFileNames.end(); ++iter)
			{
				//clear buffers
				delete buffers[iter->first];
				//close and clear files
				files[iter->first]->close();
				delete files[iter->first];
			}
		}
		if ((mTetraDataFileNames.size() < 3) || (! goodness))
		{
			//default tetramesh
			char nodeBuf[] =
			{	8, 3, 0, 0, 1, -1, 1, -1, 2, -1, -1, -1, 3, -1, -1, 1, 4, -1, 1,
				1, 5, 1, 1, -1, 6, 1, 1, 1, 7, 1, -1, -1, 8, 1, -1, 1};
			//create null tetra mesh
			this = BulletSoftBodyNode::make_tet_mesh(info, NULL,
					NULL, nodeBuf);
		}
	}
	else
	{
		//ROPE
		//get points
		if (mPoints.size() < 2)
		{
			mPoints.resize(2, LPoint3f::zero());
		}
		//get res
		if (mRes.size() < 1)
		{
			mRes.resize(1, 0);
		}
		//create rope
		this = BulletSoftBodyNode::make_rope(info, mPoints[0],
				mPoints[1], mRes[0], mFixeds);
		//link with NURBS curve
		PT(NurbsCurveEvaluator)curve = new NurbsCurveEvaluator();
		curve->reset(mRes[0] + 2);
		this->link_curve(curve);
		//visualize with RopeNode (if any)
		if (!objectNP.is_empty())
		{
			pandaNode =
					objectNP.find_all_matches("**/+RopeNode").get_path(0).node();
		}
		if (pandaNode && pandaNode->is_of_type(RopeNode::get_class_type()))
		{
			DCAST(RopeNode, pandaNode)->set_curve(curve);
		}
	}
//	//add to table of all physics components indexed by xxx
//	//(underlying) Bullet PandaNodes
//	GamePhysicsManager::GetSingletonPtr()->setPhysicsComponentByPandaNode(
//			mSoftBodyNode.p(), this);
	//set total mass
	this->set_total_mass(mBodyTotalMass, mBodyMassFromFaces);

	//attach to Bullet World
	GamePhysicsManager::get_global_ptr()->get_bullet_world()->attach(this);

	NodePath thisNP = NodePath::any_path(this);
	if (! objectNP.is_empty())
	{
		// reparent the object node path to this
		objectNP.reparent_to(thisNP);
	}

	// set the flag
	mSetup = true;
}



/**
 * On destruction cleanup.
 * Gives an BTSoftBody the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void BTSoftBody::do_finalize()
{
	//if enabled: disable
	if (mEnabled)
	{
		//actual disabling
		do_disable();
	}
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




/**
 * Updates the BTSoftBody state.
 */
void BTSoftBody::update(float dt)
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
 * Writes a sensible description of the BTSoftBody to the indicated output
 * stream.
 */
void BTSoftBody::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this BTSoftBody as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void BTSoftBody::set_update_callback(PyObject *value)
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
 * Sets the update callback as a c++ function taking this BTSoftBody as
 * an argument, or NULL.
 * \note C++ only.
 */
void BTSoftBody::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD


//TypedWritable API
/**
 * Tells the BamReader how to create objects of type BTSoftBody.
 */
void BTSoftBody::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void BTSoftBody::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this BTSoftBody.
	dg.add_string(get_name());

//	///Enable/disable flag.xxx
//	dg.add_bool(mEnabled);
//
//	///Key controls and effective keys.
//	///@{
//	dg.add_bool(mForward);
//	dg.add_bool(mBackward);
//	dg.add_bool(mStrafeLeft);
//	dg.add_bool(mStrafeRight);
//	dg.add_bool(mUp);
//	dg.add_bool(mDown);
//	dg.add_bool(mHeadLeft);
//	dg.add_bool(mHeadRight);
//	dg.add_bool(mPitchUp);
//	dg.add_bool(mPitchDown);
//	dg.add_bool(mForwardKey);
//	dg.add_bool(mBackwardKey);
//	dg.add_bool(mStrafeLeftKey);
//	dg.add_bool(mStrafeRightKey);
//	dg.add_bool(mUpKey);
//	dg.add_bool(mDownKey);
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
//	dg.add_bool(mHeadLimitEnabled);
//	dg.add_bool(mPitchLimitEnabled);
//	dg.add_stdfloat(mHLimit);
//	dg.add_stdfloat(mPLimit);
//	dg.add_int8(mSignOfTranslation);
//	dg.add_int8(mSignOfMouse);
//	///@}
//
//	/// Sensitivity settings.
//	///@{
//	dg.add_stdfloat(mFastFactor);
//	mActualSpeedXYZ.write_datagram(dg);
//	mMaxSpeedXYZ.write_datagram(dg);
//	mMaxSpeedSquaredXYZ.write_datagram(dg);
//	dg.add_stdfloat(mActualSpeedH);
//	dg.add_stdfloat(mActualSpeedP);
//	dg.add_stdfloat(mMaxSpeedHP);
//	dg.add_stdfloat(mMaxSpeedSquaredHP);
//	mAccelXYZ.write_datagram(dg);
//	dg.add_stdfloat(mAccelHP);
//	dg.add_stdfloat(mFrictionXYZ);
//	dg.add_stdfloat(mFrictionHP);
//	dg.add_stdfloat(mStopThreshold);
//	dg.add_stdfloat(mSensX);
//	dg.add_stdfloat(mSensY);
//	///@}

	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int BTSoftBody::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = PandaNode::complete_pointers(p_list, manager);

	/// Pointers
	///The reference node path.
	PT(PandaNode)referenceNPPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(referenceNPPandaNode);

	return pi;
}

/**
 * This function is called by the BamReader's factory when a new object of
 * type BTSoftBody is encountered in the Bam file.  It should create the
 * BTSoftBody and extract its information from the file.
 */
TypedWritable *BTSoftBody::make_from_bam(const FactoryParams &params)
{
	// continue only if GamePhysicsManager exists
	CONTINUE_IF_ELSE_R(GamePhysicsManager::get_global_ptr(), NULL)

	// create a BTSoftBody with default parameters' values: they'll be restored later
	GamePhysicsManager::get_global_ptr()->set_parameters_defaults(
			GamePhysicsManager::SOFTBODY);
	BTSoftBody *node = DCAST(BTSoftBody,
			GamePhysicsManager::get_global_ptr()->create_soft_body(
					"Driver").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new BTSoftBody.
 */
void BTSoftBody::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this BTSoftBody.
	set_name(scan.get_string());

//	///Enable/disable flag.
//	mEnabled = scan.get_bool();
//
//	///Key controls and effective keys.
//	///@{
//	mForward = scan.get_bool();
//	mBackward = scan.get_bool();
//	mStrafeLeft = scan.get_bool();
//	mStrafeRight = scan.get_bool();
//	mUp = scan.get_bool();
//	mDown = scan.get_bool();
//	mHeadLeft = scan.get_bool();
//	mHeadRight = scan.get_bool();
//	mPitchUp = scan.get_bool();
//	mPitchDown = scan.get_bool();
//	mForwardKey = scan.get_bool();
//	mBackwardKey = scan.get_bool();
//	mStrafeLeftKey = scan.get_bool();
//	mStrafeRightKey = scan.get_bool();
//	mUpKey = scan.get_bool();
//	mDownKey = scan.get_bool();
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
//	mHeadLimitEnabled = scan.get_bool();
//	mPitchLimitEnabled = scan.get_bool();
//	mHLimit = scan.get_stdfloat();
//	mPLimit = scan.get_stdfloat();
//	mSignOfTranslation = scan.get_int8();
//	mSignOfMouse = scan.get_int8();
//	///@}
//
//	/// Sensitivity settings.
//	///@{
//	mFastFactor = scan.get_stdfloat();
//	mActualSpeedXYZ.read_datagram(scan);
//	mMaxSpeedXYZ.read_datagram(scan);
//	mMaxSpeedSquaredXYZ.read_datagram(scan);
//	mActualSpeedH = scan.get_stdfloat();
//	mActualSpeedP = scan.get_stdfloat();
//	mMaxSpeedHP = scan.get_stdfloat();
//	mMaxSpeedSquaredHP = scan.get_stdfloat();
//	mAccelXYZ.read_datagram(scan);
//	mAccelHP = scan.get_stdfloat();
//	mFrictionXYZ = scan.get_stdfloat();
//	mFrictionHP = scan.get_stdfloat();
//	mStopThreshold = scan.get_stdfloat();
//	mSensX = scan.get_stdfloat();
//	mSensY = scan.get_stdfloat();
//	///@}

	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle BTSoftBody::_type_handle;


///////////////////////////////

void SoftBody::onRemoveFromObjectCleanup()
{
	NodePath oldObjectNodePath;
	//set the object node path to the first child of rigid body's one (if any)
	if (thisNP.get_num_children() > 0)
	{
		oldObjectNodePath = thisNP.get_child(0);
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
			mSoftBodyNode.p(), NULL);

	HOLD_REMUTEX(GamePhysicsManager::GetSingletonPtr()->getMutex())
	{
		//remove rigid body from the physics world
		GamePhysicsManager::GetSingletonPtr()->bulletWorld()->remove(
				mSoftBodyNode);
	}

	//Remove node path
	thisNP.remove_node();
	//
	reset();
}

void SoftBody::onAddToSceneSetup()
{
	//XXX: HACK: rope node's parent node path correction (see bullet samples)
	if (mBodyType == ROPE)
	{
		PT(PandaNode)pandaNode =
		thisNP.find_all_matches("**/+RopeNode").get_path(
				0).node();
		if (pandaNode && pandaNode->is_of_type(RopeNode::get_class_type()))
		{
			//the child is the rope node: reparent to the owner object node path
			mRopeNodePath = thisNP.get_child(0);
			mRopeNodePath.reparent_to(thisNP.get_parent());
		}
	}
}

void SoftBody::onRemoveFromSceneCleanup()
{
	//XXX: HACK: rope node's parent node path correction (see bullet samples)
	if (mBodyType == ROPE)
	{
		if (! mRopeNodePath.is_empty())
		{
			//the child is the rope node reparent to this node path
			mRopeNodePath.reparent_to(thisNP);
		}
	}
}


