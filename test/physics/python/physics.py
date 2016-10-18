'''
Created on Oct 09, 2016

@author: consultit
'''

from panda3d.core import load_prc_file_data, WindowProperties, BitMask32, \
        PNMImage, NodePath, AnimControlCollection, auto_bind, PartGroup, \
        ClockObject, TextNode, LPoint3f, LVecBase3f, GeoMipTerrain, \
        Filename, TextureStage, TexturePool, LVecBase2f
from direct.showbase.ShowBase import ShowBase
import panda3d.bullet
from p3physics import GamePhysicsManager, BTRigidBody
#
import sys

# # global data declaration
dataDir = "../../data"
app = None
mask = BitMask32(0x10)
updateTask = None
terrain = None
terrainRootNetPos = None
# models and animations
modelFile = ["eve.egg", "ralph.egg", "sparrow.egg", "ball.egg", "red_car.egg"]
modelAnimFiles = [["eve-walk.egg", "eve-run.egg"],
                  ["ralph-walk.egg", "ralph-run.egg"],
                  ["sparrow-flying.egg", "sparrow-flying2.egg"],
                  ["", ""],
                  ["red_car-anim.egg", "red_car-anim2.egg"]]
animRateFactor = [0.6, 0.175]
# bame file
bamFileName = "physics.boo"
# debug flag
toggleDebugFlag = False

# # specific data/functions declarations/definitions
sceneNP = None
globalClock = None
# player specifics
playerRigidBody = None
playerAnimCtls = []
playerNP = None

#
def printCreationParameters():
    """print creation parameters"""
    
    physicsMgr = GamePhysicsManager.get_global_ptr()
    #
    valueList = physicsMgr.get_parameter_name_list(GamePhysicsManager.RIGIDBODY)
    print("\n" + "BTRigidBody creation parameters:")
    for name in valueList:
        print ("\t" + name + " = " + 
               physicsMgr.get_parameter_value(GamePhysicsManager.RIGIDBODY, name))
    #
    valueList = physicsMgr.get_parameter_name_list(GamePhysicsManager.SOFTBODY)
    print("\n" + "BTSoftBody creation parameters:")
    for name in valueList:
        print ("\t" + name + " = " + 
               physicsMgr.get_parameter_value(GamePhysicsManager.SOFTBODY, name))

def setParametersBeforeCreation():
    """set parameters as strings before rigid_bodies/soft_bodies creation"""
    
    physicsMgr = GamePhysicsManager.get_global_ptr()
    # set rigid_body's parameters
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "shape_type", "box")
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "body_mass", "10.0")
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "collide_mask", "0x10")
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "object", "PlayerNP")

    # set soft_body's parameters
    physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "static",
            "false")
    #
    printCreationParameters()

def startFramework(msg):
    """start base framework"""

    global app   
    # Load your application's configuration
    load_prc_file_data("", "model-path " + dataDir)
    load_prc_file_data("", "win-size 1024 768")
    load_prc_file_data("", "show-frame-rate-meter #t")
    load_prc_file_data("", "sync-video #t")
#     load_prc_file_data("", "want-directtools #t")
#     load_prc_file_data("", "want-tk #t")
        
    # Setup your application
    app = ShowBase()
    props = WindowProperties()
    props.setTitle("p3physics: " + msg)
    app.win.requestProperties(props)
 
    # common callbacks     
    #
    return app

def readFromBamFile(fileName):
    """read scene from a file"""
    
    return GamePhysicsManager.get_global_ptr().read_from_bam_file(fileName)

def writeToBamFileAndExit(fileName):
    """write scene to a file (and exit)"""
    
    GamePhysicsManager.get_global_ptr().write_to_bam_file(fileName)
    # # this is for testing explicit removal and destruction of all elements
    physicsMgr = GamePhysicsManager.get_global_ptr()
    # destroy rigid_bodies
    for rigid_bodyTmp in physicsMgr.get_rigid_bodies():
        # destroy rigid_bodyTmp
        physicsMgr.destroy_rigid_body(NodePath.any_path(rigid_bodyTmp))
#     # destroy soft_bodies
#     for soft_bodyTmp in physicsMgr.get_soft_bodies():
#         # destroy soft_bodyTmp
#         physicsMgr.destroy_soft_body(NodePath.any_path(soft_bodyTmp))
    #
    sys.exit(0)

