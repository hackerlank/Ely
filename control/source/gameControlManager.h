/**
 * \file gameControlManager.h
 *
 * \date 2016-09-18
 * \author consultit
 */

#ifndef GAMECONTROLMANGER_H_
#define GAMECONTROLMANGER_H_

#include "../../common/source/commonTools.h"
#include "graphicsWindow.h"
#include "control_includes.h"
#include "collisionTraverser.h"
#include "collisionHandlerQueue.h"
#include "collisionRay.h"

class P3Driver;
class P3Chaser;

/**
 * GameControlManager Singleton class.
 *
 * Used for handling P3Drivers, P3Chasers.
 */
class EXPORT_CLASS GameControlManager: public TypedReferenceCount,
		public Singleton<GameControlManager>
{
PUBLISHED:
	GameControlManager(PT(GraphicsWindow) win, int taskSort = 10,
			const NodePath& root = NodePath(),
			const CollideMask& mask = GeomNode::get_default_collide_mask());
	virtual ~GameControlManager();

	/**
	 * \name REFERENCE NODES
	 */
	///@{
	INLINE NodePath get_reference_node_path() const;
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name P3Driver
	 */
	///@{
	NodePath create_driver(const string& name);
	bool destroy_driver(NodePath plugInNP);
	PT(P3Driver) get_driver(int index) const;
	INLINE int get_num_drivers() const;
	MAKE_SEQ(get_drivers, get_num_drivers, get_driver);
	///@}

	/**
	 * \name P3Chaser
	 */
	///@{
	NodePath create_chaser(const string& name);
	bool destroy_chaser(NodePath steerVehicleNP);
	PT(P3Chaser) get_chaser(int index) const;
	INLINE int get_num_chasers() const;
	MAKE_SEQ(get_chasers, get_num_chasers, get_chaser);
	///@}

	/**
	 * The type of object for creation parameters.
	 */
	enum ControlType
	{
		DRIVER = 0,
		CHASER
	};

	/**
	 * \name TEXTUAL PARAMETERS
	 */
	///@{
	ValueList<string> get_parameter_name_list(ControlType type) const;
	void set_parameter_values(ControlType type, const string& paramName, const ValueList<string>& paramValues);
	ValueList<string> get_parameter_values(ControlType type, const string& paramName) const;
	void set_parameter_value(ControlType type, const string& paramName, const string& value);
	string get_parameter_value(ControlType type, const string& paramName) const;
	void set_parameters_defaults(ControlType type);
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
	INLINE static GameControlManager* get_global_ptr();
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

	///List of P3Drivers handled by this manager.
	typedef pvector<PT(P3Driver)> DriverList;
	DriverList mDrivers;
	///P3Drivers' parameter table.
	ParameterTable mDriversParameterTable;

	///List of P3Chasers handled by this manager.
	typedef pvector<PT(P3Chaser)> ChaserList;
	ChaserList mChasers;
	///P3Chasers' parameter table.
	ParameterTable mChasersParameterTable;

	///@{
	///A task data for step simulation update.
	PT(TaskInterface<GameControlManager>::TaskData) mUpdateData;
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
		register_type(_type_handle, "GameControlManager",
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
#include "gameControlManager.I"

#endif /* GAMECONTROLMANGER_H_ */
