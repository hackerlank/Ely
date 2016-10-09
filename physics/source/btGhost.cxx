/**
 * \file btGhost.cxx
 *
 * \date 2016-10-09
 * \author consultit
 */

#if !defined(CPPPARSER) && defined(_WIN32)
#include "support_os/pstdint.h"
#endif

#include "btGhost.h"
#include "gamePhysicsManager.h"
#include <cmath>

#ifndef CPPPARSER
#endif //CPPPARSER
#ifdef PYTHON_BUILD
#include <py_panda.h>
extern Dtool_PyTypedObject Dtool_BTGhost;
#endif //PYTHON_BUILD

/**
 *
 */
BTGhost::BTGhost(const string& name) :
		PandaNode(name)
{
	do_reset();
}

/**
 *
 */
BTGhost::~BTGhost()
{
}

/**
 * Initializes the BTGhost with starting settings.
 * \note Internal use only.
 */
void BTGhost::do_initialize()
{
	WPT(GamePhysicsManager)mTmpl = GamePhysicsManager::get_global_ptr();
	//sound files
	plist<string> mSoundFileListParam =
			mTmpl->get_parameter_values(GamePhysicsManager::GHOST,
					string("sound_files"));
	//
	//set sound files
	plist<string>::iterator iter;
	for (iter = mSoundFileListParam.begin(); iter != mSoundFileListParam.end();
			++iter)
	{
		//any "sound_files" string is a "compound" one, i.e. could have the form:
		// "sound_name1@sound_file1:sound_name2@sound_file2:...:sound_nameN@sound_fileN"
		pvector<string> nameFilePairs = parseCompoundString(*iter, ':');
		pvector<string>::const_iterator iterPair;
		for (iterPair = nameFilePairs.begin();
				iterPair != nameFilePairs.end(); ++iterPair)
		{
			//an empty sound_name@sound_file is ignored
			if (not iterPair->empty())
			{
				//get sound name and sound file name
				pvector<string> nameFilePair =
				parseCompoundString(*iterPair, '@');
				//check only if there is a pair
				if (nameFilePair.size() == 2)
				{
					//sound name == nameFilePair[0]
					//sound file name == nameFilePair[1]
					PT(PhysicsSound) sound =
					GamePhysicsManager::GetSingletonPtr()->
					get_physics_manager()->get_sound(nameFilePair[1],
							true).p();
					if (not sound.is_null())
					{
						//an empty ("") sound name is allowed
						mSounds[nameFilePair[0]] =
								make_pair(sound, nameFilePair[1]);
					}
				}
			}
		}
	}
	//after getting initial sounds:
	//min distance
	set_min_distance(STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::GHOST,
							string("min_distance")).c_str(), NULL));
	//max distance
	set_max_distance(STRTOF(mTmpl->get_parameter_value(GamePhysicsManager::GHOST,
							string("max_distance")).c_str(), NULL));
	//static flag
	set_static(mTmpl->get_parameter_value(GamePhysicsManager::GHOST,
					string("static")) == string("true") ? true : false);
	//
#ifdef PYTHON_BUILD
	//Python callback
	this->ref();
	mSelf = DTool_CreatePyInstanceTyped(this, Dtool_BTGhost, true, false,
			get_type_index());
#endif //PYTHON_BUILD
}

/**
 * On destruction cleanup.
 * Gives an BTGhost the ability to do any cleaning is necessary when
 * destroyed.
 * \note Internal use only.
 */
void BTGhost::do_finalize()
{
	//stops every playing sounds
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		if (iter->second.first->status() == PhysicsSound::PLAYING)
		{
			iter->second.first->stop();
		}
	}
	//
#ifdef PYTHON_BUILD
	//Python callback
	Py_DECREF(mSelf);
	Py_XDECREF(mUpdateCallback);
	Py_XDECREF(mUpdateArgList);
#endif //PYTHON_BUILD
	do_reset();
}

/**
 * Adds dynamically a new PhysicsSound to this BTGhost by loading it from a
 * file. Parameters are the sound's name and file name. Returns a negative
 * number on error.
 */
