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
	//static flag
	set_static(mTmpl->get_parameter_value(GameAudioManager::LISTENER,
					string("static")) == string("true") ? true : false);
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
#ifdef PYTHON_BUILD
	//Python callback
	Py_DECREF(mSelf);
	Py_XDECREF(mUpdateCallback);
	Py_XDECREF(mUpdateArgList);
#endif //PYTHON_BUILD
	do_reset();
}

/**
 * Sets this P3Listener suitable for static objects.
 */
void P3Listener::set_static(bool enable)
{
	//if enabled return
	RETURN_ON_COND(mStatic == enable,)

	mStatic = enable;
	//do actual set
	if (mStatic)
	{
		do_set_3d_static_attributes();
	}
}

/**
 * Actually sets position/velocity for static objects.
 * Sets position/velocity for static objects. The position is that of this
 * P3Listener with respect to reference node. Velocity is zero.
 * \note Internal use only.
 */
void P3Listener::do_set_3d_static_attributes()
{
	mPosition = mThisNP.get_pos(mReferenceNP);
	LVector3f forward = mThisNP.get_relative_vector(mReferenceNP,
			LVector3f::forward());
	LVector3f up = mThisNP.get_relative_vector(mReferenceNP, LVector3f::up());
	GameAudioManager::GetSingletonPtr()->get_audio_manager()->audio_3d_set_listener_attributes(
			mPosition.get_x(), mPosition.get_y(), mPosition.get_z(), 0.0, 0.0,
			0.0, forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
}

/**
 * Updates the P3Listener state.
 */
void P3Listener::update(float dt)
{
	RETURN_ON_COND(mStatic,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	LPoint3f newPosition;
	LVector3f forward, up, deltaPos, velocity;

	//get the new position
	newPosition = mThisNP.get_pos(mReferenceNP);
	forward = mReferenceNP.get_relative_vector(mThisNP,
			LVector3f::forward());
	up = mReferenceNP.get_relative_vector(mThisNP, LVector3f::up());

	//get the velocity (mPosition holds the previous position)
	deltaPos = (newPosition - mPosition);
	dt > 0.0 ? velocity = deltaPos / dt : velocity = LVector3f::zero();
	//update listener velocity and position
	//note on threading: this should be an atomic operation
	GameAudioManager::GetSingletonPtr()->get_audio_manager()->audio_3d_set_listener_attributes(
			newPosition.get_x(), newPosition.get_y(), newPosition.get_z(),
			velocity.get_x(), velocity.get_y(), velocity.get_z(),
			forward.get_x(), forward.get_y(), forward.get_z(), up.get_x(),
			up.get_y(), up.get_z());
	//update current position
	mPosition = newPosition;
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

	///Static flag.
	dg.add_bool(mStatic);

	/// Sounds' characteristics.
	///@{
	mPosition.write_datagram(dg);
	///@}

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
	///The reference node path.
	PT(PandaNode)referenceNPPandaNode = DCAST(PandaNode, p_list[pi++]);
	mReferenceNP = NodePath::any_path(referenceNPPandaNode);

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

	///Static flag.
	mStatic = scan.get_bool();

	/// Sounds' characteristics.
	///@{
	mPosition.read_datagram(scan);
	///@}

	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle P3Listener::_type_handle;
