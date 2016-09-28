/**
 * \file p3Chaser.h
 *
 * \date 2016-09-27
 * \author consultit
 */

#ifndef P3CHASER_H_
#define P3CHASER_H_

#include "control_includes.h"
#include "controlTools.h"
#include "controlManager.h"

#ifndef CPPPARSER
#endif //CPPPARSER

/**
 * P3Chaser is a PandaNode class designed to make an object a chaser of another
 * object.
 *
 * Chasing can be fixed or dampened, from behind or from the front.\n
 * With dampened chasing various movement's (like friction, min/max distance,
 * min/max height etc...) can be set.\n
 * XXX See P3Driver for enabilg/activating rotations.
 * P3Chaser could be rotated by both theRotations through mouse movements could be activated.\n
 * The up axis is the "z" axis.
 *
 * > **P3Chaser text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *backward*					|single| *true* | -
 * | *chased_object*			|single| - | -
 * | *fixed_relative_position*	|single| *true* | -
 * | *reference_object*			|single| - | -
 * | *max_distance*				|single| - | -
 * | *min_distance*				|single| - | -
 * | *max_height*				|single| - | -
 * | *min_height*				|single| - | -
 * | *friction*					|single| 1.0 | -
 * | *fixed_look_at*			|single| *true* | -
 * | *look_at_distance*			|single| - | -
 * | *look_at_height*			|single| - | -
 * | *mouse_move*  				|single| *disabled* | -
 * | *mouse_head*  				|single| *disabled* | -
 * | *mouse_pitch*  			|single| *disabled* | -
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *pitch_up*  				|single| *enabled* | -
 * | *pitch_down*  				|single| *enabled* | -
 * | *sens_x*  					|single| 0.2 | -
 * | *sens_y*  					|single| 0.2 | -
 * | *inverted_rotation*		|single| *false* | -
 *
 * \note parts inside [] are optional.\n
 */
class EXPORT_CLASS P3Chaser: public PandaNode
{
PUBLISHED:

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~P3Chaser();
#endif //CPPPARSER

	/**
	 * \name REFERENCE NODE
	 */
	///@{
	INLINE void set_reference_node_path(const NodePath& reference);
	///@}

	/**
	 * \name CHASER
	 */
	///@{
	INLINE void set_chased_object(const NodePath& object);
	INLINE NodePath get_chased_object() const;
	bool enable();
	bool disable();
	INLINE bool is_enabled() const;
	void update(float dt);
	///@}

	/**
	 * \name MOVEMENT ENABLERS
	 */
	///@{
	INLINE void enable_head_left(bool enable);
	INLINE bool is_head_left_enabled() const;
	INLINE void enable_head_right(bool enable);
	INLINE bool is_head_right_enabled() const;
	INLINE void enable_pitch_up(bool enable);
	INLINE bool is_pitch_up_enabled() const;
	INLINE void enable_pitch_down(bool enable);
	INLINE bool is_pitch_down_enabled() const;
	INLINE void enable_mouse_head(bool enable);
	INLINE bool is_mouse_head_enabled() const;
	INLINE void enable_mouse_pitch(bool enable);
	INLINE bool is_mouse_pitch_enabled() const;
	INLINE void enable_mouse_move(bool enable);
	INLINE bool is_mouse_move_enabled() const;
	///@}

	/**
	 * \name MOVEMENT ACTIVATORS
	 */
	///@{
	INLINE void set_rotate_head_left(bool activate);
	INLINE bool get_rotate_head_left() const;
	INLINE void set_rotate_head_right(bool activate);
	INLINE bool get_rotate_head_right() const;
	INLINE void set_rotate_pitch_up(bool activate);
	INLINE bool get_rotate_pitch_up() const;
	INLINE void set_rotate_pitch_down(bool activate);
	INLINE bool get_rotate_pitch_down() const;
	///@}