int BTGhost::add_sound(const string& soundName, const string& fileName)
{
	int result = RESULT_ERROR;
	//get the sound from fileName
	PT(PhysicsSound)sound =
	GamePhysicsManager::GetSingletonPtr()->get_physics_manager()->get_sound(fileName,
			true).p();
	if (sound)
	{
		//add sound with soundName
		mSounds[soundName] = make_pair(sound, fileName);
		//set current parameters
		mSounds[soundName].first->set_3d_min_distance(mMinDist);
		mSounds[soundName].first->set_3d_max_distance(mMaxDist);
		if (mStatic)
		{
			mSounds[soundName].first->set_3d_attributes(mPosition.get_x(),
					mPosition.get_y(), mPosition.get_z(), 0.0, 0.0, 0.0);
		}
		//
		result = RESULT_SUCCESS;
	}
	//
	return result;
}

/**
 * Removes dynamically a sound from this BTGhost.
 * Parameters are the sound's name. Returns a negative number on error.
 */
int BTGhost::remove_sound(const string& soundName)
{
	int result = RESULT_ERROR;

	//make mSounds modifications
	size_t removed = mSounds.erase(soundName);
	if (removed == 1)
	{
		// sound is removed
		result = RESULT_SUCCESS;
	}
	//
	return result;
}

/**
 * Sets the minimum distance of all physics sounds added to this BTGhost's
 * (>=0.0).
 * Controls the distance (in units) that this sound begins to fall off. Also
 * affects the rate it falls off. Default is 1.0 CloserFaster, <1.0
 * FartherSlower, >1.0.
 * Don't forget to change this when you change the DistanceFactor
 * (through the PhysicsManager, ie GamePhysicsManager::get_physics_manager()).
 */
void BTGhost::set_min_distance(float dist)
{
	mMinDist = abs(dist);
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second.first->set_3d_min_distance(mMinDist);
	}
}

/**
 * Sets the maximum distance of all physics sounds added to this BTGhost's
 * (>=0.0).
 * Controls the maximum distance (in units) that this sound stops falling off.
 * The sound does not stop at that point, it just doesn't get any quieter. You
 * should rarely need to adjust this. Default is 1000000000.0.
 */
void BTGhost::set_max_distance(float dist)
{
	mMaxDist = abs(dist);
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second.first->set_3d_max_distance(mMaxDist);
	}
}

/**
 * Sets this BTGhost suitable for static objects.
 */
void BTGhost::set_static(bool enable)
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
 * Returns the PhysicsSound given its name, or NULL on error.
 */
PT(PhysicsSound) BTGhost::get_sound_by_name(const string& soundName) const
{
	SoundTable::const_iterator iter = mSounds.find(soundName);
	if (iter == mSounds.end())
	{
		return NULL;
	}
	return iter->second.first;
}

/**
 * Returns the PhysicsSound given its index, or NULL on error.
 */
PT(PhysicsSound) BTGhost::get_sound(int index) const
{
	SoundTable::const_iterator iter;
	unsigned int idx;
	for (idx = 0, iter = mSounds.begin(); idx < mSounds.size(); ++idx, ++iter)
	{
		if (idx == (unsigned int) index)
		{
			break;
		}
	}
	if (idx == mSounds.size())
	{
		return NULL;
	}
	return iter->second.first;
}

/**
 * Actually sets position/velocity for static objects.
 * Sets position/velocity for static objects. The position is that of this
 * BTGhost with respect to reference node. Velocity is zero.
 * \note Internal use only.
 */
void BTGhost::do_set_3d_static_attributes()
{
	mPosition = NodePath::any_path(this).get_pos(mReferenceNP);
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		iter->second.first->set_3d_attributes(mPosition.get_x(),
				mPosition.get_y(), mPosition.get_z(), 0.0, 0.0, 0.0);
	}
}

/**
 * Updates the BTGhost state.
 */
