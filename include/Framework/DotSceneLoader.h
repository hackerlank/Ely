//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef DOT_SCENELOADER_HPP
#define DOT_SCENELOADER_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

// Includes
#include <OgreString.h>
#include <OgreVector3.h>
#include <OgreQuaternion.h>
#include <OgreResourceGroupManager.h>
#include <vector>

#include "rapidxml.h"

//|||||||||||||||||||||||||||||||||||||||||||||||

// Forward declarations
namespace Ogre
{
class SceneManager;
class SceneNode;
class TerrainGroup;
class TerrainGlobalOptions;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

class nodeProperty
{
public:
	Ogre::String nodeName;
	Ogre::String propertyNm;
	Ogre::String valueName;
	Ogre::String typeName;

	nodeProperty(const Ogre::String &node, const Ogre::String &propertyName,
			const Ogre::String &value, const Ogre::String &type) :
			nodeName(node), propertyNm(propertyName), valueName(value), typeName(
					type)
	{
	}
};

//|||||||||||||||||||||||||||||||||||||||||||||||

/**
 * \brief DotScene format loader.
 *
 * This dotscene loader use RapidXML.
 * It also implements a simple Ogre terrain loader.
 * DotScene (aka .scene) is just a standardized XML file format.
 * The .dtd format is in ogreaddons/dotsceneformat.This file format
 * is meant to be used to set up a scene in Ogre. It is meant
 * to be useful for any type of application/game. Editors can export to
 * .scene format, and apps can load the format.
 * DotScene file does not contain any mesh data, texture data, etc.
 * It just contains elements that describe a scene.
 * A simple .scene file example:
 * \code
 * <scene formatVersion="">
 *   <nodes>
 *      <node name="Robot" id="3">
 *         <position x="10.0" y="5" z="10.5" />
 *         <scale x="1" y="1" z="1" />
 *         <entity name="Robot" meshFile="robot.mesh" static="false" />
 *      </node>
 *      <node name="Omni01" id="5">
 *      	<position x="-23" y="49" z="18" />
 *      	<rotation qx="0" qy="0" qz="0" qw="1" />
 *      	<scale x="1" y="1" z="1" />
 *      	<light name="Omni01" type="point" intensity="0.01" contrast="0">
 *      	    <colourDiffuse r="0.4" g="0.4" b="0.5" />
 *      	    <colourSpecular r="0.5" g="0.5" b="0.5" />
 *      	</light>
 *       </node>
 *    </nodes>
 * </scene>
 * \endcode
 * Adding new tags/elements not in the official specification is possible
 * without messing up other .scene parsers.
 */
class DotSceneLoader
{
public:
	Ogre::TerrainGlobalOptions *mTerrainGlobalOptions;

	DotSceneLoader();
	virtual ~DotSceneLoader();

	void parseDotScene(const Ogre::String &SceneName,
			const Ogre::String &groupName, Ogre::SceneManager *yourSceneMgr,
			Ogre::SceneNode *pAttachNode = NULL,
			const Ogre::String &sPrependNode = "");
	Ogre::String getProperty(const Ogre::String &ndNm,
			const Ogre::String &prop);

	Ogre::TerrainGroup* getTerrainGroup()
	{
		return mTerrainGroup;
	}

	std::vector<nodeProperty> nodeProperties;
	std::vector<Ogre::String> staticObjects;
	std::vector<Ogre::String> dynamicObjects;

protected:
	void processScene(rapidxml::xml_node<>* XMLRoot);

	void processNodes(rapidxml::xml_node<>* XMLNode);
	void processExternals(rapidxml::xml_node<>* XMLNode);
	void processEnvironment(rapidxml::xml_node<>* XMLNode);
	void processTerrain(rapidxml::xml_node<>* XMLNode);
	void processTerrainPage(rapidxml::xml_node<>* XMLNode);
	void processBlendmaps(rapidxml::xml_node<>* XMLNode);
	void processUserDataReference(rapidxml::xml_node<>* XMLNode,
			Ogre::SceneNode *pParent = 0);
	void processUserDataReference(rapidxml::xml_node<>* XMLNode,
			Ogre::Entity *pEntity);
	void processOctree(rapidxml::xml_node<>* XMLNode);
	void processLight(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent =
			0);
	void processCamera(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent =
			0);

	void processNode(rapidxml::xml_node<>* XMLNode,
			Ogre::SceneNode *pParent = 0);
	void processLookTarget(rapidxml::xml_node<>* XMLNode,
			Ogre::SceneNode *pParent);
	void processTrackTarget(rapidxml::xml_node<>* XMLNode,
			Ogre::SceneNode *pParent);
	void processEntity(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);
	void processParticleSystem(rapidxml::xml_node<>* XMLNode,
			Ogre::SceneNode *pParent);
	void processBillboardSet(rapidxml::xml_node<>* XMLNode,
			Ogre::SceneNode *pParent);
	void processPlane(rapidxml::xml_node<>* XMLNode, Ogre::SceneNode *pParent);

	void processFog(rapidxml::xml_node<>* XMLNode);
	void processSkyBox(rapidxml::xml_node<>* XMLNode);
	void processSkyDome(rapidxml::xml_node<>* XMLNode);
	void processSkyPlane(rapidxml::xml_node<>* XMLNode);
	void processClipping(rapidxml::xml_node<>* XMLNode);

	void processLightRange(rapidxml::xml_node<>* XMLNode, Ogre::Light *pLight);
	void processLightAttenuation(rapidxml::xml_node<>* XMLNode,
			Ogre::Light *pLight);

	Ogre::String getAttrib(rapidxml::xml_node<>* XMLNode,
			const Ogre::String &parameter,
			const Ogre::String &defaultValue = "");
	Ogre::Real getAttribReal(rapidxml::xml_node<>* XMLNode,
			const Ogre::String &parameter, Ogre::Real defaultValue = 0);
	bool getAttribBool(rapidxml::xml_node<>* XMLNode,
			const Ogre::String &parameter, bool defaultValue = false);

	Ogre::Vector3 parseVector3(rapidxml::xml_node<>* XMLNode);
	Ogre::Quaternion parseQuaternion(rapidxml::xml_node<>* XMLNode);
	Ogre::ColourValue parseColour(rapidxml::xml_node<>* XMLNode);

	Ogre::SceneManager *mSceneMgr;
	Ogre::SceneNode *mAttachNode;
	Ogre::String m_sGroupName;
	Ogre::String m_sPrependNode;
	Ogre::TerrainGroup* mTerrainGroup;
	Ogre::Vector3 mTerrainPosition;
	Ogre::Vector3 mLightDirection;
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||
