/**
 * \file p3Listener.cxx
 *
 * \date 2016-09-30
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "p3Listener.h"
#include "gameAudioManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include <py_panda.h>
extern Dtool_PyTypedObject Dtool_P3Listener;
#endif //PYTHON_BUILD

/**
 *
 */
P3Listener::P3Listener(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
P3Listener::~P3Listener()
{
}

/**
 * Initializes the P3Listener with starting settings.
 * \note Internal use only.
 */
void P3Listener::do_initialize()
{
	WPT(GameAudioManager)mTmpl = GameAudioManager::get_global_ptr();
	//inverted setting (1/-1): not inverted -> 1, inverted -> -1
	mSignOfMouse = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("inverted_rotation")) == string("true") ? -1 : 1);
	//backward setting
	mBackward = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("backward")) == string("true") ?
					true : false);
	//fixed relative position setting
	mFixedRelativePosition = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("fixed_relative_position")) == string("false") ?
					false : true);
	//
	float value;
	//max distance (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("max_distance")).c_str(),
			NULL);
	mAbsMaxDistance = (value >= 0.0 ? value : -value);
	//min distance (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("min_distance")).c_str(),
			NULL);
	mAbsMinDistance = (value >= 0.0 ? value : -value);
	//max height (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("max_height")).c_str(),
			NULL);
	mAbsMaxHeight = (value >= 0.0 ? value : -value);
	//min height (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("min_height")).c_str(),
			NULL);
	mAbsMinHeight = (value >= 0.0 ? value : -value);
	//friction (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("friction")).c_str(), NULL);
	mFriction = (value >= 0.0 ? value : -value);
	//fixed look at: true/false
	mFixedLookAt = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("fixed_look_at")) == string("false") ? false : true);
	//look at distance (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("look_at_distance")).c_str(),
			NULL);
	mAbsLookAtDistance = (value >= 0.0 ? value : -value);
	//look at height (>=0)
	value = STRTOF(
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("look_at_height")).c_str(),
			NULL);
	mAbsLookAtHeight = (value >= 0.0 ? value : -value);
	//mouse movement setting
	mMouseEnabledH = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("mouse_head")) == string("enabled") ? true : false);
	mMouseEnabledP = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("mouse_pitch")) == string("enabled") ? true : false);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("head_left")) == string("enabled") ? true : false);
	//headRight key
	mHeadRightKey = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("head_right")) == string("enabled") ? true : false);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("pitch_up")) == string("enabled") ? true : false);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("pitch_down")) == string("enabled") ? true : false);
	//mouseMove key: enabled/disabled
	mMouseMoveKey = (
			mTmpl->get_parameter_value(GameAudioManager::LISTENER, string("mouse_move"))
			== string("enabled") ? true : false);
	//sens x (>=0)
	value =
			STRTOF(
					mTmpl->get_parameter_value(GameAudioManager::LISTENER,
							string("sens_x")).c_str(),
					NULL);
	mSensX = (value >= 0.0 ? value : -value);
	mHeadSensX = mSensX * 375.0;
	//sens y (>=0)
	value =
			STRTOF(
					mTmpl->get_parameter_value(GameAudioManager::LISTENER,
							string("sens_y")).c_str(),
					NULL);
	mSensY = (value >= 0.0 ? value : -value);
	mHeadSensY = mSensY * 375.0;
	//
	mCentX = mWin->get_properties().get_x_size() / 2;
	mCentY = mWin->get_properties().get_y_size() / 2;
	//enabling setting
	if((
			mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("enabled")) == string("true") ? true : false))
	{
		do_enable();
	}
	//
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_P3Listener, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * On destruction cleanup.
 * Gives an P3Listener the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void P3Listener::do_finalize()
{
	//if enabled: disable
	if (mEnabled)
	{
		//actual disabling
		do_disable();
	}
#ifdef PYTHON_BUILD
	//Python callback
	Py_DECREF(mSelf);
	Py_XDECREF(mUpdateCallback);
	Py_XDECREF(mUpdateArgList);
#endif //PYTHON_BUILD
	do_reset();
	//
	return;
}

/**
 * Enables the P3Listener to perform its task (default: enabled).
 */
bool P3Listener::enable()
{
	//if enabled return or chased node path is empty return
	RETURN_ON_COND(mEnabled || mChasedNP.is_empty(), false)

	//actual enabling
	do_enable();
	//
	return true;
}

