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
 * \brief Component designed to make an object a chaser of another object.
 *
 * The up axis is the "z" axis.
 *
 * > **XML Param(s)**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *backward*					|single| *true* | -
 * | *chased_object*			|single| - | -
 * | *fixed_relative_position*	|single| *true* | -
 * | *reference_object*			|single| - | -
 * | *abs_max_distance*			|single| - | -
 * | *abs_min_distance*			|single| - | -
 * | *abs_max_height*			|single| - | -
 * | *abs_min_height*			|single| - | -
 * | *friction*					|single| 1.0 | -
 * | *fixed_lookat*				|single| *true* | -
 * | *abs_lookat_distance*		|single| - | -
 * | *abs_lookat_height*		|single| - | -
 * | *mouse_head*  				|single| *false* | -
 * | *mouse_pitch*  			|single| *false* | -
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *pitch_up*  				|single| *enabled* | -
 * | *pitch_down*  				|single| *enabled* | -
 * | *sens_x*  					|single| 0.2 | -
 * | *sens_y*  					|single| 0.2 | -
 * | *inverted_rotation*		|single| *false* | -
 *
 * \note parts inside [] are optional.\n *
 *
 *
 * P3Chaser is a PandaNode class designed for the control of
 * translation/rotation movements. To be driven, a PandaNode object should be
 * attached to this P3Chaser.\n
 *
 * P3Chaser can be enabled/disabled as a whole (enabled by default).\n
 * P3Chaser can handle a given basic movement (forward, backward, head_left,
 * head_right etc...) only if it is enabled to do so by calling the
 * corresponding "enabler". In turn, an enabled basic movement can be
 * activated/deactivated through the corresponding "activator".\n
 * Movement is, by default, based on acceleration (i.e. "dynamic"):
 * accelerations and max speeds can be independently set for any local direction
 * (translation) and local axis (rotation).\n
 * Translation is updated relative to:
 * - x local axis, ie left-right side direction
 * - y local axis, ie forward-backward direction
 * - z local axis, ie up-down direction
 * Rotation is updated relative to:
 * - z local axis, ie head (yaw)
 * - x local axis, ie pitch
 * Rotation through y local axis (roll) is not considered.\n
 * To obtain a fixed movement/rotation (i.e. "kinematic") in any direction, the
 * related acceleration and friction value should be set to zero and a very high
 * value respectively.\n
 * A task could update the position/orientation of the attached PandaNode object
 * based on the currently enabled basic movements, by calling the "update()"
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
 * All movements (but up and down) can be inverted (default: not inverted).\n
 * All movements are computed wrt reference NodePath.\n
 *
 * > **P3Chaser text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *enabled*  				|single| *true* | -
 * | *forward*  				|single| *enabled* | -
 * | *backward*  				|single| *enabled* | -
 * | *head_limit*  				|single| *false@0.0* | specified as "enabled@[limit] with enabled = true,false, with limit >= 0.0
 * | *head_left*  				|single| *enabled* | -
 * | *head_right*  				|single| *enabled* | -
 * | *pitch_limit*  			|single| *false@0.0* | specified as "enabled@[limit] with enabled = true,false, with limit >= 0.0
 * | *pitch_up*  				|single| *enabled* | -
 * | *pitch_down*  				|single| *enabled* | -
 * | *strafe_left*  			|single| *enabled* | -
 * | *strafe_right*  			|single| *enabled* | -
 * | *up*  						|single| *enabled* | -
 * | *down*  					|single| *enabled* | -
 * | *mouse_move*  				|single| *disabled* | -
 * | *mouse_head*  				|single| *disabled* | -
 * | *mouse_pitch*  			|single| *disabled* | -
 * | *speed_key*  				|single| *shift* | -
 * | *inverted_translation*  	|single| *false* | -
 * | *inverted_rotation*		|single| *false* | -
 * | *max_linear_speed*  		|single| 5.0 | -
 * | *max_angular_speed*  		|single| 5.0 | -
 * | *linear_accel*  			|single| 5.0 | -
 * | *angular_accel*  			|single| 5.0 | -
 * | *linear_friction*  		|single| 0.1 | -
 * | *angular_friction*  		|single| 0.1 | -
 * | *stop_threshold*	  		|single| 0.01 | -
 * | *fast_factor*  			|single| 5.0 | -
 * | *sens_x*  					|single| 0.2 | -
 * | *sens_y*  					|single| 0.2 | -
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
	bool enable(); //xxx
	bool disable(); //xxx
	INLINE bool is_enabled() const;
	void update(float dt);//xxx
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
	INLINE void enable_mouse_head(bool enable); //xxx
	INLINE bool is_mouse_head_enabled() const; //xxx
	INLINE void enable_mouse_pitch(bool enable); //xxx
	INLINE bool is_mouse_pitch_enabled() const; //xxx
	INLINE void enable_mouse_move(bool enable); //xxx
	INLINE bool is_mouse_move_enabled() const; //xxx
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
	INLINE void set_fixed_relative_position(bool enable);
	INLINE bool get_fixed_relative_position();
	INLINE void set_inverted_rotation(bool enable);
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
//	///The chased object's node path. //XXX
//	NodePath mChasedNodePath;
	///This NodePath.
	NodePath mThisNP;
