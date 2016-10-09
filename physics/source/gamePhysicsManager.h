/**
 * \file gamePhysicsManager.h
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef GAMEPHYSICSMANGER_H_
#define GAMEPHYSICSMANGER_H_

#include "graphicsWindow.h"
#include "collisionTraverser.h"
#include "collisionHandlerQueue.h"
#include "collisionRay.h"
#include "physics_includes.h"

class BTSoftBody;
class BTRigidBody;

/**
 * GamePhysicsManager Singleton class.
 *
 * Used for handling BTSoftBodys, BTRigidBodys.
 */
class EXPORT_CLASS GamePhysicsManager: public TypedReferenceCount,
		public Singleton<GamePhysicsManager>
{
PUBLISHED:
	GamePhysicsManager(PT(GraphicsWindow) win, int taskSort = 10,
			const NodePath& root = NodePath(),
			const CollideMask& mask = GeomNode::get_default_collide_mask());
	virtual ~GamePhysicsManager();

	/**
	 * \name REFERENCE NODES
	 */
	///@{
	INLINE NodePath get_reference_node_path() const;
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name BTSoftBody
	 */
	///@{
	NodePath create_soft_body(const string& name);
	bool destroy_soft_body(NodePath plugInNP);
	PT(BTSoftBody) get_soft_body(int index) const;
	INLINE int get_num_soft_bodies() const;
	MAKE_SEQ(get_soft_bodies, get_num_soft_bodies, get_soft_body);
	///@}

	/**
	 * \name BTRigidBody
	 */
	///@{
	NodePath create_rigid_body(const string& name);
	bool destroy_rigid_body(NodePath steerVehicleNP);
	PT(BTRigidBody) get_rigid_body(int index) const;
	INLINE int get_num_rigid_bodies() const;
	MAKE_SEQ(get_rigid_bodies, get_num_rigid_bodies, get_rigid_body);
	///@}

	/**
	 * The type of object for creation parameters.
	 */
	enum PhysicsType
	{
		RIGIDBODY = 0,
		SOFTBODY,
		GHOST
	};

	/**
	 * \name TEXTUAL PARAMETERS
	 */
	///@{
	ValueList<string> get_parameter_name_list(PhysicsType type) const;
	void set_parameter_values(PhysicsType type, const string& paramName, const ValueList<string>& paramValues);
	ValueList<string> get_parameter_values(PhysicsType type, const string& paramName) const;
	void set_parameter_value(PhysicsType type, const string& paramName, const string& value);
	string get_parameter_value(PhysicsType type, const string& paramName) const;
	void set_parameters_defaults(PhysicsType type);
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
	INLINE static GamePhysicsManager* get_global_ptr();
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

public:

	///Unique ref producer.
	inline int unique_ref();

private:
	///The reference graphic window.
	PT(GraphicsWindow) mWin;
	///The update task sort (should be >0).
	int mTaskSort;

	///The reference node path.
	NodePath mReferenceNP;

	///List of BTSoftBodys handled by this manager.
	typedef pvector<PT(BTSoftBody)> DriverList;
	DriverList mDrivers;
	///BTSoftBodys' parameter table.
	ParameterTable mDriversParameterTable;

	///List of BTRigidBodys handled by this manager.
	typedef pvector<PT(BTRigidBody)> RigidBodyList;
	RigidBodyList mRigidBodys;
	///BTRigidBodys' parameter table.
	ParameterTable mRigidBodysParameterTable;

	///@{
	///A task data for step simulation update.
	PT(TaskInterface<GamePhysicsManager>::TaskData) mUpdateData;
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
		register_type(_type_handle, "GamePhysicsManager",
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
#include "gamePhysicsManager.I"

#endif /* GAMEPHYSICSMANGER_H_ */
