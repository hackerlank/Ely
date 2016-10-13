/**
 * \file gamePhysicsManager.I
 *
 * \date 2016-10-09
 * \author consultit
 */

#ifndef GAMEPHYSICSMANGER_I_
#define GAMEPHYSICSMANGER_I_

///GamePhysicsManager inline definitions

/**
 * Returns the reference NodePath.
 */
INLINE NodePath GamePhysicsManager::get_reference_node_path() const
{
	return mReferenceNP;
}

/**
 * Returns the Bullet Debug NodePath.
 */
INLINE NodePath GamePhysicsManager::get_bullet_debug_node_path() const
{
	return mBulletDebugNodePath;
}

/**
 * Sets the reference NodePath.
 * This is usually called after restoring from a bam file.
 */
INLINE void GamePhysicsManager::set_reference_node_path(const NodePath& reference)
{
	mReferenceNP = reference;
}

/**
 * Returns the number of BTRigidBodys.
 */
INLINE int GamePhysicsManager::get_num_rigid_bodies() const
{
	return (int) mRigidBodies.size();
}

///** xxx
// * Returns the number of BTSoftBodys.
// */
//INLINE int GamePhysicsManager::get_num_soft_bodies() const
//{
//	return (int) mSoftBodies.size();
//}
//
///**
// * Returns the number of BTGhosts.
// */
//INLINE int GamePhysicsManager::get_num_ghosts() const
//{
//	return (int) mGhosts.size();
//}

/**
 * Returns the singleton pointer.
 */
INLINE GamePhysicsManager* GamePhysicsManager::get_global_ptr()
{
	return Singleton<GamePhysicsManager>::GetSingletonPtr();
}

/**
 * Returns a reference to the Bullet world.
 */
INLINE PT(BulletWorld) GamePhysicsManager::bulletWorld() const
{
	return mBulletWorld;
}

/**
 * Returns the physic PandaNode given the (underlying) Bullet PandaNode.
 */
INLINE PT(PandaNode/*Component*/) GamePhysicsManager::getPhysicsComponentByPandaNode(
		PT(PandaNode) pandaNode)
{
	return mPhysicsComponentPandaNodeTable[pandaNode];
}

/**
 * Sets the physics PandaNode given the (underlying) Bullet PandaNode as first
 * parameter.
 * If the Bullet PandaNode is set to NULL the physics PandaNode related to it
 * will be erased.
 */
INLINE void GamePhysicsManager::setPhysicsComponentByPandaNode(PT(PandaNode) pandaNode,
		PT(PandaNode/*Component*/) physicsComponent)
{
	if (physicsComponent)
	{
		mPhysicsComponentPandaNodeTable[pandaNode] = physicsComponent;
	}
	else
	{
		mPhysicsComponentPandaNodeTable.erase(pandaNode);
	}
}

INLINE void GamePhysicsManager::enableCollisionNotify(EventThrown event, ThrowEventData eventData)
{
	doEnableCollisionNotify(event, eventData);
}

/**
 * Returns the collide mask.
 */
INLINE CollideMask GamePhysicsManager::get_collide_mask() const
{
	return mMask;
}

/**
 *
 */
inline int GamePhysicsManager::unique_ref()
{
	return ++mRef;
}

#endif /* GAMEPHYSICSMANGER_I_ */
