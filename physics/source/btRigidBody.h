/**
 * \file btRigidBody.h
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef BTRIGIDBODY_H_
#define BTRIGIDBODY_H_

#include "gamePhysicsManager.h"
#include "physics_includes.h"

#ifndef CPPPARSER
#include "support/common.h"
#endif //CPPPARSER

/**
 * BTRigidBody is a PandaNode representing a single "rigid body" of the Bullet
 * library integration of Panda3d.
 *
 * It constructs a rigid body with the single specified collision shape_type
 * along with relevant parameters.\n
 * Collision shape types are:
 * - *sphere*
 * - *plane*
 * - *box*
 * - *cylinder*
 * - *capsule*
 * - *cone*
 * - *heightfield*
 * - *triangle mesh*
 * .
 * In case of *sphere*, *box*, *cylinder*, *capsule*, *cone*, if any of
 * the relevant parameters is missing, the shape is automatically
 * constructed by guessing them through calculation of a tight bounding volume
 * of object geometry (supposedly specified by the model component).\n
 * For *plane* shape, in case of missing parameters, the default is
 * a plane with normal = (0,0,1) and d = 0.
 *
 * > **BTRigidBody text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *object*					|single| - | -
 * | *body_type*  				|single| *dynamic* | values: static,dynamic,kinematic
 * | *body_mass*  				|single| 1.0 | -
 * | *body_friction*  			|single| 0.8 | -
 * | *body_restitution*  		|single| 0.1 | -
 * | *collide_mask*  			|single| *all_on* | -
 * | *shape_type*  				|single| *sphere* | values: sphere,plane,box,cylinder,capsule,cone,heightfield,triangle_mesh
 * | *shape_size*  				|single| *medium* | values: minimum,medium,maximum
 * | *use_shape_of*				|single| - | - xxx
 * | *shape_radius*  			|single| - | for sphere,cylinder,capsule,cone
 * | *shape_norm_x*  			|single| - | for plane
 * | *shape_norm_y*  			|single| - | for plane
 * | *shape_norm_z*  			|single| - | for plane
 * | *shape_d*  				|single| - | for plane
 * | *shape_half_x*  			|single| - | for box
 * | *shape_half_y*  			|single| - | for box
 * | *shape_half_z*  			|single| - | for box
 * | *shape_height*  			|single| 1.0 | for cylinder,capsule,cone,heightfield
 * | *shape_up*  				|single| *z* | values: x,y,z for cylinder,capsule,cone,heightfield
 * | *shape_heightfield_file* 	|single| - | for heightfield
 * | *shape_scale_w*  			|single| 1.0 | for heightfield
 * | *shape_scale_d*  			|single| 1.0 | for heightfield
 * | *ccd_motion_threshold*  	|single| - | -
 * | *ccd_swept_sphere_radius* 	|single| - | -
 *
 * \note parts inside [] are optional.\n
 */
class EXPORT_CLASS BTRigidBody: public BulletRigidBodyNode
{
PUBLISHED:

	/**
	 * The body type.
	 * It may change during the BTRigidBody's lifetime.
	 */
	enum BodyType
	{
		DYNAMIC, //!< DYNAMIC: mass != 0.0, physics driven (default)
		STATIC,//!< STATIC: mass == 0.0, no driven
		KINEMATIC//!< KINEMATIC: mass == 0.0, user driven
	};

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~BTRigidBody();
#endif //CPPPARSER

	/**
	 * \name REFERENCE NODE
	 */
	///@{
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name RIGIDBODY
	 */
	///@{
	void setup(NodePath& objectNP);//xxx a.k.a. RigidBody::onAddToObjectSetup()
	void cleanup();//xxx a.k.a. RigidBody::onRemoveFromObjectCleanup()
	void update(float dt);
	///@}

	/**
	 * \name PARAMETERS' GETTERS/SETTERS
	 */
	///@{
	void switch_body_type(BodyType bodyType);
	INLINE void set_shape_type(GamePhysicsManager::ShapeType value);
	INLINE GamePhysicsManager::ShapeType get_shape_type() const;
	INLINE void set_shape_size(GamePhysicsManager::ShapeSize value);
	INLINE GamePhysicsManager::ShapeSize get_shape_size() const;
//	use_shape_of() xxx ?
	INLINE void set_shape_radius(float value);
	INLINE float get_shape_radius() const;
	INLINE void set_shape_norm(const LVector3f& value);
	INLINE LVector3f get_shape_norm() const;
	INLINE void set_shape_d(float value);
	INLINE float get_shape_d() const;
	INLINE void set_shape_half_dims(const LVecBase3f& value);
	INLINE LVecBase3f get_shape_half_dims() const;
	INLINE void set_shape_height(float value);
	INLINE float get_shape_height() const;
	INLINE void set_shape_up(BulletUpAxis value);
	INLINE BulletUpAxis get_shape_up() const;
	INLINE void set_shape_heightfield_file(const string& value);
	INLINE string get_shape_heightfield_file() const;
	INLINE void set_shape_scale_width_depth(const LVecBase2f& value);
	INLINE LVecBase2f get_shape_scale_width_depth() const;
	///@}

