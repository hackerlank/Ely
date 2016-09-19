/**
 * \file p3Driver.cxx
 *
 * \date 2016-09-18
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "p3Driver.h"
#include "controlManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include <py_panda.h>
extern Dtool_PyTypedObject Dtool_Driver;
#endif //PYTHON_BUILD

/**
 *
 */
P3Driver::P3Driver(const string& name) :
		PandaNode(name)
{
	do_reset();
}
P3Driver::P3Driver(PandaFramework* framework, const NodePath& ownerObjectNP,
		int taskSort)
{
	mWin = framework->get_window(0)->get_graphics_window();
	mOwnerObjectNP = ownerObjectNP;
	mTaskSort = taskSort;
	do_reset();
	do_initialize();
}

/**
 *
 */
P3Driver::~P3Driver()
{
}
P3Driver::~P3Driver()
{
	do_finalize();
	do_reset();
	mOwnerObjectNP.clear();
	mWin = nullptr;
}



/**
 * Enables the debug drawing.
 * A camera node path should be passed as argument.
 */
/**
 * Disables the debug drawing.
 */
/**
 * Enables/disables debugging.
 * Returns a negative value on error.
 */
/**
 * Writes the P3Driver's (static) debug drawing projected to the x,y plane
 * into a (square) texture, given the world scene, a GraphicsOutput and the
 * size.
 * Output will be a size x size texture, written to the "fileName" file into
 * current directory and the event "debug_drawing_texture_ready" (with the
 * texture as parameter) is thrown when all is ready.
 */
/**
 * Draws static geometry.
 * \note Internal use only.
 */
/**
 * Drawing to texture one shot task.
 * \note Internal use only.
 */
/**
 * Sets the type of proximity database:
 * - BruteForceProximityDatabase
 * - LQProximityDatabase (default).
 * \note PEDESTRIAN, BOID P3Driver only.
 */
/**
 * Returns the type of proximity database
 * - BruteForceProximityDatabase
 * - LQProximityDatabase (default)
 * or a negative value on error.
 * \note PEDESTRIAN, BOID P3Driver only.
 */
/**
 * Sets the world center point.
 * \note BOID P3Driver only.
 */
/**
 * Returns the world center point.
 * Returns LPoint3f::zero() on error.
 * \note BOID P3Driver only.
 */
/**
 * Sets the world radius.
 * \note BOID P3Driver only.
 */
/**
 * Returns the world radius, or a negative value on error.
 * \note BOID P3Driver only.
 */
/**
 * Adds a player (OSSteerVehicle) to one of two teams: teamA (= true) or teamB
 * (= false).
 * Returns a negative value on error.
 * \note SOCCER P3Driver only.
 */
/**
 * Removes a player (OSSteerVehicle) from his/her current team: teamA or teamB.
 * Returns a negative value on error.
 * \note SOCCER P3Driver only.
 */
/**
 * Returns the two extreme points, with respect to reference node, of a playing
 * field.
 * \note SOCCER P3Driver only.
 */
/**
 * Sets a playing field, given two extreme points with respect to reference
 * node and the goal fraction.
 * \note The field will be planar and axis aligned (xy plane), and placed at
 * medium z-height of the two points; the goal fraction is specified with
 * respect to the field's y-dimension. By default a field with dimensions 40x20
 * and placed at (0,0,0) is created, and minimum field's dimensions are 40x20
 * anyway.
 * \note SOCCER P3Driver only.
 */
/**
 * Returns the goal fraction, with respect to the field's y-dimension, of a
 * playing field, or a negative value on error.
 * \note SOCCER P3Driver only.
 */
/**
 * Returns the current score of TEAM_A, or a negative value on error.
 * \note SOCCER P3Driver only.
 */
/**
 * Returns the current score of TEAM_B, or a negative value on error.
 * \note SOCCER P3Driver only.
 */
/**
 * Sets home base center.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Returns home base center.
 * Returns LPoint3f::zero() on error.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Sets home base radius.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Returns home base radius, or a negative value on error.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Sets braking rate (>=0).
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Returns braking rate (>=0), or a negative value on error.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Sets avoidance predict time min.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Returns avoidance predict time min, or a negative value on error.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Sets avoidance predict time max.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Returns avoidance predict time max, or a negative value on error.
 * \note CAPTURE_THE_FLAG P3Driver only.
 */
/**
 * Makes the map based on the defined pathway and on its resolution.
 * \note The map is square, and it is divided into resolution x resolution
 * square elements
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Returns the map center.
 * Returns LPoint3f::zero() on error.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Returns the map dimension, or a negative value on error.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Returns the map resolution, or a negative value on error.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Sets the use of path fences on the map.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Returns the use of path fences on the map, or a negative value on error.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Sets the steering mode on the map: path follow or wander steering.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Returns the steering mode on the map, or a negative value on error.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Sets the default prediction type on the map (curved or linear): each newly
 * added OSSteerVehicle will use it by default.
 * \note Also the type of prediction of the already added OSSteerVehicle(s) will
 * be replaced by this one.
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Returns the default prediction type on the map.
 * \note
 * \note MAP_DRIVE P3Driver only.
 */
