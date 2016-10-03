/**
 * \file aiTools.I
 *
 * \date 2016-09-17
 * \author consultit
 */

#ifndef AITOOLS_I_
#define AITOOLS_I_

///Vehicle settings.
INLINE float OSVehicleSettings::get_mass() const
{
	return _vehicleSettings.m_mass;
}
INLINE void OSVehicleSettings::set_mass(float value)
{
	_vehicleSettings.m_mass = value;
}
INLINE float OSVehicleSettings::get_radius() const
{
	return _vehicleSettings.m_radius;
}
INLINE void OSVehicleSettings::set_radius(float value)
{
	_vehicleSettings.m_radius = value;
}
INLINE float OSVehicleSettings::get_speed() const
{
	return _vehicleSettings.m_speed;
}
INLINE void OSVehicleSettings::set_speed(float value)
{
	_vehicleSettings.m_speed = value;
}
INLINE float OSVehicleSettings::get_maxForce() const
{
	return _vehicleSettings.m_maxForce;
}
INLINE void OSVehicleSettings::set_maxForce(float value)
{
	_vehicleSettings.m_maxForce = value;
}
INLINE float OSVehicleSettings::get_maxSpeed() const
{
	return _vehicleSettings.m_maxSpeed;
}
INLINE void OSVehicleSettings::set_maxSpeed(float value)
{
	_vehicleSettings.m_maxSpeed = value;
}
INLINE LVector3f OSVehicleSettings::get_forward() const
{
	return ossup::OpenSteerVec3ToLVecBase3f(_vehicleSettings.m_forward);
}
INLINE void OSVehicleSettings::set_forward(const LVector3f& value)
{
	_vehicleSettings.m_forward =
			ossup::LVecBase3fToOpenSteerVec3(value).normalize();
}
INLINE LVector3f OSVehicleSettings::get_side() const
{
	return ossup::OpenSteerVec3ToLVecBase3f(_vehicleSettings.m_side);
}
INLINE void OSVehicleSettings::set_side(const LVector3f& value)
{
	_vehicleSettings.m_side =
			ossup::LVecBase3fToOpenSteerVec3(value).normalize();
}
INLINE LVector3f OSVehicleSettings::get_up() const
{
	return ossup::OpenSteerVec3ToLVecBase3f(_vehicleSettings.m_up);
}
INLINE void OSVehicleSettings::set_up(const LVector3f& value)
{
	_vehicleSettings.m_up = ossup::LVecBase3fToOpenSteerVec3(value).normalize();
}
INLINE LPoint3f OSVehicleSettings::get_position() const
{
	return ossup::OpenSteerVec3ToLVecBase3f(_vehicleSettings.m_position);
}
INLINE void OSVehicleSettings::set_position(const LPoint3f& value)
{
	_vehicleSettings.m_position = ossup::LVecBase3fToOpenSteerVec3(value);
}
INLINE LPoint3f OSVehicleSettings::get_start() const
{
	return ossup::OpenSteerVec3ToLVecBase3f(_vehicleSettings.m_start);
}
INLINE void OSVehicleSettings::set_start(const LPoint3f& value)
{
	_vehicleSettings.m_start = ossup::LVecBase3fToOpenSteerVec3(value);
}
INLINE float OSVehicleSettings::get_path_pred_time() const
{
	return _vehicleSettings.m_pathPredTime;
}
INLINE void OSVehicleSettings::set_path_pred_time(float value)
{
	_vehicleSettings.m_pathPredTime = value;
}
INLINE float OSVehicleSettings::get_obstacle_min_time_coll() const
{
	return _vehicleSettings.m_obstacleMinTimeColl;
}
INLINE void OSVehicleSettings::set_obstacle_min_time_coll(float value)
{
	_vehicleSettings.m_obstacleMinTimeColl = value;
}
INLINE float OSVehicleSettings::get_neighbor_min_time_coll() const
{
	return _vehicleSettings.m_neighborMinTimeColl;
}
INLINE void OSVehicleSettings::set_neighbor_min_time_coll(float value)
{
	_vehicleSettings.m_neighborMinTimeColl = value;
}
INLINE float OSVehicleSettings::get_neighbor_min_sep_dist() const
{
	return _vehicleSettings.m_neighborMinSepDist;
}
INLINE void OSVehicleSettings::set_neighbor_min_sep_dist(float value)
{
	_vehicleSettings.m_neighborMinSepDist = value;
}
INLINE float OSVehicleSettings::get_separation_max_dist() const
{
	return _vehicleSettings.m_separationMaxDist;
}
INLINE void OSVehicleSettings::set_separation_max_dist(float value)
{
	_vehicleSettings.m_separationMaxDist = value;
}
INLINE float OSVehicleSettings::get_separation_cos_max_angle() const
{
	return _vehicleSettings.m_separationCosMaxAngle;
}
INLINE void OSVehicleSettings::set_separation_cos_max_angle(float value)
{
	_vehicleSettings.m_separationCosMaxAngle = value;
}
INLINE float OSVehicleSettings::get_alignment_max_dist() const
{
	return _vehicleSettings.m_alignmentMaxDist;
}
INLINE void OSVehicleSettings::set_alignment_max_dist(float value)
{
	_vehicleSettings.m_alignmentMaxDist = value;
}
INLINE float OSVehicleSettings::get_alignment_cos_max_angle() const
{
	return _vehicleSettings.m_alignmentCosMaxAngle;
}
INLINE void OSVehicleSettings::set_alignment_cos_max_angle(float value)
{
	_vehicleSettings.m_alignmentCosMaxAngle = value;
}
INLINE float OSVehicleSettings::get_cohesion_max_dist() const
{
	return _vehicleSettings.m_cohesionMaxDist;
}
INLINE void OSVehicleSettings::set_cohesion_max_dist(float value)
{
	_vehicleSettings.m_cohesionMaxDist = value;
}
INLINE float OSVehicleSettings::get_cohesion_cos_max_angle() const
{
	return _vehicleSettings.m_cohesionCosMaxAngle;
}
INLINE void OSVehicleSettings::set_cohesion_cos_max_angle(float value)
{
	_vehicleSettings.m_cohesionCosMaxAngle = value;
}
INLINE float OSVehicleSettings::get_pursuit_max_pred_time() const
{
	return _vehicleSettings.m_pursuitMaxPredTime;
}
INLINE void OSVehicleSettings::set_pursuit_max_pred_time(float value)
{
	_vehicleSettings.m_pursuitMaxPredTime = value;
}
INLINE float OSVehicleSettings::get_evasion_max_pred_time() const
{
	return _vehicleSettings.m_evasionMaxPredTime;
}
INLINE void OSVehicleSettings::set_evasion_max_pred_time(float value)
{
	_vehicleSettings.m_evasionMaxPredTime = value;
}
INLINE float OSVehicleSettings::get_target_speed() const
{
	return _vehicleSettings.m_targetSpeed;
}
INLINE void OSVehicleSettings::set_target_speed(float value)
{
	_vehicleSettings.m_targetSpeed = value;
}
INLINE ostream &operator << (ostream &out, const OSVehicleSettings & settings)
{
	settings.output(out);
	return out;
}

