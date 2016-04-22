/**
 * Created on Mar 30, 2016
 *
 * @author: consultit
 */

#include <collisionHandlerQueue.h>
#include <collisionNode.h>
#include <lpoint3.h>
#include <collisionTraverser.h>
#include <collisionRay.h>
#include <geomNode.h>
#include <nodePath.h>
#include <collideMask.h>
#include <windowFramework.h>

/**
 * Panda3D Manual: Clicking on 3D Objects.
 */
class Picker
{
public:
	/**
	 * Setup.
	 */
	Picker(WindowFramework *app, const std::string& event,
			const CollideMask& mask = GeomNode::get_default_collide_mask(),
			void (*handleFunc)(NodePath, CollisionEntry*, void*)=nullptr,
			void* handleFuncArgs = nullptr);
	virtual ~Picker();
	static void pickObject(const Event *e, void *data);

	CollisionTraverser* get_traverser()
	{
		return &cTrav;
	}

	CollisionHandlerQueue* get_handler()
	{
		return collisionHandler;
	}

	CollisionRay* get_ray()
	{
		return pickerRay;
	}

private:
	WindowFramework *app;
	void (*handleFunc)(NodePath, CollisionEntry*, void*);
	void* handleFuncArgs;
	CollisionTraverser cTrav;
	CollisionHandlerQueue* collisionHandler;
	CollisionRay* pickerRay;
};
