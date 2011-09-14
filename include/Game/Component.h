/*
 * Component.h
 *
 *  Created on: 14/set/2011
 *      Author: marco
 */

#ifndef COMPONENT_H_
#define COMPONENT_H_

namespace game
{

/**
 * The Component class is the base class from which specialised components
 * are created.
 * Those specialised components are the basic parts composing an object.
 * Each component adds particular behavior and properties to the object.
 *
 * @todo Transform this class into a ProxyComponent through which
 *       the 'true' instanciated component is accessed. This will permit
 *       to have specialised proxy for remote access, ... and allow the
 *       implementation of the flyweight pattern.
 *
 *       class ProxyComponent : public Component, Serialisable
 *
 * @author Steven 'lazalong' GAY, Mehdi Toghianifar
 */
class Component: public Serialisable
{
public:
	enum UpdateRate
	{
		NO_UPDATE = -1,

		/*
		 EVERY_TICK_HIGH_PRIORITY will execute updates, pretick, posttick before
		 medium and low priority.  This is useful when you have a component
		 that relies on another component in the same family to be updated
		 first like a tracking camera needs to be updated after a model is
		 re-positioned and a gui tracking element might need to update after
		 both the camera and model because it relies on both of those being
		 up-to-date.
		 */
		EVERY_TICK_HIGH_PRIORITY, EVERY_TICK, EVERY_TICK_LOW_PRIORITY,

		/** TODO/NOTE: Such comps updates should be DIVIDED between ticks to have a smooth frame rate;
		 In other words a specific percent of every list should be updated in each tick.
		 So note about adding/removing comps to/from that 'broken' lists.
		 EVERY_2_TICKS,
		 EVERY_4_TICKS,
		 EVERY_100MSEC,
		 EVERY_1SEC,*/

		NUM_UPDATE_RATES
	};

private:
	ComponentType mType;
	ComponentType mFamily;
	ObjectId mObjectId; // The object this component is a member of

public:
	inline Component(const ComponentType& type, const ComponentType& family) :
			mType(type), mFamily(family), mObjectId("")
	{
	}
	virtual ~Component()
	{
	}

	/**
	 * Set the object owning this component. Used for callbacks.
	 * @todo Study how to decrease the redundency because each components
	 *       has a reference to the object -> one per object would be better.
	 * @todo Replace by an uint to decrease mem usage
	 */
	void setObjectId(const ObjectId& id)
	{
		mObjectId = id;
	}
	const ObjectId& getObjectId() const
	{
		return mObjectId;
	}

	const ComponentType& getType() const
	{
		return mType;
	}
	const ComponentType& getFamily() const
	{
		return mFamily;
	}

	/// Set the default values TODO rename to _init()
	virtual bool _reset(const MessageList& params,
			SceneManager* sceneManager) = 0;
	/// Destroy the component but DOESN'T delete
	virtual void _destroy(SceneManager* sceneManager) = 0;
	///
	virtual void _activate(bool activate, SceneManager* sceneManager) = 0;
	/**
	 * For example return Component::EVERY_TICK;
	 */
	virtual UpdateRate _getUpdateRate() const = 0;
	/**
	 * This function should be called after the manager for this
	 * component runs tick().  This allows the component to
	 * update based on the tick timing settings for the manager that
	 * owns this component.
	 * @param currentTime The time that was used for tick()
	 */
	virtual void _postTick(double currentTime)
	{
	}
	/**
	 * This function should be called before the manager for this
	 * component runs tick().  This allows the component to
	 * update based on the tick timing settings for the manager that
	 * owns this component.
	 * @param currentTime The time that will be used for tick()
	 */
	virtual void _preTick(double currentTime)
	{
	}
	/**
	 * Each component is updated via the ObjectManager by way of
	 * the ObjectSceneManager.  This means that this function is called
	 * based on the tick timing settings for the ObjectManager and NOT
	 * based on the tick timing settings of the manager that owns this
	 * component.
	 * @note Use _preTick() and _postTick() to perform updates based on
	 * the tick rate of the manager that owns this component.
	 */
	virtual void _update(double deltaTime) = 0;

	/**
	 * Implement Serialisable methods
	 * @note Actually the serialisation methods read/write the whole object.
	 * @todo Only the differences from the object template should be serialised.
	 */
	virtual void write(Serialiser* serialiser) = 0;
	virtual void read(Serialiser* serialiser) = 0;
	virtual bool fixup() = 0;

	/**
	 * This method redirect the messages to the actual methods to be used.
	 *
	 *    switch (message.getType())
	 *    {
	 *        case ObjectMessage::SET_POSITION: _setPositionByMessage( message ); return;
	 *        case ObjectMessage::SET_SCALE: _setScaleByMessage( message ); return;
	 *        etc...
	 *    }
	 */
	virtual void _processMessage(const Message& message) = 0;

private:
	/// Default Constructor. Should not be used.
	Component(); // : mType("Unknown!"), mFamily("Unknown!") {}
};

//----------------------------------------------------------------------------

/**
 * Builder that is used to create Components based on a template.
 *
 * @author Steven 'lazalong' GAY
 */
class OGE_CORE_API ComponentTemplate
{
private:
	ComponentType mType;
	/**
	 * Component Family
	 * @note Not to be confused with the Scene Manager "family" (Graphics, AI, etc).
	 */
	ComponentType mFamily;

public:
	inline ComponentTemplate()
	: mType("Not defined!"), mFamily("Not defined!")
	{}
	virtual ~ComponentTemplate()
	{}

	inline const ComponentType& getType() const
	{	return mType;}
	inline const ComponentType& getFamily() const
	{	return mFamily;}
	inline void setType(const ComponentType& type)
	{	mType = type;}
	inline void setFamily(const ComponentType& type)
	{	mFamily = type;}

	/// Create the component
	virtual Component* createComponent() = 0;
};

} /* namespace game */
#endif /* COMPONENT_H_ */