///Flock settings.
INLINE float OSFlockSettings::get_separation_weight() const
{
	return _separationWeight;
}
INLINE void OSFlockSettings::set_separation_weight(float value)
{
	_separationWeight = value;
}
INLINE float OSFlockSettings::get_alignment_weight() const
{
	return _alignmentWeight;
}
INLINE void OSFlockSettings::set_alignment_weight(float value)
{
	_alignmentWeight = value;
}
INLINE float OSFlockSettings::get_cohesion_weight() const
{
	return _cohesionWeight;
}
INLINE void OSFlockSettings::set_cohesion_weight(float value)
{
	_cohesionWeight = value;
}
INLINE ostream &operator << (ostream &out, const OSFlockSettings & settings)
{
	settings.output(out);
	return out;
}

///OSObstacleSettings.
INLINE bool OSObstacleSettings::operator==(
		const OSObstacleSettings &other) const
{
	return (_type == other._type) && (_seenFromState == other._seenFromState)
			&& (_position == other._position) && (_forward == other._forward)
			&& (_up == other._up) && (_side == other._side)
			&& (_width == other._width) && (_height == other._height)
			&& (_depth == other._depth) && (_radius == other._radius)
			&& (_ref == other._ref) && (_obstacle == other._obstacle);
}
INLINE string OSObstacleSettings::get_type() const
{
	return _type;
}
INLINE void OSObstacleSettings::set_type(const string& value)
{
	_type = value;
}
INLINE string OSObstacleSettings::get_seenFromState() const
{
	return _seenFromState;
}
INLINE void OSObstacleSettings::set_seenFromState(const string& value)
{
	_seenFromState = value;
}
INLINE LPoint3f OSObstacleSettings::get_position() const
{
	return _position;
}
INLINE void OSObstacleSettings::set_position(const LPoint3f& value)
{
	_position = value;
}
INLINE LVector3f OSObstacleSettings::get_forward() const
{
	return _forward;
}
INLINE void OSObstacleSettings::set_forward(const LVector3f& value)
{
	_forward = value;
}
INLINE LVector3f OSObstacleSettings::get_up() const
{
	return _up;
}
INLINE void OSObstacleSettings::set_up(const LVector3f& value)
{
	_up = value;
}
INLINE LVector3f OSObstacleSettings::get_side() const
{
	return _side;
}
INLINE void OSObstacleSettings::set_side(const LVector3f& value)
{
	_side = value;
}
INLINE float OSObstacleSettings::get_width() const
{
	return _width;
}
INLINE void OSObstacleSettings::set_width(float value)
{
	_width = value;
}
INLINE float OSObstacleSettings::get_height() const
{
	return _height;
}
INLINE void OSObstacleSettings::set_height(float value)
{
	_height = value;
}
INLINE float OSObstacleSettings::get_depth() const
{
	return _depth;
}
INLINE void OSObstacleSettings::set_depth(float value)
{
	_depth = value;
}
INLINE float OSObstacleSettings::get_radius() const
{
	return _radius;
}
INLINE void OSObstacleSettings::set_radius(float value)
{
	_radius = value;
}
INLINE int OSObstacleSettings::get_ref() const
{
	return _ref;
}
INLINE void OSObstacleSettings::set_ref(int value)
{
	_ref = value;
}
INLINE ostream &operator << (ostream &out, const OSObstacleSettings & settings)
{
	settings.output(out);
	return out;
}
inline OpenSteer::AbstractObstacle* OSObstacleSettings::get_obstacle() const
{
	return _obstacle;
}
inline void OSObstacleSettings::set_obstacle(OpenSteer::AbstractObstacle* value)
{
	_obstacle = value;
}

