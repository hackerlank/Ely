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
#include "bulletSoftBodyWorldInfo.h"
#include "bulletSoftBodyShape.h"
#include "bulletHelper.h"
#include "ropeNode.h"
#include "geomNode.h"
#include "geomTriangles.h"
#include "geomVertexWriter.h"
#include "geomVertexReader.h"
#include "nodePathCollection.h"
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
		BulletSoftBodyNode(do_make_fake_soft_body(), name.c_str())
{
	do_reset();
	// HACK: remove fake soft body and its related data;
	// the other member data remain with the values set:
	// _sync = TransformState::make_identity();
	// _sync_disable = false;
	// _geom = NULL;
	// _curve = NULL;
	// _surface = NULL;
	delete _soft;
	_soft = NULL;
	_shapes.pop_back();
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
	pvector<string> paramValuesStr;
	float value;
	int valueInt;
	//body total mass (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("body_total_mass")).c_str(),
	NULL);
	mBodyTotalMass = (value >= 0.0 ? value : -value);
	//body mass from faces
	mBodyMassFromFaces = (
			mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("body_mass_from_faces"))
					== string("true") ? true : false);
	//air density (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("air_density")).c_str(),
	NULL);
	mAirDensity = (value >= 0.0 ? value : -value);
	//water density (>=0.0)
	value = STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("water_density")).c_str(),
	NULL);
	mWaterDensity = (value >= 0.0 ? value : -value);
	//water offset (>=0.0)
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
		pvector<string> paramValuesStrExt = parseCompoundString(
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
	//radius (each component >=0.0)
	param = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY, string("radius"));
	paramValuesStr = parseCompoundString(param, ',');
	valueNum = paramValuesStr.size();
	if (valueNum < 3)
	{
		paramValuesStr.resize(3, "0.0");
	}
	for (idx = 0; idx < 3; ++idx)
	{
		mRadius[idx] = abs(STRTOF(paramValuesStr[idx].c_str(), NULL));
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

	//object setting
	string object = mTmpl->get_parameter_value(GamePhysicsManager::SOFTBODY,
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
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_BTSoftBody, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * Sets the BTSoftBody up for the given object (if not empty), given the body
 * type and other parameters.
 */
void BTSoftBody::setup(NodePath& objectNP)
{
	RETURN_ON_COND(mSetup,)

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
		do_make_patch(info, mPoints[0],
						mPoints[1], mPoints[2], mPoints[3], mRes[0], mRes[1], mFixeds,
						mGendiags);
		//visualize
		if (!objectNP.is_empty())
		{
			if (objectNP.node()->is_of_type(GeomNode::get_class_type()))
			{
				pandaNode = objectNP.node();
			}
			else
			{
				pandaNode = objectNP.find_all_matches("**/+GeomNode").get_path(
						0).node();
			}
		}
		if (mShowModel)
		{
			//visualize with model GeomNode (if any)
			if (pandaNode)
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
			if (pandaNode)
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
		do_make_ellipsoid(info, mPoints[0], mRadius, mRes[0]);
		//visualize
		if (!objectNP.is_empty())
		{
			if (objectNP.node()->is_of_type(GeomNode::get_class_type()))
			{
				pandaNode = objectNP.node();
			}
			else
			{
				pandaNode = objectNP.find_all_matches("**/+GeomNode").get_path(
						0).node();
			}
		}
		if (mShowModel)
		{
			//visualize with model GeomNode (if any)
			if (pandaNode)
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
			if (pandaNode)
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
			if (objectNP.node()->is_of_type(GeomNode::get_class_type()))
			{
				pandaNode = objectNP.node();
			}
			else
			{
				pandaNode = objectNP.find_all_matches("**/+GeomNode").get_path(
						0).node();
			}
		}
		if (pandaNode)
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
		do_make_tri_mesh(info, geom, mRandomizeConstraints);
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
				do_make_tet_mesh(info, buffers["elems"], buffers["faces"],
						buffers["nodes"]);
				//visualize
				if (!objectNP.is_empty())
				{
					if (objectNP.node()->is_of_type(GeomNode::get_class_type()))
					{
						pandaNode = objectNP.node();
					}
					else
					{
						pandaNode = objectNP.find_all_matches("**/+GeomNode").get_path(
								0).node();
					}
				}
				if (mShowModel)
				{
					//visualize with model GeomNode (if any)
					if (pandaNode)
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
					if (pandaNode)
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
			do_make_tet_mesh(info, NULL, NULL, nodeBuf);
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
		do_make_rope(info, mPoints[0], mPoints[1], mRes[0], mFixeds);
		//link with NURBS curve
		PT(NurbsCurveEvaluator)curve = new NurbsCurveEvaluator();
		curve->reset(mRes[0] + 2);
		this->link_curve(curve);
		//visualize with RopeNode (if any)
		if (!objectNP.is_empty())
		{
			if (objectNP.node()->is_of_type(RopeNode::get_class_type()))
			{
				pandaNode = objectNP.node();
			}
			else
			{
				pandaNode = objectNP.find_all_matches("**/+RopeNode").get_path(
						0).node();
			}
		}
		if (pandaNode)
		{
			DCAST(RopeNode, pandaNode)->set_curve(curve);
		}
	}

	//set total mass
	this->set_total_mass(mBodyTotalMass, mBodyMassFromFaces);

	//attach to Bullet World
	GamePhysicsManager::get_global_ptr()->get_bullet_world()->attach(this);

	NodePath thisNP = NodePath::any_path(this);
	if (! objectNP.is_empty())
	{
		// inherit the TrasformState from the object
		set_transform(objectNP.node()->get_transform());
		// reset object's TrasformState
		objectNP.set_transform(TransformState::make_identity());
		// reparent the object node path to this
		objectNP.reparent_to(thisNP);
	}

	//HACK: rope node's parent node path correction (see bullet samples)
	if (mBodyType == ROPE)
	{
		PT(PandaNode)pandaNode =
				thisNP.find_all_matches("**/+RopeNode").get_path(0).node();
		if (pandaNode && pandaNode->is_of_type(RopeNode::get_class_type()))
		{
			// the child is the rope node: reparent to the parent of
			// this node (thisNP.get_parent()), ie the reference node path
			mRopeNodePath = thisNP.get_child(0);
			mRopeNodePath.reparent_to(mReferenceNP);
		}
	}

	// set the flag
	mSetup = true;
}


/**
 * Makes a fake (temporary) Bullet soft body.
 * \note Internal use only.
 */
btSoftBody* BTSoftBody::do_make_fake_soft_body()
{
	BulletSoftBodyWorldInfo info =
			GamePhysicsManager::get_global_ptr()->get_bullet_world()->get_world_info();

	return btSoftBodyHelpers::CreateRope(info.get_info(),
			LVecBase3_to_btVector3(LPoint3f::zero()),
			LVecBase3_to_btVector3(LPoint3f::zero()), 0, 0);
}

/**
 * Sets the actual Bullet soft body.
 * \note Internal use only.
 */
void BTSoftBody::do_set_soft_body(btSoftBody* body)
{
	// Softbody
	_soft = body;
	_soft->setUserPointer(this);
	// Shape
	btCollisionShape *shape_ptr = _soft->getCollisionShape();
	nassertv(shape_ptr != NULL);
	nassertv(shape_ptr->getShapeType() == SOFTBODY_SHAPE_PROXYTYPE);
	_shapes.push_back(
			new BulletSoftBodyShape((btSoftBodyCollisionShape *) shape_ptr));
}

/**
 * Makes a rope Bullet soft body.
 * \note Internal use only.
 */
void BTSoftBody::do_make_rope(BulletSoftBodyWorldInfo &info,
		const LPoint3 &from, const LPoint3 &to, int res, int fixeds)
{
	btSoftBody *body = btSoftBodyHelpers::CreateRope(info.get_info(),
			LVecBase3_to_btVector3(from), LVecBase3_to_btVector3(to), res,
			fixeds);

	// set the soft body and its related data
	do_set_soft_body(body);
}

/**
 * Makes a patch Bullet soft body.
 * \note Internal use only.
 */
void BTSoftBody::do_make_patch(BulletSoftBodyWorldInfo &info,
		const LPoint3 &corner00, const LPoint3 &corner10,
		const LPoint3 &corner01, const LPoint3 &corner11, int resx, int resy,
		int fixeds, bool gendiags)
{
	btSoftBody *body = btSoftBodyHelpers::CreatePatch(info.get_info(),
			LVecBase3_to_btVector3(corner00), LVecBase3_to_btVector3(corner10),
			LVecBase3_to_btVector3(corner01), LVecBase3_to_btVector3(corner11),
			resx, resy, fixeds, gendiags);

	// set the soft body and its related data
	do_set_soft_body(body);
}

/**
 * Makes a ellipsoid Bullet soft body.
 * \note Internal use only.
 */
void BTSoftBody::do_make_ellipsoid(BulletSoftBodyWorldInfo &info,
		const LPoint3 &center, const LVecBase3 &radius, int res)
{
	btSoftBody *body = btSoftBodyHelpers::CreateEllipsoid(info.get_info(),
			LVecBase3_to_btVector3(center), LVecBase3_to_btVector3(radius),
			res);

	// set the soft body and its related data
	do_set_soft_body(body);
}

/**
 * Makes a triangle mesh Bullet soft body.
 * \note Internal use only.
 */
void BTSoftBody::do_make_tri_mesh(BulletSoftBodyWorldInfo &info,
		const Geom *geom, bool randomizeConstraints)
{
	// Read vertex data
	PTA_LVecBase3 points;
	PTA_int indices;
	CPT(GeomVertexData)vdata = geom->get_vertex_data();
	nassertv(vdata->has_column(InternalName::get_vertex()));
	GeomVertexReader vreader(vdata, InternalName::get_vertex());
	while (!vreader.is_at_end())
	{
		LVecBase3 v = vreader.get_data3();
		points.push_back(v);
	}
	// Read indices
	for (int i = 0; i < geom->get_num_primitives(); i++)
	{
		CPT(GeomPrimitive)prim = geom->get_primitive(i);
		prim = prim->decompose();
		for (int j=0; j<prim->get_num_primitives(); j++)
		{
			int s = prim->get_primitive_start(j);
			int e = prim->get_primitive_end(j);
			for (int k=s; k<e; k++)
			{
				indices.push_back(prim->get_vertex(k));
			}
		}
	}
	btSoftBody *body = NULL;
	{
		// Eliminate duplicate vertices
		PTA_LVecBase3 mapped_points;
		PTA_int mapped_indices;
		pmap<int, int> mapping;
		for (PTA_LVecBase3::size_type i = 0; i < points.size(); i++)
		{
			LVecBase3 p = points[i];
			int j = get_point_index(p, mapped_points);
			if (j < 0)
			{
				mapping[i] = mapped_points.size();
				mapped_points.push_back(p);
			}
			else
			{
				mapping[i] = j;
			}
		}
		for (PTA_int::size_type i = 0; i < indices.size(); i++)
		{
			int idx = indices[i];
			int mapped_idx = mapping[idx];
			mapped_indices.push_back(mapped_idx);
		}
		points = mapped_points;
		indices = mapped_indices;
		// Convert arrays
		int num_vertices = points.size();
		int num_triangles = indices.size() / 3;
		btScalar *vertices = new btScalar[num_vertices * 3];
		for (int i = 0; i < num_vertices; i++)
		{
			vertices[3 * i] = points[i].get_x();
			vertices[3 * i + 1] = points[i].get_y();
			vertices[3 * i + 2] = points[i].get_z();
		}
		int *triangles = new int[num_triangles * 3];
		for (int i = 0; i < num_triangles * 3; i++)
		{
			triangles[i] = indices[i];
		}
		// Create body
		body = btSoftBodyHelpers::CreateFromTriMesh(info.get_info(),
				vertices, triangles, num_triangles, randomizeConstraints);
		nassertv(body);

		delete[] vertices;
		delete[] triangles;
	}

	// set the soft body and its related data
	do_set_soft_body(body);
}

/**
 * Makes a tetra mesh Bullet soft body.
 * \note Internal use only.
 */
void BTSoftBody::do_make_tet_mesh(BulletSoftBodyWorldInfo &info,
		const char *ele, const char *face, const char *node)
{
	nassertv(node && node[0]);

	// Nodes
	btAlignedObjectArray<btVector3> pos;
	int npos = 0;
	int ndims = 0; // not used
	int nattrb = 0; // not used
	int hasbounds = 0; // not used
	sscanf(node, "%d %d %d %d", &npos, &ndims, &nattrb, &hasbounds);
	node += next_line(node);
	pos.resize(npos);
	for (int i = 0; i < pos.size(); ++i)
	{
		int index = 0;
		float x, y, z;

		sscanf(node, "%d %f %f %f", &index, &x, &y, &z);
		node += next_line(node);

		pos[index].setX(btScalar(x));
		pos[index].setY(btScalar(y));
		pos[index].setZ(btScalar(z));
	}
	// Body
	btSoftBody *body = new btSoftBody(&info.get_info(), npos, &pos[0], 0);
	// Faces
	if (face && face[0])
	{
		int nface = 0;
		int hasbounds = 0; // not used
		sscanf(face, "%d %d", &nface, &hasbounds);
		face += next_line(face);
		for (int i = 0; i < nface; ++i)
		{
			int index = 0;
			int ni[3];
			sscanf(face, "%d %d %d %d", &index, &ni[0], &ni[1], &ni[2]);
			face += next_line(face);
			body->appendFace(ni[0], ni[1], ni[2]);
		}
	}
	// Links
	if (ele && ele[0])
	{
		int ntetra = 0;
		int ncorner = 0;
		int neattrb = 0;
		sscanf(ele, "%d %d %d", &ntetra, &ncorner, &neattrb);
		ele += next_line(ele);
		for (int i = 0; i < ntetra; ++i)
		{
			int index = 0;
			int ni[4];
			sscanf(ele, "%d %d %d %d %d", &index, &ni[0], &ni[1], &ni[2],
					&ni[3]);
			ele += next_line(ele);
			body->appendTetra(ni[0], ni[1], ni[2], ni[3]);
			body->appendLink(ni[0], ni[1], 0, true);
			body->appendLink(ni[1], ni[2], 0, true);
			body->appendLink(ni[2], ni[0], 0, true);
			body->appendLink(ni[0], ni[3], 0, true);
			body->appendLink(ni[1], ni[3], 0, true);
			body->appendLink(ni[2], ni[3], 0, true);
		}
	}

	// set the soft body and its related data
	do_set_soft_body(body);
}

/**
 * On destruction cleanup.
 * Gives an BTSoftBody the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void BTSoftBody::do_finalize()
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

/**
 * Cleans the BTSoftBody up from detaching any child objects (if not empty).
 */
void BTSoftBody::cleanup()
{

	RETURN_ON_COND(!mSetup,)

	NodePath thisNP = NodePath::any_path(this);

	//HACK: rope node's parent node path correction (see bullet samples)
	if (mBodyType == ROPE)
	{
		if (! mRopeNodePath.is_empty())
		{
			//the child is the rope node reparent to this node path
			mRopeNodePath.reparent_to(thisNP);
		}
	}

	// remove rigid body from the physics world
	GamePhysicsManager::GetSingletonPtr()->get_bullet_world()->remove(this);

	// HACK: remove fake soft body and its related data;
	// the other member data will reset the values:
	_sync = TransformState::make_identity();
	_sync_disable = false;
	_geom.clear();
	_curve.clear();
	_surface.clear();
	delete _soft;
	_soft = NULL;
	_shapes.pop_back();

	// detach this rigid body's children
	for (int i = 0; i < thisNP.get_num_children(); ++i)
	{
		NodePath childNP = thisNP.get_child(i);
		// detach childNP
		thisNP.get_child(i).detach_node();
	}

	// set the flag
	mSetup = false;
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
	BulletSoftBodyNode::write_datagram(manager, dg);

	///Name of this BTSoftBody.
	dg.add_string(get_name());

	///TODO

	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int BTSoftBody::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = BulletSoftBodyNode::complete_pointers(p_list, manager);

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
					"BTSoftBody").node());

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
	BulletSoftBodyNode::fillin(scan, manager);

	///Name of this BTSoftBody.
	set_name(scan.get_string());

	///TODO

	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle BTSoftBody::_type_handle;