/**
 * Sets steering speed (>=0).
 * \note LOW_SPEED_TURN P3Driver only.
 */
/**
 * Returns steering speed (>=0), or a negative value on error.
 * \note LOW_SPEED_TURN P3Driver only.
 */
/**
 * Sets the P3Driver type.
 * \note P3Driver's type can only be changed if there are no attached
 * OSSteerVehicle(s).
 */
/**
 * Creates actually the steer plug-in.
 * \note Internal use only.
 */
/**
 * Builds the pathway.
 * \note Internal use only.
 */
/**
 * Adds the initial set of obstacles.
 * \note Obstacles' NodePaths are searched as descendants of the reference node
 * (and directly reparented to it if necessary).
 * \note Internal use only.
 */
/**
 * Adds a OSSteerVehicle to this P3Driver (ie to the underlying OpenSteer
 * management mechanism).
 * Returns a negative value on error.
 */
/**
 * Removes a OSSteerVehicle from this P3Driver (ie from the OpenSteer
 * handling mechanism).
 * Returns a negative value on error.
 */
/**
 * Checks if an OSSteerVehicle could be handled by this P3Driver.
 * \note The check is done by effective type comparison.
 */
/**
 * Sets the pathway of this P3Driver.
 * \note pointList and radiusList should have the same number of elements; in
 * any case, the number of segments is equal to the number of points if the
 * cycle is closed otherwise to the number of points - 1.
 * \note PEDESTRIAN P3Driver supports currently only single radius
 * pathway.
 */
/**
 * Adds an obstacle given a non empty NodePath (objectNP), which will correspond
 * to the underlying OpenSteer obstacle, and is directly reparented to the
 * reference node. In this case parameters are extracted from the NodePath.
 * objectNP, type and seenFromState parameters must be always specified.\n
 * Returns the obstacle's unique reference (>0), or a negative value on error.
 */
/**
 * Adds actually the obstacle.
 * \note Internal use only.
 */
/**
 * Removes an obstacle given its unique ref (>0).\n
 * Returns the NodePath (possibly empty) that was associated to the underlying
 * obstacle just removed, otherwise an empty NodePath with the ET_fail error
 * type set on error.
 * \note Obstacle will be removed only if it was added by this P3Driver.
 */
/**
 * Updates the P3Driver static geometry if needed.
 * \note It is called on pathway and/or obstacles changes.
 * \note Internal use only.
 */



/**
 * Initializes the P3Driver with starting settings.
 * \note Internal use only.
 */