//	///The reference object's node path. //XXX
//	NodePath mReferenceNodePath;
	///The reference node path.
	NodePath mReferenceNP;
	///The reference graphic window.
	PT(GraphicsWindow) mWin;
	///Auxiliary node path to track the fixed look at.
	NodePath mFixedLookAtNodePath;
	///Flags.
	bool mStartEnabled, mEnabled, mFixedRelativePosition, mBackward,
	mFixedLookAt, mHoldLookAt;
//	/** XXX
//	 * \name Main parameters.
//	 */
//	///@{
//	///Chased object id.
//	ObjectId mChasedId;
//	///Reference object id.
//	ObjectId mReferenceId;
//	///@}
	///Kinematic parameters.
	float mAbsLookAtDistance, mAbsLookAtHeight, mAbsMaxDistance, mAbsMinDistance,
	mAbsMinHeight, mAbsMaxHeight, mFriction;
	///Positions.
	LPoint3f mChaserPosition, mLookAtPosition;
	///@{
	///Key controls and effective keys.
	bool mHeadLeft, mHeadRight, mPitchUp, mPitchDown;
	bool mHeadLeftKey, mHeadRightKey, mPitchUpKey, mPitchDownKey;
	///@}
	///@{
	///Key control values.
	bool mMouseEnabledH, mMouseEnabledP;
	int mSignOfMouse;
	///@}
	///@{
	/// Sensitivity settings.
	float mSensX, mSensY, mHeadSensX, mHeadSensY;
	int mCentX, mCentY;
	///@}

	inline void do_reset(); //xxx
	void do_initialize(); //xxx
	void do_finalize(); //xxx

	/**
	 * \name Helpers variables/functions.
	 */
	///@{
	void do_enable(); //xxx
	void do_disable(); //xxx
	void do_handle_mouse(); //xxx
	///@}
	/**
	 * \name Actual enabling/disabling.
	 */
	///@{
	void doEnable(); //xxx
	void doDisable();//xxx
	///@}


	/**
	 * \brief Calculates the dynamic position of the chaser.
	 * \see OgreBulletDemos.
	 * @param desiredChaserPos The desired chaser position (wrt reference).
	 * @param currentChaserPos The current chaser position (wrt reference).
	 * @param deltaTime The delta time update.
	 * @return The dynamic chaser position.
	 */
	LPoint3f doGetChaserPos(LPoint3f desiredChaserPos,
			LPoint3f currentChaserPos, float deltaTime);
	/**
	 * \brief Correct the dynamic height of the chaser.
	 * @param newPos The position whose height may be corrected.
	 * @param baseHeight The corrected height cannot be shorter than this.
	 */
	void doCorrectChaserHeight(LPoint3f& newPos, float baseHeight);



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
