/**
 * \file btSoftBody.h
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef BTSOFTBODY_H_
#define BTSOFTBODY_H_

#include "gamePhysicsManager.h"
#include "physics_includes.h"

#ifndef CPPPARSER
#include "support/common.h"
#endif //CPPPARSER

/**
 * BTSoftBody is an extension/specialization of the BulletSoftBodyNode PandaNode
 * representing a "soft body" of the Bullet library integration of Panda3d.
 *
 * It constructs a soft body with basic parameters.\n
 * Others settings could be made in programs by directly referring to the
 * underlying BulletSoftBodyNode.\n
 * \note rope is visualized as nurbs curse; patch is visualized as GeomNode.\n
 *
 * Soft body types are:
 * - *rope*
 * - *patch*
 * - *ellipsoid*
 * - *tri(angle) mesh*
 * - *tetra(hedron) mesh*
 *
 * > **BTSoftBody text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *object*					|single| - | -
 * | *body_type*				|single| *rope* | values: rope,patch,ellipsoid,tri_mesh,tetra_mesh
 * | *collide_mask*				|single| *all_on* | -
 * | *body_total_mass*			|single| 1.0 | -
 * | *body_mass_from_faces*		|single| *false* | -
 * | *air_density*				|single| 1.2 | -
 * | *water_density*  			|single| 0.0 | -
 * | *water_offset*  			|single| 0.0 | -
 * | *water_normal*  			|single| 0.0,0.0,0.0 | -
 * | *show_model*				|single| *false* | for patch,ellipsoid,tetra_mesh
 * | *points*  					|single| - | for rope,patch,ellipsoid specified as "x1,y1,z1[:x2,y2,z2:...:xN,yN,zN]" with N=1..4
 * | *res*  					|single| - | for rope,patch,ellipsoid specified as "res1[:res2]"
 * | *fixeds*  					|single| - | for rope,patch
 * | *gendiags*  				|single| *true* | for patch
 * | *radius*					|single| 1.0,1.0,1.0 | for ellipsoid
 * | *randomize_constraints*	|single| *true* | for tri_mesh
 * | *tetra_data_files*			|single| - | for tetra_mesh specified as "elems,faces,nodes"
 *
 * \note parts inside [] are optional.\n
 */
class EXPORT_CLASS BTSoftBody: public BulletSoftBodyNode
{
PUBLISHED:

	/**
	 * The soft body type.
	 * It may change during the BTRigidBody's lifetime.
	 */
	enum BTBodyType
	{
		ROPE,
		PATCH,
		ELLIPSOID,
		TRIMESH,
		TETRAMESH
	};

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~BTSoftBody();
#endif //CPPPARSER

	/**
	 * \name REFERENCE NODE
	 */
	///@{
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name SOFTBODY
	 */
	///@{
	void setup(NodePath& objectNP);
	void cleanup();
	void update(float dt);
	///@}

	/**
	 * \name PARAMETERS' GETTERS/SETTERS
	 */
	///@{
	INLINE void set_body_type(BTBodyType value);
	INLINE BTBodyType get_body_type() const;
	INLINE void set_body_total_mass(float value);
	INLINE float get_body_total_mass() const;
	INLINE void set_body_mass_from_faces(bool value);
	INLINE bool get_body_mass_from_faces() const;
	INLINE void set_air_density(float value);
	INLINE float get_air_density() const;
	INLINE void set_water_density(float value);
	INLINE float get_water_density() const;
	INLINE void set_water_offset(float value);
	INLINE float get_water_offset() const;
	INLINE void set_water_normal(const LVector3f& value);
	INLINE LVector3f get_water_normal() const;
	INLINE void set_show_model(bool value);
	INLINE bool get_show_model() const;
	INLINE void set_points(const ValueList<LPoint3f>& value);
	INLINE ValueList<LPoint3f> get_points() const;
	INLINE void set_res(const ValueList<int>& value);
	INLINE ValueList<int> get_res() const;
	INLINE void set_fixeds(int value);
	INLINE int get_fixeds() const;
	INLINE void set_gendiags(bool value);
	INLINE bool get_gendiags() const;
	INLINE void set_radius(const LVecBase3f& value);
	INLINE LVecBase3f get_radius() const;
	INLINE void set_randomize_constraints(bool value);
	INLINE bool get_randomize_constraints() const;
	INLINE void set_tetra_data_files(const ValueList<string>& value);
	INLINE ValueList<string> get_tetra_data_files() const;
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
	typedef void (*UPDATECALLBACKFUNC)(PT(BTSoftBody));
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
	friend void unref_delete<BTSoftBody>(BTSoftBody*);
	friend class GamePhysicsManager;

	BTSoftBody(const string& name);
	virtual ~BTSoftBody();

private:
	///The reference node path.
	NodePath mReferenceNP;
	///The setup flag.
	bool mSetup;
	///@{
	///Physical parameters.
	BTBodyType mBodyType;
	float mBodyTotalMass, mAirDensity, mWaterDensity, mWaterOffset;
	bool mBodyMassFromFaces;
	LVector3f mWaterNormal;
	///@}
	///@{
	///Geometric, structural and other parameters.
	bool mShowModel;
	pvector<LPoint3f> mPoints;
	pvector<int> mRes;
	int mFixeds;
	bool mGendiags;
	LVecBase3f mRadius;
	bool mRandomizeConstraints;
	pmap<string, string> mTetraDataFileNames;
	///HACK: rope node's parent node path correction (see bullet samples).
	NodePath mRopeNodePath;
	///@}

	inline void do_reset();
	void do_initialize();
	void do_finalize();

	///@{
	//HACK 1
	btSoftBody* do_make_fake_soft_body();
	void do_set_soft_body(btSoftBody* body);
	//HACK 2: helpers
	void do_make_rope(BulletSoftBodyWorldInfo &info, const LPoint3 &from,
			const LPoint3 &to, int res, int fixeds);
	void do_make_patch(BulletSoftBodyWorldInfo &info, const LPoint3 &corner00,
			const LPoint3 &corner10, const LPoint3 &corner01, const LPoint3 &corner11,
			int resx, int resy, int fixeds, bool gendiags);
	void do_make_ellipsoid(BulletSoftBodyWorldInfo &info, const LPoint3 &center,
			const LVecBase3 &radius, int res);
	void do_make_tri_mesh(BulletSoftBodyWorldInfo &info,const Geom *geom,
			bool randomizeConstraints);
	void do_make_tet_mesh(BulletSoftBodyWorldInfo &info, const char *ele,
			const char *face, const char *node);
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
	BTSoftBody(const BTSoftBody&);
	BTSoftBody& operator=(const BTSoftBody&);

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
		BulletSoftBodyNode::init_type();
		register_type(_type_handle, "BTSoftBody", BulletSoftBodyNode::get_class_type());
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

INLINE ostream &operator << (ostream &out, const BTSoftBody & soft_body);

///inline
#include "btSoftBody.I"

#endif /* BTSOFTBODY_H_ */