///NavMeshSettings
INLINE float RNNavMeshSettings::get_cellSize() const
{
	return _navMeshSettings.m_cellSize;
}
INLINE void RNNavMeshSettings::set_cellSize(float value)
{
	_navMeshSettings.m_cellSize = value;
}
INLINE float RNNavMeshSettings::get_cellHeight() const
{
	return _navMeshSettings.m_cellHeight;
}
INLINE void RNNavMeshSettings::set_cellHeight(float value)
{
	_navMeshSettings.m_cellHeight = value;
}
INLINE float RNNavMeshSettings::get_agentHeight() const
{
	return _navMeshSettings.m_agentHeight;
}
INLINE void RNNavMeshSettings::set_agentHeight(float value)
{
	_navMeshSettings.m_agentHeight = value;
}
INLINE float RNNavMeshSettings::get_agentRadius() const
{
	return _navMeshSettings.m_agentRadius;
}
INLINE void RNNavMeshSettings::set_agentRadius(float value)
{
	_navMeshSettings.m_agentRadius = value;
}
INLINE float RNNavMeshSettings::get_agentMaxClimb() const
{
	return _navMeshSettings.m_agentMaxClimb;
}
INLINE void RNNavMeshSettings::set_agentMaxClimb(float value)
{
	_navMeshSettings.m_agentMaxClimb = value;
}
INLINE float RNNavMeshSettings::get_agentMaxSlope() const
{
	return _navMeshSettings.m_agentMaxSlope;
}
INLINE void RNNavMeshSettings::set_agentMaxSlope(float value)
{
	_navMeshSettings.m_agentMaxSlope = value;
}
INLINE float RNNavMeshSettings::get_regionMinSize() const
{
	return _navMeshSettings.m_regionMinSize;
}
INLINE void RNNavMeshSettings::set_regionMinSize(float value)
{
	_navMeshSettings.m_regionMinSize = value;
}
INLINE float RNNavMeshSettings::get_regionMergeSize() const
{
	return _navMeshSettings.m_regionMergeSize;
}
INLINE void RNNavMeshSettings::set_regionMergeSize(float value)
{
	_navMeshSettings.m_regionMergeSize = value;
}
INLINE float RNNavMeshSettings::get_edgeMaxLen() const
{
	return _navMeshSettings.m_edgeMaxLen;
}
INLINE void RNNavMeshSettings::set_edgeMaxLen(float value)
{
	_navMeshSettings.m_edgeMaxLen = value;
}
INLINE float RNNavMeshSettings::get_edgeMaxError() const
{
	return _navMeshSettings.m_edgeMaxError;
}
INLINE void RNNavMeshSettings::set_edgeMaxError(float value)
{
	_navMeshSettings.m_edgeMaxError = value;
}
INLINE float RNNavMeshSettings::get_vertsPerPoly() const
{
	return _navMeshSettings.m_vertsPerPoly;
}
INLINE void RNNavMeshSettings::set_vertsPerPoly(float value)
{
	_navMeshSettings.m_vertsPerPoly = value;
}
INLINE float RNNavMeshSettings::get_detailSampleDist() const
{
	return _navMeshSettings.m_detailSampleDist;
}
INLINE void RNNavMeshSettings::set_detailSampleDist(float value)
{
	_navMeshSettings.m_detailSampleDist = value;
}
INLINE float RNNavMeshSettings::get_detailSampleMaxError() const
{
	return _navMeshSettings.m_detailSampleMaxError;
}
INLINE void RNNavMeshSettings::set_detailSampleMaxError(float value)
{
	_navMeshSettings.m_detailSampleMaxError = value;
}
INLINE int RNNavMeshSettings::get_partitionType() const
{
	return _navMeshSettings.m_partitionType;
}
INLINE void RNNavMeshSettings::set_partitionType(int value)
{
	_navMeshSettings.m_partitionType = value;
}
INLINE ostream &operator << (ostream &out, const RNNavMeshSettings & settings)
{
	settings.output(out);
	return out;
}

