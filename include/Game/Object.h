/**
 * \file Object.h
 *
 * \date 14/set/2011
 * \author Marco Paone
 */

#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "Utilities/Message.h"

namespace game
{
    /// Message types related to game object (setting position, etc).
    struct ObjectMessage
    {
        enum ObjectMsg
        {
            //-------------- General Messages ------------

            /**
             * SET_CUSTOM Used to pass a custom message
             *
             * For example an HydraxComponent could have a strange
             * method (for example setPlanesError(Real)) and no ObjectMsg
             * was defined for it. With SET_CUSTOM you could pass a message
             * such as:
             *      msg.param1 = (String) type
             *      msg.param2 = (Real) param
             * Of course the developer needs to implement a custom method
             * treating this messages. See HydraxComponent::_processCustomMessage()
             *
             * @note that this should be reserved for rarely used methods.
             */
            SET_CUSTOM = utils::Message::LAST,
            /// Using a Vector3
            SET_POSITION,
            /// Using a Vector3
            SET_SCALE,
            /// Using a Vector3. NOTE: Should only be handled by LocationComp; it sends back a proper msg.
            SET_DIMENSIONS,
            /// Using a Quaternion
            SET_ORIENTATION,
            /// Using a Vector3 for euler angle. NOTE: Should only be handled by LocationComp; it sends back a proper msg.
            SET_ORIENTATION_EULER,
            /// Using a Vector3 and a Quaternion
            SET_POSITION_ORIENTATION,
            /// Set offset position (mostly used to move slightly a mesh if not centered correctly)
            SET_OFFSET_POSITION,
            /// Set offset orientation (mostly used to rotate a mesh if not oriented along the negative-y-axis correctly)
            SET_OFFSET_ORIENTATION,
            /// Set offset position & orientation
            SET_OFFSET_POSITION_ORIENTATION,

			/// Using an ObjectId and a bool
			SET_OBJECT_ACTIVE,
            /// Using an Object* >>>>> TODO: Here or as an internal message?
            DESTROY_OBJECT,
			/// Using an ObjectId
			OBJECT_CREATED,

            //-------------- Audio Messages --------------


            //-------------- Graphics Messages -----------

            /// Unit graphics from a file. Using String
            INIT_GRAPHICS_FILE,
            /// Using a String
            INIT_GRAPHICS_MESH,
            /// Using a String and a bool indicating it is looped or not.
            SET_GRAPHICS_MATERIAL,
            SET_ANIMATION_STATE,
            SET_SHADOWS_ENABLED,
            SET_CAMERA_FARCLIP,
            SET_CAMERA_NEARCLIP,
            /// Using a bool and Vector3 for locking the yaw axis
            SET_CAMERA_FIXED_YAW,

            // light
            INIT_LIGHT_TYPE,
            SET_INTENSITY,
            SET_DIFFUSE_COLOUR,
            SET_SPECULAR_COLOUR,

            //-------------- Network Messages ------------
			CLIENT_COMMAND,
			/// using a network id
			SET_NETWORK_OWNER,

            //-------------- Physics Messages ------------
            /// using a String
            INIT_PHYSICS_SHAPE,
            /// using a bool
            SET_PHYSICS_DYNAMIC,
            /// using a String
            SET_PHYSICS_CONTACT_GROUP,
            /// using a double
            SET_PHYSICS_MASS,
            /// collision group (bit wise short)
            SET_COLLISION_GROUP,
            /// collision mask (bit wise short)
            SET_COLLISION_MASK,
			/// using a double
			SET_DRAG,
            /// using a Vector3
            SET_ANGULAR_VELOCITY,
			/// using a double
			SET_MAX_ANGULAR_VELOCITY,
            /// using a Vector3
            SET_VELOCITY,
			/// using a Real
			SET_MAX_VELOCITY,
			/// using a Vector3 and bool designating if force should be constant
			APPLY_FORCE,
			/// using a Vector3 and bool designating if torque should be constant
			APPLY_TORQUE,
			/// using a bool - if true this object will not react to collisions
			SET_PHYSICS_TRIGGER_ONLY,
            // some msg types for a physics character component
            SET_WALK_SPEED,
            SET_STEP_HEIGHT,
            GO_FORWARD,
            GO_BACKWARD,
            GO_LEFT,
            GO_RIGHT,
            JUMP,
			DUCK,

            //-------------- Script Messages -------------
			// using a String
			SET_NAME,
			// using a String and bool if should immediately switch
			SET_NEXT_STATE,
			// passes true or false when the object's on ground state changes
			ON_GROUND,
			// passes the object id string of the object that entered (trigger object id can be retrieved from sender param)
			TRIGGER_ON_ENTER,
			// passes the object id string of the object that exited (trigger object id can be retrieved from sender param)
			TRIGGER_ON_EXIT,
			/// set health amount using an int for amount
			SET_HEALTH,
			/// set power using an int for amount
			SET_POWER,
			/// set armour using an int for amount and int for type
			SET_ARMOUR,
			/// set ammo using an int for amount and int for type (optional)
			SET_AMMO,
			/// set damage using an int for amount and a int for type
			SET_DAMAGE,