void P3Driver::do_initialize()
{
	WPT(ControlManager)mTmpl = ControlManager::get_global_ptr();
	//inverted setting (1/-1): not inverted -> 1, inverted -> -1
	mSignOfTranslation = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("inverted_translation"))
					== string("true") ? -1 : 1);
	mSignOfMouse = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("inverted_rotation"))
					== string("true") ? -1 : 1);
	//head limit: enabled@[limit]; limit >= 0.0
	pvector<string> paramValuesStr = parseCompoundString(mTmpl->get_parameter_value(ControlManager::DRIVER, string("head_limit")), '@');
	if (paramValuesStr.size() >= 2)
	{
		//enabled
		mHeadLimitEnabled = (
				paramValuesStr[0] == string("true") ? true : false);
		float value;
		//limit
		value = STRTOF(paramValuesStr[1].c_str(), NULL);
		value >= 0.0 ? mHLimit = value : mHLimit = -value;
	}
	//pitch limit: enabled@[limit]; limit >= 0.0
	paramValuesStr = parseCompoundString(
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("pitch_limit")), '@');
	if (paramValuesStr.size() >= 2)
	{
		//enabled
		mPitchLimitEnabled = (
				paramValuesStr[0] == string("true") ? true : false);
		float value;
		//limit
		value = STRTOF(paramValuesStr[1].c_str(), NULL);
		value >= 0.0 ? mPLimit = value : mPLimit = -value;
	}
	//mouse movement setting
	mMouseEnabledH = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("mouse_enabled_h"))
					== string("true") ? true : false);
	mMouseEnabledP = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("mouse_enabled_p"))
					== string("true") ? true : false);
	//key events setting
	//backward key
	mBackwardKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("backward"))
					== string("disabled") ? false : true);
	//down key
	mDownKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("down")) == string("disabled") ?
					false : true);
	//forward key
	mForwardKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("forward"))
					== string("disabled") ? false : true);
	//strafeLeft key
	mStrafeLeftKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("strafe_left"))
					== string("disabled") ? false : true);
	//strafeRight key
	mStrafeRightKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("strafe_right"))
					== string("disabled") ? false : true);
	//headLeft key
	mHeadLeftKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("head_left"))
					== string("disabled") ? false : true);
	//headRight key
	mHeadRightKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("head_right"))
					== string("disabled") ? false : true);
	//pitchUp key
	mPitchUpKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("pitch_up"))
					== string("disabled") ? false : true);
	//pitchDown key
	mPitchDownKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("pitch_down"))
					== string("disabled") ? false : true);
	//up key
	mUpKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("up")) == string("disabled") ?
					false : true);
	//mouseMove key: enabled/disabled
	mMouseMoveKey = (
			mTmpl->get_parameter_value(ControlManager::DRIVER, string("mouse_move"))
					== string("enabled") ? true : false);
	//speedKey
	mSpeedKey = mTmpl->get_parameter_value(ControlManager::DRIVER, string("speed_key"));
	if ((mSpeedKey != string("control")	and (mSpeedKey != string("alt"))))
	{
		mSpeedKey = string("shift");
	}
	//
	float value, absValue;
	//max linear speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("max_linear_speed")).c_str(),
	NULL);
	absValue = (value >= 0.0 ? value : -value);
	mMaxSpeedXYZ = LVecBase3f(absValue, absValue, absValue);
	mMaxSpeedSquaredXYZ = LVector3f(mMaxSpeedXYZ.get_x() * mMaxSpeedXYZ.get_x(),
			mMaxSpeedXYZ.get_y() * mMaxSpeedXYZ.get_y(),
			mMaxSpeedXYZ.get_z() * mMaxSpeedXYZ.get_z());
	//max angular speed (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("max_angular_speed")).c_str(),
	NULL);
	mMaxSpeedHP = (value >= 0.0 ? value : -value);
	mMaxSpeedSquaredHP = mMaxSpeedHP * mMaxSpeedHP;
	//linear accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("linear_accel")).c_str(), NULL);
	absValue = (value >= 0.0 ? value : -value);
	mAccelXYZ = LVecBase3f(absValue, absValue, absValue);
	//angular accel (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("angular_accel")).c_str(),
	NULL);
	mAccelHP = (value >= 0.0 ? value : -value);
	//reset actual speeds
	mActualSpeedXYZ = LVector3f::zero();
	mActualSpeedH = 0.0;
	mActualSpeedP = 0.0;
	//linear friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("linear_friction")).c_str(),
			NULL);
	mFrictionXYZ = (value >= 0.0 ? value : -value);
	//angular friction (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("angular_friction")).c_str(),
			NULL);
	mFrictionHP = (value >= 0.0 ? value : -value);
	//stop threshold ([0.0, 1.0])
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("stop_threshold")).c_str(),
	NULL);
	mStopThreshold =
			(value >= 0.0 ? value - floor(value) : ceil(value) - value);
	//fast factor (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("fast_factor")).c_str(),
	NULL);
	mFastFactor = (value >= 0.0 ? value : -value);
	//sens x (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("sens_x")).c_str(),
	NULL);
	mSensX = (value >= 0.0 ? value : -value);
	//sens_y (>=0)
	value = STRTOF(mTmpl->get_parameter_value(ControlManager::DRIVER, string("sens_y")).c_str(),
	NULL);
	mSensY = (value >= 0.0 ? value : -value);
	//
	mCentX = mWin->get_properties().get_x_size() / 2;
	mCentY = mWin->get_properties().get_y_size() / 2;
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_Driver, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * On destruction cleanup.
 * Gives an P3Driver the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void P3Driver::onRemoveFromSceneCleanup()
{
	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);
}
void P3Driver::onRemoveFromObjectCleanup()
{
	//see disable
	if (mEnabled and (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey))
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
	//
	reset();
}
void P3Driver::do_finalize()
{
	//disable debug drawing if enabled
	disable_debug_drawing();
	//remove all local obstacles from the global
	OpenSteer::ObstacleGroup::iterator iterLocal;
	AIManager::GlobalObstacles& globalObstacles =
			AIManager::get_global_ptr()->get_global_obstacles();
	for (iterLocal = mLocalObstacles.first().begin();
			iterLocal != mLocalObstacles.first().end(); ++iterLocal)
	{
		if(	globalObstacles.first().size()
						!= globalObstacles.second().size())
		{
			//VERY BAD!
			abort();
		}

		//find in global obstacles and remove it
		//1: remove the OpenSteer obstacle's pointer from the global list
		OpenSteer::ObstacleGroup::iterator iterO = find(
				globalObstacles.first().begin(), globalObstacles.first().end(),
				(*iterLocal));
		globalObstacles.first().erase(iterO);
		//2: remove the obstacle's attributes from the global list
		//NOTE: the i-th obstacle has pointer and attributes placed into the
		//i-th places of their respective lists.
		unsigned int pointerIdx = iterO - globalObstacles.first().begin();
		pvector<AIManager::ObstacleAttributes>::iterator iterA =
				globalObstacles.second().begin() + pointerIdx;
		/*for (iterA = globalObstacles.second().begin();
				iterA != globalObstacles.second().end(); ++iterA)
		{
			if ((*iterA).first().get_obstacle() == (*iterLocal))
			{
				break;
			}
		}*/
		globalObstacles.second().erase(iterA);
		//3: deallocate the OpenSteer obstacle
		delete *iterLocal;
	}
	//remove all added OSSteerVehicle(s) (if any) from update
	PTA(PT(OSSteerVehicle))::const_iterator iter;
	for (iter = mSteerVehicles.begin(); iter != mSteerVehicles.end(); ++iter)
	{
		//set steerVehicle reference to null
		(*iter)->mSteerPlugIn.clear();
		//do remove from real update list
		static_cast<ossup::PlugIn*>(mPlugIn)->removeVehicle(
				&(*iter)->get_abstract_vehicle());
	}
	//close the steer plug in
	mPlugIn->close();
	//delete the steer plug in
	delete mPlugIn;
#ifdef PYTHON_BUILD
	//Python callback
	Py_DECREF(mSelf);
	Py_XDECREF(mUpdateCallback);
	Py_XDECREF(mUpdateArgList);
#endif //PYTHON_BUILD
	do_reset();
}

