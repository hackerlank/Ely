/**
 * \file gameAudioManager.cxx
 *
 * \date 2016-09-30
 * \author consultit
 */

#include "gameAudioManager.h"

#include "p3Sound3d.h"
#include "p3Listener.h"
#include "asyncTaskManager.h"
#include "graphicsWindow.h"
#include "bamFile.h"

/**
 *
 */
GameAudioManager::GameAudioManager(PT(GraphicsWindow) win, int taskSort, const NodePath& root,
		const CollideMask& mask):
		mWin(win),
		mTaskSort(taskSort),
		mReferenceNP(NodePath("ReferenceNode")),
		mRoot(root),
		mMask(mask),
		mCollisionHandler(NULL),
		mPickerRay(NULL),
		mCTrav(NULL),
		mRef(0)
{
	PRINT_DEBUG("GameAudioManager::GameAudioManager: creating the singleton manager.");

	mSound3ds.clear();
	mSound3dsParameterTable.clear();
	mListeners.clear();
	mListenersParameterTable.clear();
	set_parameters_defaults(SOUND3D);
	set_parameters_defaults(LISTENER);
	//
	mUpdateData.clear();
	mUpdateTask.clear();
	//
	if (!mRoot.is_empty())
	{
		mCTrav = new CollisionTraverser();
		mCollisionHandler = new CollisionHandlerQueue();
		mPickerRay = new CollisionRay();
		PT(CollisionNode)pickerNode = new CollisionNode(string("GameAudioManager::pickerNode"));
		pickerNode->add_solid(mPickerRay);
		pickerNode->set_from_collide_mask(mMask);
		pickerNode->set_into_collide_mask(BitMask32::all_off());
		mCTrav->add_collider(mRoot.attach_new_node(pickerNode),
				mCollisionHandler);
	}
}

/**
 *
 */
GameAudioManager::~GameAudioManager()
{
	PRINT_DEBUG("GameAudioManager::~GameAudioManager: destroying the singleton manager.");

	//stop any default update
	stop_default_update();
	{
		//destroy all P3Sound3ds
		PTA(PT(P3Sound3d))::iterator iterN = mSound3ds.begin();
		while (iterN != mSound3ds.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to P3Sound3d to cleanup itself before being destroyed.
			(*iterN)->do_finalize();
			//remove the P3Sound3ds from the inner list (and from the update task)
			iterN = mSound3ds.erase(iterN);
		}

		//destroy all P3Listeners
		PTA(PT(P3Listener))::iterator iterC = mListeners.begin();
		while (iterC != mListeners.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to P3Listener to cleanup itself before being destroyed.
			(*iterC)->do_finalize();
			//remove the P3Listeners from the inner list (and from the update task)
			iterC = mListeners.erase(iterC);
		}
	}
	//clear parameters' tables
	mSound3dsParameterTable.clear();
	mListenersParameterTable.clear();
	//
	delete mCTrav;
}

/**
 * Creates a P3Sound3d with a given (mandatory and not empty) name.
 * Returns a NodePath to the new P3Sound3d,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GameAudioManager::create_sound3d(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(P3Sound3d)newSound3d = new P3Sound3d(name);
	nassertr_always(newSound3d && (!mWin.is_null()), NodePath::fail())

	// set reference node
	newSound3d->mReferenceNP = mReferenceNP;
	// set the reference graphic window.
	newSound3d->mWin = mWin;
	// initialize the new Sound3d
	newSound3d->do_initialize();

	// add the new Sound3d to the inner list (and to the update task)
	mSound3ds.push_back(newSound3d);
	// reparent to reference node and set "this" NodePath
	newSound3d->mThisNP = mReferenceNP.attach_new_node(newSound3d);
	//
	return newSound3d->mThisNP;
}

/**
 * Destroys a P3Sound3d.
 * Returns false on error.
 */
bool GameAudioManager::destroy_sound3d(NodePath sound3dNP)
{
	CONTINUE_IF_ELSE_R(
			sound3dNP.node()->is_of_type(P3Sound3d::get_class_type()),
			false)

	PT(P3Sound3d)sound3d = DCAST(P3Sound3d, sound3dNP.node());
	Sound3dList::iterator iter = find(mSound3ds.begin(),
			mSound3ds.end(), sound3d);
	CONTINUE_IF_ELSE_R(iter != mSound3ds.end(), false)

	// give a chance to P3Sound3d to cleanup itself before being destroyed.
	sound3d->do_finalize();
	// reset the reference graphic window.
	sound3d->mWin.clear();
	//remove the P3Sound3d from the inner list (and from the update task)
	mSound3ds.erase(iter);
	//
	return true;
}

