/*
 -----------------------------------------------------------------------------
 Filename:    TutorialApplication.cpp
 -----------------------------------------------------------------------------

 This source file is part of the
 ___                 __    __ _ _    _
 /___\__ _ _ __ ___  / / /\ \ (_) | _(_)
 //  // _` | '__/ _ \ \ \/  \/ / | |/ / |
 / \_// (_| | | |  __/  \  /\  /| |   <| |
 \___/ \__, |_|  \___|   \/  \/ |_|_|\_\_|
 |___/
 Tutorial Framework
 http://www.ogre3d.org/tikiwiki/
 -----------------------------------------------------------------------------
 */
#include "TutorialApplication.h"

//-------------------------------------------------------------------------------------
TutorialApplication::TutorialApplication(void)
{
}
//-------------------------------------------------------------------------------------
TutorialApplication::~TutorialApplication(void)
{
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(0.2f, 0.2f, 0.2f));

	Ogre::Light* light = mSceneMgr->createLight("MainLight");
	light->setPosition(20, 80, 50);

	mCamera->setPosition(60, 200, 70);
	mCamera->lookAt(0, 0, 0);

	Ogre::Entity* ent;
	for (int i = 0; i < 6; i++)
	{
		Ogre::SceneNode* headNode =
				mSceneMgr->getRootSceneNode()->createChildSceneNode();
		ent = mSceneMgr->createEntity("head" + Ogre::StringConverter::toString(
				i), "ogrehead.mesh");
		headNode->attachObject(ent);

		Ogre::Radian angle(i + Ogre::Math::TWO_PI / 6);
		headNode->setPosition(75 * Ogre::Math::Cos(angle), 0, 75
				* Ogre::Math::Sin(angle));
	}
	createProjector();
	for (unsigned int i = 0; i < ent->getNumSubEntities(); i++)
	{
		makeMaterialReceiveDecal(ent->getSubEntity(i)->getMaterialName());
	}
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mProjectorNode->rotate(Ogre::Vector3::UNIT_Y, Ogre::Degree(
			evt.timeSinceLastFrame * 10));
	return BaseApplication::frameRenderingQueued(evt);
}

void TutorialApplication::createProjector()
{
	mDecalFrustum = new Ogre::Frustum();
	mProjectorNode = mSceneMgr->getRootSceneNode()->createChildSceneNode(
			"DecalProjectorNode");
	mProjectorNode->attachObject(mDecalFrustum);
	mProjectorNode->setPosition(0, 5, 0);
	mFilterFrustum = new Ogre::Frustum();
	mFilterFrustum->setProjectionType(Ogre::PT_ORTHOGRAPHIC);
	Ogre::SceneNode *filterNode = mProjectorNode->createChildSceneNode(
			"DecalFilterNode");
	filterNode->attachObject(mFilterFrustum);
	filterNode->setOrientation(Ogre::Quaternion(Ogre::Degree(90),
			Ogre::Vector3::UNIT_Y));
}

void TutorialApplication::makeMaterialReceiveDecal(const Ogre::String& matName)
{
	Ogre::MaterialPtr
			mat =
					(Ogre::MaterialPtr) Ogre::MaterialManager::getSingleton().getByName(
							matName);
	Ogre::Pass *pass = mat->getTechnique(0)->createPass();
	pass->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	pass->setDepthBias(1);
	pass->setLightingEnabled(false);
	Ogre::TextureUnitState *texState =
			pass->createTextureUnitState("decal.png");
	texState->setProjectiveTexturing(true, mDecalFrustum);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(Ogre::FO_POINT, Ogre::FO_LINEAR,
			Ogre::FO_NONE);

	texState = pass->createTextureUnitState("decal_filter.png");
	texState->setProjectiveTexturing(true, mFilterFrustum);
	texState->setTextureAddressingMode(Ogre::TextureUnitState::TAM_CLAMP);
	texState->setTextureFiltering(Ogre::TFO_NONE);
}

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
int main(int argc, char *argv[])
#endif
{
	// Create application object
	TutorialApplication app;

	try
	{
		app.go();
	} catch (Ogre::Exception& e)
	{
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
		std::cerr << "An exception has occured: "
				<< e.getFullDescription().c_str() << std::endl;
#endif
	}

	return 0;
}

#ifdef __cplusplus
}
#endif
