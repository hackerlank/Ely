/**
 * \file aiTools.cxx
 *
 * \date 2016-09-17
 * \author consultit
 */

#include "aiTools.h"

///Vehicle settings.
/**
 *
 */
OSVehicleSettings::OSVehicleSettings(): _vehicleSettings()
{
}
/**
 * Writes the Vehicle settings into a datagram.
 */
void OSVehicleSettings::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_mass());
	dg.add_stdfloat(get_radius());
	dg.add_stdfloat(get_speed());
	dg.add_stdfloat(get_maxForce());
	dg.add_stdfloat(get_maxSpeed());
	get_forward().write_datagram(dg);
	get_side().write_datagram(dg);
	get_up().write_datagram(dg);
	get_position().write_datagram(dg);
	get_start().write_datagram(dg);
	dg.add_stdfloat(get_path_pred_time());
	dg.add_stdfloat(get_obstacle_min_time_coll());
	dg.add_stdfloat(get_neighbor_min_time_coll());
	dg.add_stdfloat(get_neighbor_min_sep_dist());
	dg.add_stdfloat(get_separation_max_dist());
	dg.add_stdfloat(get_separation_cos_max_angle());
	dg.add_stdfloat(get_alignment_max_dist());
	dg.add_stdfloat(get_alignment_cos_max_angle());
	dg.add_stdfloat(get_cohesion_max_dist());
	dg.add_stdfloat(get_cohesion_cos_max_angle());
	dg.add_stdfloat(get_pursuit_max_pred_time());
	dg.add_stdfloat(get_evasion_max_pred_time());
	dg.add_stdfloat(get_target_speed());
}

/**
 * Restores the Vehicle settings from the datagram.
 */
void OSVehicleSettings::read_datagram(DatagramIterator &scan)
{
	set_mass(scan.get_stdfloat());
	set_radius(scan.get_stdfloat());
	set_speed(scan.get_stdfloat());
	set_maxForce(scan.get_stdfloat());
	set_maxSpeed(scan.get_stdfloat());
	LVector3f value;
	value.read_datagram(scan);
	set_forward(value);
	value.read_datagram(scan);
	set_side(value);
	value.read_datagram(scan);
	set_up(value);
	value.read_datagram(scan);
	set_position(value);
	value.read_datagram(scan);
	set_start(value);
	set_path_pred_time(scan.get_stdfloat());
	set_obstacle_min_time_coll(scan.get_stdfloat());
	set_neighbor_min_time_coll(scan.get_stdfloat());
	set_neighbor_min_sep_dist(scan.get_stdfloat());
	set_separation_max_dist(scan.get_stdfloat());
	set_separation_cos_max_angle(scan.get_stdfloat());
	set_alignment_max_dist(scan.get_stdfloat());
	set_alignment_cos_max_angle(scan.get_stdfloat());
	set_cohesion_max_dist(scan.get_stdfloat());
	set_cohesion_cos_max_angle(scan.get_stdfloat());
	set_pursuit_max_pred_time(scan.get_stdfloat());
	set_evasion_max_pred_time(scan.get_stdfloat());
	set_target_speed(scan.get_stdfloat());
}

/**
 * Writes a sensible description of the OSVehicleSettings to the indicated
 * output stream.
 */
void OSVehicleSettings::output(ostream &out) const
{
	out << "mass: " << get_mass() << endl;
	out << "radius: " << get_radius() << endl;
	out << "speed: " << get_speed() << endl;
	out << "maxForce: " << get_maxForce() << endl;
	out << "maxSpeed: " << get_maxSpeed() << endl;
	out << "forward: " << get_forward() << endl;
	out << "side: " << get_side() << endl;
	out << "up: " << get_up() << endl;
	out << "position: " << get_position() << endl;
	out << "start: " << get_start() << endl;
	out << "path_pred_time: " << get_path_pred_time() << endl;
	out << "obstacle_min_time_coll: " << get_obstacle_min_time_coll() << endl;
	out << "neighbor_min_time_coll: " << get_neighbor_min_time_coll() << endl;
	out << "neighbor_min_sep_dist: " << get_neighbor_min_sep_dist() << endl;
	out << "separation_max_dist: " << get_separation_max_dist() << endl;
	out << "separation_cos_max_angle: " << get_separation_cos_max_angle()
			<< endl;
	out << "alignment_max_dist: " << get_alignment_max_dist() << endl;
	out << "alignment_cos_max_angle: " << get_alignment_cos_max_angle() << endl;
	out << "cohesion_max_dist: " << get_cohesion_max_dist() << endl;
	out << "cohesion_cos_max_angle: " << get_cohesion_cos_max_angle() << endl;
	out << "pursuit_max_pred_time: " << get_pursuit_max_pred_time() << endl;
	out << "evasion_max_pred_time: " << get_evasion_max_pred_time() << endl;
	out << "speed: " << get_speed() << endl;
}

