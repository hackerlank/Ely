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
 * Sets the body_total_mass.
 */
INLINE void BTSoftBody::set_body_total_mass(float value)
{
}

/**
 * Returns the body_total_mass.
 */
INLINE float BTSoftBody::get_body_total_mass() const
{
}

/**
 * Sets the body_mass_from_faces.
 */
INLINE void BTSoftBody::set_body_mass_from_faces(bool value)
{
}

/**
 * Returns the body_mass_from_faces.
 */
INLINE bool BTSoftBody::get_body_mass_from_faces() const
{
}

/**
 * Sets the air_density.
 */
INLINE void BTSoftBody::set_air_density(float value)
{
}

/**
 * Returns the air_density.
 */
INLINE float BTSoftBody::get_air_density() const
{
}

/**
 * Sets the water_density.
 */
INLINE void BTSoftBody::set_water_density(float value)
{
}

/**
 * Returns the water_density.
 */
INLINE float BTSoftBody::get_water_density() const
{
}

/**
 * Sets the water_offset.
 */
INLINE void BTSoftBody::set_water_offset(float value)
{
}

/**
 * Returns the water_offset.
 */
INLINE float BTSoftBody::get_water_offset() const
{
}

/**
 * Sets the water_normal.
 */
INLINE void BTSoftBody::set_water_normal(const LVector3f& value)
{
}

/**
 * Returns the water_normal.
 */
INLINE LVector3f BTSoftBody::get_water_normal() const
{
}

/**
 * Sets the show_model.
 */
INLINE void BTSoftBody::set_show_model(bool value)
{
}

/**
 * Returns the show_model.
 */
INLINE bool BTSoftBody::get_show_model() const
{
}

/**
 * Sets the points.
 */
INLINE void BTSoftBody::set_points(const ValueList<LPoint3f>& value)
{
}

/**
 * Returns the points.
 */
INLINE ValueList<LPoint3f> BTSoftBody::get_points() const
{
}

/**
 * Sets the res.
 */
INLINE void BTSoftBody::set_res(const ValueList<int>& value)
{
}

/**
 * Returns the res.
 */
INLINE ValueList<int> BTSoftBody::get_res() const
{
}

/**
 * Sets the fixeds.
 */
INLINE void BTSoftBody::set_fixeds(int value)
{
}

/**
 * Returns the fixeds.
 */
INLINE int BTSoftBody::get_fixeds() const
{
}

/**
 * Sets the gendiags.
 */
INLINE void BTSoftBody::set_gendiags(bool value)
{
}

/**
 * Returns the gendiags.
 */
INLINE bool BTSoftBody::get_gendiags() const
{
}

/**
 * Sets the radius.
 */
INLINE void BTSoftBody::set_radius(const LVecBase3f& value)
{
}

/**
 * Returns the radius.
 */
INLINE LVecBase3f BTSoftBody::get_radius() const
{
}

/**
 * Sets the randomize_constraints.
 */
INLINE void BTSoftBody::set_randomize_constraints(bool value)
{
}

/**
 * Returns the randomize_constraints.
 */
INLINE bool BTSoftBody::get_randomize_constraints() const
{
}

/**
 * Sets the tetra_data_files.
 */
INLINE void BTSoftBody::set_tetra_data_files(const ValueList<string>& value)
{
}

/**
 * Returns the tetra_data_files.
 */
INLINE ValueList<string> BTSoftBody::get_tetra_data_files() const
{
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