def loadTerrainLowPoly(name, widthScale=128, heightScale=64.0,
                       texture="dry-grass.png"):
    """load terrain low poly stuff"""
    
    global app
    terrainNP = app.loader.load_model("terrain-low-poly.egg")
    terrainNP.set_name(name)
    terrainNP.set_scale(widthScale, widthScale, heightScale)
    tex = app.loader.load_texture(texture)
    terrainNP.set_texture(tex)
    return terrainNP

def terrainUpdate(task):
    """terrain update"""

    global app, terrain, terrainRootNetPos
    # set focal point
    # see https:#www.panda3d.org/forums/viewtopic.php?t=5384
    focalPointNetPos = app.camera.get_net_transform().get_pos()
    terrain.set_focal_point(focalPointNetPos - terrainRootNetPos)
    # update every frame
    terrain.update()
    #
    return task.cont

def loadTerrain(name, widthScale = 0.5, heightScale = 10.0):
    """load terrain stuff"""

    global app, terrain, terrainRootNetPos
        
    terrain = GeoMipTerrain("terrain")
    heightField = PNMImage(Filename(dataDir + "/heightfield.png"))
    terrain.set_heightfield(heightField)
    # sizing
    environmentWidthX = (heightField.get_x_size() - 1) * widthScale
    environmentWidthY = (heightField.get_y_size() - 1) * widthScale
    environmentWidth = (environmentWidthX + environmentWidthY) / 2.0
    terrain.get_root().set_sx(widthScale)
    terrain.get_root().set_sy(widthScale)
    terrain.get_root().set_sz(heightScale)
    # set other terrain's properties
    blockSize, minimumLevel = (64, 0)
    nearPercent, farPercent = (0.1, 0.7)
    terrainLODmin = min(minimumLevel, terrain.get_max_level())
    flattenMode = GeoMipTerrain.AFM_off
    terrain.set_block_size(blockSize)
    terrain.set_near(nearPercent * environmentWidth)
    terrain.set_far(farPercent * environmentWidth)
    terrain.set_min_level(terrainLODmin)
    terrain.set_auto_flatten(flattenMode)
    # terrain texturing
    textureStage0 = TextureStage("TextureStage0")
    textureImage = TexturePool.load_texture(Filename("terrain.png"))
    terrain.get_root().set_tex_scale(textureStage0, 1.0, 1.0)
    terrain.get_root().set_texture(textureStage0, textureImage, 1)
    # reparent this Terrain node path to the object node path
    terrain.get_root().set_collide_mask(mask)
    terrain.get_root().set_name(name)
    # brute force generation
    bruteForce = True
    terrain.set_bruteforce(bruteForce)
    # Generate the terrain
    terrain.generate()
    # check if terrain needs update or not
    if not bruteForce:
        # save the net pos of terrain root
        terrainRootNetPos = terrain.get_root().get_net_transform().get_pos()
        # Add a task to keep updating the terrain
        app.taskMgr.add(terrainUpdate, "terrainUpdate", appendTask=True)
    #
    return terrain.get_root()

def getModelAnims(name, scale, modelFileIdx, modelAnimCtls):
    """get model and animations"""

    global app, modelFile, modelAnimFiles
    # get some models, with animations
    # get the model
    modelNP = app.loader.load_model(modelFile[modelFileIdx])
    # set the name
    modelNP.set_name(name)
    # set scale
    modelNP.set_scale(scale)
    # associate an anim with a given anim control
    tmpAnims = AnimControlCollection()
    modelAnimNP = [None, None]
    modelAnimCtls.append([None, None])        
    if(len(modelAnimFiles[modelFileIdx][0]) != 0) and \
            (len(modelAnimFiles[modelFileIdx][1]) != 0):
        # first anim . modelAnimCtls[i][0]
        modelAnimNP[0] = app.loader.load_model(modelAnimFiles[modelFileIdx][0])
        modelAnimNP[0].reparent_to(modelNP)
        auto_bind(modelNP.node(), tmpAnims,
                  PartGroup.HMF_ok_part_extra | 
                  PartGroup.HMF_ok_anim_extra | 
                  PartGroup.HMF_ok_wrong_root_name)
        modelAnimCtls[-1][0] = tmpAnims.get_anim(0)
        tmpAnims.clear_anims()
        modelAnimNP[0].detach_node()
        # second anim . modelAnimCtls[i][1]
        modelAnimNP[1] = app.loader.load_model(modelAnimFiles[modelFileIdx][1])
        modelAnimNP[1].reparent_to(modelNP)
        auto_bind(modelNP.node(), tmpAnims,
                  PartGroup.HMF_ok_part_extra | 
                  PartGroup.HMF_ok_anim_extra | 
                  PartGroup.HMF_ok_wrong_root_name)
        modelAnimCtls[-1][1] = tmpAnims.get_anim(0)
        tmpAnims.clear_anims()
        modelAnimNP[1].detach_node()
        # reparent all node paths
        modelAnimNP[0].reparent_to(modelNP)
        modelAnimNP[1].reparent_to(modelNP)    
    #
    return modelNP