///Flock settings.
/**
 *
 */
OSFlockSettings::OSFlockSettings() :
		_separationWeight(0.0), _alignmentWeight(0.0), _cohesionWeight(0.0)
{
}/**
 *
 */
OSFlockSettings::OSFlockSettings(float sW, float aW, float cW) :
		_separationWeight(sW), _alignmentWeight(aW), _cohesionWeight(cW)
{
}
/**
 * Writes the flock settings into a datagram.
 */
void OSFlockSettings::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_separation_weight());
	dg.add_stdfloat(get_alignment_weight());
	dg.add_stdfloat(get_cohesion_weight());
}

/**
 * Restores the flock settings from the datagram.
 */
void OSFlockSettings::read_datagram(DatagramIterator &scan)
{
	set_separation_weight(scan.get_stdfloat());
	set_alignment_weight(scan.get_stdfloat());
	set_cohesion_weight(scan.get_stdfloat());
}

/**
 * Writes a sensible description of the OSFlockSettings to the indicated output
 * stream.
 */
void OSFlockSettings::output(ostream &out) const
{
	out << "separation_weight: " << get_separation_weight() << endl;
	out << "alignment_weight: " << get_alignment_weight() << endl;
	out << "cohesion_weight: " << get_cohesion_weight() << endl;
}

///OSObstacleSettings.
/**
 *
 */
OSObstacleSettings::OSObstacleSettings() :
		_width(0), _height(0), _depth(0), _radius(0), _ref(0), _obstacle(0)
{
}
/**
 * Writes the OSObstacleSettings into a datagram.
 */
void OSObstacleSettings::write_datagram(Datagram &dg) const
{
	dg.add_string(_type);
	dg.add_string(_seenFromState);
	_position.write_datagram(dg);
	_forward.write_datagram(dg);
	_up.write_datagram(dg);
	_side.write_datagram(dg);
	dg.add_stdfloat(get_width());
	dg.add_stdfloat(get_height());
	dg.add_stdfloat(get_depth());
	dg.add_stdfloat(get_radius());
	dg.add_int32(get_ref());
}
/**
 * Restores the OSObstacleSettings from the datagram.
 */
void OSObstacleSettings::read_datagram(DatagramIterator &scan)
{
	set_type(scan.get_string());
	set_seenFromState(scan.get_string());
	_position.read_datagram(scan);
	_forward.read_datagram(scan);
	_up.read_datagram(scan);
	_side.read_datagram(scan);
	set_width(scan.get_stdfloat());
	set_height(scan.get_stdfloat());
	set_depth(scan.get_stdfloat());
	set_radius(scan.get_stdfloat());
	set_ref(scan.get_int32());
}

/**
 * Writes a sensible description of the OSObstacleSettings to the indicated
 * output stream.
 */
void OSObstacleSettings::output(ostream &out) const
{
	out << "type: " << get_type() << endl;
	out << "seenFromState: " << get_seenFromState() << endl;
	out << "position: " << get_position() << endl;
	out << "forward: " << get_forward() << endl;
	out << "up: " << get_up() << endl;
	out << "side: " << get_side() << endl;
	out << "width: " << get_width() << endl;
	out << "height: " << get_height() << endl;
	out << "depth: " << get_depth() << endl;
	out << "radius: " << get_radius() << endl;
	out << "ref: " << get_ref() << endl;
}

///NavMeshSettings
/**
 *
 */
RNNavMeshSettings::RNNavMeshSettings(): _navMeshSettings()
{
}
/**
 * Writes the NavMeshSettings into a datagram.
 */
void RNNavMeshSettings::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_cellSize());
	dg.add_stdfloat(get_cellHeight());
	dg.add_stdfloat(get_agentHeight());
	dg.add_stdfloat(get_agentRadius());
	dg.add_stdfloat(get_agentMaxClimb());
	dg.add_stdfloat(get_agentMaxSlope());
	dg.add_stdfloat(get_regionMinSize());
	dg.add_stdfloat(get_regionMergeSize());
	dg.add_stdfloat(get_edgeMaxLen());
	dg.add_stdfloat(get_edgeMaxError());
	dg.add_stdfloat(get_vertsPerPoly());
	dg.add_stdfloat(get_detailSampleDist());
	dg.add_stdfloat(get_detailSampleMaxError());
	dg.add_int32(get_partitionType());
}

/**
 * Restores the NavMeshSettings from the datagram.
 */
