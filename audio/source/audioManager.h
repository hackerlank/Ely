/**
 * \file audioManager.h
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef AUDIOMANGER_H_
#define AUDIOMANGER_H_

#include "graphicsWindow.h"
#include "audio_includes.h"
#include "audioTools.h"
#include "collisionTraverser.h"
#include "collisionHandlerQueue.h"
#include "collisionRay.h"

class P3Sound3d;
class P3Listener;

/**
 * AudioManager Singleton class.
 *
 * Used for handling P3Sound3ds, P3Listeners.
 */
class EXPORT_CLASS AudioManager: public TypedReferenceCount,
		public Singleton<AudioManager>
{
PUBLISHED:
	AudioManager(PT(GraphicsWindow) win, int taskSort = 10,
			const NodePath& root = NodePath(),
			const CollideMask& mask = GeomNode::get_default_collide_mask());
	virtual ~AudioManager();

	/**
	 * \name REFERENCE NODES
	 */
	///@{
	INLINE NodePath get_reference_node_path() const;
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name P3Sound3d
	 */
	///@{
	NodePath create_sound3d(const string& name);
	bool destroy_sound3d(NodePath plugInNP);
	PT(P3Sound3d) get_sound3d(int index) const;
	INLINE int get_num_sound3ds() const;
	MAKE_SEQ(get_sound3ds, get_num_sound3ds, get_sound3d);
	///@}

	/**
	 * \name P3Listener
	 */
	///@{
	NodePath create_listener(const string& name);
	bool destroy_listener(NodePath steerVehicleNP);
	PT(P3Listener) get_listener(int index) const;
	INLINE int get_num_listeners() const;
	MAKE_SEQ(get_listeners, get_num_listeners, get_listener);
	///@}

	/**
	 * The type of object for creation parameters.
	 */
	enum AudioType
	{
		DRIVER = 0,
		CHASER
	};

	/**
	 * \name TEXTUAL PARAMETERS
	 */
	///@{
	ValueList<string> get_parameter_name_list(AudioType type) const;
	void set_parameter_values(AudioType type, const string& paramName, const ValueList<string>& paramValues);
	ValueList<string> get_parameter_values(AudioType type, const string& paramName) const;
	void set_parameter_value(AudioType type, const string& paramName, const string& value);
	string get_parameter_value(AudioType type, const string& paramName) const;
	void set_parameters_defaults(AudioType type);
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
	INLINE static AudioManager* get_global_ptr();
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

	///List of P3Sound3ds handled by this manager.
	typedef pvector<PT(P3Sound3d)> Sound3dList;
	Sound3dList mSound3ds;
	///P3Sound3ds' parameter table.
	ParameterTable mSound3dsParameterTable;

	///List of Listeners handled by this manager.
	typedef pvector<PT(P3Listener)> ListenerList;
	ListenerList mListeners;
	///Listeners' parameter table.
	ParameterTable mListenersParameterTable;

	///@{
	///A task data for step simulation update.
	PT(TaskInterface<AudioManager>::TaskData) mUpdateData;
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
		register_type(_type_handle, "AudioManager",
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
#include "audioManager.I"

#endif /* AUDIOMANGER_H_ */
