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

	Ogre::MaterialPtr mat =
			Ogre::MaterialManager::getSingleton().create("PlaneMat",
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::TextureUnitState* tuisTexture =
			mat->getTechnique(0)->getPass(0)->createTextureUnitState(
					"grass_1024.jpg");

	mPlane = new Ogre::MovablePlane("Plane");
	mPlane->d = 0;
	mPlane->normal = Ogre::Vector3::UNIT_Y;

	Ogre::MeshManager::getSingleton().createPlane("PlaneMesh",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, *mPlane,
			120, 120, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
	mPlaneEnt = mSceneMgr->createEntity("PlaneEntity", "PlaneMesh");
	mPlaneEnt->setMaterialName("PlaneMat");

	mPlaneNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
	mPlaneNode->attachObject(mPlaneEnt);

	Ogre::TexturePtr rtt_texture =
			Ogre::TextureManager::getSingleton().createManual("RttTex",
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
					Ogre::TEX_TYPE_2D, mWindow->getWidth(),
					mWindow->getHeight(), 0, Ogre::PF_R8G8B8,
					Ogre::TU_RENDERTARGET);

	Ogre::RenderTexture *renderTexture =
			rtt_texture->getBuffer()->getRenderTarget();
	renderTexture->addViewport(mCamera);
	renderTexture->getViewport(0)->setClearEveryFrame(true);
	renderTexture->getViewport(0)->setBackgroundColour(Ogre::ColourValue::Blue);
	renderTexture->getViewport(0)->setOverlaysEnabled(false);

	mMiniScreen = new Ogre::Rectangle2D(true);
	mMiniScreen->setCorners(0.5f, 1.0f, 1.0f, 0.5f);
	mMiniScreen->setBoundingBox(Ogre::AxisAlignedBox(-100000.0f
			* Ogre::Vector3::UNIT_SCALE, 100000.0f * Ogre::Vector3::UNIT_SCALE));
	Ogre::SceneNode* miniScreenNode =
			mSceneMgr->getRootSceneNode()->createChildSceneNode(
					"MiniScreenNode");
	miniScreenNode->attachObject(mMiniScreen);
	Ogre::MaterialPtr renderMaterial =
			Ogre::MaterialManager::getSingleton().create("RttMat",
					Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::Technique* matTechnique = renderMaterial->createTechnique();
	matTechnique->createPass();
	renderMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	renderMaterial->getTechnique(0)->getPass(0)->createTextureUnitState(
			"RttTex");
	mMiniScreen->setMaterial("RttMat");

	renderTexture->addListener(this);
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	mPlaneNode->yaw(Ogre::Radian(evt.timeSinceLastFrame));
	return BaseApplication::frameRenderingQueued(evt);
}

void TutorialApplication::preRenderTargetUpdate(
		const Ogre::RenderTargetEvent& evt)
{
	mMiniScreen->setVisible(false);
}

void TutorialApplication::postRenderTargetUpdate(
		const Ogre::RenderTargetEvent& evt)
{
	mMiniScreen->setVisible(true);
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