///NavMeshTileSettings
INLINE bool RNNavMeshTileSettings::get_buildAllTiles() const
{
	return _navMeshTileSettings.m_buildAllTiles;
}
INLINE void RNNavMeshTileSettings::set_buildAllTiles(bool value)
{
	_navMeshTileSettings.m_buildAllTiles = value;
}
INLINE int RNNavMeshTileSettings::get_maxTiles() const
{
	return _navMeshTileSettings.m_maxTiles;
}
INLINE void RNNavMeshTileSettings::set_maxTiles(int value)
{
	_navMeshTileSettings.m_maxTiles = value;
}
INLINE int RNNavMeshTileSettings::get_maxPolysPerTile() const
{
	return _navMeshTileSettings.m_maxPolysPerTile;
}
INLINE void RNNavMeshTileSettings::set_maxPolysPerTile(int value)
{
	_navMeshTileSettings.m_maxPolysPerTile = value;
}
INLINE float RNNavMeshTileSettings::get_tileSize() const
{
	return _navMeshTileSettings.m_tileSize;
}
INLINE void RNNavMeshTileSettings::set_tileSize(float value)
{
	_navMeshTileSettings.m_tileSize = value;
}
INLINE ostream &operator << (ostream &out, const RNNavMeshTileSettings & settings)
{
	settings.output(out);
	return out;
}