	/**
	 * \name PARAMETERS' GETTERS/SETTERS
	 */
	///@{
	INLINE void set_hold_look_at(bool activate);
	INLINE bool get_hold_look_at() const;
	INLINE void set_backward(bool activate);
	INLINE bool get_backward() const;
	INLINE void set_fixed_relative_position(bool activate);
	INLINE bool get_fixed_relative_position();
	INLINE void set_inverted_rotation(bool activate);
	INLINE bool get_inverted_rotation() const;
	INLINE void set_max_distance(float absMaxDistance);
	INLINE float get_max_distance() const;
	INLINE void set_min_distance(float absMinDistance);
	INLINE float get_min_distance() const;
	INLINE void set_max_height(float absMaxHeight);
	INLINE float get_max_height() const;
	INLINE void set_min_height(float absMinHeight);
	INLINE float get_min_height() const;
	INLINE void set_look_at_distance(float absLookAtDistance);
	INLINE float get_look_at_distance() const;
	INLINE void set_look_at_height(float absLookAtHeight);
	INLINE float get_look_at_height() const;
	INLINE void set_friction(float friction);
	INLINE float get_friction() const;
	INLINE void set_sens(float sensX, float sensY);
	INLINE ValueList<float> get_sens() const;
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
	typedef void (*UPDATECALLBACKFUNC)(PT(P3Chaser));
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
	friend void unref_delete<P3Chaser>(P3Chaser*);
	friend class ControlManager;

	P3Chaser(const string& name);
	virtual ~P3Chaser();

private:
	///The chased object's node path.
	NodePath mChasedNP;
	///This NodePath.
	NodePath mThisNP;
	///The reference node path.
	NodePath mReferenceNP;
	///The reference graphic window.
	PT(GraphicsWindow) mWin;
	///Auxiliary node path to track the fixed look at.
	NodePath mFixedLookAtNP;
	///Flags.
	bool mEnabled, mFixedRelativePosition, mBackward, mFixedLookAt, mHoldLookAt;
	///Kinematic parameters.
	float mAbsLookAtDistance, mAbsLookAtHeight, mAbsMaxDistance, mAbsMinDistance,
	mAbsMinHeight, mAbsMaxHeight, mFriction;
	///Positions.
	LPoint3f mChaserPosition, mLookAtPosition;
	///@{
	///Key controls and effective keys.
	bool mHeadLeft, mHeadRight, mPitchUp, mPitchDown;
	bool mHeadLeftKey, mHeadRightKey, mPitchUpKey, mPitchDownKey, mMouseMoveKey;
	///@}
	///@{
	///Key control values.
	bool mMouseEnabledH, mMouseEnabledP, mMouseHandled;
	char mSignOfMouse;
	///@}
	///@{
	/// Sensitivity settings.
	float mSensX, mSensY, mHeadSensX, mHeadSensY;
	int mCentX, mCentY;
	///@}

	inline void do_reset();
	void do_initialize();
	void do_finalize();

	/**
	 * \name Helpers variables/functions.
	 */
	///@{
	void do_enable();
	void do_disable();
	void do_handle_mouse();
	/*
	 * Calculates the dynamic position of the chaser.
	 * \see OgreBulletDemos.
	 * - desiredChaserPos: the desired chaser position (wrt reference).
	 * - currentChaserPos: the current chaser position (wrt reference).
	 * - deltaTime: the delta time update.
	 * Returns the dynamic chaser position.
	 */
	LPoint3f do_get_chaser_pos(LPoint3f desiredChaserPos,
			LPoint3f currentChaserPos, float deltaTime);
	/*
	 * Correct the dynamic height of the chaser.
	 * - newPos: the position whose height may be corrected.
	 * - baseHeight: the corrected height cannot be shorter than this.
	 */
	void do_correct_chaser_height(LPoint3f& newPos, float baseHeight);
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
	P3Chaser(const P3Chaser&);
	P3Chaser& operator=(const P3Chaser&);

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
		PandaNode::init_type();
		register_type(_type_handle, "P3Chaser", PandaNode::get_class_type());
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

INLINE ostream &operator << (ostream &out, const P3Chaser & chaser);

///inline
#include "p3Chaser.I"

#endif /* P3CHASER_H_ */