void RNNavMeshSettings::read_datagram(DatagramIterator &scan)
{
	set_cellSize(scan.get_stdfloat());
	set_cellHeight(scan.get_stdfloat());
	set_agentHeight(scan.get_stdfloat());
	set_agentRadius(scan.get_stdfloat());
	set_agentMaxClimb(scan.get_stdfloat());
	set_agentMaxSlope(scan.get_stdfloat());
	set_regionMinSize(scan.get_stdfloat());
	set_regionMergeSize(scan.get_stdfloat());
	set_edgeMaxLen(scan.get_stdfloat());
	set_edgeMaxError(scan.get_stdfloat());
	set_vertsPerPoly(scan.get_stdfloat());
	set_detailSampleDist(scan.get_stdfloat());
	set_detailSampleMaxError(scan.get_stdfloat());
	set_partitionType(scan.get_int32());
}

/**
 * Writes a sensible description of the RNNavMeshSettings to the indicated
 * output stream.
 */
void RNNavMeshSettings::output(ostream &out) const
{
	out << "cellSize: " << get_cellSize() << endl;
	out << "cellHeight: " << get_cellHeight() << endl;
	out << "agentHeight: " << get_agentHeight() << endl;
	out << "agentRadius: " << get_agentRadius() << endl;
	out << "agentMaxClimb: " << get_agentMaxClimb() << endl;
	out << "agentMaxSlope: " << get_agentMaxSlope() << endl;
	out << "regionMinSize: " << get_regionMinSize() << endl;
	out << "regionMergeSize: " << get_regionMergeSize() << endl;
	out << "edgeMaxLen: " << get_edgeMaxLen() << endl;
	out << "edgeMaxError: " << get_edgeMaxError() << endl;
	out << "vertsPerPoly: " << get_vertsPerPoly() << endl;
	out << "detailSampleDist: " << get_detailSampleDist() << endl;
	out << "detailSampleMaxError: " << get_detailSampleMaxError() << endl;
	out << "partitionType: " << get_partitionType() << endl;
}

///NavMeshTileSettings
/**
 *
 */
RNNavMeshTileSettings::RNNavMeshTileSettings() :
		_navMeshTileSettings()
{
}
/**
 * Writes the NavMeshTileSettings into a datagram.
 */
void RNNavMeshTileSettings::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_buildAllTiles());
	dg.add_int32(get_maxTiles());
	dg.add_int32(get_maxPolysPerTile());
	dg.add_stdfloat(get_tileSize());
}
/**
 * Restores the NavMeshTileSettings from the datagram.
 */
void RNNavMeshTileSettings::read_datagram(DatagramIterator &scan)
{
	set_buildAllTiles(scan.get_stdfloat());
	set_maxTiles(scan.get_int32());
	set_maxPolysPerTile(scan.get_int32());
	set_tileSize(scan.get_stdfloat());
}

/**
 * Writes a sensible description of the RNNavMeshTileSettings to the indicated
 * output stream.
 */
void RNNavMeshTileSettings::output(ostream &out) const
{
	out << "buildAllTiles: " << get_buildAllTiles() << endl;
	out << "maxTiles: " << get_maxTiles() << endl;
	out << "maxPolysPerTile: " << get_maxPolysPerTile() << endl;
	out << "tileSize: " << get_tileSize() << endl;
}

///Convex volume settings.
/**
 *
 */
RNConvexVolumeSettings::RNConvexVolumeSettings() :
		_area(0), _flags(0), _ref(0)
{
}
/**
 * Writes the RNConvexVolumeSettings into a datagram.
 */
void RNConvexVolumeSettings::write_datagram(Datagram &dg) const
{
	dg.add_int32(get_area());
	dg.add_int32(get_flags());
	_centroid.write_datagram(dg);
	dg.add_int32(get_ref());
}
/**
 * Restores the RNConvexVolumeSettings from the datagram.
 */
void RNConvexVolumeSettings::read_datagram(DatagramIterator &scan)
{
	set_area(scan.get_int32());
	set_flags(scan.get_int32());
	_centroid.read_datagram(scan);
	set_ref(scan.get_int32());
}

/**
 * Writes a sensible description of the RNConvexVolumeSettings to the indicated
 * output stream.
 */
void RNConvexVolumeSettings::output(ostream &out) const
{
	out << "area: " << get_area() << endl;
	out << "flags: " << get_flags() << endl;
	out << "centroid: " << get_centroid() << endl;
	out << "ref: " << get_ref() << endl;
}

///Off mesh connection settings.
/**
 *
 */
RNOffMeshConnectionSettings::RNOffMeshConnectionSettings() :
		_area(0), _bidir(false), _userId(0), _flags(0), _rad(0.0), _ref(0)
{
}
/**
 * Writes the RNOffMeshConnectionSettings into a datagram.
 */