/**
 * Enables actually the P3Listener.
 * \note Internal use only.
 */
void P3Listener::do_enable()
{
	mEnabled = true;
	//check if backward located
	float sign = (mBackward ? 1.0 : -1.0);
	//set listener position (wrt chased node)
	mListenerPosition = LPoint3f(0.0, -mAbsMaxDistance * sign,
			mAbsMinHeight * 1.5);
	//set "look at" position (wrt chased node)
	mLookAtPosition = LPoint3f(0.0, mAbsLookAtDistance * sign,
			mAbsLookAtHeight);
	// handle mouse if possible
	if ((!mFixedLookAt) && (mMouseEnabledH || mMouseEnabledP)
			&& (!mMouseMoveKey))
	{
		//we want audio through mouse movements
		//hide mouse cursor
		WindowProperties props = mWin->get_properties();
		props.set_cursor_hidden(true);
		mWin->request_properties(props);
		//reset mouse to start position
		mWin->move_pointer(0, mCentX, mCentY);
		// start handle mouse
		mMouseHandled = true;
	}
	//set fixed look at node
	mFixedLookAtNP = mReferenceNP.attach_new_node("fixedLookAtNP");
}

/**
 * Disables the P3Listener to perform its task (default: enabled).
 */
bool P3Listener::disable()
{
	//if not enabled return
	RETURN_ON_COND((!mEnabled) || mChasedNP.is_empty(), false)

	//actual disabling
	do_disable();
	//
	return true;
}


/**
 * Disables actually the P3Listener.
 * \note Internal use only.
 */
void P3Listener::do_disable()
{
	mEnabled = false;
	//
	mFixedLookAtNP.remove_node();
	//
	// don't handle mouse
	WindowProperties props = mWin->get_properties();
	//show mouse cursor if hidden
	props.set_cursor_hidden(false);
	mWin->request_properties(props);
	// stop handle mouse
	mMouseHandled = false;
}

/**
 * Make mouse handled if possible.
 * \note Internal use only.
 */
void P3Listener::do_handle_mouse()
{
	// handle mouse if possible
	if (mEnabled && (mMouseEnabledH || mMouseEnabledP) && (!mFixedLookAt)
			&& (!mMouseMoveKey))
	{
		//we want audio through mouse movements
		//hide mouse cursor
		WindowProperties props = mWin->get_properties();
		props.set_cursor_hidden(true);
		mWin->request_properties(props);
		//reset mouse to start position
		mWin->move_pointer(0, mCentX, mCentY);
		// start handle mouse
		mMouseHandled = true;
	}
	else
	{
		// don't handle mouse
		WindowProperties props = mWin->get_properties();
		//show mouse cursor if hidden
		props.set_cursor_hidden(false);
		mWin->request_properties(props);
		// stop handle mouse
		mMouseHandled = false;
	}
}

/**
 * Calculates the dynamic position of the listener.
 * \see OgreBulletDemos.
 * - desiredListenerPos: the desired listener position (wrt reference).
 * - currentListenerPos: the current listener position (wrt reference).
 * - deltaTime: the delta time update.
 * Returns the dynamic listener position.
 */
LPoint3f P3Listener::do_get_listener_pos(LPoint3f desiredListenerPos,
		LPoint3f currentListenerPos, float deltaTime)
{
	float kReductFactor = mFriction * deltaTime;
	if (kReductFactor > 1.0)
	{
		kReductFactor = 1.0;
	}
	//calculate difference between desiredListenerPos and currentListenerPos
	LVector3f deltaPos = currentListenerPos - desiredListenerPos;
	//converge deltaPos.lenght toward zero: proportionally to deltaPos.lenght
	if (deltaPos.length_squared() > 0.0)
	{
		deltaPos -= deltaPos * kReductFactor;
	}
	//calculate new position
	LPoint3f newPos = desiredListenerPos + deltaPos;
	//correct min distance
	LPoint3f chasedPos = mReferenceNP.get_relative_point(mChasedNP,
			LPoint3f::zero());
	LVector3f newTargetDir = newPos - chasedPos;
	if (newTargetDir.length() < mAbsMinDistance)
	{
		newTargetDir.normalize();
		newPos = chasedPos + newTargetDir * mAbsMinDistance;
	}
	//
	return newPos;
}

