/**
 * \file Object.cpp
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#include "Game/Object.h"

namespace game
{

//-------------------------------------------------------------------------
//-------------- ObjectTemplate -----------------------------------------------
//-------------------------------------------------------------------------
void ObjectTemplate::addComponent(const ComponentType& type)
{
	if (type.empty())
	{
		LOGW("Can't add an empty component!");
		assert(!type.empty());
		return;
	}

	ComponentTypeListIter iter = std::find(mComponentTypes.begin(),
			mComponentTypes.end(), type);
	if (iter != mComponentTypes.end())
	{
		LOGW(
				"There already is a component of this type: " + type
						+ " in this ObjectTemplate: " + this->getType());
		return;
	}

	mComponentTypes.push_back(type);
}
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//-------------- ObjectFactory ------------------------------------------------
//-------------------------------------------------------------------------
ObjectFactory::~ObjectFactory()
{
	ObjectTemplateIter iter = mObjectTemplateMap.begin();
	for (iter = mObjectTemplateMap.begin(); iter != mObjectTemplateMap.end();
			++iter)
			{
		ObjectTemplate* value = (*iter).second;
		if (value)
			delete value;
	}

	mObjectTemplateMap.clear();

	ComponentTemplateIter iter2 = mComponentsTemplates.begin();
	for (iter2 = mComponentsTemplates.begin();
			iter2 != mComponentsTemplates.end(); ++iter2)
			{
		ComponentTemplate* value = (*iter2).second;
		if (value)
			delete value;
	}
	mComponentsTemplates.clear();

	mSceneManagerTypePerComponentType.clear();
}
//-------------------------------------------------------------------------
bool ObjectFactory::registerObjectTemplate(ObjectTemplate* objectTemplate)
{
	assert(objectTemplate);
	if (objectTemplate == 0)
	{
		LOGW("Can't register an empty template!");
		return false;
	}

	ObjectType type = objectTemplate->getType();

	ObjectTemplateIter it = mObjectTemplateMap.find(type);
	if (it != mObjectTemplateMap.end())
	{
		LOGW(
				"Rejection of a duplicated object template of type: "
						+ (String) objectTemplate->getType());
		return false;
	}

	mObjectTemplateMap[type] = objectTemplate;

	return true;
}
//-------------------------------------------------------------------------
ObjectTemplate* ObjectFactory::getObjectTemplate(const ObjectType& type)
{
	ObjectTemplateIter it = mObjectTemplateMap.find(type);
	if (it == mObjectTemplateMap.end())
		return 0;

	return it->second;
}
//-------------------------------------------------------------------------
const String& ObjectFactory::getSceneManagerFamily(const ComponentType& type)
{
	SceneManagerTypePerComponentTypeIter it =
			mSceneManagerTypePerComponentType.find(type);
	if (it == mSceneManagerTypePerComponentType.end())
		return StringUtil::BLANK;

	return it->second;
}
//-------------------------------------------------------------------------
bool ObjectFactory::registerComponentTemplate(const String& scenemManagerType,
		ComponentTemplate* componentTemplate)
{
	assert(componentTemplate);
	if (0 == componentTemplate)
	{
		LOGW("Can't register an empty template!");
		return false;
	}

	if (scenemManagerType.empty())
	{
		LOGW(
				String("Is this intended? The scene manager type is empty for ")
						+ "the componentTemplate "
						+ componentTemplate->getType());
	}

	ComponentType type = componentTemplate->getType();

	ComponentTemplateIter it = mComponentsTemplates.find(type);
	if (it != mComponentsTemplates.end())
	{
		LOGI(
				"A component template of type '" + type
						+ "' is already registered.");
		delete componentTemplate;
		componentTemplate = 0;
		return false;
	}

	mComponentsTemplates[type] = componentTemplate;
	mSceneManagerTypePerComponentType[type] = scenemManagerType;

	return true;
}
//-------------------------------------------------------------------------
Component* ObjectFactory::createComponent(const ComponentType& type)
{
	ComponentTemplateIter it = mComponentsTemplates.find(type);
	if (it == mComponentsTemplates.end())
		return 0;

	return ((*it).second)->createComponent();
}
//-------------------------------------------------------------------------

}