/**
 * Gets an P3Sound3d by index, or NULL on error.
 */
PT(P3Sound3d) GameAudioManager::get_sound3d(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mSound3ds.size()),
			NULL)

	return mSound3ds[index];
}

/**
 * Creates a P3Listener with a given (mandatory and not empty) name.
 * Returns a NodePath to the new P3Listener,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath GameAudioManager::create_listener(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(P3Listener) newListener = new P3Listener(name);
	nassertr_always(newListener && (!mWin.is_null()), NodePath::fail())

	// set reference node
	newListener->mReferenceNP = mReferenceNP;
	// set the reference graphic window.
	newListener->mWin = mWin;
	// initialize the new Listener
	newListener->do_initialize();

	// add the new Listener to the inner list (and to the update task)
	mListeners.push_back(newListener);
	// reparent to reference node and set "this" NodePath
	newListener->mThisNP = mReferenceNP.attach_new_node(newListener);
	//
	return newListener->mThisNP;
}

/**
 * Destroys a P3Listener.
 * Returns false on error.
 */
bool GameAudioManager::destroy_listener(NodePath listenerNP)
{
	CONTINUE_IF_ELSE_R(
			listenerNP.node()->is_of_type(P3Listener::get_class_type()),
			false)

	PT(P3Listener)listener = DCAST(P3Listener, listenerNP.node());
	ListenerList::iterator iter = find(mListeners.begin(),
			mListeners.end(), listener);
	CONTINUE_IF_ELSE_R(iter != mListeners.end(), false)

	// give a chance to P3Listener to cleanup itself before being destroyed.
	listener->do_finalize();
	// reset the reference graphic window.
	listener->mWin.clear();
	//remove the P3Listener from the inner list (and from the update task)
	mListeners.erase(iter);
	//
	return true;
}

/**
 * Gets an P3Listener by index, or NULL on error.
 */
PT(P3Listener) GameAudioManager::get_listener(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mListeners.size()),
			NULL)

	return mListeners[index];
}

/**
 * Sets a multi-valued parameter to a multi-value overwriting the existing one(s).
 */
void GameAudioManager::set_parameter_values(AudioType type, const string& paramName,
		const ValueList<string>& paramValues)
{
	pair<ParameterTableIter, ParameterTableIter> iterRange;

	if (type == SOUND3D)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mSound3dsParameterTable.equal_range(paramName);
		//...and erase them
		mSound3dsParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mSound3dsParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == LISTENER)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mListenersParameterTable.equal_range(paramName);
		//...and erase them
		mListenersParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mListenersParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
}

/**
 * Gets the multiple values of a (actually set) parameter.
 */