            //============== System/Internal Messages =========

            //TODO: Msgs like TOGGLE_FULLSCREEN, etc. Put them in Message class? or ObjMgr? or EngineMgr???

            LAST
        };
    };

    /**
     * This class will create the object and contain the list of components type
     *
     * @note The components are NOT created here but in the ObjectFactory
     *       which contain all the templates (object and components)
     *
     * @author Steven 'lazalong' Gay
     */
    class ObjectTemplate
    {
    public:
        /**
         * Map of components type (not the components themselves) by family
         * map<type, family> or if you prefer map<sub-scenemanager type, component type>
         * For example, <"Graphics", "CameraComponent">
         *
* TODO NOW See comment in ObjectManager::registerComponentTemplate()
         */
        typedef std::vector<ComponentType> ComponentTypeList;
        typedef ComponentTypeList::const_iterator ComponentTypeListIter;

    protected:
        ObjectType mType;
        ComponentTypeList mComponentTypes;

    public:
        /**
         * Constructor
         * @param type Unique object type (aka the class).
         *        If multiple instance of the same type
         *        exist the object factory will actually reject it.
         * @note NO components created - should not be used directly.
         *       but if you do don't forget to add the components!
         * @todo Should be protected but can be useful for creating
         *       programatically an object.
         *
         * @note The components are not created here but in the ObjectFactory
         *       altough in an derived objecttemplate the components could be
         *       created here
         */
        ObjectTemplate(const ObjectType& type) : mType(type)
        {
        }
        ~ObjectTemplate()
        {
        }

        inline const ObjectType& getType() const { return mType; }

        /**
         * Adds a component type to the object template
         * @note Each component type must be unique.
         *       If one is already present the second addition is ignored.
         */
        void addComponent(const ComponentType& type);

        // wip todo use an iterator?
        const ComponentTypeList& getComponents() { return mComponentTypes; }
    };

    /**
     * ObjectFactory - this is an Abstract Factory that :
     *  - Stores the available templates (builders in fact)
     *  - Provides a factory method : i.e. createObject( "Object template name" );
     *
     * Object could be created by script files such as :
     *
     *  <?xml version="1.0"?>
     *  <objectTemplate type="ManWarrior">
     *      <components>
     *          <component id="ComponentVisualHuman">
     *              <model name="Warrior" />
     *              <scale value="1.2" />
     *          </component>
     *          <component type="ComponentHealthHuman">
     *              <hitpoints
     *                  head="100"
     *                  torso="50"
     *                  leftArm="10"
     *                  rightArm="10"
     *                  leftLeg="20"
     *                  rightLeg="20" />
     *          </component>
     *          <component type="ComponentAIWarrior"/>
     *      </components>
     *  </objectTemplate>
     *
     * @author Steven 'lazalong' Gay
     */
    class OGE_CORE_API ObjectFactory
    {
    private:
        typedef std::map<ObjectType, ObjectTemplate*> ObjectTemplateMap;
        typedef std::map<ObjectType, ObjectTemplate*>::iterator ObjectTemplateIter;

        typedef std::map<ComponentType, ComponentTemplate*> ComponentTemplateMap;
        typedef std::map<ComponentType, ComponentTemplate*>::iterator ComponentTemplateIter;

        typedef std::map<ComponentType, String> SceneManagerTypePerComponentType;
        typedef std::map<ComponentType, String>::iterator SceneManagerTypePerComponentTypeIter;

        /// Store the object templates
        ObjectTemplateMap mObjectTemplateMap;
        /// Store the component templates
        ComponentTemplateMap mComponentsTemplates;
        /// Store the scene manager type per component type
        SceneManagerTypePerComponentType mSceneManagerTypePerComponentType;

    public:
        /// Will delete all registered templates
        virtual ~ObjectFactory();
        /**
         * Register an object template to this factory.
         * @note Only one template of each type can be registred.
         */
        bool registerObjectTemplate(ObjectTemplate* objectTemplate);
        /// Returns the desired ObjectTemplate (0 if not found)
        ObjectTemplate* getObjectTemplate(const ObjectType& type);
        /// Return the scene manager type associated with a component type
        const String& getSceneManagerFamily(const ComponentType& type);
        /**
         * Register a component template
         *
         * @param sceneManagerType is the SM which manage it such as "Graphics", "Physics", "Object", ...
         * @param type of the component. Must be unique among a scenemManagerType
         */
        bool registerComponentTemplate(const String& scenemManagerType, ComponentTemplate* componentTemplate);

        /// Returns all components templates
        ComponentTemplateMap& getComponentTemplates() { return mComponentsTemplates; };
        /**
         * Create a component of a certain family and type
         * @note No initialisation of initial values is done. You need to reset it with
         *       Component::_reset() and activate it.
         * @see  Component::_reset() and ObjectManager::activateObject()
         * @note If you call this method you need to destroyed the component yourself
         */
        Component* createComponent(const ComponentType& type);
    };


}

#endif /* GAMEOBJECT_H_ */
