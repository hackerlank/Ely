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
	mSceneMgr->destroyQuery(mVolQuery);

	if (mSelectionBox)
	{
		delete mSelectionBox;
	}
}

//-------------------------------------------------------------------------------------
void TutorialApplication::createScene(void)
{
	mSceneMgr->setAmbientLight(Ogre::ColourValue(1.0f, 1.0f, 1.0f));

	for (int i = 0; i < 10; ++i)
	{
		for (int j = 0; j < 10; ++j)
		{
			Ogre::Entity* ent =
					mSceneMgr->createEntity("Robot"
							+ Ogre::StringConverter::toString(i + j * 10),
							"robot.mesh");
			Ogre::SceneNode* node =
					mSceneMgr->getRootSceneNode()->createChildSceneNode(
							Ogre::Vector3(1 * 15, 0, j * 15));
			node->attachObject(ent);
			node->setScale(0.1f, 0.1f, 0.1f);
		}
	}

	mCamera->setPosition(-60, 100, -60);
	mCamera->lookAt(60, 0, 60);

	mGUIRenderer = &CEGUI::OgreRenderer::bootstrapSystem();

	CEGUI::SchemeManager::getSingleton().create(
			(CEGUI::utf8*) "TaharezLook.scheme");
	CEGUI::MouseCursor::getSingleton().setImage("TaharezLook", "MouseArrow");

	mSelectionBox = new SelectionBox("SelectionBox");
	mSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(
			mSelectionBox);

	mVolQuery = mSceneMgr->createPlaneBoundedVolumeQuery(
			Ogre::PlaneBoundedVolumeList());
}

bool TutorialApplication::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	return BaseApplication::frameRenderingQueued(evt);
}

bool TutorialApplication::mouseMoved(const OIS::MouseEvent& arg)
{
	CEGUI::System::getSingleton().injectMouseMove(arg.state.X.rel,
			arg.state.Y.rel);
	if (mSelecting)
	{
		CEGUI::MouseCursor *mouse = CEGUI::MouseCursor::getSingletonPtr();
		mStop.x = mouse->getPosition().d_x / (float) arg.state.width;
		mStop.y = mouse->getPosition().d_y / (float) arg.state.height;

		mSelectionBox->setCorners(mStart, mStop);
	}
	return true;
}

bool TutorialApplication::mousePressed(const OIS::MouseEvent& arg,
		OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left)
	{
		CEGUI::MouseCursor *mouse = CEGUI::MouseCursor::getSingletonPtr();
		mStart.x = mouse->getPosition().d_x / (float) arg.state.width;
		mStart.y = mouse->getPosition().d_y / (float) arg.state.height;
		mStop = mStart;

		mSelecting = true;
		mSelectionBox->clear();
		mSelectionBox->setVisible(true);
	}

	return true;
}

bool TutorialApplication::mouseReleased(const OIS::MouseEvent& arg,
		OIS::MouseButtonID id)
{
	if (id == OIS::MB_Left)
	{
		performSelection(mStart, mStop);
		mSelecting = false;
		mSelectionBox->setVisible(false);
	}

	return true;
}

void TutorialApplication::performSelection(const Ogre::Vector2& first,
		const Ogre::Vector2& second)
{
	float left = first.x, right = second.x, top = first.y, bottom = second.y;

	if (left > right)
		swap(left, right);
	if (top > bottom)
		swap(top, bottom);

	if ((right - left) * (bottom - top) < 0.0001)
		return;

	Ogre::Ray topLeft = mCamera->getCameraToViewportRay(left, top);
	Ogre::Ray topRight = mCamera->getCameraToViewportRay(right, top);
	Ogre::Ray bottomLeft = mCamera->getCameraToViewportRay(left, bottom);
	Ogre::Ray bottomRight = mCamera->getCameraToViewportRay(right, bottom);
	Ogre::PlaneBoundedVolume vol;
	vol.planes.push_back(Ogre::Plane(topLeft.getPoint(3), topRight.getPoint(3),
			bottomRight.getPoint(3))); // front plane
	vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(),
			topLeft.getPoint(100), topRight.getPoint(100))); // top plane
	vol.planes.push_back(Ogre::Plane(topLeft.getOrigin(), bottomLeft.getPoint(
			100), topLeft.getPoint(100))); // left plane
	vol.planes.push_back(Ogre::Plane(bottomLeft.getOrigin(),
			bottomRight.getPoint(100), bottomLeft.getPoint(100))); // bottom plane
	vol.planes.push_back(Ogre::Plane(topRight.getOrigin(), topRight.getPoint(
			100), bottomRight.getPoint(100))); // right plane
	Ogre::PlaneBoundedVolumeList volList;
	volList.push_back(vol);
	mVolQuery->setVolumes(volList);
	Ogre::SceneQueryResult result = mVolQuery->execute();

	deselectObjects();
	Ogre::SceneQueryResultMovableList::iterator iter;
	for (iter = result.movables.begin(); iter != result.movables.end(); ++iter)
		selectObject(*iter);
}

void TutorialApplication::deselectObjects()
{
	std::list<Ogre::MovableObject*>::iterator iter;
	for (iter = mSelected.begin(); iter != mSelected.end(); iter++)
	{
		(*iter)->getParentSceneNode()->showBoundingBox(false);
	}
}

void TutorialApplication::selectObject(Ogre::MovableObject* obj)
{
	obj->getParentSceneNode()->showBoundingBox(true);
	mSelected.push_back(obj);
}

void TutorialApplication::swap(float& x, float& y)
{
	float temp = x;
	x = y;
	y = temp;
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