/**
 * Correct the dynamic height of the listener.
 * - newPos: the position whose height may be corrected.
 * - baseHeight: the corrected height cannot be shorter than this.
 */
void P3Listener::do_correct_listener_height(LPoint3f& newPos, float baseHeight)
{
	//correct listener height (not in OgreBulletDemos)
	// get audio manager
	WPT(GameAudioManager)navMeshMgr = GameAudioManager::get_global_ptr();
	// correct panda's Z: set the collision ray origin wrt collision root
	LPoint3f pOrig = navMeshMgr->get_collision_root().get_relative_point(
			mReferenceNP, newPos);
	// get the collision height wrt the reference node path
	Pair<bool,float> gotCollisionZ = navMeshMgr->get_collision_height(pOrig,
			mReferenceNP);
	if (gotCollisionZ.get_first())
	{
		float hitPosZ = max(gotCollisionZ.get_second(), baseHeight);
		if (newPos.get_z() < hitPosZ + mAbsMinHeight)
		{
			newPos.set_z(hitPosZ + mAbsMinHeight);
		}
		else if (newPos.get_z() > hitPosZ + mAbsMaxHeight)
		{
			newPos.set_z(hitPosZ + mAbsMaxHeight);
		}
	}
}

/**
 * Updates the P3Listener state.
 */
