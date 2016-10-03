/**
 * \file gameAIManager.h
 *
 * \date 2016-09-17
 * \author consultit
 */

#ifndef GAMEAIMANAGER_H_
#define GAMEAIMANAGER_H_

#include "aiTools.h"
#include "../../common/source/commonTools.h"
#include "opensteer_includes.h"
#include "recastnavigation_includes.h"
#include "collisionTraverser.h"
#include "collisionHandlerQueue.h"
#include "collisionRay.h"

class OSSteerPlugIn;
class OSSteerVehicle;
class RNNavMesh;
class RNCrowdAgent;

/**
 * GameAIManager Singleton class.
 *
 * Used for handling OSSteerPlugIns, OSSteerVehicles, RNNavMeshes and RNCrowdAgents.
 */
class EXPORT_CLASS GameAIManager: public TypedReferenceCount,
		public Singleton<GameAIManager>
{
public:
	typedef Pair<OSObstacleSettings, NodePath> ObstacleAttributes;
	typedef Pair<OpenSteer::ObstacleGroup, pvector<ObstacleAttributes> > GlobalObstacles;

PUBLISHED:
	GameAIManager(int taskSort = 0, const NodePath& root = NodePath(),
			const CollideMask& mask = GeomNode::get_default_collide_mask());
	virtual ~GameAIManager();

	/**
	 * \name REFERENCE NODES
	 */
	///@{
	INLINE NodePath get_reference_node_path() const;
	INLINE void set_reference_node_path(const NodePath& reference);
	INLINE NodePath get_reference_node_path_debug() const;
	INLINE NodePath get_reference_node_path_debug_2d() const;
	///@}

	/**
	 * \name OSSteerPlugIn
	 */
	///@{
	NodePath create_steer_plug_in();
	bool destroy_steer_plug_in(NodePath plugInNP);
	PT(OSSteerPlugIn) get_steer_plug_in(int index) const;
	INLINE int get_num_steer_plug_ins() const;
	MAKE_SEQ(get_steer_plug_ins, get_num_steer_plug_ins, get_steer_plug_in);
	///@}

	/**
	 * \name OSSteerVehicle
	 */
	///@{
	NodePath create_steer_vehicle(const string& name);
	bool destroy_steer_vehicle(NodePath steerVehicleNP);
	PT(OSSteerVehicle) get_steer_vehicle(int index) const;
	INLINE int get_num_steer_vehicles() const;
	MAKE_SEQ(get_steer_vehicles, get_num_steer_vehicles, get_steer_vehicle);
	///@}

	/**
	 * \name RNNavMesh
	 */
	///@{
	NodePath create_nav_mesh();
	bool destroy_nav_mesh(NodePath navMeshNP);
	PT(RNNavMesh) get_nav_mesh(int index) const;
	INLINE int get_num_nav_meshes() const;
	MAKE_SEQ(get_nav_meshes, get_num_nav_meshes, get_nav_mesh);
	///@}

	/**
	 * \name RNCrowdAgent
	 */
	///@{
	NodePath create_crowd_agent(const string& name);
	bool destroy_crowd_agent(NodePath crowdAgentNP);
	PT(RNCrowdAgent) get_crowd_agent(int index) const;
	INLINE int get_num_crowd_agents() const;
	MAKE_SEQ(get_crowd_agents, get_num_crowd_agents, get_crowd_agent);
	///@}

	/**
	 * The type of object for creation parameters.
	 */
	enum AIType
	{
		STEERPLUGIN = 0,
		STEERVEHICLE,
		NAVMESH,
		CROWDAGENT
	};

	/**
	 * \name TEXTUAL PARAMETERS
	 */
	///@{
	ValueList<string> get_parameter_name_list(AIType type) const;
	void set_parameter_values(AIType type, const string& paramName, const ValueList<string>& paramValues);
	ValueList<string> get_parameter_values(AIType type, const string& paramName) const;
	void set_parameter_value(AIType type, const string& paramName, const string& value);
	string get_parameter_value(AIType type, const string& paramName) const;
	void set_parameters_defaults(AIType type);
	///@}

	/**
	 * \name DEFAULT UPDATE
	 */
	///@{
	AsyncTask::DoneStatus update(GenericAsyncTask* task);
	void start_default_update();
	void stop_default_update();
	///@}

	/**
	 * \name SINGLETON
	 */
	///@{
	INLINE static GameAIManager* get_global_ptr();
	///@}

	/**
	 * \name OpenSteer OBSTACLES
	 */
	///@{
	OSObstacleSettings get_obstacle_settings(int ref) const;
	NodePath get_obstacle_by_ref(int ref) const;
	INLINE int get_obstacle(int index) const;
	INLINE int get_num_obstacles() const;
	MAKE_SEQ(get_obstacles, get_num_obstacles, get_obstacle);
	///@}

	/**
	 * \name UTILITIES
	 */
	///@{
	float get_bounding_dimensions(NodePath modelNP, LVecBase3f& modelDims,
			LVector3f& modelDeltaCenter) const;
	Pair<bool,float> get_collision_height(const LPoint3f& origin,
			const NodePath& space = NodePath()) const;
	INLINE CollideMask get_collide_mask() const;
	INLINE NodePath get_collision_root() const;
	INLINE CollisionTraverser* get_collision_traverser() const;
	INLINE CollisionHandlerQueue* get_collision_handler() const;
	INLINE CollisionRay* get_collision_ray() const;
	///@}

	/**
	 * \name SERIALIZATION
	 */
	///@{
	bool write_to_bam_file(const string& fileName);
	bool read_from_bam_file(const string& fileName);
	///@}

	/**
	 * Equivalent to DrawMeshDrawer::DrawPrimitive.
	 */
	enum OSDebugDrawPrimitives
	{
#ifndef CPPPARSER
		OS_POINTS = ossup::DrawMeshDrawer::DRAW_POINTS,
		OS_LINES = ossup::DrawMeshDrawer::DRAW_LINES,
		OS_TRIS = ossup::DrawMeshDrawer::DRAW_TRIS,
		OS_QUADS = ossup::DrawMeshDrawer::DRAW_QUADS,
#else
		OS_POINTS,OS_LINES,OS_TRIS,OS_QUADS
#endif //CPPPARSER
	};

	/**
	 * Equivalent to duDebugDrawPrimitives.
	 */
	enum RNDebugDrawPrimitives
	{
#ifndef CPPPARSER
		RN_POINTS = DU_DRAW_POINTS,
		RN_LINES = DU_DRAW_LINES,
		RN_TRIS = DU_DRAW_TRIS,
		RN_QUADS = DU_DRAW_QUADS,
#else
		RN_POINTS,RN_LINES,RN_TRIS,RN_QUADS
#endif //CPPPARSER
	};

	/**
	 * \name LOW LEVEL DEBUG DRAWING
	 */
	///@{
	void debug_draw_primitive(OSDebugDrawPrimitives primitive,
			const ValueList<LPoint3f>& points, const LVecBase4f color = LVecBase4f::zero(), float size =
					1.0f);
	void debug_draw_primitive(RNDebugDrawPrimitives primitive,
			const ValueList<LPoint3f>& points, const LVecBase4f color = LVecBase4f::zero(), float size =
					1.0f);
	void debug_draw_reset(OSDebugDrawPrimitives primitive);
	void debug_draw_reset(RNDebugDrawPrimitives primitive);
	///@}

public:
	/**
	 * \name Obstacles added by all OSSteerPlugIn(s) (C++ only).
	 */
	///@{
	inline Pair<OpenSteer::ObstacleGroup,
			pvector<ObstacleAttributes> >& get_global_obstacles();
	///@}

	///Unique ref producer.
	inline int unique_ref();

private:
	///The reference node path.
	NodePath mReferenceNP;
	///The update task sort (should be >=0).
	int mTaskSort;

	///List of OSSteerPlugIns handled by this manager.
	typedef pvector<PT(OSSteerPlugIn)> SteerPlugInList;
	SteerPlugInList mSteerPlugIns;
	///OSSteerPlugIns' parameter table.
	ParameterTable mSteerPlugInsParameterTable;

	///List of OSSteerVehicles handled by this manager.
	typedef pvector<PT(OSSteerVehicle)> SteerVehicleList;
	SteerVehicleList mSteerVehicles;
	///OSSteerVehicles' parameter table.
	ParameterTable mSteerVehiclesParameterTable;

	///List of RNNavMeshes handled by this manager.
	typedef pvector<PT(RNNavMesh)> NavMeshList;
	NavMeshList mNavMeshes;
	///RNNavMeshes' parameter table.
	ParameterTable mNavMeshesParameterTable;

	///List of RNCrowdAgents handled by this manager.
	typedef pvector<PT(RNCrowdAgent)> CrowdAgentList;
	CrowdAgentList mCrowdAgents;
	///RNCrowdAgents' parameter table.
	ParameterTable mCrowdAgentsParameterTable;

	///This is a Pair:
	/// -first == list of pointers to all OpenSteer obstacles
	/// -second == list of attributes of all obstacles
	///The two lists are synchronized: pointer and attributes of the i-th
	///obstacle are located in the i-th places of their respective lists.
	GlobalObstacles mObstacles;

	///@{
	///A task data for step simulation update.
	PT(TaskInterface<GameAIManager>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;
	///@}

	///Unique ref.
	int mRef;

	///Utilities.
	NodePath mRoot;
	CollideMask mMask; //a.k.a. BitMask32
	CollisionTraverser* mCTrav;
	CollisionHandlerQueue* mCollisionHandler;
	CollisionRay* mPickerRay;

	///The reference node paths for debug drawing.
	NodePath mReferenceDebugNP, mReferenceDebug2DNP;
#ifdef ELY_DEBUG
	class DebugDrawPrimitives: public rnsup::DebugDrawPanda3d
	{
	public:
		DebugDrawPrimitives(NodePath render): rnsup::DebugDrawPanda3d(render)
		{
		}
		void vertex(const LVector3f& vertex, const LVector4f& color)
		{
			doVertex(vertex, color);
		}
	};
	/// DebugDrawers.
	ossup::DebugDrawPanda3d* mOSDD;
	DebugDrawPrimitives* mRNDD;
#endif //ELY_DEBUG

public:
	/**
	 * \name TypedObject API
	 */
	///@{
	static TypeHandle get_class_type()
	{
		return _type_handle;
	}
	static void init_type()
	{
		TypedReferenceCount::init_type();
		register_type(_type_handle, "GameAIManager",
				TypedReferenceCount::get_class_type());
	}
	virtual TypeHandle get_type() const override
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type() override
	{
		init_type();
		return get_class_type();
	}
	///@}

private:
	static TypeHandle _type_handle;

};

///inline
#include "gameAIManager.I"

#endif /* GAMEAIMANAGER_H_ */