void RNOffMeshConnectionSettings::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_rad());
	dg.add_bool(get_bidir());
	dg.add_uint32(get_userId());
	dg.add_int32(get_area());
	dg.add_int32(get_flags());
	dg.add_int32(get_ref());
}
/**
 * Restores the RNOffMeshConnectionSettings from the datagram.
 */
void RNOffMeshConnectionSettings::read_datagram(DatagramIterator &scan)
{
	set_rad(scan.get_stdfloat());
	set_bidir(scan.get_bool());
	set_userId(scan.get_uint32());
	set_area(scan.get_int32());
	set_flags(scan.get_int32());
	set_ref(scan.get_int32());
}

/**
 * Writes a sensible description of the RNOffMeshConnectionSettings to the
 * indicated output stream.
 */
void RNOffMeshConnectionSettings::output(ostream &out) const
{
	out << "rad: " << get_rad() << endl;
	out << "bidir: " << get_bidir() << endl;
	out << "userId: " << get_userId() << endl;
	out << "area: " << get_area() << endl;
	out << "flags: " << get_flags() << endl;
	out << "ref: " << get_ref() << endl;
}

///Obstacle settings.
/**
 *
 */
RNObstacleSettings::RNObstacleSettings() :
		_radius(0.0), _dims(LVecBase3f()), _ref(0)
{
}

/**
 * Writes the RNObstacleSettings into a datagram.
 */
void RNObstacleSettings::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_radius());
	_dims.write_datagram(dg);
	dg.add_uint32(get_ref());
}
/**
 * Restores the RNObstacleSettings from the datagram.
 */
void RNObstacleSettings::read_datagram(DatagramIterator &scan)
{
	set_radius(scan.get_stdfloat());
	_dims.read_datagram(scan);
	set_ref(scan.get_uint32());
}

/**
 * Writes a sensible description of the RNObstacleSettings to the indicated
 * output stream.
 */
void RNObstacleSettings::output(ostream &out) const
{
	out << "radius: " << get_radius() << endl;
	out << "dims: " << get_dims() << endl;
	out << "ref: " << get_ref() << endl;
}

///CrowdAgentParams
/**
 *
 */
RNCrowdAgentParams::RNCrowdAgentParams(): _dtCrowdAgentParams()
{
}

/**
 * Writes the CrowdAgentParams into a datagram.
 */
void RNCrowdAgentParams::write_datagram(Datagram &dg) const
{
	dg.add_stdfloat(get_radius());
	dg.add_stdfloat(get_height());
	dg.add_stdfloat(get_maxAcceleration());
	dg.add_stdfloat(get_maxSpeed());
	dg.add_stdfloat(get_collisionQueryRange());
	dg.add_stdfloat(get_pathOptimizationRange());
	dg.add_stdfloat(get_separationWeight());
	dg.add_uint8(get_updateFlags());
	dg.add_uint8(get_obstacleAvoidanceType());
	dg.add_uint8(get_queryFilterType());
	//Note: void *dtCrowdAgentParams::userData is not used
}
/**
 * Restores the CrowdAgentParams from the datagram.
 */
void RNCrowdAgentParams::read_datagram(DatagramIterator &scan)
{
	set_radius(scan.get_stdfloat());
	set_height(scan.get_stdfloat());
	set_maxAcceleration(scan.get_stdfloat());
	set_maxSpeed(scan.get_stdfloat());
	set_collisionQueryRange(scan.get_stdfloat());
	set_pathOptimizationRange(scan.get_stdfloat());
	set_separationWeight(scan.get_stdfloat());
	set_updateFlags(scan.get_uint8());
	set_obstacleAvoidanceType(scan.get_uint8());
	set_queryFilterType(scan.get_uint8());
	//Note: void *dtCrowdAgentParams::userData is not used
}

/**
 * Writes a sensible description of the RNCrowdAgentParams to the indicated
 * output stream.
 */
void RNCrowdAgentParams::output(ostream &out) const
{
	out << "radius: " << get_radius() << endl;
	out << "height: " << get_height() << endl;
	out << "maxAcceleration: " << get_maxAcceleration() << endl;
	out << "maxSpeed: " << get_maxSpeed() << endl;
	out << "collisionQueryRange: " << get_collisionQueryRange() << endl;
	out << "pathOptimizationRange: " << get_pathOptimizationRange() << endl;
	out << "separationWeight: " << get_separationWeight() << endl;
	out << "updateFlags: " << static_cast<unsigned int>(get_updateFlags())
			<< endl;
	out << "obstacleAvoidanceType: "
			<< static_cast<unsigned int>(get_obstacleAvoidanceType()) << endl;
	out << "queryFilterType: " << get_queryFilterType() << endl;
	out << "userData: " << get_userData() << endl;
}
