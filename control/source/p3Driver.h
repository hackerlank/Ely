/**
 * \file p3Driver.h
 *
 * \date 2016-09-18
 * \author consultit
 */

#ifndef P3DRIVER_H_
#define P3DRIVER_H_

#include "control_includes.h"
#include "controlTools.h"
#include "controlManager.h"

#ifndef CPPPARSER
#endif //CPPPARSER

/**
 * \brief Component designed for the control of object movement
 * through keyboard/mouse button events and mouse movement.
 *
 * Movement is, by default, based on acceleration (i.e. "dynamic"):
 * accelerations (and max speeds) can independently set for any (local)
 * direction and for angular movement.\n
 * To obtain a fixed movement (i.e. "kinematic") in any direction or
 * angular, the related acceleration and friction value should be set
 * to zero and one respectively.\n
 * Each basic movement (forward, backward, head_left, head_right etc...)
 * can be enabled/disabled through a corresponding "enabler", which in
 * turn set a control key true or false.\n
 * An event handlers could enable/disable movement calling the "enablers".
 * A task updates the position/orientation of the controlled object
 * based on the value of control keys.\n
 * The component can be enabled/disabled as a whole.\n
 * At configuration level (from xml config file), any "enabler" can be
 * enabled/disabled by setting corresponding configuration key
 * to "enabled"/"disabled".\n
 * Mouse movements tracking are special. Since "mouse move" events are not
 * defined by default (they can by using ButtonThrower::set_move_event(),
 * \see: http://www.panda3d.org/forums/viewtopic.php?t=9326
 * 	http://www.panda3d.org/forums/viewtopic.php?t=6049)),
 * mouse movements are polled by default during the "update" task, that is
 * the corresponding "enabler" is disabled.\n
 * The object HEAD (i.e. YAW) and PITCH control through mouse movements
 * can be enabled/disabled separately and if both are disabled (the default)
 * no control through mouse movements.\n
 * All movements (but up and down) can be inverted (default: not inverted).
 *
 * > **XML Param(s)**:
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
 * | *mouse_enabled_h*  		|single| *false* | -
 * | *mouse_enabled_p*  		|single| *false* | -
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
/**
 * This class represents a "plug-in" of the OpenSteer library.
 *
 * \see http://opensteer.sourceforge.net
 *
 * This PandaNode will create a "steer plug-in".\n
 * Each OSSteerPlugIn object could handle a single pathway and several
 * obstacles.\n
 * An "update" task should call this OSSteerPlugIn's update() method to allow
 * the OSSteerVehicle(s) (simple vehicles), which are added to it, to perform
 * their own steering behaviors.\n
 * \note A OSSteerPlugIn will be reparented to the default reference node on
 * creation (see AIManager).
 *
 * > **OSSteerPlugIn text parameters**:
 * param | type | default | note
 * ------|------|---------|-----
 * | *plugin_type*		|single| *one_turning* | values: one_turning,pedestrian,boid,multiple_pursuit,soccer,capture_the_flag,low_speed_turn,map_drive
 * | *pathway*			|single|"0.0,0.0,0.0:1.0,1.0,1.0$1.0$false" (specified as "p1,py1,pz1:px2,py2,pz2[:...:pxN,pyN,pzN]$r1[:r2:...:rM]$closedCycle" with M,closedCycle=N-1,false,N,true)
 * | *obstacles*  		|multiple| - | each one specified as "objectId1@shape1@seenFromState1[:objectId2@shape2@seenFromState2:...:objectIdN@shapeN@seenFromStateN]"] with shapeX=sphere,box,plane,rectangle and seenFromStateX=outside,inside,both
 *
 * \note parts inside [] are optional.\n
 */
class EXPORT_CLASS P3Driver: public PandaNode
{
PUBLISHED:

	// To avoid interrogatedb warning.
#ifdef CPPPARSER
	virtual ~P3Driver();
#endif //CPPPARSER

	/**
	 * \name DRIVER
	 */
	///@{
	bool enable();
	bool disable();
	INLINE bool is_enabled() const;
	void update(float dt);
	///@}