void P3Driver::do_finalize()
{
	//if enabled: disable
	if (mEnabled)
	{
		//actual disabling
		do_disable();
	}
	//
	return;
}



P3Driver::Result P3Driver::enable()
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//if enabled return
	RETURN_ON_COND(mEnabled, Result::ERROR)

	//actual ebnabling
	doEnable();
	//
	return Result::OK;
}

void P3Driver::doEnable()
{
	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(true);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
		//reset mouse to start position
		win->move_pointer(0, mCentX, mCentY);
	}
	//
	mEnabled = true;
	//register event callbacks if any
	registerEventCallbacks();

	//add to the control manager update
	GameControlManager::GetSingletonPtr()->addToControlUpdate(this);
}

bool P3Driver::enable()
{
	//if enabled return
	RETURN_ON_COND(mEnabled, false)

	//actual enabling
	do_enable();
	//
	return true;
}

void P3Driver::do_enable()
{
	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we want control through mouse movements
		//hide mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(true);
		mWin->request_properties(props);
		//reset mouse to start position
		mWin->move_pointer(0, mCentX, mCentY);
	}
	//
	mEnabled = true;

	//Add mUpdateTask to the active queue.
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
	}
}

P3Driver::Result P3Driver::disable()
{
	{
		//lock (guard) the mutex
		HOLD_REMUTEX(mMutex)

		//if disabling return
		RETURN_ON_ASYNC_COND(mDisabling, Result::DRIVER_DISABLING)

		//if not enabled return
		RETURN_ON_COND(not mEnabled, Result::ERROR)

#ifdef ELY_THREAD
		mDisabling = true;
#endif
	}

	//remove from control manager update
	GameControlManager::GetSingletonPtr()->removeFromControlUpdate(this);

	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	//return if destroying
	RETURN_ON_ASYNC_COND(mDestroying, Result::DESTROYING)

	//actual disabling
	doDisable();
	//
	return Result::OK;
}

void P3Driver::doDisable()
{
	//unregister event callbacks if any
	unregisterEventCallbacks();

	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		SMARTPTR(GraphicsWindow)win =
		mTmpl->windowFramework()->get_graphics_window();
		win->request_properties(props);
	}
	//
#ifdef ELY_THREAD
	mDisabling = false;
#endif
	//
	mEnabled = false;
}

bool P3Driver::disable()
{
	//if not enabled return
	RETURN_ON_COND(not mEnabled, false)

	//actual disabling
	do_disable();
	//
	return true;
}

void P3Driver::do_disable()
{
	if (mMouseEnabledH or mMouseEnabledP or mMouseMoveKey)
	{
		//we have control through mouse movements
		//show mouse cursor
		WindowProperties props;
		props.set_cursor_hidden(false);
		mWin->request_properties(props);
	}
	//
	mEnabled = false;

	//Remove mUpdateTask from the active queue.
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
}


/**
 * Updates the underlying OpenSteer plug-in.
 * It allows the added OSSteerVehicle(s) to perform their "steering behaviors".
 */

