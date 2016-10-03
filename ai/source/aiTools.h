/**
 * \file aiTools.h
 *
 * \date 2016-09-17
 * \author consultit
 */

#ifndef AITOOLS_H_
#define AITOOLS_H_

#include "pandabase.h"
#include "lvector3.h"
#include "lpoint3.h"
#include "opensteer_includes.h"
#include "recastnavigation_includes.h"

//
#ifndef CPPPARSER
#include "support_os/common.h"
#include "support_os/DrawMeshDrawer.h"
#include "support_rn/NavMeshType.h"
#endif //CPPPARSER

///Vehicle settings.
struct EXPORT_CLASS OSVehicleSettings
{
PUBLISHED:
	OSVehicleSettings();
#ifndef CPPPARSER
	OSVehicleSettings(const ossup::VehicleSettings& settings) :
		_vehicleSettings(settings)
	{
	}
	operator ossup::VehicleSettings() const
	{
		return _vehicleSettings;
	}
#endif
	INLINE float get_mass() const;
	INLINE void set_mass(float value);
	INLINE float get_radius() const;
	INLINE void set_radius(float value);
	INLINE float get_speed() const;
	INLINE void set_speed(float value);
	INLINE float get_maxForce() const;
	INLINE void set_maxForce(float value);
	INLINE float get_maxSpeed() const;
	INLINE void set_maxSpeed(float value);
	INLINE LVector3f get_forward() const;
	INLINE void set_forward(const LVector3f& value);
	INLINE LVector3f get_side() const;
	INLINE void set_side(const LVector3f& value);
	INLINE LVector3f get_up() const;
	INLINE void set_up(const LVector3f& value);
	INLINE LPoint3f get_position() const;
	INLINE void set_position(const LPoint3f& value);
	INLINE LPoint3f get_start() const;
	INLINE void set_start(const LPoint3f& value);
	INLINE float get_path_pred_time() const;
	INLINE void set_path_pred_time(float value);
	INLINE float get_obstacle_min_time_coll() const;
	INLINE void set_obstacle_min_time_coll(float value);
	INLINE float get_neighbor_min_time_coll() const;
	INLINE void set_neighbor_min_time_coll(float value);
	INLINE float get_neighbor_min_sep_dist() const;
	INLINE void set_neighbor_min_sep_dist(float value);
	INLINE float get_separation_max_dist() const;
	INLINE void set_separation_max_dist(float value);
	INLINE float get_separation_cos_max_angle() const;
	INLINE void set_separation_cos_max_angle(float value);
	INLINE float get_alignment_max_dist() const;
	INLINE void set_alignment_max_dist(float value);
	INLINE float get_alignment_cos_max_angle() const;
	INLINE void set_alignment_cos_max_angle(float value);
	INLINE float get_cohesion_max_dist() const;
	INLINE void set_cohesion_max_dist(float value);
	INLINE float get_cohesion_cos_max_angle() const;
	INLINE void set_cohesion_cos_max_angle(float value);
	INLINE float get_pursuit_max_pred_time() const;
	INLINE void set_pursuit_max_pred_time(float value);
	INLINE float get_evasion_max_pred_time() const;
	INLINE void set_evasion_max_pred_time(float value);
	INLINE float get_target_speed() const;
	INLINE void set_target_speed(float value);
	void output(ostream &out) const;
private:
	ossup::VehicleSettings _vehicleSettings;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const OSVehicleSettings & settings);

///Flock settings.
struct EXPORT_CLASS OSFlockSettings
{
PUBLISHED:
	OSFlockSettings();
#ifndef CPPPARSER
	OSFlockSettings(float sW, float aW,	float cW);
#endif

	INLINE float get_separation_weight() const;
	INLINE void set_separation_weight(float value);
	INLINE float get_alignment_weight() const;
	INLINE void set_alignment_weight(float value);
	INLINE float get_cohesion_weight() const;
	INLINE void set_cohesion_weight(float value);
	void output(ostream &out) const;
#ifndef CPPPARSER
	float& separation_weight(){return _separationWeight;}
	float& alignment_weight(){return _alignmentWeight;}
	float& cohesion_weight(){return _cohesionWeight;}
#endif
private:
	float _separationWeight;
	float _alignmentWeight;
	float _cohesionWeight;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const OSFlockSettings & settings);

