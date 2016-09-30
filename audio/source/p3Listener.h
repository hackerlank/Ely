/**
 * \file p3Listener.h
 *
 * \date 2016-09-30
 * \author consultit
 */

#ifndef P3CHASER_H_
#define P3CHASER_H_

#include "audio_includes.h"
#include "audioTools.h"
#include "audioManager.h"

#ifndef CPPPARSER
#endif //CPPPARSER

/**
 * P3Listener is a PandaNode class designed to make an object a listener of another
 * object.
 *
 * P3Listener can be enabled/disabled as a whole (enabled by default).\n
 * P3Listener can handle basic rotations movement (head_left, head_right,
 * pitch_left, pitch_right) only if it is enabled to do so by calling the
 * corresponding "enabler". In turn, an enabled basic rotation movement can be
 * activated/deactivated through the corresponding "activator".\n
 * Rotation is updated relative to:
 * - z local axis, ie head (yaw)
 * - x local axis, ie pitch
 * Rotation through y local axis (roll) is not considered.\n
 * Chasing can be fixed or dampened, from behind or from the front.\n
 * With dampened chasing various movement's parameters (like friction, min/max
 * distance, min/max height etc...) can be set.\n
 * A task could update the position/orientation of the attached PandaNode object
 * based on the current position/orientation of the chased object, and the
 * currently enabled basic rotation movements, by calling the "update()"
 * method.\n
 * Usually movements are activated/deactivated through callback associated to
 * events (keyboard, mouse etc...).\n
 * Since in Panda3d by default, "mouse-move" events are not defined, mouse
 * movements can be enabled/disabled as "implicit activators" of head/pitch
 * basic rotations by calling "enable_mouse_head()"/"enable_mouse_pitch()"
 * methods. In this way head/pitch basic rotations can be activated
 * independently through mouse movements and/or normal activator methods.\n
 * On the other hand, "mouse-move" event could be defined by using
 * \code
 * ButtonThrower::set_move_event()
 * \endcode
 * (\see: http://www.panda3d.org/forums/viewtopic.php?t=9326 and
 * \see: http://www.panda3d.org/forums/viewtopic.php?t=6049), and in this case
 * if an application wishes to handle directly these events, it has to disable
 * the previously described mouse movements handling by calling:
 * \code
 * enable_mouse_move(true)
 * \endcode
 * The up axis is the "z" axis.
 * Rotation movements can be inverted (default: not inverted).\n
 *
 * > **P3Listener text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *backward*					|single| *true* | -
 * | *fixed_relative_position*	|single| *true* | -
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
class EXPORT_CLASS P3Listener: public PandaNode
{
PUBLISHED:

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~P3Listener();
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
	typedef void (*UPDATECALLBACKFUNC)(PT(P3Listener));
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
	friend void unref_delete<P3Listener>(P3Listener*);
	friend class AudioManager;

	P3Listener(const string& name);
	virtual ~P3Listener();

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
	LPoint3f mListenerPosition, mLookAtPosition;
	///@{
	///Key audios and effective keys.
	bool mHeadLeft, mHeadRight, mPitchUp, mPitchDown;
	bool mHeadLeftKey, mHeadRightKey, mPitchUpKey, mPitchDownKey, mMouseMoveKey;
	///@}
	///@{
	///Key audio values.
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
	LPoint3f do_get_listener_pos(LPoint3f desiredListenerPos,
			LPoint3f currentListenerPos, float deltaTime);
	void do_correct_listener_height(LPoint3f& newPos, float baseHeight);
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
	P3Listener(const P3Listener&);
	P3Listener& operator=(const P3Listener&);

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
		register_type(_type_handle, "P3Listener", PandaNode::get_class_type());
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

INLINE ostream &operator << (ostream &out, const P3Listener & listener);

///inline
#include "p3Listener.I"

#endif /* P3CHASER_H_ */