def handlePlayerUpdate(currentVelSize = 0.0):
    """handles player on every update"""
    
    global playerAnimCtls#, playerRigidBody, playerNP, playerHeightRayCast
    # handle vehicle's animation
    for i in range(len(playerAnimCtls)):
        if currentVelSize > 0.0:
            if currentVelSize < 5.0: 
                animOnIdx = 0
            else:
                animOnIdx = 1
            animOffIdx = (animOnIdx + 1) % 2
            # Off anim (0:walk, 1:run)
            if playerAnimCtls[i][animOffIdx].is_playing():
                playerAnimCtls[i][animOffIdx].stop()
            # On amin (0:walk, 1:run)
            playerAnimCtls[i][animOnIdx].set_play_rate(currentVelSize * 
                                                    animRateFactor[animOnIdx])
            if not playerAnimCtls[i][animOnIdx].is_playing():
                playerAnimCtls[i][animOnIdx].loop(True)
        else:
            # stop any animation
            playerAnimCtls[i][0].stop()
            playerAnimCtls[i][1].stop()
#     playerRigidBodyNP = NodePath.any_path(playerRigidBody)
#     # make playerNP kinematic (ie stand on floor)
#     if currentVelSize > 0.0:
#         # get control manager
#         controlMgr = GameControlManager.get_global_ptr()
#         # correct player's Z: set the collision ray origin wrt collision root
#         pOrig = controlMgr.get_collision_root().get_relative_point(
#                 controlMgr.get_reference_node_path(), playerRigidBodyNP.get_pos()) + \
#                                         playerHeightRayCast * 2.0
#         # get the collision height wrt the reference node path
#         gotCollisionZ = controlMgr.get_collision_height(pOrig,
#                                         controlMgr.get_reference_node_path())
#         if gotCollisionZ.get_first():
#             # updatedPos.z needs correction
#             playerRigidBodyNP.set_z(gotCollisionZ.get_second())
            
def updateControls(task):
    """custom update task for controls"""

    global playerRigidBody
    # call update for controls
    dt = ClockObject.get_global_clock().get_dt()
    playerRigidBody.update(dt)
    # handle player on update
    handlePlayerUpdate()
    #
    return task.cont

def rigidBodyCallback(rigidBody):
    """rigid body update callback function"""
    
    global playerRigidBody
    if rigidBody != playerRigidBody:
        return
    currentVelSize = abs(playerRigidBody.get_linear_velocity().length())
    # handle player on callback
    handlePlayerUpdate(currentVelSize)

# def softBodyCallback(soft_body):
#     """soft body update callback function"""  
# 
#     global playerRigidBody
#     refNP = GamePhysicsManager.get_global_ptr().get_reference_node_path()
#     distLS = (NodePath.any_path(playerRigidBody).get_pos(refNP) - 
#             NodePath.any_path(soft_body).get_pos(refNP)).length()
#     print(soft_body, " " + str(globalClock.get_real_time()) + " - " + str(distLS))

def toggleDebugDraw():
    """toggle debug draw"""
    
    global toggleDebugFlag

    toggleDebugFlag = not toggleDebugFlag
    GamePhysicsManager.get_global_ptr().debug(toggleDebugFlag)

def collisionNotify(name, object0, object1):
    """collision notify"""

    print ("got '" + name + "' between '" + object0.get_name() +
           "' and '" + object1.get_name() + "'")

if __name__ == '__main__':

    msg = "'BTRigidBody & BTSoftBody & BTGhost'"
    app = startFramework(msg)
      
    # # here is room for your own code
    # print some help to screen
    text = TextNode("Help")
    text.set_text(
            msg + "\n\n"
            "- press \"d\" to toggle debug drawing\n"
            "- press \"up\"/\"left\"/\"down\"/\"right\" arrows to move the player\n")
    textNodePath = app.aspect2d.attach_new_node(text)
    textNodePath.set_pos(-1.25, 0.0, 0.8)
    textNodePath.set_scale(0.035)

    # create a physics manager
    physicsMgr = GamePhysicsManager(10, app.render, mask)

    # print creation parameters: defult values
    print("\n" + "Default creation parameters:")
    printCreationParameters()

    # load or restore all scene stuff: if passed an argument
    # try to read it from bam file
    if (not len(sys.argv) > 1) or (not readFromBamFile(sys.argv[1])):
        # no argument or no valid bamFile
        # reparent reference node to render
        physicsMgr.get_reference_node_path().reparent_to(app.render)
        
        # get a sceneNP, naming it with "SceneNP" to ease restoring from bam file