void P3Listener::update(float dt)
{
	RETURN_ON_COND(!mEnabled,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//update listener position and orientation (see OgreBulletDemos)
	//position
	LPoint3f currentListenerPos = mReferenceNP.get_relative_point(
			mThisNP, LPoint3f::zero());
	LPoint3f newPos;
	LPoint3f desiredListenerPos;
	if (mFixedRelativePosition)
	{
		//follow chased node from fixed position wrt it
		desiredListenerPos = mReferenceNP.get_relative_point(
				mChasedNP, mListenerPosition);
		newPos = do_get_listener_pos(desiredListenerPos, currentListenerPos, dt);
		LPoint3f currentChasedPos = mReferenceNP.get_relative_point(
				mChasedNP, LPoint3f::zero());
		do_correct_listener_height(newPos, currentChasedPos.get_z());
	}
	else
	{
		//correct position only if distance < min distance or distance > max distance
		LPoint3f currentChasedPos = mReferenceNP.get_relative_point(
				mChasedNP, LPoint3f::zero());
		LVector3f distanceDir = currentListenerPos - currentChasedPos;
		float distance = distanceDir.length();
		if (distance < mAbsMinDistance)
		{
			distanceDir.normalize();
			desiredListenerPos = currentChasedPos + distanceDir * mAbsMinDistance;
			newPos = do_get_listener_pos(desiredListenerPos, currentListenerPos, dt);
			do_correct_listener_height(newPos, currentChasedPos.get_z());
		}
		else if (distance > mAbsMaxDistance)
		{
			distanceDir.normalize();
			desiredListenerPos = currentChasedPos + distanceDir * mAbsMaxDistance;
			newPos = do_get_listener_pos(desiredListenerPos, currentListenerPos, dt);
			do_correct_listener_height(newPos, currentChasedPos.get_z());
		}
		else
		{
			newPos = currentListenerPos;
		}
	}
	//
	mThisNP.set_pos(mReferenceNP, newPos);
	//orientation
	if (mFixedLookAt)
	{
		//look at fixed location
		mThisNP.look_at(mChasedNP, mLookAtPosition,
				LVector3f::up());
	}
	else
	{
		//adjust look at with mouse and/or key events
		//handle mouse
		float deltaH = 0.0, deltaP = 0.0, deltaR = 0.0;
		bool wantRotate = false;
		if (mMouseHandled)
		{
			MouseData md = mWin->get_pointer(0);
			float deltaX = md.get_x() - mCentX;
			float deltaY = md.get_y() - mCentY;

			if (mWin->move_pointer(0, mCentX, mCentY))
			{
				if (mMouseEnabledH && (deltaX != 0.0))
				{
					deltaH -= deltaX * mSensX * mSignOfMouse;
					wantRotate = true;
				}
				if (mMouseEnabledP && (deltaY != 0.0))
				{
					deltaP -= deltaY * mSensY * mSignOfMouse;
					wantRotate = true;
				}
			}
		}

		//handle keys:
		if (mHeadLeft && (! mHeadRight))
		{
			deltaH += mHeadSensX * dt * mSignOfMouse;
			wantRotate = true;
		}
		else if (mHeadRight && (! mHeadLeft))
		{
			deltaH -= mHeadSensX * dt * mSignOfMouse;
			wantRotate = true;
		}
		if (mPitchUp && (! mPitchDown))
		{
			deltaP += mHeadSensY * dt * mSignOfMouse;
			wantRotate = true;
		}
		else if (mPitchDown && (! mPitchUp))
		{
			deltaP -= mHeadSensY * dt * mSignOfMouse;
			wantRotate = true;
		}

		//update look at
		if (wantRotate)
		{
			//want rotate: update to desired look up
			mThisNP.set_hpr(
					mThisNP.get_hpr() + LVecBase3f(deltaH, deltaP, deltaR));
		}
		else if (! mHoldLookAt)
		{
			//don't want rotate: return to look up to fixed location
			mFixedLookAtNP.set_pos(mThisNP.get_pos());
			mFixedLookAtNP.look_at(mChasedNP, mLookAtPosition,
					LVector3f::up());
			LVecBase3f deltaHPR = mThisNP.get_hpr(mFixedLookAtNP);
			float kReductFactor = mFriction * dt;
			if (kReductFactor > 1.0)
			{
				kReductFactor = 1.0;
			}
			if (deltaHPR.length_squared() > 0.0)
			{
				deltaHPR -= deltaHPR * kReductFactor;
			}
			mThisNP.set_hpr(mFixedLookAtNP, deltaHPR);
		}
	}
	//
#ifdef PYTHON_BUILD
	// execute python callback (if any)
	if (mUpdateCallback && (mUpdateCallback != Py_None))
	{
		PyObject *result;
		result = PyObject_CallObject(mUpdateCallback, mUpdateArgList);
		if (result == NULL)
		{
			string errStr = get_name() +
					string(": Error calling callback function");
			PyErr_SetString(PyExc_TypeError, errStr.c_str());
			return;
		}
		Py_DECREF(result);
	}
#else
	// execute c++ callback (if any)
	if (mUpdateCallback)
	{
		mUpdateCallback(this);
	}
#endif //PYTHON_BUILD
}

/**
 * Writes a sensible description of the P3Listener to the indicated output
 * stream.
 */
void P3Listener::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this P3Listener as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void P3Listener::set_update_callback(PyObject *value)
{
	if ((!PyCallable_Check(value)) && (value != Py_None))
	{
		PyErr_SetString(PyExc_TypeError,
				"Error: the argument must be callable or None");
		return;
	}

	if (mUpdateArgList == NULL)
	{
		mUpdateArgList = Py_BuildValue("(O)", mSelf);
		if (mUpdateArgList == NULL)
		{
			return;
		}
	}
	Py_DECREF(mSelf);

	Py_XDECREF(mUpdateCallback);
	Py_INCREF(value);
	mUpdateCallback = value;
}
#else
/**
 * Sets the update callback as a c++ function taking this P3Listener as
 * an argument, or NULL.
 * \note C++ only.
 */
void P3Listener::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD

//TypedWritable API
/**
 * Tells the BamReader how to create objects of type P3Listener.
 */
void P3Listener::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void P3Listener::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this P3Listener.
	dg.add_string(get_name());

	///Enable/disable flag.
	dg.add_bool(mEnabled);

	///Flags.
	///@{
	dg.add_bool(mFixedRelativePosition);
	dg.add_bool(mBackward);
	dg.add_bool(mFixedLookAt);
	dg.add_bool(mHoldLookAt);
	///@}

	///Kinematic parameters.
	///@{
	dg.add_stdfloat(mAbsLookAtDistance);
	dg.add_stdfloat(mAbsLookAtHeight);
	dg.add_stdfloat(mAbsMaxDistance);
	dg.add_stdfloat(mAbsMinDistance);
	dg.add_stdfloat(mAbsMinHeight);
	dg.add_stdfloat(mAbsMaxHeight);
	dg.add_stdfloat(mFriction);
	///@}

	///Positions.
	///@{
	mListenerPosition.write_datagram(dg);
	mLookAtPosition.write_datagram(dg);
	///@}

	///Key audios and effective keys.
	///@{
	dg.add_bool(mHeadLeft);
	dg.add_bool(mHeadRight);
	dg.add_bool(mPitchUp);
	dg.add_bool(mPitchDown);
	dg.add_bool(mHeadLeftKey);
	dg.add_bool(mHeadRightKey);
	dg.add_bool(mPitchUpKey);
	dg.add_bool(mPitchDownKey);
	dg.add_bool(mMouseMoveKey);
	///@}

	///Key audio values.
	///@{
	dg.add_bool(mMouseEnabledH);
	dg.add_bool(mMouseEnabledP);
	dg.add_bool(mMouseHandled);
	dg.add_int8(mSignOfMouse);
	///@}

	/// Sensitivity settings.
	///@{
	dg.add_stdfloat(mSensX);
	dg.add_stdfloat(mSensY);
	dg.add_stdfloat(mHeadSensX);
	dg.add_stdfloat(mHeadSensY);
	///@}

	///The chased object's node path.
	manager->write_pointer(dg, mChasedNP.node());
	///Auxiliary node path to track the fixed look at.
	manager->write_pointer(dg, mFixedLookAtNP.node());
	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int P3Listener::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = PandaNode::complete_pointers(p_list, manager);

	/// Pointers
	PT(PandaNode)savedPandaNode;
	///The chased object's node path.
	savedPandaNode = DCAST(PandaNode, p_list[pi++]);
	mChasedNP = NodePath::any_path(savedPandaNode);
	///Auxiliary node path to track the fixed look at.
	savedPandaNode = DCAST(PandaNode, p_list[pi++]);
	mFixedLookAtNP = NodePath::any_path(savedPandaNode);
	///The reference node path.
	savedPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(savedPandaNode);

	return pi;
}