///Convex volume settings.
INLINE bool RNConvexVolumeSettings::operator==(
		const RNConvexVolumeSettings &other) const
{
	return (_area == other._area) && (_flags == other._flags)
			&& (_ref == other._ref);
}
INLINE int RNConvexVolumeSettings::get_area() const
{
	return _area;
}
INLINE void RNConvexVolumeSettings::set_area(int value)
{
	_area = value;
}
INLINE int RNConvexVolumeSettings::get_flags() const
{
	return _flags;
}
INLINE void RNConvexVolumeSettings::set_flags(int value)
{
	_flags = value;
}
INLINE LPoint3f RNConvexVolumeSettings::get_centroid() const
{
	return _centroid;
}
INLINE void RNConvexVolumeSettings::set_centroid(LPoint3f value)
{
	_centroid = value;
}
INLINE int RNConvexVolumeSettings::get_ref() const
{
	return _ref;
}
INLINE void RNConvexVolumeSettings::set_ref(int value)
{
	_ref = value;
}
INLINE ostream &operator << (ostream &out, const RNConvexVolumeSettings & settings)
{
	settings.output(out);
	return out;
}

///Off mesh connection settings.
INLINE bool RNOffMeshConnectionSettings::operator==(
		const RNOffMeshConnectionSettings &other) const
{
	return (_rad == other._rad) && (_bidir == other._bidir)
			&& (_area == other._area) && (_flags == other._flags)
			&& (_ref == other._ref);
}
INLINE float RNOffMeshConnectionSettings::get_rad() const
{
	return _rad;
}
INLINE void RNOffMeshConnectionSettings::set_rad(float value)
{
	_rad = value;
}
INLINE bool RNOffMeshConnectionSettings::get_bidir() const
{
	return _bidir;
}
INLINE void RNOffMeshConnectionSettings::set_bidir(bool value)
{
	_bidir = value;
}
INLINE unsigned int RNOffMeshConnectionSettings::get_userId() const
{
	return _userId;
}
INLINE void RNOffMeshConnectionSettings::set_userId(unsigned int value)
{
	_userId = value;
}
INLINE int RNOffMeshConnectionSettings::get_area() const
{
	return _area;
}
INLINE void RNOffMeshConnectionSettings::set_area(int value)
{
	_area = value;
}
INLINE int RNOffMeshConnectionSettings::get_flags() const
{
	return _flags;
}
INLINE void RNOffMeshConnectionSettings::set_flags(int value)
{
	_flags = value;
}
INLINE int RNOffMeshConnectionSettings::get_ref() const
{
	return _ref;
}
INLINE void RNOffMeshConnectionSettings::set_ref(int value)
{
	_ref = value;
}
INLINE ostream &operator << (ostream &out, const RNOffMeshConnectionSettings & settings)
{
	settings.output(out);
	return out;
}

