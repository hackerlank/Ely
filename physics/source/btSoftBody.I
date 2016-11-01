/**
 * \file btSoftBody.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef BTSOFTBODY_I_
#define BTSOFTBODY_I_


///BTSoftBody inline definitions

/**
 * Sets the reference NodePath of this BTSoftBody.
 */
INLINE void BTSoftBody::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Sets the body type.
 */
INLINE void BTSoftBody::set_body_type(BTBodyType value)
{
	mBodyType = value;
}

/**
 * Returns the body type.
 */
INLINE BTSoftBody::BTBodyType BTSoftBody::get_body_type() const
{
	return mBodyType;
}

/**
 * Sets the body_total_mass (>=0.0).
 */
INLINE void BTSoftBody::set_body_total_mass(float value)
{
	mBodyTotalMass = abs(value);
}

/**
 * Returns the body_total_mass (>=0.0).
 */
INLINE float BTSoftBody::get_body_total_mass() const
{
	return mBodyTotalMass;
}

/**
 * Sets the body_mass_from_faces.
 */
INLINE void BTSoftBody::set_body_mass_from_faces(bool value)
{
	mBodyMassFromFaces = value;
}

/**
 * Returns the body_mass_from_faces.
 */
INLINE bool BTSoftBody::get_body_mass_from_faces() const
{
	return mBodyMassFromFaces;
}

/**
 * Sets the air_density (>=0.0).
 */
INLINE void BTSoftBody::set_air_density(float value)
{
	mAirDensity = abs(value);
}

/**
 * Returns the air_density (>=0.0).
 */
INLINE float BTSoftBody::get_air_density() const
{
	return mAirDensity;
}

/**
 * Sets the water_density (>=0.0).
 */
INLINE void BTSoftBody::set_water_density(float value)
{
	mWaterDensity = abs(value);
}

/**
 * Returns the water_density (>=0.0).
 */
INLINE float BTSoftBody::get_water_density() const
{
	return mWaterDensity;
}

/**
 * Sets the water_offset (>=0.0).
 */
INLINE void BTSoftBody::set_water_offset(float value)
{
	mWaterOffset = abs(value);
}

/**
 * Returns the water_offset (>=0.0).
 */
INLINE float BTSoftBody::get_water_offset() const
{
	return mWaterOffset;
}

/**
 * Sets the water_normal.
 */
INLINE void BTSoftBody::set_water_normal(const LVector3f& value)
{
	mWaterNormal = value;
}

/**
 * Returns the water_normal.
 */
INLINE LVector3f BTSoftBody::get_water_normal() const
{
	return mWaterNormal;
}

/**
 * Sets the show_model.
 */
INLINE void BTSoftBody::set_show_model(bool value)
{
	mShowModel = value;
}

/**
 * Returns the show_model.
 */
INLINE bool BTSoftBody::get_show_model() const
{
	return mShowModel;
}

/**
 * Sets the points.
 */
INLINE void BTSoftBody::set_points(const ValueList<LPoint3f>& value)
{
	mPoints.clear();
	for (int i = 0; i < value.get_num_values(); ++i)
	{
		mPoints.push_back(value.get_value(i));
	}
}

/**
 * Returns the points.
 */
INLINE ValueList<LPoint3f> BTSoftBody::get_points() const
{
	ValueList<LPoint3f> points;
	for (unsigned int i = 0; i < mPoints.size(); ++i)
	{
		points.add_value(mPoints[i]);
	}
	return points;
}

/**
 * Sets the res.
 */
INLINE void BTSoftBody::set_res(const ValueList<int>& value)
{
	mRes.clear();
	for (int i = 0; i < value.get_num_values(); ++i)
	{
		mRes.push_back(value.get_value(i));
	}
}

/**
 * Returns the res.
 */
INLINE ValueList<int> BTSoftBody::get_res() const
{
	ValueList<int> res;
	for (unsigned int i = 0; i < mRes.size(); ++i)
	{
		res.add_value(mRes[i]);
	}
	return res;
}

/**
 * Sets the fixeds.
 */
INLINE void BTSoftBody::set_fixeds(int value)
{
	mFixeds = value;
}

/**
 * Returns the fixeds.
 */
INLINE int BTSoftBody::get_fixeds() const
{
	return mFixeds;
}

/**
 * Sets the gendiags.
 */
INLINE void BTSoftBody::set_gendiags(bool value)
{
	mGendiags = value;
}

/**
 * Returns the gendiags.
 */
INLINE bool BTSoftBody::get_gendiags() const
{
	return mGendiags;
}

/**
 * Sets the radius (each component >=0.0).
 */
INLINE void BTSoftBody::set_radius(const LVecBase3f& value)
{
	mRadius = LVecBase3f(abs(value.get_x()), abs(value.get_y()),
			abs(value.get_z()));
}

/**
 * Returns the radius (each component >=0.0).
 */
INLINE LVecBase3f BTSoftBody::get_radius() const
{
	return mRadius;
}

/**
 * Sets the randomize_constraints.
 */
INLINE void BTSoftBody::set_randomize_constraints(bool value)
{
	mRandomizeConstraints = value;
}

/**
 * Returns the randomize_constraints.
 */
INLINE bool BTSoftBody::get_randomize_constraints() const
{
	return mRandomizeConstraints;
}

/**
 * Sets the tetra_data_files.
 */
INLINE void BTSoftBody::set_tetra_data_files(const ValueList<string>& value)
{
	if (value.get_num_values() >= 3)
	{
		mTetraDataFileNames["elems"] = value.get_value(0);
		mTetraDataFileNames["faces"] = value.get_value(1);
		mTetraDataFileNames["nodes"] = value.get_value(2);
	}
}

/**
 * Returns the tetra_data_files.
 */
INLINE ValueList<string> BTSoftBody::get_tetra_data_files() const
{
	ValueList<string> tetraFiles;
	if (mTetraDataFileNames.size() >= 3)
	{
		tetraFiles.add_value(mTetraDataFileNames.find("elems")->second);
		tetraFiles.add_value(mTetraDataFileNames.find("faces")->second);
		tetraFiles.add_value(mTetraDataFileNames.find("nodes")->second);
	}
	return tetraFiles;
}

/**
 * Resets the BTSoftBody.
 * \note Internal use only.
 */
inline void BTSoftBody::do_reset()
{
	//
	mSetup = false;
	mBodyType = ROPE;
	mBodyTotalMass = mAirDensity = mWaterDensity = mWaterOffset = 0.0;
	mBodyMassFromFaces = false;
	mWaterNormal = LVector3f::zero();
	mShowModel = false;
	mPoints.clear();
	mRes.clear();
	mFixeds = 0;
	mGendiags = true;
	mRadius = LVecBase3f::zero();
	mRandomizeConstraints = true;
	mTetraDataFileNames.clear();
	mRopeNodePath.clear();
	mReferenceNP.clear();
#ifdef PYTHON_BUILD
	mSelf = NULL;
	mUpdateCallback = NULL;
	mUpdateArgList = NULL;
#else
	mUpdateCallback = NULL;
#endif //PYTHON_BUILD
}

INLINE ostream &operator <<(ostream &out, const BTSoftBody& soft_body)
{
	soft_body.output(out);
	return out;
}

#endif /* BTSOFTBODY_I_ */