///OSObstacleSettings.
struct EXPORT_CLASS OSObstacleSettings
{
PUBLISHED:
	OSObstacleSettings();

	INLINE bool operator==(
			const OSObstacleSettings &other) const;
	INLINE string get_type() const;
	INLINE void set_type(const string& value);
	INLINE string get_seenFromState() const;
	INLINE void set_seenFromState(const string& value);
	INLINE LPoint3f get_position() const;
	INLINE void set_position(const LPoint3f& value);
	INLINE LVector3f get_forward() const;
	INLINE void set_forward(const LVector3f& value);
	INLINE LVector3f get_up() const;
	INLINE void set_up(const LVector3f& value);
	INLINE LVector3f get_side() const;
	INLINE void set_side(const LVector3f& value);
	INLINE float get_width() const;
	INLINE void set_width(float value);
	INLINE float get_height() const;
	INLINE void set_height(float value);
	INLINE float get_depth() const;
	INLINE void set_depth(float value);
	INLINE float get_radius() const;
	INLINE void set_radius(float value);
	INLINE int get_ref() const;
	INLINE void set_ref(int value);
	void output(ostream &out) const;
public:
	inline OpenSteer::AbstractObstacle* get_obstacle() const;
	inline void set_obstacle(OpenSteer::AbstractObstacle* value);
private:
	string _type;
	string _seenFromState;
	LPoint3f _position;
	LVector3f _forward;
	LVector3f _up;
	LVector3f _side;
	float _width;
	float _height;
	float _depth;
	float _radius;
	int _ref;
	//not serialized
	OpenSteer::AbstractObstacle* _obstacle;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const OSObstacleSettings & settings);

///NavMesh settings.
struct EXPORT_CLASS RNNavMeshSettings
{
PUBLISHED:
	RNNavMeshSettings();
#ifndef CPPPARSER
	RNNavMeshSettings(const rnsup::NavMeshSettings& settings) :
			_navMeshSettings(settings)
	{
	}
	operator rnsup::NavMeshSettings() const
	{
		return _navMeshSettings;
	}
#endif
	INLINE float get_cellSize() const;
	INLINE void set_cellSize(float value);
	INLINE float get_cellHeight() const;
	INLINE void set_cellHeight(float value);
	INLINE float get_agentHeight() const;
	INLINE void set_agentHeight(float value);
	INLINE float get_agentRadius() const;
	INLINE void set_agentRadius(float value);
	INLINE float get_agentMaxClimb() const;
	INLINE void set_agentMaxClimb(float value);
	INLINE float get_agentMaxSlope() const;
	INLINE void set_agentMaxSlope(float value);
	INLINE float get_regionMinSize() const;
	INLINE void set_regionMinSize(float value);
	INLINE float get_regionMergeSize() const;
	INLINE void set_regionMergeSize(float value);
	INLINE float get_edgeMaxLen() const;
	INLINE void set_edgeMaxLen(float value);
	INLINE float get_edgeMaxError() const;
	INLINE void set_edgeMaxError(float value);
	INLINE float get_vertsPerPoly() const;
	INLINE void set_vertsPerPoly(float value);
	INLINE float get_detailSampleDist() const;
	INLINE void set_detailSampleDist(float value);
	INLINE float get_detailSampleMaxError() const;
	INLINE void set_detailSampleMaxError(float value);
	INLINE int get_partitionType() const;
	INLINE void set_partitionType(int value);
	void output(ostream &out) const;
private:
	rnsup::NavMeshSettings _navMeshSettings;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const RNNavMeshSettings & settings);

///NavMesh tile settings.
struct EXPORT_CLASS RNNavMeshTileSettings
{
PUBLISHED:
	RNNavMeshTileSettings();
#ifndef CPPPARSER
	RNNavMeshTileSettings(const rnsup::NavMeshTileSettings& settings) :
			_navMeshTileSettings(settings)
	{
	}
	operator rnsup::NavMeshTileSettings() const
	{
		return _navMeshTileSettings;
	}
#endif //CPPPARSER
	INLINE bool get_buildAllTiles() const;
	INLINE void set_buildAllTiles(bool value);
	INLINE int get_maxTiles() const;
	INLINE void set_maxTiles(int value);
	INLINE int get_maxPolysPerTile() const;
	INLINE void set_maxPolysPerTile(int value);
	INLINE float get_tileSize() const;
	INLINE void set_tileSize(float value);
	void output(ostream &out) const;
private:
	rnsup::NavMeshTileSettings _navMeshTileSettings;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const RNNavMeshTileSettings & settings);