ValueList<string> GameAudioManager::get_parameter_values(AudioType type,
		const string& paramName) const
{
	ValueList<string> strList;
	ParameterTableConstIter iter;
	pair<ParameterTableConstIter, ParameterTableConstIter> iterRange;

	if (type == SOUND3D)
	{
		iterRange = mSound3dsParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	if (type == LISTENER)
	{
		iterRange = mListenersParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	//
	return strList;
}

/**
 * Sets a multi/single-valued parameter to a single value overwriting the existing one(s).
 */
void GameAudioManager::set_parameter_value(AudioType type, const string& paramName,
		const string& value)
{
	ValueList<string> valueList;
	valueList.add_value(value);
	set_parameter_values(type, paramName, valueList);
}

/**
 * Gets a single value (i.e. the first one) of a parameter.
 */
string GameAudioManager::get_parameter_value(AudioType type,
		const string& paramName) const
{
	ValueList<string> valueList = get_parameter_values(type, paramName);
	return (valueList.size() != 0 ? valueList[0] : string(""));
}

/**
 * Gets a list of the names of the parameters actually set.
 */
ValueList<string> GameAudioManager::get_parameter_name_list(AudioType type) const
{
	ValueList<string> strList;
	ParameterTableIter iter;
	ParameterTable tempTable;

	if (type == SOUND3D)
	{
		tempTable = mSound3dsParameterTable;
		for (iter = tempTable.begin(); iter != tempTable.end(); ++iter)
		{
			string name = (*iter).first;
			if (!strList.has_value(name))
			{
				strList.add_value(name);
			}
		}
		return strList;
	}
	if (type == LISTENER)
	{
		tempTable = mListenersParameterTable;
		for (iter = tempTable.begin(); iter != tempTable.end(); ++iter)
		{
			string name = (*iter).first;
			if (!strList.has_value(name))
			{
				strList.add_value(name);
			}
		}
		return strList;
	}
	//
	return strList;
}

/**
 * Sets all parameters to their default values (if any).
 * \note: After reading objects from bam files, the objects' creation parameters
 * which reside in the manager, are reset to their default values.
 */
void GameAudioManager::set_parameters_defaults(AudioType type)
{
	if (type == SOUND3D)
	{
		///mSound3dsParameterTable must be the first cleared
		mSound3dsParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mSound3dsParameterTable.insert(ParameterNameValue("enabled", "true"));
		mSound3dsParameterTable.insert(ParameterNameValue("forward", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("backward", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("up", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("down", "enabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("mouse_head", "disabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("mouse_pitch", "disabled"));
		mSound3dsParameterTable.insert(ParameterNameValue("speed_key", "shift"));
		mSound3dsParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
		mSound3dsParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		mSound3dsParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
		mSound3dsParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
		mSound3dsParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
		mSound3dsParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
		mSound3dsParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mSound3dsParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		return;
	}
	if (type == LISTENER)
	{
		///mListenersParameterTable must be the first cleared
		mListenersParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mListenersParameterTable.insert(ParameterNameValue("enabled", "true"));
		mListenersParameterTable.insert(ParameterNameValue("backward", "true"));
		mListenersParameterTable.insert(ParameterNameValue("fixed_relative_position", "true"));
		mListenersParameterTable.insert(ParameterNameValue("friction", "1.0"));
		mListenersParameterTable.insert(ParameterNameValue("fixed_look_at", "true"));
		mListenersParameterTable.insert(ParameterNameValue("mouse_move", "false"));
		mListenersParameterTable.insert(ParameterNameValue("mouse_head", "false"));
		mListenersParameterTable.insert(ParameterNameValue("mouse_pitch", "false"));
		mListenersParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mListenersParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mListenersParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mListenersParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mListenersParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mListenersParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		mListenersParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		return;
	}
}

/**
 * Updates audio objects.
 *
 * Will be called automatically in a task.
 */
AsyncTask::DoneStatus GameAudioManager::update(GenericAsyncTask* task)
{
	float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all P3Sound3ds' update functions, passing delta time
	for (PTA(PT(P3Sound3d))::size_type index = 0;
			index < mSound3ds.size(); ++index)
	{
		mSound3ds[index]->update(dt);
	}
	// call all P3Listeners' update functions, passing delta time
	for (PTA(PT(P3Listener))::size_type index = 0;
			index < mListeners.size(); ++index)
	{
		mListeners[index]->update(dt);
	}
	//
	return AsyncTask::DS_cont;
}


/**
 * Adds a task to repeatedly call audio updates.
 */
void GameAudioManager::start_default_update()
{
	//create the task for updating AI objects
	mUpdateData = new TaskInterface<GameAudioManager>::TaskData(this,
			&GameAudioManager::update);
	mUpdateTask = new GenericAsyncTask(string("GameAudioManager::update"),
			&TaskInterface<GameAudioManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	mUpdateTask->set_sort(mTaskSort);
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
}

/**
 * Removes a task to repeatedly call audio updates.
 */
void GameAudioManager::stop_default_update()
{
	if (mUpdateTask)
	{
		AsyncTaskManager::get_global_ptr()->remove(mUpdateTask);
	}
	//
	mUpdateData.clear();
	mUpdateTask.clear();
}

/**
 * Gets bounding dimensions of a model NodePath.
 * Puts results into the out parameters: modelDims, modelDeltaCenter and returns
 * modelRadius.
 * - modelDims = absolute dimensions of the model
 * - modelCenter + modelDeltaCenter = origin of coordinate system
 * - modelRadius = radius of the containing sphere
 */
float GameAudioManager::get_bounding_dimensions(NodePath modelNP,
		LVecBase3f& modelDims, LVector3f& modelDeltaCenter) const
{
	//get "tight" dimensions of model
	LPoint3f minP, maxP;
	modelNP.calc_tight_bounds(minP, maxP);
	//
	LVecBase3 delta = maxP - minP;
	LVector3f deltaCenter = -(minP + delta / 2.0);
	//
	modelDims.set(abs(delta.get_x()), abs(delta.get_y()), abs(delta.get_z()));
	modelDeltaCenter.set(deltaCenter.get_x(), deltaCenter.get_y(),
			deltaCenter.get_z());
	float modelRadius = max(max(modelDims.get_x(), modelDims.get_y()),
			modelDims.get_z()) / 2.0;
	return modelRadius;
}

/**
 * Throws a ray downward (-z direction default) from rayOrigin.
 * If collisions are found returns a Pair<bool,float> == (true, height),
 * with height equal to the z-value of the first one.
 * If collisions are not found returns a Pair<bool,float> == (false, 0.0).
 */
Pair<bool,float> GameAudioManager::get_collision_height(const LPoint3f& rayOrigin,
		const NodePath& space) const
{
	//traverse downward starting at rayOrigin
	mPickerRay->set_direction(LVecBase3f(0.0, 0.0, -1.0));
	mPickerRay->set_origin(rayOrigin);
	mCTrav->traverse(mRoot);
	if (mCollisionHandler->get_num_entries() > 0)
	{
		mCollisionHandler->sort_entries();
		CollisionEntry *entry0 =
				mCollisionHandler->get_entry(0);
		LPoint3f target = entry0->get_surface_point(space);
		float collisionHeight = target.get_z();
		return Pair<bool,float>(true, collisionHeight);
	}
	//
	return Pair<bool,float>(false, 0.0);
}

/**
 * Writes to a bam file the entire collections of audio objects and related
 * geometries (i.e. models' NodePaths)
 */
bool GameAudioManager::write_to_bam_file(const string& fileName)
{
	string errorReport;
	// write to bam file
	BamFile outBamFile;
	if (outBamFile.open_write(Filename(fileName)))
	{
		cout << "Current system Bam version: "
				<< outBamFile.get_current_major_ver() << "."
				<< outBamFile.get_current_minor_ver() << endl;
		// just write the reference node
		if (!outBamFile.write_object(mReferenceNP.node()))
		{
			errorReport += string("Error writing ") + mReferenceNP.get_name()
					+ string(" node in ") + fileName + string("\n");
		}
		// close the file
		outBamFile.close();
	}
	else
	{
		errorReport += string("\nERROR: cannot open ") + fileName;
	}
	//check
	if (errorReport.empty())
	{
		cout
				<< "SUCCESS: all audio object collections were written to "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

/**
 * Reads from a bam file the entire hierarchy of audio objects and related
 * geometries (i.e. models' NodePaths)
 */
bool GameAudioManager::read_from_bam_file(const string& fileName)
{
	string errorReport;
	//read from bamFile
	BamFile inBamFile;
	if (inBamFile.open_read(Filename(fileName)))
	{
		cout << "Current system Bam version: "
				<< inBamFile.get_current_major_ver() << "."
				<< inBamFile.get_current_minor_ver() << endl;
		cout << "Bam file version: " << inBamFile.get_file_major_ver() << "."
				<< inBamFile.get_file_minor_ver() << endl;
		// just read the reference node
		TypedWritable* reference = inBamFile.read_object();
		if (reference)
		{
			//resolve pointers
			if (!inBamFile.resolve())
			{
				errorReport += string("Error resolving pointers in ") + fileName
						+ string("\n");
			}
		}
		else
		{
			errorReport += string("Error reading ") + fileName + string("\n");
		}
		// close the file
		inBamFile.close();
		// restore reference node
		mReferenceNP = NodePath::any_path(DCAST(PandaNode, reference));
	}
	else
	{
		errorReport += string("\nERROR: cannot open ") + fileName;
	}
	//check
	if (errorReport.empty())
	{
		cout << "SUCCESS: all audio objects were read from "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

//TypedObject semantics: hardcoded
TypeHandle GameAudioManager::_type_handle;