void P3Driver::update(void* data)
{
	//lock (guard) the mutex
	HOLD_REMUTEX(mMutex)

	float dt = *(reinterpret_cast<float*>(data));

	NodePath ownerNodePath = mOwnerObject->getNodePath();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//handle mouse
	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
	{
		GraphicsWindow* win = mTmpl->windowFramework()->get_graphics_window();
		MouseData md = win->get_pointer(0);
		float deltaX = md.get_x() - mCentX;
		float deltaY = md.get_y() - mCentY;

		if (win->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH and (deltaX != 0.0))
			{
				ownerNodePath.set_h(
						ownerNodePath.get_h()
						- deltaX * mSensX * mSignOfMouse);
			}
			if (mMouseEnabledP and (deltaY != 0.0))
			{
				ownerNodePath.set_p(
						ownerNodePath.get_p()
						- deltaY * mSensY * mSignOfMouse);
			}
		}
		//if mMouseMoveKey is true we are controlling mouse movements
		//so we need to reset mMouseMove to false
		if (mMouseMoveKey)
		{
			mMouseMove = false;
		}
	}
	//update position/orientation
	ownerNodePath.set_y(ownerNodePath,
			mActualSpeedXYZ.get_y() * dt * mSignOfTranslation);
	ownerNodePath.set_x(ownerNodePath,
			mActualSpeedXYZ.get_x() * dt * mSignOfTranslation);
	ownerNodePath.set_z(ownerNodePath,
			mActualSpeedXYZ.get_z() * dt);
	//head
	if (mHeadLimitEnabled)
	{
		float head = ownerNodePath.get_h()
				+ mActualSpeedH * dt * mSignOfMouse;
		if (head > mHLimit)
		{
			head = mHLimit;
		}
		else if(head < -mHLimit)
		{
			head = -mHLimit;
		}
		ownerNodePath.set_h(head);
	}
	else
	{
		ownerNodePath.set_h(ownerNodePath.get_h()
				+ mActualSpeedH * dt * mSignOfMouse);
	}
	//pitch
	if (mPitchLimitEnabled)
	{
		float pitch = ownerNodePath.get_p()
						+ mActualSpeedP * dt * mSignOfMouse;
		if (pitch > mPLimit)
		{
			pitch = mPLimit;
		}
		else if(pitch < -mPLimit)
		{
			pitch = -mPLimit;
		}
		ownerNodePath.set_p(pitch);
	}
	else
	{
		ownerNodePath.set_p(ownerNodePath.get_p()
				+ mActualSpeedP * dt * mSignOfMouse);
	}

	//update speeds
	float kLinearReductFactor = mFrictionXYZ * dt;
	if (kLinearReductFactor > 1.0)
	{
		kLinearReductFactor = 1.0;
	}
	//y axis
	if (mForward and (not mBackward))
	{
		if(mAccelXYZ.get_y() != 0)
		{
			//accelerate
			mActualSpeedXYZ.set_y(
				mActualSpeedXYZ.get_y() - mAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() < -mMaxSpeedXYZ.get_y())
			{
				//limit speed
				mActualSpeedXYZ.set_y(-mMaxSpeedXYZ.get_y());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_y(-mMaxSpeedXYZ.get_y());
		}
	}
	else if (mBackward and (not mForward))
	{
		if(mAccelXYZ.get_y() != 0)
		{
			//accelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() + mAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() > mMaxSpeedXYZ.get_y())
			{
				//limit speed
				mActualSpeedXYZ.set_y(mMaxSpeedXYZ.get_y());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_y(mMaxSpeedXYZ.get_y());
		}
	}
	else if (mActualSpeedXYZ.get_y() != 0.0)
	{
		if (mActualSpeedXYZ.get_y() * mActualSpeedXYZ.get_y() <
				mMaxSpeedSquaredXYZ.get_y() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_y(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() * (1 - kLinearReductFactor));
		}
	}
	//x axis
	if (mStrafeLeft and (not mStrafeRight))
	{
		if(mAccelXYZ.get_x() != 0)
		{
			//accelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() + mAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() > mMaxSpeedXYZ.get_x())
			{
				//limit speed
				mActualSpeedXYZ.set_x(mMaxSpeedXYZ.get_x());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_x(mMaxSpeedXYZ.get_x());
		}
	}
	else if (mStrafeRight and (not mStrafeLeft))
	{
		if(mAccelXYZ.get_x() != 0)
		{
			//accelerate
			mActualSpeedXYZ.set_x(
				mActualSpeedXYZ.get_x() - mAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() < -mMaxSpeedXYZ.get_x())
			{
				//limit speed
				mActualSpeedXYZ.set_x(-mMaxSpeedXYZ.get_x());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_x(-mMaxSpeedXYZ.get_y());
		}
	}
	else if (mActualSpeedXYZ.get_x() != 0.0)
	{
		if (mActualSpeedXYZ.get_x() * mActualSpeedXYZ.get_x() <
				mMaxSpeedSquaredXYZ.get_x() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_x(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() * (1 - kLinearReductFactor));
		}
	}
	//z axis
	if (mUp and (not mDown))
	{
		if(mAccelXYZ.get_z() != 0)
		{
			//accelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() + mAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() > mMaxSpeedXYZ.get_z())
			{
				//limit speed
				mActualSpeedXYZ.set_z(mMaxSpeedXYZ.get_z());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_z(mMaxSpeedXYZ.get_z());
		}
	}
	else if (mDown and (not mUp))
	{
		if(mAccelXYZ.get_z() != 0)
		{
			//accelerate
			mActualSpeedXYZ.set_z(
				mActualSpeedXYZ.get_z() - mAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() < -mMaxSpeedXYZ.get_z())
			{
				//limit speed
				mActualSpeedXYZ.set_z(-mMaxSpeedXYZ.get_z());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_z(-mMaxSpeedXYZ.get_z());
		}
	}
	else if (mActualSpeedXYZ.get_z() != 0.0)
	{
		if (mActualSpeedXYZ.get_z() * mActualSpeedXYZ.get_z() <
				mMaxSpeedSquaredXYZ.get_z() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_z(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() * (1 - kLinearReductFactor));
		}
	}
	//rotation h
	if (mHeadLeft and (not mHeadRight))
	{
		if(mAccelHP != 0)
		{
			//accelerate
			mActualSpeedH += mAccelHP * dt;
			if (mActualSpeedH > mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedH = mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = mMaxSpeedHP;
		}
	}
	else if (mHeadRight and (not mHeadLeft))
	{
		if(mAccelHP != 0)
		{
			//accelerate
			mActualSpeedH -= mAccelHP * dt;
			if (mActualSpeedH < -mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedH = -mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = -mMaxSpeedHP;
		}
	}
	else if (mActualSpeedH != 0.0)
	{
		if (mActualSpeedH * mActualSpeedH <
				mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedH = 0.0;
		}
		else
		{
			//decelerate
			float kAngularReductFactor = mFrictionHP * dt;
			if (kAngularReductFactor > 1.0)
			{
				kAngularReductFactor = 1.0;
			}
			mActualSpeedH = mActualSpeedH * (1 - kAngularReductFactor);
		}
	}
	//rotation p
	if (mPitchUp and (not mPitchDown))
	{
		if(mAccelHP != 0)
		{
			//accelerate
			mActualSpeedP += mAccelHP * dt;
			if (mActualSpeedP > mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedP = mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedP = mMaxSpeedHP;
		}
	}
	else if (mPitchDown and (not mPitchUp))
	{
		if(mAccelHP != 0)
		{
			//accelerate
			mActualSpeedP -= mAccelHP * dt;
			if (mActualSpeedP < -mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedP = -mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedP = -mMaxSpeedHP;
		}
	}
	else if (mActualSpeedP != 0.0)
	{
		if (mActualSpeedP * mActualSpeedP <
				mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedP = 0.0;
		}
		else
		{
			//decelerate
			float kAngularReductFactor = mFrictionHP * dt;
			if (kAngularReductFactor > 1.0)
			{
				kAngularReductFactor = 1.0;
			}
			mActualSpeedP = mActualSpeedP * (1 - kAngularReductFactor);
		}
	}
}
void P3Driver::update(float dt)
{
	//currentTime
	mCurrentTime += dt;

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

#ifdef ELY_DEBUG
	{
		if (mEnableDebugDrawUpdate && mDrawer3d && mDrawer2d)
		{
			//set enableAnnotation
			ossup::enableAnnotation = true;

			//drawers' initializations
			mDrawer3d->initialize();
			mDrawer2d->initialize();

			/// invoke PlugIn's Update method
			mPlugIn->update(mCurrentTime, dt);

			// invoke selected PlugIn's Redraw method
			mPlugIn->redraw(mCurrentTime, dt);

			// draw any annotation queued up during selected PlugIn's Update method
			OpenSteer::drawAllDeferredLines();
			OpenSteer::drawAllDeferredCirclesOrDisks();

			//drawers' finalizations
			mDrawer3d->finalize();
			mDrawer2d->finalize();
		}
		else
		{
			//clear enableAnnotation
			ossup::enableAnnotation = false;
#endif //ELY_DEBUG

			/// invoke PlugIn's Update method
			mPlugIn->update(mCurrentTime, dt);

#ifdef ELY_DEBUG
		}
	}
#endif //ELY_DEBUG
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

AsyncTask::DoneStatus P3Driver::update(GenericAsyncTask* task)
{
	float dt = ClockObject::get_global_clock()->get_dt();

	//handle mouse
	if (mMouseMove and (mMouseEnabledH or mMouseEnabledP))
	{
		MouseData md = mWin->get_pointer(0);
		float deltaX = md.get_x() - mCentX;
		float deltaY = md.get_y() - mCentY;

		if (mWin->move_pointer(0, mCentX, mCentY))
		{
			if (mMouseEnabledH and (deltaX != 0.0))
			{
				mOwnerObjectNP.set_h(
						mOwnerObjectNP.get_h() - deltaX * mSensX * mSignOfMouse);
			}
			if (mMouseEnabledP and (deltaY != 0.0))
			{
				mOwnerObjectNP.set_p(
						mOwnerObjectNP.get_p() - deltaY * mSensY * mSignOfMouse);
			}
		}
		//if mMouseMoveKey is true we are controlling mouse movements
		//so we need to reset mMouseMove to false
		if (mMouseMoveKey)
		{
			mMouseMove = false;
		}
	}
	//update position/orientation
	mOwnerObjectNP.set_y(mOwnerObjectNP,
			mActualSpeedXYZ.get_y() * dt * mSignOfTranslation);
	mOwnerObjectNP.set_x(mOwnerObjectNP,
			mActualSpeedXYZ.get_x() * dt * mSignOfTranslation);
	mOwnerObjectNP.set_z(mOwnerObjectNP, mActualSpeedXYZ.get_z() * dt);
	//head
	if (mHeadLimitEnabled)
	{
		float head = mOwnerObjectNP.get_h() + mActualSpeedH * dt * mSignOfMouse;
		if (head > mHLimit)
		{
			head = mHLimit;
		}
		else if (head < -mHLimit)
		{
			head = -mHLimit;
		}
		mOwnerObjectNP.set_h(head);
	}
	else
	{
		mOwnerObjectNP.set_h(
				mOwnerObjectNP.get_h() + mActualSpeedH * dt * mSignOfMouse);
	}
	//pitch
	if (mPitchLimitEnabled)
	{
		float pitch = mOwnerObjectNP.get_p() + mActualSpeedP * dt * mSignOfMouse;
		if (pitch > mPLimit)
		{
			pitch = mPLimit;
		}
		else if (pitch < -mPLimit)
		{
			pitch = -mPLimit;
		}
		mOwnerObjectNP.set_p(pitch);
	}
	else
	{
		mOwnerObjectNP.set_p(
				mOwnerObjectNP.get_p() + mActualSpeedP * dt * mSignOfMouse);
	}

	//update speeds
	//y axis
	if (mForward and (not mBackward))
	{
		if (mAccelXYZ.get_y() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() - mAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() < -mMaxSpeedXYZ.get_y())
			{
				//limit speed
				mActualSpeedXYZ.set_y(-mMaxSpeedXYZ.get_y());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_y(-mMaxSpeedXYZ.get_y());
		}
	}
	else if (mBackward and (not mForward))
	{
		if (mAccelXYZ.get_y() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() + mAccelXYZ.get_y() * dt);
			if (mActualSpeedXYZ.get_y() > mMaxSpeedXYZ.get_y())
			{
				//limit speed
				mActualSpeedXYZ.set_y(mMaxSpeedXYZ.get_y());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_y(mMaxSpeedXYZ.get_y());
		}
	}
	else if (mActualSpeedXYZ.get_y() != 0.0)
	{
		if (mActualSpeedXYZ.get_y() * mActualSpeedXYZ.get_y()
				< mMaxSpeedSquaredXYZ.get_y() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_y(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_y(
					mActualSpeedXYZ.get_y() * (1.0 - min(mFrictionXYZ * dt, 1.0f)));
		}
	}
	//x axis
	if (mStrafeLeft and (not mStrafeRight))
	{
		if (mAccelXYZ.get_x() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() + mAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() > mMaxSpeedXYZ.get_x())
			{
				//limit speed
				mActualSpeedXYZ.set_x(mMaxSpeedXYZ.get_x());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_x(mMaxSpeedXYZ.get_x());
		}
	}
	else if (mStrafeRight and (not mStrafeLeft))
	{
		if (mAccelXYZ.get_x() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() - mAccelXYZ.get_x() * dt);
			if (mActualSpeedXYZ.get_x() < -mMaxSpeedXYZ.get_x())
			{
				//limit speed
				mActualSpeedXYZ.set_x(-mMaxSpeedXYZ.get_x());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_x(-mMaxSpeedXYZ.get_y());
		}
	}
	else if (mActualSpeedXYZ.get_x() != 0.0)
	{
		if (mActualSpeedXYZ.get_x() * mActualSpeedXYZ.get_x()
				< mMaxSpeedSquaredXYZ.get_x() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_x(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_x(
					mActualSpeedXYZ.get_x() * (1.0 - min(mFrictionXYZ * dt, 1.0f)));
		}
	}
	//z axis
	if (mUp and (not mDown))
	{
		if (mAccelXYZ.get_z() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() + mAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() > mMaxSpeedXYZ.get_z())
			{
				//limit speed
				mActualSpeedXYZ.set_z(mMaxSpeedXYZ.get_z());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_z(mMaxSpeedXYZ.get_z());
		}
	}
	else if (mDown and (not mUp))
	{
		if (mAccelXYZ.get_z() != 0.0)
		{
			//accelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() - mAccelXYZ.get_z() * dt);
			if (mActualSpeedXYZ.get_z() < -mMaxSpeedXYZ.get_z())
			{
				//limit speed
				mActualSpeedXYZ.set_z(-mMaxSpeedXYZ.get_z());
			}
		}
		else
		{
			//kinematic
			mActualSpeedXYZ.set_z(-mMaxSpeedXYZ.get_z());
		}
	}
	else if (mActualSpeedXYZ.get_z() != 0.0)
	{
		if (mActualSpeedXYZ.get_z() * mActualSpeedXYZ.get_z()
				< mMaxSpeedSquaredXYZ.get_z() * mStopThreshold)
		{
			//stop
			mActualSpeedXYZ.set_z(0.0);
		}
		else
		{
			//decelerate
			mActualSpeedXYZ.set_z(
					mActualSpeedXYZ.get_z() * (1.0 - min(mFrictionXYZ * dt, 1.0f)));
		}
	}
	//rotation h
	if (mHeadLeft and (not mHeadRight))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedH += mAccelHP * dt;
			if (mActualSpeedH > mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedH = mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = mMaxSpeedHP;
		}
	}
	else if (mHeadRight and (not mHeadLeft))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedH -= mAccelHP * dt;
			if (mActualSpeedH < -mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedH = -mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedH = -mMaxSpeedHP;
		}
	}
	else if (mActualSpeedH != 0.0)
	{
		if (mActualSpeedH * mActualSpeedH < mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedH = 0.0;
		}
		else
		{
			//decelerate
            mActualSpeedH = mActualSpeedH * (1.0 - min(mFrictionHP * dt, 1.0f));
		}
	}
	//rotation p
	if (mPitchUp and (not mPitchDown))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedP += mAccelHP * dt;
			if (mActualSpeedP > mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedP = mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedP = mMaxSpeedHP;
		}
	}
	else if (mPitchDown and (not mPitchUp))
	{
		if (mAccelHP != 0.0)
		{
			//accelerate
			mActualSpeedP -= mAccelHP * dt;
			if (mActualSpeedP < -mMaxSpeedHP)
			{
				//limit speed
				mActualSpeedP = -mMaxSpeedHP;
			}
		}
		else
		{
			//kinematic
			mActualSpeedP = -mMaxSpeedHP;
		}
	}
	else if (mActualSpeedP != 0.0)
	{
		if (mActualSpeedP * mActualSpeedP < mMaxSpeedSquaredHP * mStopThreshold)
		{
			//stop
			mActualSpeedP = 0.0;
		}
		else
		{
			//decelerate
			mActualSpeedP = mActualSpeedP * (1.0 - min(mFrictionHP * dt, 1.0f));
		}
	}
	//
	return AsyncTask::DS_cont;
}


/**
 * Writes a sensible description of the P3Driver to the indicated output
 * stream.
 */
void P3Driver::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this P3Driver as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void P3Driver::set_update_callback(PyObject *value)
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
 * Sets the update callback as a c++ function taking this P3Driver as
 * an argument, or NULL.
 * \note C++ only.
 */
void P3Driver::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD


//TypedWritable API
/**
 * Tells the BamReader how to create objects of type P3Driver.
 */
void P3Driver::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void P3Driver::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this P3Driver.
	dg.add_string(get_name());

}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int P3Driver::complete_pointers(TypedWritable **p_list, BamReader *manager)
{
	int pi = PandaNode::complete_pointers(p_list, manager);

	/// Pointers
	///The reference node path.
	PT(PandaNode)referenceNPPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(referenceNPPandaNode);

	return pi;
}

/**
 * Called by the BamReader to perform any final actions needed for setting up
 * the object after all objects have been read and all pointers have been
 * completed.
 */
void P3Driver::finalize(BamReader *manager)
{

	///TYPE SPECIFIC
	nassertv_always(mSerializedDataTmpPtr != NULL)

	// deallocate SerializedDataTmp
	delete mSerializedDataTmpPtr;
	mSerializedDataTmpPtr = NULL;
}

/**
 * Some objects require all of their nested pointers to have been completed
 * before the objects themselves can be completed.  If this is the case,
 * override this method to return true, and be careful with circular
 * references (which would make the object unreadable from a bam file).
 */
bool P3Driver::require_fully_complete() const
{
	return true;
}

/**
 * This function is called by the BamReader's factory when a new object of
 * type P3Driver is encountered in the Bam file.  It should create the
 * P3Driver and extract its information from the file.
 */
TypedWritable *P3Driver::make_from_bam(const FactoryParams &params)
{
	// continue only if AIManager exists
	CONTINUE_IF_ELSE_R(AIManager::get_global_ptr(), NULL)

	// create a P3Driver with default parameters' values: they'll be restored later
	AIManager::get_global_ptr()->set_parameters_defaults(
			AIManager::STEERPLUGIN);
	P3Driver *node = DCAST(P3Driver,
			AIManager::get_global_ptr()->create_steer_plug_in().node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);
	manager->register_finalize(node);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new P3Driver.
 */
void P3Driver::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this P3Driver.
	set_name(scan.get_string());

	///TYPE SPECIFIC
	nassertv_always(mSerializedDataTmpPtr == NULL)

	// allocate SerializedDataTmp

}

//TypedObject semantics: hardcoded
TypeHandle P3Driver::_type_handle;