///Convex volume settings.
struct EXPORT_CLASS RNConvexVolumeSettings
{
PUBLISHED:
	RNConvexVolumeSettings();

	INLINE bool operator== (const RNConvexVolumeSettings &other) const;
	INLINE int get_area() const;
	INLINE void set_area(int value);
	INLINE int get_flags() const;
	INLINE void set_flags(int value);
	INLINE LPoint3f get_centroid() const;
	INLINE void set_centroid(LPoint3f value);
	INLINE int get_ref() const;
	INLINE void set_ref(int value);
	void output(ostream &out) const;
private:
	int _area;
	int _flags;
	LPoint3f _centroid;
	int _ref;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const RNConvexVolumeSettings & settings);

///Off mesh connection settings.
struct EXPORT_CLASS RNOffMeshConnectionSettings
{
PUBLISHED:
	RNOffMeshConnectionSettings();

	INLINE bool operator==(
			const RNOffMeshConnectionSettings &other) const;
	INLINE float get_rad() const;
	INLINE void set_rad(float value);
	INLINE bool get_bidir() const;
	INLINE void set_bidir(bool value);
	INLINE unsigned int get_userId() const;
	INLINE void set_userId(unsigned int value);
	INLINE int get_area() const;
	INLINE void set_area(int value);
	INLINE int get_flags() const;
	INLINE void set_flags(int value);
	INLINE int get_ref() const;
	INLINE void set_ref(int value);
	void output(ostream &out) const;
private:
	float _rad;
	bool _bidir;
	unsigned int _userId;
	int _area;
	int _flags;
	int _ref;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const RNOffMeshConnectionSettings & settings);

///Obstacle settings.
struct EXPORT_CLASS RNObstacleSettings
{
PUBLISHED:
	RNObstacleSettings();

	INLINE bool operator==(
			const RNObstacleSettings &other) const;
	INLINE float get_radius() const;
	INLINE void set_radius(float value);
	INLINE LVecBase3f get_dims() const;
	INLINE void set_dims(const LVecBase3f& value);
	INLINE unsigned int get_ref() const;
	INLINE void set_ref(unsigned int value);
	void output(ostream &out) const;
private:
	float _radius;
	LVecBase3f _dims;
	unsigned int _ref;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const RNObstacleSettings & settings);

///CrowdAgentParams
struct EXPORT_CLASS RNCrowdAgentParams
{
PUBLISHED:
	RNCrowdAgentParams();
#ifndef CPPPARSER
	RNCrowdAgentParams(const dtCrowdAgentParams& params) :
			_dtCrowdAgentParams(params)
	{
	}
	operator dtCrowdAgentParams() const
	{
		return _dtCrowdAgentParams;
	}
#endif //CPPPARSER
	INLINE float get_radius() const;
	INLINE void set_radius(float value);
	INLINE float get_height() const;
	INLINE void set_height(float value);
	INLINE float get_maxAcceleration() const;
	INLINE void set_maxAcceleration(float value);
	INLINE float get_maxSpeed() const;
	INLINE void set_maxSpeed(float value);
	INLINE float get_collisionQueryRange() const;
	INLINE void set_collisionQueryRange(float value);
	INLINE float get_pathOptimizationRange() const;
	INLINE void set_pathOptimizationRange(float value);
	INLINE float get_separationWeight() const;
	INLINE void set_separationWeight(float value);
	INLINE unsigned char get_updateFlags() const;
	INLINE void set_updateFlags(unsigned char value);
	INLINE unsigned char get_obstacleAvoidanceType() const;
	INLINE void set_obstacleAvoidanceType(unsigned char value);
	INLINE unsigned char get_queryFilterType() const;
	INLINE void set_queryFilterType(unsigned char value);
	INLINE void* get_userData() const;
	INLINE void set_userData(void* value);
	void output(ostream &out) const;
private:
	dtCrowdAgentParams _dtCrowdAgentParams;

public:
	void write_datagram(Datagram &dg) const;
	void read_datagram(DatagramIterator &scan);
};
INLINE ostream &operator << (ostream &out, const RNCrowdAgentParams & params);

///inline
#include "aiTools.I"

#endif /* AITOOLS_H_ */