	/**
	 * \name ENABLE/DISABLE CONTROLS
	 */
	///@{
	INLINE void enable_forward(bool enable);
	INLINE bool is_forward_enabled() const;
	INLINE void enable_backward(bool enable);
	INLINE bool is_backward_enabled() const;
	INLINE void enable_strafe_left(bool enable);
	INLINE bool is_strafe_left_enabled() const;
	INLINE void enable_strafe_right(bool enable);
	INLINE bool is_strafe_right_enabled() const;
	INLINE void enable_up(bool enable);
	INLINE bool is_up_enabled() const;
	INLINE void enable_down(bool enable);
	INLINE bool is_down_enabled() const;
	INLINE void enable_head_left(bool enable);
	INLINE bool is_head_left_enabled() const;
	INLINE void enable_head_right(bool enable);
	INLINE bool is_head_right_enabled() const;
	INLINE void enable_pitch_up(bool enable);
	INLINE bool is_pitch_up_enabled() const;
	INLINE void enable_pitch_down(bool enable);
	INLINE bool is_pitch_down_enabled() const;
	INLINE void enable_mouse_move(bool enable);
	INLINE bool is_mouse_move_enabled() const;
	///@}

	/**
	 * \name PARAMETERS' GETTERS/SETTERS
	 */
	///@{
	INLINE void set_head_limit(bool enabled = false, float hLimit = 0.0);
	INLINE void set_pitch_limit(bool enabled = false, float pLimit = 0.0);
	INLINE void set_max_linear_speed(const LVector3f& linearSpeed);
	INLINE void set_max_angular_speed(float angularSpeed);
	INLINE Pair<LVector3f, float> get_max_speeds() const;
	INLINE void set_linear_accel(const LVector3f& linearAccel);
	INLINE void set_angular_accel(float angularAccel);
	INLINE Pair<LVector3f, float> get_accels() const;
	INLINE void set_linear_friction(float linearFriction);
	INLINE void set_angular_friction(float angularFriction);
	INLINE ValueList<float> get_frictions() const;
	INLINE void set_sens(float sensX, float sensY);
	INLINE ValueList<float> get_sens() const;
	INLINE void set_fast_factor(float factor);
	INLINE float get_fast_factor() const;
	INLINE Pair<LVector3f, ValueList<float> > get_current_speeds() const;
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
	typedef void (*UPDATECALLBACKFUNC)(PT(P3Driver));
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
	friend void unref_delete<P3Driver>(P3Driver*);
	friend class ControlManager;

	P3Driver(const string& name);
	virtual ~P3Driver();

private:
	///This NodePath.
	NodePath mThisNP;
	///The reference node path.
	NodePath mReferenceNP;
	///The reference graphic window.
	PT(GraphicsWindow) mWin;
	///Enable/disable flag.
	bool mEnabled;
	///Key controls and effective keys.
	///@{
	bool mForward, mBackward, mStrafeLeft, mStrafeRight, mUp, mDown, mHeadLeft,
			mHeadRight, mPitchUp, mPitchDown, mMouseMove;
	bool mForwardKey, mBackwardKey, mStrafeLeftKey, mStrafeRightKey, mUpKey,
			mDownKey, mHeadLeftKey, mHeadRightKey, mPitchUpKey, mPitchDownKey,
			mMouseMoveKey;
	string mSpeedKey;
	///@}
	///Key control values.
	///@{
	bool mMouseEnabledH, mMouseEnabledP;
	bool mHeadLimitEnabled, mPitchLimitEnabled;
	float mHLimit, mPLimit;
	int mSignOfTranslation, mSignOfMouse;
	///@}
	/// Sensitivity settings.
	///@{
	float mFastFactor;
	LVecBase3f mActualSpeedXYZ, mMaxSpeedXYZ, mMaxSpeedSquaredXYZ;
	float mActualSpeedH, mActualSpeedP, mMaxSpeedHP, mMaxSpeedSquaredHP;
	LVecBase3f mAccelXYZ;
	float mAccelHP;
	float mFrictionXYZ;
	float mFrictionHP;
	float mStopThreshold;
	float mSensX, mSensY;
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

	/**
	 * \name SERIALIZATION ONLY SETTINGS.
	 */
	///@{
	// temporary storage for serialized data
	struct SerializedDataTmp
	{
	}*mSerializedDataTmpPtr;
	// persistent storage for serialized data
	///@}

	// Explicitly disabled copy constructor and copy assignment operator.
	P3Driver(const P3Driver&);
	P3Driver& operator=(const P3Driver&);

public:
	/**
	 * \name TypedWritable API
	 */
	///@{
	static void register_with_read_factory();
	virtual void write_datagram (BamWriter *manager, Datagram &dg) override;
	virtual int complete_pointers(TypedWritable **p_list, BamReader *manager) override;
	virtual void finalize(BamReader *manager); //XXX
	bool require_fully_complete() const; //XXX
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
		register_type(_type_handle, "P3Driver", PandaNode::get_class_type());
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

INLINE ostream &operator << (ostream &out, const P3Driver & driver);

///inline
#include "p3Driver.I"

#endif /* P3DRIVER_H_ */
