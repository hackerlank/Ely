/**
 * Created on Mar 30, 2016
 *
 * @author: consultit
 */

#include "picker.h"
#include <mouseWatcher.h>
#include <lpoint2.h>
#include <camera.h>
#include <lens.h>
#include <collisionEntry.h>
#include <pandaFramework.h>

Picker::Picker(WindowFramework *app, const std::string& event,
		const CollideMask& mask,
		void (*handleFunc)(NodePath, CollisionEntry*, void*),
		void* handleFuncArgs) :
		app(app), handleFunc(handleFunc), handleFuncArgs(handleFuncArgs)
{
	collisionHandler = new CollisionHandlerQueue;
	pickerRay = new CollisionRay();
	PT(CollisionNode)pickerNode = new CollisionNode("pickerNode");
	pickerNode->add_solid(pickerRay);
	pickerNode->set_from_collide_mask(mask);
	pickerNode->set_into_collide_mask(BitMask32::all_off());
	cTrav.add_collider(app->get_render().attach_new_node(pickerNode),
			collisionHandler);
	//
	app->get_panda_framework()->define_key(event, "Picker event", pickObject,
			(void*)this);
}

Picker::~Picker()
{

}

void Picker::pickObject(const Event *e, void *data)
{
	Picker* THIS = reinterpret_cast<Picker*>(data);
	//get the mouse watcher
	PT(MouseWatcher)mwatcher = DCAST(MouseWatcher, THIS->app->get_mouse().node());
	if (mwatcher->has_mouse())
	{
		// Get to and from pos in camera coordinates
		LPoint2f pMouse = mwatcher->get_mouse();
		//
		LPoint3f pFrom, pTo;
		NodePath mCamera = THIS->app->get_camera_group();
		PT(Lens)mCamLens = DCAST(Camera, mCamera.get_child(0).node())->get_lens();
		if (mCamLens->extrude(pMouse, pFrom, pTo))
		{
			//Transform to global coordinates
			pFrom = THIS->app->get_render().get_relative_point(mCamera,
					pFrom);
			pTo = THIS->app->get_render().get_relative_point(mCamera, pTo);
			LVector3f direction = (pTo - pFrom).normalized();
			THIS->pickerRay->set_origin(pFrom);
			THIS->pickerRay->set_direction(direction);
			THIS->cTrav.traverse(THIS->app->get_render());
			// check collisions
			if (THIS->collisionHandler->get_num_entries() > 0)
			{
				// Get the closest entry
				THIS->collisionHandler->sort_entries();
				PT(CollisionEntry)entry0 = THIS->collisionHandler->get_entry(0);
				std::cout << *entry0 << std::endl;
				NodePath pickedObj = entry0->get_into_node_path();
				if ((not pickedObj.is_empty()) and THIS->handleFunc)
				{
					THIS->handleFunc(pickedObj, entry0.p(),
							THIS->handleFuncArgs);
				}
			}
		}
	}
}