void BTGhost::update(float dt)
{
	RETURN_ON_COND(mStatic,)

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	//get the new position
	//note on threading: this should be an atomic operation
	LPoint3f newPosition = NodePath::any_path(this).get_pos(mReferenceNP);
	//get the velocity (mPosition holds the previous position)
	LVector3f deltaPos = (newPosition - mPosition);
	LVector3f velocity;
	dt > 0.0 ? velocity = deltaPos / dt : velocity = LVector3f::zero();
	//update sounds' velocity and position
	SoundTable::iterator iter;
	for (iter = mSounds.begin(); iter != mSounds.end(); ++iter)
	{
		//note on threading: this should be an atomic operation
		iter->second.first->set_3d_attributes(newPosition.get_x(),
				newPosition.get_y(), newPosition.get_z(), velocity.get_x(),
				velocity.get_y(), velocity.get_z());
	}
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
 * Writes a sensible description of the BTGhost to the indicated output
 * stream.
 */
void BTGhost::output(ostream &out) const
{
	out << get_type() << " " << get_name();
}

#ifdef PYTHON_BUILD
/**
 * Sets the update callback as a python function taking this BTGhost as
 * an argument, or None. On error raises an python exception.
 * \note Python only.
 */
void BTGhost::set_update_callback(PyObject *value)
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
 * Sets the update callback as a c++ function taking this BTGhost as
 * an argument, or NULL.
 * \note C++ only.
 */
void BTGhost::set_update_callback(UPDATECALLBACKFUNC value)
{
	mUpdateCallback = value;
}
#endif //PYTHON_BUILD


//TypedWritable API
/**
 * Tells the BamReader how to create objects of type BTGhost.
 */
void BTGhost::register_with_read_factory()
{
	BamReader::get_factory()->register_factory(get_class_type(), make_from_bam);
}

/**
 * Writes the contents of this object to the datagram for shipping out to a
 * Bam file.
 */
void BTGhost::write_datagram(BamWriter *manager, Datagram &dg)
{
	PandaNode::write_datagram(manager, dg);

	///Name of this BTGhost.
	dg.add_string(get_name());

	///Static flag.
	dg.add_bool(mStatic);

	/// Sounds' characteristics. To be saved first.
	///@{
	dg.add_stdfloat(mMinDist);
	dg.add_stdfloat(mMaxDist);
	mPosition.write_datagram(dg);
	///@}

	///The set of sounds attached to this component.
	dg.add_uint32(mSounds.size());
	{
		SoundTable::iterator iter;
		for (iter = mSounds.begin(); iter != mSounds.end();
				++iter)
		{
			// sound name
			dg.add_string(iter->first);
			// sound file name
			dg.add_string(iter->second.second);
		}
	}

	///The reference node path.
	manager->write_pointer(dg, mReferenceNP.node());
}

/**
 * Receives an array of pointers, one for each time manager->read_pointer()
 * was called in fillin(). Returns the number of pointers processed.
 */
int BTGhost::complete_pointers(TypedWritable **p_list, BamReader *manager)
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
 * type BTGhost is encountered in the Bam file.  It should create the
 * BTGhost and extract its information from the file.
 */
TypedWritable *BTGhost::make_from_bam(const FactoryParams &params)
{
	// continue only if GamePhysicsManager exists
	CONTINUE_IF_ELSE_R(GamePhysicsManager::get_global_ptr(), NULL)

	// create a BTGhost with default parameters' values: they'll be restored later
	GamePhysicsManager::get_global_ptr()->set_parameters_defaults(
			GamePhysicsManager::GHOST);
	BTGhost *node = DCAST(BTGhost,
			GamePhysicsManager::get_global_ptr()->create_sound3d(
					"Sound3d").node());

	DatagramIterator scan;
	BamReader *manager;

	parse_params(params, scan, manager);
	node->fillin(scan, manager);

	return node;
}

/**
 * This internal function is called by make_from_bam to read in all of the
 * relevant data from the BamFile for the new BTGhost.
 */
void BTGhost::fillin(DatagramIterator &scan, BamReader *manager)
{
	PandaNode::fillin(scan, manager);

	///Name of this BTGhost.
	set_name(scan.get_string());

	///Static flag.
	mStatic = scan.get_bool();

	/// Sounds' characteristics. To be saved first.
	///@{
	mMinDist = scan.get_stdfloat();
	mMaxDist = scan.get_stdfloat();
	mPosition.read_datagram(scan);
	///@}

	///The set of sounds attached to this component.
	mSounds.clear();
	unsigned int size = scan.get_uint32();
	for (unsigned int i = 0; i < size; ++i)
	{
		string name = scan.get_string();
		string fileName = scan.get_string();
		// insert into mSounds
		add_sound(name, fileName);
	}

	///The reference node path.
	manager->read_pointer(scan);
}

//TypedObject semantics: hardcoded
TypeHandle BTGhost::_type_handle;