#         sceneNP = loadTerrainLowPoly("SceneNP")
        sceneNP = loadTerrain("SceneNP")
        # create scene's rigid_body (attached to the reference node)
        sceneRigidBodyNP = physicsMgr.create_rigid_body("SceneRigidBody")
        # get a reference to the scene's rigid_body
        sceneRigidBody = sceneRigidBodyNP.node()
        # set some parameters: trimesh shape, static, collide mask etc...
#         sceneRigidBody.set_shape_type(GamePhysicsManager.TRIANGLEMESH)
        sceneRigidBody.set_shape_type(GamePhysicsManager.HEIGHTFIELD)
        sceneRigidBody.set_shape_heightfield_file(dataDir + "/heightfield.png")
        sceneRigidBody.set_shape_scale_width_depth(LVecBase2f(0.5, 0.5))
        sceneRigidBody.set_shape_height(10.0)
        # other parameters
        sceneRigidBody.switch_body_type(BTRigidBody.STATIC)
        sceneRigidBodyNP.set_collide_mask(mask)
        sceneRigidBodyNP.set_pos(LPoint3f(0.0, 0.0, 0.0))
        # setup the player's rigid body
        sceneRigidBody.setup(sceneNP)

        # set various creation parameters as string for other rigid bodies
        setParametersBeforeCreation()

        # get a player with anims, reparent to reference node, set transform
        playerNP = getModelAnims("PlayerNP", 1.2, 4, playerAnimCtls)
        playerNP.reparent_to(physicsMgr.get_reference_node_path())
        playerNP.set_pos(LPoint3f(4.1, -12.0, 100.1))
        playerNP.set_hpr(LVecBase3f(0.0, 90.0, 0.0))
        # create player's rigid_body (attached to the reference node)
        playerRigidBodyNP = physicsMgr.create_rigid_body("PlayerRigidBody")
        # get a reference to the player's rigid_body
        playerRigidBody = playerRigidBodyNP.node()
    else:
        # valid bamFile
        # reparent reference node to render
        physicsMgr.get_reference_node_path().reparent_to(app.render)

        # restore sceneNP: through panda3d
        sceneNP = physicsMgr.get_reference_node_path().find("**/SceneNP")
        # restore the player's reference
        playerNP = physicsMgr.get_reference_node_path().find("**/PlayerNP")
    
        # restore rigid_bodies: through physics manager
        for rigid_body in physicsMgr.get_rigid_bodies():
            if rigid_body.get_name() == "PlayerRigidBody":
                playerRigidBody = rigid_body
        
        # set creation parameters as strings before other drivers creation
        print("\n" + "Current creation parameters:")
        setParametersBeforeCreation()

    # setup DEBUG DRAWING
    physicsMgr.init_debug()
    app.accept("d", toggleDebugDraw)
            
    # enable collision notify event: BTRigidBody_BTRigidBody_Collision
    physicsMgr.enable_collision_notify(GamePhysicsManager.COLLISIONNOTIFY, 10.0)
    app.accept("BTRigidBody_BTRigidBody_Collision", collisionNotify, 
               ["BTRigidBody_BTRigidBody_Collision"])

    # # first option: start the default update task for all plug-ins
    physicsMgr.start_default_update()
    playerRigidBody.set_update_callback(rigidBodyCallback)
    globalClock = ClockObject.get_global_clock()

    # # second option: start the custom update task for all plug-ins
#     app.taskMgr.add(updateControls, "updateControls", 10, appendTask=True)

    # write to bam file on exit
    app.win.set_close_request_event("close_request_event")
    app.accept("close_request_event", writeToBamFileAndExit, [bamFileName])

    # place camera
    trackball = app.trackball.node()
    trackball.set_pos(0.0, 120.0, 5.0)
    trackball.set_hpr(0.0, 10.0, 0.0)
   
    # app.run(), equals to do the main loop in C++
    app.run()