/**
 * This function is called by the BamReader's factory when a new object of
 * type P3Listener is encountered in the Bam file.  It should create the
 * P3Listener and extract its information from the file.
 */
TypedWritable *P3Listener::make_from_bam(const FactoryParams &params)
{
	// continue only if GameAudioManager exists
	CONTINUE_IF_ELSE_R(GameAudioManager::get_global_ptr(), NULL)

	// create a P3Listener with default parameters' values: they'll be restored later
	GameAudioManager::get_global_ptr()->set_parameters_defaults(
			GameAudioManager::LISTENER);
	P3Listener *node = DCAST(P3Listener,
			GameAudioManager::get_global_ptr()->create_listener(
					"P3Listener").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new P3Listener.
 */
void P3Listener::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this P3Listener.
	set_name(scan.get_string());

	///Enable/disable flag.
	mEnabled = scan.get_bool();

	///Flags.
	///@{
	mFixedRelativePosition = scan.get_bool();
	mBackward = scan.get_bool();
	mFixedLookAt = scan.get_bool();
	mHoldLookAt = scan.get_bool();
	///@}

	///Kinematic parameters.
	///@{
	mAbsLookAtDistance = scan.get_stdfloat();
	mAbsLookAtHeight = scan.get_stdfloat();
	mAbsMaxDistance = scan.get_stdfloat();
	mAbsMinDistance = scan.get_stdfloat();
	mAbsMinHeight = scan.get_stdfloat();
	mAbsMaxHeight = scan.get_stdfloat();
	mFriction = scan.get_stdfloat();
	///@}

	///Positions.
	///@{
	mListenerPosition.read_datagram(scan);
	mLookAtPosition.read_datagram(scan);
	///@}

	///Key audios and effective keys.
	///@{
	mHeadLeft = scan.get_bool();
	mHeadRight = scan.get_bool();
	mPitchUp = scan.get_bool();
	mPitchDown = scan.get_bool();
	mHeadLeftKey = scan.get_bool();
	mHeadRightKey = scan.get_bool();
	mPitchUpKey = scan.get_bool();
	mPitchDownKey = scan.get_bool();
	mMouseMoveKey = scan.get_bool();
	///@}

	///Key audio values.
	///@{
	mMouseEnabledH = scan.get_bool();
	mMouseEnabledP = scan.get_bool();
	mMouseHandled = scan.get_bool();
	mSignOfMouse = scan.get_int8();
	///@}

	/// Sensitivity settings.
	///@{
	mSensX = scan.get_stdfloat();
	mSensY = scan.get_stdfloat();
	mHeadSensX = scan.get_stdfloat();
	mHeadSensY = scan.get_stdfloat();
	///@}

	///The chased object's node path.
	manager->read_pointer(scan);
	///Auxiliary node path to track the fixed look at.
	manager->read_pointer(scan);
	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle P3Listener::_type_handle;