	/**
	 * \name OUTPUT
	 */
	///@{
	void output(ostream &out) const;
	///@}

#if defined(PYTHON_BUILD) || defined(CPPPARSER)
	/**
	 * \name PYTHON UPDATE CALLBACK
	 */
	///@{
	void set_update_callback(PyObject *value);
	///@}
#else
	/**
	 * \name C++ UPDATE CALLBACK
	 */
	///@{
	typedef void (*UPDATECALLBACKFUNC)(PT(BTRigidBody));
	void set_update_callback(UPDATECALLBACKFUNC value);
	///@}
#endif //PYTHON_BUILD

public:
	/**
	 * \name C++ ONLY
	 * Library & support low level related methods.
	 */
	///@{
	///@}

protected:
	friend void unref_delete<BTRigidBody>(BTRigidBody*);
	friend class GamePhysicsManager;

	BTRigidBody(const string& name);
	virtual ~BTRigidBody();

private:
	///The reference node path.
	NodePath mReferenceNP;
	///The setup flag.
	bool mSetup;
	///@{
	///Physical parameters.
	BodyType mBodyType;
	GamePhysicsManager::ShapeType mShapeType;
	GamePhysicsManager::ShapeSize mShapeSize;

	inline void do_reset();
	void do_initialize();
	void do_finalize();

	/**
	 * \name Helpers variables/functions.
	 */
	///@{
	void do_check_auto_shaping();
	///Geometric functions and parameters.
	PT(BulletShape) do_create_shape(GamePhysicsManager::ShapeType shapeType,
			const NodePath& objectNP);
	LVecBase3f mModelDims;
	float mModelRadius;
	//use shape of (another object).
//	ObjectId mUseShapeOfId; //xxx
	//any model has a local frame and the tight bounding box is computed
	//wrt it; so mModelDeltaCenter represents a transform (translation) to
	//be applied to the model node path so that the middle point of the
	//bounding box will overlap the frame center of the parent's node path .
	LVector3f mModelDeltaCenter;
	bool mAutomaticShaping;
	float mDim1, mDim2, mDim3, mDim4;
	Filename mHeightfieldFile;
	BulletUpAxis mUpAxis;
	///@}

#if defined(PYTHON_BUILD) || defined(CPPPARSER)
	/**
	 * \name Python callback.
	 */
	///@{
	PyObject *mSelf;
	PyObject *mUpdateCallback;
	PyObject *mUpdateArgList;
	///@}
#else
	/**
	 * \name C++ callback.
	 */
	///@{
	UPDATECALLBACKFUNC mUpdateCallback;
	///@}
#endif //PYTHON_BUILD

	// Explicitly disabled copy constructor and copy assignment operator.
	BTRigidBody(const BTRigidBody&);
	BTRigidBody& operator=(const BTRigidBody&);

public:
	/**
	 * \name TypedWritable API
	 */
	///@{
	static void register_with_read_factory();
	virtual void write_datagram (BamWriter *manager, Datagram &dg) override;
	virtual int complete_pointers(TypedWritable **p_list, BamReader *manager) override;
	///@}

protected:
	static TypedWritable *make_from_bam(const FactoryParams &params);
	virtual void fillin(DatagramIterator &scan, BamReader *manager) override;

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
		BulletRigidBodyNode::init_type();
		register_type(_type_handle, "BTRigidBody", BulletRigidBodyNode::get_class_type());
	}
	virtual TypeHandle get_type() const
	{
		return get_class_type();
	}
	virtual TypeHandle force_init_type()
	{
		init_type();
		return get_class_type();
	}
	///@}

private:
	static TypeHandle _type_handle;

};

INLINE ostream &operator << (ostream &out, const BTRigidBody & rigid_body);

///inline
#include "btRigidBody.I"

#endif /* BTRIGIDBODY_H_ */