///Obstacle settings.
INLINE bool RNObstacleSettings::operator==(
		const RNObstacleSettings &other) const
{
	return (_radius == other._radius) && (_dims == other._dims)
			&& (_ref == other._ref);
}
INLINE float RNObstacleSettings::get_radius() const
{
	return _radius;
}
INLINE void RNObstacleSettings::set_radius(float value)
{
	_radius = value;
}
INLINE LVecBase3f RNObstacleSettings::get_dims() const
{
	return _dims;
}
INLINE void RNObstacleSettings::set_dims(const LVecBase3f& value)
{
	_dims = value;
}
INLINE unsigned int RNObstacleSettings::get_ref() const
{
	return _ref;
}
INLINE void RNObstacleSettings::set_ref(unsigned int value)
{
	_ref = value;
}
INLINE ostream &operator << (ostream &out, const RNObstacleSettings & settings)
{
	settings.output(out);
	return out;
}

///CrowdAgentParams
INLINE float RNCrowdAgentParams::get_radius() const
{
	return _dtCrowdAgentParams.radius;
}
INLINE void RNCrowdAgentParams::set_radius(float value)
{
	_dtCrowdAgentParams.radius = value;
}
INLINE float RNCrowdAgentParams::get_height() const
{
	return _dtCrowdAgentParams.height;
}
INLINE void RNCrowdAgentParams::set_height(float value)
{
	_dtCrowdAgentParams.height = value;
}
INLINE float RNCrowdAgentParams::get_maxAcceleration() const
{
	return _dtCrowdAgentParams.maxAcceleration;
}
INLINE void RNCrowdAgentParams::set_maxAcceleration(float value)
{
	_dtCrowdAgentParams.maxAcceleration = value;
}
INLINE float RNCrowdAgentParams::get_maxSpeed() const
{
	return _dtCrowdAgentParams.maxSpeed;
}
INLINE void RNCrowdAgentParams::set_maxSpeed(float value)
{
	_dtCrowdAgentParams.maxSpeed = value;
}
INLINE float RNCrowdAgentParams::get_collisionQueryRange() const
{
	return _dtCrowdAgentParams.collisionQueryRange;
}
INLINE void RNCrowdAgentParams::set_collisionQueryRange(float value)
{
	_dtCrowdAgentParams.collisionQueryRange = value;
}
INLINE float RNCrowdAgentParams::get_pathOptimizationRange() const
{
	return _dtCrowdAgentParams.pathOptimizationRange;
}
INLINE void RNCrowdAgentParams::set_pathOptimizationRange(float value)
{
	_dtCrowdAgentParams.pathOptimizationRange = value;
}
INLINE float RNCrowdAgentParams::get_separationWeight() const
{
	return _dtCrowdAgentParams.separationWeight;
}
INLINE void RNCrowdAgentParams::set_separationWeight(float value)
{
	_dtCrowdAgentParams.separationWeight = value;
}
INLINE unsigned char RNCrowdAgentParams::get_updateFlags() const
{
	return _dtCrowdAgentParams.updateFlags;
}
INLINE void RNCrowdAgentParams::set_updateFlags(unsigned char value)
{
	_dtCrowdAgentParams.updateFlags = value;
}
INLINE unsigned char RNCrowdAgentParams::get_obstacleAvoidanceType() const
{
	return _dtCrowdAgentParams.obstacleAvoidanceType;
}
INLINE void RNCrowdAgentParams::set_obstacleAvoidanceType(unsigned char value)
{
	_dtCrowdAgentParams.obstacleAvoidanceType = value;
}
INLINE unsigned char RNCrowdAgentParams::get_queryFilterType() const
{
	return _dtCrowdAgentParams.queryFilterType;
}
INLINE void RNCrowdAgentParams::set_queryFilterType(unsigned char value)
{
	_dtCrowdAgentParams.queryFilterType = value;
}
INLINE void* RNCrowdAgentParams::get_userData() const
{
	return _dtCrowdAgentParams.userData;
}
INLINE void RNCrowdAgentParams::set_userData(void* value)
{
	_dtCrowdAgentParams.userData = value;
}
INLINE ostream &operator << (ostream &out, const RNCrowdAgentParams & params)
{
	params.output(out);
	return out;
}

#endif /* AITOOLS_I_ */
