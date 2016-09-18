/**
 * \file aiManager.I
 *
 * \date 2016-09-17
 * \author consultit
 */

#ifndef AIMANGER_I_
#define AIMANGER_I_

///AIManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath AIManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void AIManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the reference NodePath for debug drawing.
 * This node should be made sibling of the reference NodePath in the scene.
 */
INLINE NodePath AIManager::get_reference_node_path_debug() const
{
	return mReferenceDebugNP;
}

/**
 * Returns the reference NodePath 2D for debug drawing.
 * This node should be a descendant of a 2D top NodePath of the scene graph (ie
 * aspect2d, render2d, pixel2d).
 */
INLINE NodePath AIManager::get_reference_node_path_debug_2d() const
{
	return mReferenceDebug2DNP;
}

/**
 * Returns the number of OSSteerPlugIns.
 */
INLINE int AIManager::get_num_steer_plug_ins() const
{
	return (int) mSteerPlugIns.size();
}

/**
 * Returns the number of OSSteerVehicles.
 */
INLINE int AIManager::get_num_steer_vehicles() const
{
	return (int) mSteerVehicles.size();
}

/**
 * Returns the number of RNNavMeshes.
 */
INLINE int AIManager::get_num_nav_meshes() const
{
	return (int) mNavMeshes.size();
}

/**
 * Returns the number of RNCrowdAgents.
 */
INLINE int AIManager::get_num_crowd_agents() const
{
	return (int) mCrowdAgents.size();
}

/**
 * Returns the obstacle's unique reference (>0) given its index into the list of
 * all defined obstacles, or a negative number on error.
 */
INLINE int AIManager::get_obstacle(int index) const
{
	pvector<ObstacleAttributes>& obstacleAttrs =
			const_cast<GlobalObstacles&>(mObstacles).second();
	CONTINUE_IF_ELSE_R((index >= 0) && (index < (int) obstacleAttrs.size()),
			AI_ERROR)

	return obstacleAttrs[index].first().get_ref();
}

/**
 * Returns the number of all defined obstacles.
 */
INLINE int AIManager::get_num_obstacles() const
{
	return (int) const_cast<GlobalObstacles&>(mObstacles).second().size();
}

/**
 * Returns a reference to the storage of all obstacles.
 */
inline AIManager::GlobalObstacles& AIManager::get_global_obstacles()
{
	return mObstacles;
}

/**
 * Get singleton pointer.
 */
INLINE AIManager* AIManager::get_global_ptr()
{
	return Singleton<AIManager>::GetSingletonPtr();
}

/**
 * Get the collide mask.
 */
INLINE CollideMask AIManager::get_collide_mask() const
{
	return mMask;
}

/**
 * Get the collision root.
 */
INLINE NodePath AIManager::get_collision_root() const
{
	return mRoot;
}

/**
 * Get the collision traverser.
 */
INLINE CollisionTraverser* AIManager::get_collision_traverser() const
{
	return mCTrav;
}

/**
 * Get the collision handler.
 */
INLINE CollisionHandlerQueue* AIManager::get_collision_handler() const
{
	return mCollisionHandler;
}

/**
 * Get the collision ray.
 */
INLINE CollisionRay* AIManager::get_collision_ray() const
{
	return mPickerRay;
}

/**
 *
 */
inline int AIManager::unique_ref()
{
	return ++mRef;
}

#endif /* AIMANGER_I_ */
