/**
 * \file gamePhysicsManager.h
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef GAMEPHYSICSMANGER_H_
#define GAMEPHYSICSMANGER_H_

#include "bulletWorld.h"
#include "physics_includes.h"

class BTRigidBody;
//class BTSoftBody; xxx
//class BTGhost;

/**
 * \brief Singleton manager updating attributes of physics components.
 *
 * Prepared for multi-threading.
 * .
 * This manager could throw events when objects collide. By default this
 * feature is disabled.\n
 * The behavior is:
 * - when two objects collide, it is thrown the
 * "<CollidingObjectType1>_<CollidingObjectType2>_Collision" event that has as
 * parameters the two colliding PandNodes. This event is thrown continuously
 * until the two objects keep overlapping, at a frequency which is the minimum
 * between the fps && the frequency specified as parameter (which defaults to 30
 * times per seconds);
 * - when the two objects stop collide,  it is thrown the
 * "<CollidingObjectType1>_<CollidingObjectType2>_CollisionOff" event  that has
 * as parameters the two PandNodes that have just finished colliding. This
 * event is thrown only once.
 */
/**
 * GamePhysicsManager Singleton class.
 *
 * Used for handling BTRigidBodys, BTSoftBodys, BTGhosts.
 */
class EXPORT_CLASS GamePhysicsManager: public TypedReferenceCount,
		public Singleton<GamePhysicsManager>
{
PUBLISHED:
	GamePhysicsManager(int taskSort = 10, const NodePath& root = NodePath(),
			const CollideMask& mask = GeomNode::get_default_collide_mask());
	virtual ~GamePhysicsManager();

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
	 * Shape type.
	 */
	enum ShapeType
	{
		SPHERE = 0, //!< SPHERE (radius)
		PLANE,//!< PLANE (norm_x, norm_y, norm_z, d)
		BOX,//!< BOX (half_x, half_y, half_z)
		CYLINDER,//!< CYLINDER (radius, height, up)
		CAPSULE,//!< CAPSULE (radius, height, up)
		CONE,//!< CONE (radius, height, up)
		HEIGHTFIELD,//!< HEIGHTFIELD (image, height, up, scale_w, scale_d)
		TRIANGLEMESH,//!< TRIANGLEMESH (dynamic)
	};

	/**
	 * Shape size.
	 */
	enum ShapeSize
	{
		MINIMUN = 0, //!< MINIMUN shape
		MAXIMUM,//!< MAXIMUM shape
		MEDIUM,//!< MEDIUM shape
	};

	/**
	 * \name REFERENCE NODES
	 */
	///@{
	INLINE NodePath get_reference_node_path() const;
	INLINE void set_reference_node_path(const NodePath& reference);
	INLINE NodePath get_bullet_debug_node_path() const;
	///@}

	/**
	 * \name BTRigidBody
	 */
	///@{
	NodePath create_rigid_body(const string& name);
	bool destroy_rigid_body(NodePath rigidBodyNP);
	PT(BTRigidBody) get_rigid_body(int index) const;
	INLINE int get_num_rigid_bodies() const;
	MAKE_SEQ(get_rigid_bodies, get_num_rigid_bodies, get_rigid_body);
	///@}

//	/**
//	 * \name BTSoftBody xxx
//	 */
//	///@{
//	NodePath create_soft_body(const string& name);
//	bool destroy_soft_body(NodePath softBodyNP);
//	PT(BTSoftBody) get_soft_body(int index) const;
//	INLINE int get_num_soft_bodies() const;
//	MAKE_SEQ(get_soft_bodies, get_num_soft_bodies, get_soft_body);
//	///@}

//	/**
//	 * \name BTGhost xxx
//	 */
//	///@{
//	NodePath create_ghost(const string& name);
//	bool destroy_ghost(NodePath ghostNP);
//	PT(BTGhost) get_ghost(int index) const;
//	INLINE int get_num_ghosts() const;
//	MAKE_SEQ(get_ghosts, get_num_ghosts, get_ghost);
//	///@}

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
	 * \name BULLET WORLD
	 */
	///@{
	INLINE PT(BulletWorld) get_bullet_world() const;
	///@}

	/**
	 * \name UTILITIES
	 */
	///@{
	PT(BulletShape) create_shape(NodePath modelNP, ShapeType shapeType,
			ShapeSize shapeSize, LVecBase3f& modelDims, LVector3f& modelDeltaCenter,
			float& modelRadius, float& dim1, float& dim2, float& dim3, float& dim4,
			bool automaticShaping = true, BulletUpAxis upAxis=Z_up,
			const Filename& heightfieldFile = Filename(""), bool dynamic = false);
	float get_bounding_dimensions(NodePath modelNP, LVecBase3f& modelDims,
			LVector3f& modelDeltaCenter) const;
	Pair<bool,float> get_collision_height(const LPoint3f& fromPos);
	INLINE CollideMask get_collide_mask() const;
	///@}

	///Thrown events.
	enum EventThrown
	{
		COLLISIONNOTIFY
	};

	/**
	 * \brief Enable/disable collisions' notification through events.
	 *
	 */
	/**
	 * \brief Enable/disable collisions' notification through events.
	 * @param enable Enabling flag.
	 */
	void enable_collision_notify(EventThrown event, /*const string& eventName, xxx*/
			float eventFreq);

	/**
	 * \brief Initializes debugging.
	 */
	void init_debug();
	/**
	 * \brief Enables/disables debugging.
	 * @param enable True to enable, false to disable.
	 */
	void debug(bool enable);

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
	/// Bullet world.
	PT(BulletWorld) mBulletWorld;

	///The update task sort (should be >0).
	int mTaskSort;

	///The reference node path.
	NodePath mReferenceNP;

	///List of BTRigidBodys handled by this manager.
	typedef pvector<PT(BTRigidBody)> RigidBodyList;
	RigidBodyList mRigidBodies;
	///BTRigidBodys' parameter table.
	ParameterTable mRigidBodiesParameterTable;

//	///List of BTSoftBodys handled by this manager. xxx
//	typedef pvector<PT(BTSoftBody)> SoftBodyList;
//	SoftBodyList mSoftBodies;
//	///BTSoftBodys' parameter table.
//	ParameterTable mSoftBodiesParameterTable;

//	///List of BTGhosts handled by this manager. xxx
//	typedef pvector<PT(BTGhost)> GhostList;
//	GhostList mGhosts;
//	///BTGhosts' parameter table.
//	ParameterTable mGhostsParameterTable;

	///@{
	///A task data for step simulation update.
	PT(TaskInterface<GamePhysicsManager>::TaskData) mUpdateData;
	PT(AsyncTask) mUpdateTask;
	///@}

	///Unique ref.
	int mRef;

	///Utilities.
	CollideMask mMask; //a.k.a. BitMask32

	/**
	 * \name Collision notification  through events.
	 */
	///@{
	struct CollidingNodePair
	{
		struct CollidingNodePairData
		{
			pair<PandaNode*, PandaNode*> mPnode;
			string mEventName;
			EventParameter mEventParameters[2];
			int mCount;
		};
		//main constructors
		CollidingNodePair(PandaNode *_pnode0, PandaNode *_pnode1) :
				mCollidingNodePairData(new CollidingNodePairData), mRefCount(new int)
		{
			// always create the pair in a predictable order
			// (use the pointer value..)
			if (_pnode0 > _pnode1)
			{
				mCollidingNodePairData->mPnode = make_pair(_pnode1, _pnode0);
			}
			else
			{
				mCollidingNodePairData->mPnode = make_pair(_pnode0, _pnode1);
			}
			*mRefCount = 1;
		}
		CollidingNodePair(PandaNode *_pnode0, PandaNode *_pnode1, const string& _name, int _count) :
				mCollidingNodePairData(new CollidingNodePairData), mRefCount(new int)
		{
			// always create the pair in a predictable order
			// (use the pointer value..)
			if (_pnode0 > _pnode1)
			{
				mCollidingNodePairData->mPnode = make_pair(_pnode1, _pnode0);
			}
			else
			{
				mCollidingNodePairData->mPnode = make_pair(_pnode0, _pnode1);
			}
			mCollidingNodePairData->mEventName = _name;
			mCollidingNodePairData->mCount = _count;
			*mRefCount = 1;
		}
		//copy constructor
		CollidingNodePair(const CollidingNodePair& on) :
				mCollidingNodePairData(on.mCollidingNodePairData), mRefCount(on.mRefCount)
		{
			++(*mRefCount);
		}
		//destructor
		~CollidingNodePair()
		{
			if (--(*mRefCount) == 0)
			{
				delete mCollidingNodePairData;
				delete mRefCount;
			}
		}
		//assignment operator
		CollidingNodePair& operator=(const CollidingNodePair& on)
		{
			mCollidingNodePairData = on.mCollidingNodePairData;
			++(*mRefCount);
			return *this;
		}
		//comparison operator
		bool operator<(const CollidingNodePair& on) const
		{
			return mCollidingNodePairData->mPnode < on.mCollidingNodePairData->mPnode;
		}
		//owned resource
		CollidingNodePairData* mCollidingNodePairData;
	private:
		int* mRefCount;
	};
	ThrowEventData mCollisionNotify;
	pset<CollidingNodePair> mCollidingNodePairs;
	btCollisionDispatcher* mCollisionDispatcher;
	///Helpers.
	float do_get_dim(ShapeSize shapeSize, float d1, float d2);
	///@}

	/// Bullet Debug node path.
	NodePath mBulletDebugNodePath;

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
