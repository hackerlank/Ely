/**
 * \file controlManager.cxx
 *
 * \date 2016-09-18
 * \author consultit
 */

#include "controlManager.h"

#include "p3Driver.h"
//#include "p3Chaser.h" XXX
#include "asyncTaskManager.h"
#include "graphicsWindow.h"
#include "bamFile.h"

/**
 *
 */
ControlManager::ControlManager(PT(GraphicsWindow) win, int taskSort, const NodePath& root,
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
	PRINT_DEBUG("ControlManager::ControlManager: creating the singleton manager.");

	mDrivers.clear();
	mDriversParameterTable.clear();
//	mChasers.clear(); XXX
	mChasersParameterTable.clear();
	set_parameters_defaults(DRIVER);
	set_parameters_defaults(CHASER);
	//
	mUpdateData.clear();
	mUpdateTask.clear();
	//
	if (!mRoot.is_empty())
	{
		mCTrav = new CollisionTraverser();
		mCollisionHandler = new CollisionHandlerQueue();
		mPickerRay = new CollisionRay();
		PT(CollisionNode)pickerNode = new CollisionNode(string("ControlManager::pickerNode"));
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
ControlManager::~ControlManager()
{
	PRINT_DEBUG("ControlManager::~ControlManager: destroying the singleton manager.");

	//stop any default update
	stop_default_update();
	{
		//destroy all P3Drivers
		PTA(PT(P3Driver))::iterator iterN = mDrivers.begin();
		while (iterN != mDrivers.end())
		{
			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
			//give a chance to P3Driver to cleanup itself before being destroyed.
			(*iterN)->do_finalize();
			//remove the P3Drivers from the inner list (and from the update task)
			iterN = mDrivers.erase(iterN);
		}

//		//destroy all P3Chasers XXX
//		PTA(PT(P3Chaser))::iterator iterC = mChasers.begin();
//		while (iterC != mChasers.end())
//		{
//			//\see http://stackoverflow.com/questions/596162/can-you-remove-elements-from-a-stdlist-while-iterating-through-it
//			//give a chance to P3Chaser to cleanup itself before being destroyed.
//			(*iterC)->do_finalize();
//			//remove the P3Chasers from the inner list (and from the update task)
//			iterC = mChasers.erase(iterC);
//		}
	}
	//clear parameters' tables
	mDriversParameterTable.clear();
	mChasersParameterTable.clear();
	//
	delete mCTrav;
}

/**
 * Creates a P3Driver.
 * Returns a NodePath to the new P3Driver,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath ControlManager::create_driver(const string& name)
{
	nassertr_always(!name.empty(), NodePath::fail())

	PT(P3Driver)newDriver = new P3Driver(name);
	nassertr_always(newDriver && (!mWin.is_null()), NodePath::fail())

	// set reference nodes
	newDriver->mReferenceNP = mReferenceNP;
	// set the reference graphic window.
	newDriver->mWin = mWin;
	// initialize the new Driver
	newDriver->do_initialize();

	// add the new Driver to the inner list (and to the update task)
	mDrivers.push_back(newDriver);
	// reparent to reference node and set "this" NodePath
	newDriver->mThisNP = mReferenceNP.attach_new_node(newDriver);
	//
	return newDriver->mThisNP;
}

/**
 * Destroys a P3Driver.
 * Returns false on error.
 */
bool ControlManager::destroy_driver(NodePath driverNP)
{
	CONTINUE_IF_ELSE_R(
			driverNP.node()->is_of_type(P3Driver::get_class_type()),
			false)

	PT(P3Driver)driver = DCAST(P3Driver, driverNP.node());
	DriverList::iterator iter = find(mDrivers.begin(),
			mDrivers.end(), driver);
	CONTINUE_IF_ELSE_R(iter != mDrivers.end(), false)

	// reset the reference graphic window.
	driver->mWin.clear();
	// give a chance to P3Driver to cleanup itself before being destroyed.
	driver->do_finalize();
	//remove the P3Driver from the inner list (and from the update task)
	mDrivers.erase(iter);
	//
	return true;
}

/**
 * Gets an P3Driver by index, or NULL on error.
 */
PT(P3Driver) ControlManager::get_driver(int index) const
{
	nassertr_always((index >= 0) && (index < (int ) mDrivers.size()),
			NULL)

	return mDrivers[index];
}

/**
 * Creates a P3Chaser with a given (mandatory and not empty) name.
 * Returns a NodePath to the new P3Chaser,or an empty NodePath with the
 * ET_fail error type set on error.
 */
NodePath ControlManager::create_chaser(const string& name)
{
//	nassertr_always(! name.empty(), NodePath::fail()) XXX
//
//	PT(P3Chaser) newChaser = new P3Chaser(name);
//	nassertr_always(newChaser, NodePath::fail())
//
//	// set reference node
//	newChaser->mReferenceNP = mReferenceNP;
//	//initialize the new Chaser
//	newChaser->do_initialize();
//
//	//add the new Chaser to the inner list
//	mChasers.push_back(newChaser);
//	// reparent to reference node
	NodePath np/* = mReferenceNP.attach_new_node(newChaser)*/;
	//
	return np;
}

/**
 * Destroys a P3Chaser.
 * Returns false on error.
 */
bool ControlManager::destroy_chaser(NodePath chaserNP)
{
//	CONTINUE_IF_ELSE_R( XXX
//			chaserNP.node()->is_of_type(P3Chaser::get_class_type()),
//			false)
//
//	PT(P3Chaser)chaser = DCAST(P3Chaser, chaserNP.node());
//	ChaserList::iterator iter = find(mChasers.begin(),
//			mChasers.end(), chaser);
//	CONTINUE_IF_ELSE_R(iter != mChasers.end(), false)
//
//	//give a chance to P3Chaser to cleanup itself before being destroyed.
//	chaser->do_finalize();
//	//remove the P3Chaser from the inner list (and from the update task)
//	mChasers.erase(iter);
	//
	return true;
}

/**
 * Gets an P3Chaser by index, or NULL on error.
 */
//PT(P3Chaser) ControlManager::get_chaser(int index) const XXX
//{
//	nassertr_always((index >= 0) && (index < (int ) mChasers.size()),
//			NULL)
//
//	return mChasers[index];
//}

/**
 * Sets a multi-valued parameter to a multi-value overwriting the existing one(s).
 */
void ControlManager::set_parameter_values(ControlType type, const string& paramName,
		const ValueList<string>& paramValues)
{
	pair<ParameterTableIter, ParameterTableIter> iterRange;

	if (type == DRIVER)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mDriversParameterTable.equal_range(paramName);
		//...and erase them
		mDriversParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mDriversParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
	if (type == CHASER)
	{
		//find from mParameterTable the paramName's values to be overwritten
		iterRange = mChasersParameterTable.equal_range(paramName);
		//...and erase them
		mChasersParameterTable.erase(iterRange.first, iterRange.second);
		//insert the new values
		for (int idx = 0; idx < paramValues.size(); ++idx)
		{
			mChasersParameterTable.insert(
					ParameterNameValue(paramName, paramValues[idx]));
		}
		return;
	}
}

/**
 * Gets the multiple values of a (actually set) parameter.
 */
ValueList<string> ControlManager::get_parameter_values(ControlType type,
		const string& paramName) const
{
	ValueList<string> strList;
	ParameterTableConstIter iter;
	pair<ParameterTableConstIter, ParameterTableConstIter> iterRange;

	if (type == DRIVER)
	{
		iterRange = mDriversParameterTable.equal_range(paramName);
		if (iterRange.first != iterRange.second)
		{
			for (iter = iterRange.first; iter != iterRange.second; ++iter)
			{
				strList.add_value(iter->second);
			}
		}
		return strList;
	}
	if (type == CHASER)
	{
		iterRange = mChasersParameterTable.equal_range(paramName);
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
void ControlManager::set_parameter_value(ControlType type, const string& paramName,
		const string& value)
{
	ValueList<string> valueList;
	valueList.add_value(value);
	set_parameter_values(type, paramName, valueList);
}

/**
 * Gets a single value (i.e. the first one) of a parameter.
 */
string ControlManager::get_parameter_value(ControlType type,
		const string& paramName) const
{
	ValueList<string> valueList = get_parameter_values(type, paramName);
	return (valueList.size() != 0 ? valueList[0] : string(""));
}

/**
 * Gets a list of the names of the parameters actually set.
 */
ValueList<string> ControlManager::get_parameter_name_list(ControlType type) const
{
	ValueList<string> strList;
	ParameterTableIter iter;
	ParameterTable tempTable;

	if (type == DRIVER)
	{
		tempTable = mDriversParameterTable;
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
	if (type == CHASER)
	{
		tempTable = mChasersParameterTable;
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
void ControlManager::set_parameters_defaults(ControlType type)
{
	if (type == DRIVER)
	{
		///mDriversParameterTable must be the first cleared
		mDriversParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mDriversParameterTable.insert(ParameterNameValue("enabled", "true"));
		mDriversParameterTable.insert(ParameterNameValue("forward", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("backward", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("head_limit", "false@0.0"));
		mDriversParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("pitch_limit", "false@0.0"));
		mDriversParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("strafe_left", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("strafe_right", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("up", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("down", "enabled"));
		mDriversParameterTable.insert(ParameterNameValue("mouse_move", "disabled"));
		mDriversParameterTable.insert(ParameterNameValue("mouse_head", "disabled"));
		mDriversParameterTable.insert(ParameterNameValue("mouse_pitch", "disabled"));
		mDriversParameterTable.insert(ParameterNameValue("speed_key", "shift"));
		mDriversParameterTable.insert(ParameterNameValue("inverted_translation", "false"));
		mDriversParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		mDriversParameterTable.insert(ParameterNameValue("max_linear_speed", "5.0"));
		mDriversParameterTable.insert(ParameterNameValue("max_angular_speed", "5.0"));
		mDriversParameterTable.insert(ParameterNameValue("linear_accel", "5.0"));
		mDriversParameterTable.insert(ParameterNameValue("angular_accel", "5.0"));
		mDriversParameterTable.insert(ParameterNameValue("linear_friction", "0.1"));
		mDriversParameterTable.insert(ParameterNameValue("angular_friction", "0.1"));
		mDriversParameterTable.insert(ParameterNameValue("stop_threshold", "0.01"));
		mDriversParameterTable.insert(ParameterNameValue("fast_factor", "5.0"));
		mDriversParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mDriversParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		return;
	}
	if (type == CHASER)
	{
		///mChasersParameterTable must be the first cleared
		mChasersParameterTable.clear();
		//sets the (mandatory) parameters to their default values:
		mChasersParameterTable.insert(ParameterNameValue("enabled", "true"));
		mChasersParameterTable.insert(ParameterNameValue("backward", "true"));
		mChasersParameterTable.insert(ParameterNameValue("fixed_relative_position", "true"));
		mChasersParameterTable.insert(ParameterNameValue("friction", "1.0"));
		mChasersParameterTable.insert(ParameterNameValue("fixed_lookat", "true"));
		mChasersParameterTable.insert(ParameterNameValue("mouse_head", "false"));
		mChasersParameterTable.insert(ParameterNameValue("mouse_pitch", "false"));
		mChasersParameterTable.insert(ParameterNameValue("head_left", "enabled"));
		mChasersParameterTable.insert(ParameterNameValue("head_right", "enabled"));
		mChasersParameterTable.insert(ParameterNameValue("pitch_up", "enabled"));
		mChasersParameterTable.insert(ParameterNameValue("pitch_down", "enabled"));
		mChasersParameterTable.insert(ParameterNameValue("sens_x", "0.2"));
		mChasersParameterTable.insert(ParameterNameValue("sens_y", "0.2"));
		mChasersParameterTable.insert(ParameterNameValue("inverted_rotation", "false"));
		return;
	}
}

/**
 * Updates control objects.
 *
 * Will be called automatically in a task.
 */
AsyncTask::DoneStatus ControlManager::update(GenericAsyncTask* task)
{
	float dt = ClockObject::get_global_clock()->get_dt();

#ifdef TESTING
	dt = 0.016666667; //60 fps
#endif

	// call all P3Drivers' update functions, passing delta time
	for (PTA(PT(P3Driver))::size_type index = 0;
			index < mDrivers.size(); ++index)
	{
		mDrivers[index]->update(dt);
	}
//	// call all P3Chasers' update functions, passing delta time XXX
//	for (PTA(PT(P3Chaser))::size_type index = 0;
//			index < mChasers.size(); ++index)
//	{
//		mChasers[index]->update(dt);
//	}
	//
	return AsyncTask::DS_cont;
}


/**
 * Adds a task to repeatedly call control updates.
 */
void ControlManager::start_default_update()
{
	//create the task for updating AI objects
	mUpdateData = new TaskInterface<ControlManager>::TaskData(this,
			&ControlManager::update);
	mUpdateTask = new GenericAsyncTask(string("ControlManager::update"),
			&TaskInterface<ControlManager>::taskFunction,
			reinterpret_cast<void*>(mUpdateData.p()));
	mUpdateTask->set_sort(mTaskSort);
	//Adds mUpdateTask to the active queue.
	AsyncTaskManager::get_global_ptr()->add(mUpdateTask);
}

/**
 * Removes a task to repeatedly call control updates.
 */
void ControlManager::stop_default_update()
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
float ControlManager::get_bounding_dimensions(NodePath modelNP,
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
Pair<bool,float> ControlManager::get_collision_height(const LPoint3f& rayOrigin,
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
 * Writes to a bam file the entire collections of control objects and related
 * geometries (i.e. models' NodePaths)
 */
bool ControlManager::write_to_bam_file(const string& fileName)
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
				<< "SUCCESS: all control object collections were written to "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

/**
 * Reads from a bam file the entire hierarchy of control objects and related
 * geometries (i.e. models' NodePaths)
 */
bool ControlManager::read_from_bam_file(const string& fileName)
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
		cout << "SUCCESS: all control objects were read from "
				<< fileName << endl;
	}
	else
	{
		cerr << errorReport << endl;
	}
	return errorReport.empty();
}

//TypedObject semantics: hardcoded
TypeHandle ControlManager::_type_handle;
