'''
Created on Sep 20, 2016

@author: consultit
'''

from panda3d.core import load_prc_file_data, WindowProperties, BitMask32, \
        LVector3f, NodePath, AnimControlCollection, auto_bind, PartGroup, \
        ClockObject, TextNode, LPoint3f, LVecBase3f, BehaviorManager
from direct.showbase.ShowBase import ShowBase
from p3behavior import GameBehaviorManager
from p3control import GameControlManager
#
import sys

# # global data declaration
dataDir = "../../data"
app = None
mask = BitMask32(0x10)
updateTask = None
# models and animations
modelFile = ["eve.egg", "ralph.egg", "sparrow.egg", "ball.egg", "red_car.egg"]
modelAnimFiles = [["eve-walk.egg", "eve-run.egg"],
                  ["ralph-walk.egg", "ralph-run.egg"],
                  ["sparrow-flying.egg", "sparrow-flying2.egg"],
                  ["", ""],
                  ["red_car-anim.egg", "red_car-anim2.egg"]]
animRateFactor = [0.6, 0.175]
# sound effects
soundFile = ["eve_voice.wav", "", "sparrow_chirp.wav", "", "red_car_engine.wav"]
soundName = ["eve-voice", "", "sparrow-chirp", "", "red-car-engine"]
# bame file
bamFileName = "behavior.boo"

# # specific data/functions declarations/definitions
sceneNP = None
globalClock = None
# camera specifics
cameraActivity = None
# player specifics
playerSound3d = None
playerAnimCtls = []
playerNP = None
playerDriver = None
playerHeightRayCast = LVector3f()
forwardMove = 1
forwardMoveStop = -1
leftMove = 2
leftMoveStop = -2
backwardMove = 3
backwardMoveStop = -3
rightMove = 4
rightMoveStop = -4
# pursuer specifics
pursuerSound3d = None
pursuerAnimCtls = []
pursuerNP = None
pursuerChaser = None

#
def printCreationParameters():
    """print creation parameters"""
    
    behaviorMgr = GameBehaviorManager.get_global_ptr()
    #
    valueList = behaviorMgr.get_parameter_name_list(GameBehaviorManager.SOUND3D)
    print("\n" + "P3Sound3d creation parameters:")
    for name in valueList:
        print ("\t" + name + " = " + 
               behaviorMgr.get_parameter_value(GameBehaviorManager.SOUND3D, name))
    #
    valueList = behaviorMgr.get_parameter_name_list(GameBehaviorManager.LISTENER)
    print("\n" + "P3Activity creation parameters:")
    for name in valueList:
        print ("\t" + name + " = " + 
               behaviorMgr.get_parameter_value(GameBehaviorManager.LISTENER, name))

def setParametersBeforeCreation():
    """set parameters as strings before sound3ds/listeners creation"""
    
    behaviorMgr = GameBehaviorManager.get_global_ptr()
    # set sound3d's parameters
    behaviorMgr.set_parameter_value(GameBehaviorManager.SOUND3D, "static", "false")
    behaviorMgr.set_parameter_value(GameBehaviorManager.SOUND3D, "min_distance",
            "0.5")
    # set listener's parameters
    behaviorMgr.set_parameter_value(GameBehaviorManager.LISTENER, "static",
            "false")
    #
    controlMgr = GameControlManager.get_global_ptr()
    # set driver's parameters
    controlMgr.set_parameter_value(GameControlManager.DRIVER, "max_angular_speed",
            "50.0")
    controlMgr.set_parameter_value(GameControlManager.DRIVER, "angular_accel",
            "10.0")
    controlMgr.set_parameter_value(GameControlManager.DRIVER, "max_linear_speed",
            "150.0")
    controlMgr.set_parameter_value(GameControlManager.DRIVER, "linear_accel",
            "10.0")
    controlMgr.set_parameter_value(GameControlManager.DRIVER, "linear_friction",
            "0.5")
    controlMgr.set_parameter_value(GameControlManager.DRIVER, "angular_friction",
            "5.0")
    # set chaser's parameters
    controlMgr.set_parameter_value(GameControlManager.CHASER, "fixed_relative_position",
            "false")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "max_distance",
            "25.0")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "min_distance",
            "18.0")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "max_height",
            "18.0")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "min_height",
            "15.0")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "friction",
            "5.0")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "fixed_look_at",
            "true")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "mouse_head",
            "true")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "mouse_pitch",
            "true")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "look_at_distance",
            "5.0")
    controlMgr.set_parameter_value(GameControlManager.CHASER, "look_at_height",
            "12.5")
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
    props.setTitle("p3behavior: " + msg)
    app.win.requestProperties(props)
 
    # common callbacks     
    #
    return app

def readFromBamFile(fileName):
    """read scene from a file"""
    
    return GameBehaviorManager.get_global_ptr().read_from_bam_file(fileName)

def writeToBamFileAndExit(fileName):
    """write scene to a file (and exit)"""
    
    # before saving to bam file, reparent listener to reference node
    NodePath.any_path(cameraActivity).reparent_to(
            GameBehaviorManager.get_global_ptr().get_reference_node_path())
    GameBehaviorManager.get_global_ptr().write_to_bam_file(fileName)
    # # this is for testing explicit removal and destruction of all elements
    behaviorMgr = GameBehaviorManager.get_global_ptr()
    # destroy sound3ds
    for sound3dTmp in behaviorMgr.get_sound3ds():
        # destroy sound3dTmp
        behaviorMgr.destroy_sound3d(NodePath.any_path(sound3dTmp))
    # destroy listeners
    for listenerTmp in behaviorMgr.get_listeners():
        # destroy listenerTmp
        behaviorMgr.destroy_listener(NodePath.any_path(listenerTmp))
    #
    controlMgr = GameControlManager.get_global_ptr()
    # destroy drivers
    for driverTmp in controlMgr.get_drivers():
        # destroy driverTmp
        controlMgr.destroy_driver(NodePath.any_path(driverTmp))
    # destroy chasers
    for chaserTmp in controlMgr.get_chasers():
        # destroy chaserTmp
        controlMgr.destroy_chaser(NodePath.any_path(chaserTmp))
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

def handlePlayerUpdate():
    """handles player on every update"""
    
    global playerDriver, playerAnimCtls, playerNP, playerHeightRayCast
    # get current forward velocity size
    currentVelSize = abs(playerDriver.get_current_speeds().get_first().get_y())
    playerDriverNP = NodePath.any_path(playerDriver)
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
    # make playerNP kinematic (ie stand on floor)
    if currentVelSize > 0.0:
        # get control manager
        controlMgr = GameControlManager.get_global_ptr()
        # correct player's Z: set the collision ray origin wrt collision root
        pOrig = controlMgr.get_collision_root().get_relative_point(
                controlMgr.get_reference_node_path(), playerDriverNP.get_pos()) + \
                                        playerHeightRayCast * 2.0
        # get the collision height wrt the reference node path
        gotCollisionZ = controlMgr.get_collision_height(pOrig,
                                        controlMgr.get_reference_node_path())
        if gotCollisionZ.get_first():
            # updatedPos.z needs correction
            playerDriverNP.set_z(gotCollisionZ.get_second())
            
def handlePursuerUpdate():
    """handles pursuer on every update"""
    
    global pursuerChaser, pursuerAnimCtls, pursuerNP
    # get current forward velocity size
    currentVelSize = abs(pursuerChaser.get_chased_object().node().
            get_current_speeds().get_first().get_y())
    # handle vehicle's animation
    for i in range(len(pursuerAnimCtls)):
        if currentVelSize < 5.0: 
            animOnIdx = 0
        else:
            animOnIdx = 1
        animOffIdx = (animOnIdx + 1) % 2
        # Off anim (0:walk, 1:run)
        if pursuerAnimCtls[i][animOffIdx].is_playing():
            pursuerAnimCtls[i][animOffIdx].stop()
        # On amin (0:walk, 1:run)
        pursuerAnimCtls[i][animOnIdx].set_play_rate(
            (currentVelSize + 1.0) * 0.5)
        if not pursuerAnimCtls[i][animOnIdx].is_playing():
            pursuerAnimCtls[i][animOnIdx].loop(True)
            
def updateControls(task):
    """custom update task for controls"""

    global playerDriver, pursuerChaser
    # call update for controls
    dt = ClockObject.get_global_clock().get_dt()
    playerDriver.update(dt)
    pursuerChaser.update(dt)
    # handle player on update
    handlePlayerUpdate()
    # handle player on update
    handlePursuerUpdate()
    #
    return task.cont
    
def movePlayer(data):
    """player's movement callback"""
    
    global playerDriver, forwardMove, leftMove, backwardMove, rightMove
    if not playerDriver:
        return

    action = data
    if action > 0:
        # start movement
        enable = True
    else:
        action = -action
        # stop movement
        enable = False
    #
    if action == forwardMove:
        playerDriver.set_move_forward(enable)
    elif action == leftMove:
        playerDriver.set_rotate_head_left(enable)
    elif action == backwardMove:
        playerDriver.set_move_backward(enable)
    elif action == rightMove:
        playerDriver.set_rotate_head_right(enable)

def sound3dCallback(sound3d):
    """sound3d update callback function"""
    
    global playerDriver
    if sound3d != playerSound3d:
        return
    currentVelSize = abs(playerDriver.get_current_speeds().get_first().get_y())
    sound3d[0].set_play_rate(0.1 + currentVelSize * 0.05)

def listenerCallback(listener):
    """listener update callback function"""  

    global playerSound3d
    refNP = GameBehaviorManager.get_global_ptr().get_reference_node_path()
    distLS = (NodePath.any_path(playerSound3d).get_pos(refNP) - 
            NodePath.any_path(listener).get_pos(refNP)).length()
    print(listener, " " + str(globalClock.get_real_time()) + " - " + str(distLS))

if __name__ == '__main__':

    msg = "'P3Sound3d & P3Activity'"
    app = startFramework(msg)
      
    # # here is room for your own code
    # print some help to screen
    text = TextNode("Help")
    text.set_text(
            msg + "\n\n"
            "- press \"up\"/\"left\"/\"down\"/\"right\" arrows to move the player\n")
    textNodePath = app.aspect2d.attach_new_node(text)
    textNodePath.set_pos(-1.25, 0.0, 0.8)
    textNodePath.set_scale(0.035)

    # create a behavior manager
    behaviorMgr = GameBehaviorManager()
    # create a control manager set root and mask to manage 'kinematic' players
    controlMgr = GameControlManager(app.win, 10, app.render, mask)

    # print creation parameters: defult values
    print("\n" + "Default creation parameters:")
    printCreationParameters()

    # load or restore all scene stuff: if passed an argument
    # try to read it from bam file
    if (not len(sys.argv) > 1) or (not readFromBamFile(sys.argv[1])):
        # no argument or no valid bamFile
        # set a common reference node and reparent it to render
        controlMgr.set_reference_node_path(behaviorMgr.get_reference_node_path())
        behaviorMgr.get_reference_node_path().reparent_to(app.render)
        
        # get a sceneNP, naming it with "SceneNP" to ease restoring from bam file
        sceneNP = loadTerrainLowPoly("SceneNP")
        # and reparent to the reference node
        sceneNP.reparent_to(behaviorMgr.get_reference_node_path())

        # set sceneNP's collide mask
        sceneNP.set_collide_mask(mask)

        # set driver's various creation parameters as string
        setParametersBeforeCreation()
        # get a player with anims
        playerNP = getModelAnims("PlayerNP", 1.2, 4, playerAnimCtls)
        # get a pursuer with anims
        pursuerNP = getModelAnims("PursuerNP", 0.01, 2, pursuerAnimCtls)
        pursuerNP.set_h(180)
        
        # create the driver (attached to the reference node)
        playerDriverNP = controlMgr.create_driver("PlayerDriver")
        # get a reference to the player driver
        playerDriver = playerDriverNP.node()
        # set the position
        playerDriverNP.set_pos(LPoint3f(4.1, -12.0, 1.5))
        # attach some geometry (a model) to control vehicle
        playerNP.reparent_to(playerDriverNP)
        
        # create the pursuer (attached to the reference node)
        pursuerChaserNP = controlMgr.create_chaser("PursuerChaser")
        # get a reference to the pursuer's chaser
        pursuerChaser = pursuerChaserNP.node()
        # set the chased object: playerDriverNP or playerNP
        pursuerChaser.set_chased_object(playerDriverNP)
        # attach some geometry (a model) to pursuer's chaser
        pursuerNP.reparent_to(pursuerChaserNP)
        
        # create some sound3ds (attached to the reference node)
        playerSound3dNP = behaviorMgr.create_sound3d("PlayerSound3d")
        pursuerSound3dNP = behaviorMgr.create_sound3d("PursuerSound3d")
        # get a reference to the sound3ds
        playerSound3d = playerSound3dNP.node()
        pursuerSound3d = pursuerSound3dNP.node()
        # reparent the sound3ds
        playerSound3dNP.reparent_to(playerNP)
        pursuerSound3dNP.reparent_to(pursuerNP)
        # attach some sounds to the sound3ds
        playerSound3d.add_sound(soundName[4], soundFile[4])
        pursuerSound3d.add_sound(soundName[2], soundFile[2])
        # set sounds looping
        sound = playerSound3d.get_sound_by_name(soundName[4])
        sound.set_loop(True)
        sound.play()
        #
        sound = pursuerSound3d.get_sound_by_name(soundName[2])
        sound.set_loop(True)
        sound.play()

        # create a listener (attached to the reference node)
        cameraActivityNP = behaviorMgr.create_listener("CameraActivity")
        # get a reference to the camera's listener
        cameraActivity = cameraActivityNP.node()
        # reparent listener to the camera
        cameraActivityNP.reparent_to(app.camera)
        
    else:
        # valid bamFile
        # set a common reference node and reparent it to render
        controlMgr.set_reference_node_path(behaviorMgr.get_reference_node_path())
        behaviorMgr.get_reference_node_path().reparent_to(app.render)

        # restore sceneNP: through panda3d
        sceneNP = behaviorMgr.get_reference_node_path().find("**/SceneNP")
        # restore the player's reference
        playerNP = behaviorMgr.get_reference_node_path().find("**/PlayerNP")
    
        # restore driver: through control manager
        playerDriver = controlMgr.get_driver(0)
        # restore animations
        tmpList = [None for i in range(1)]
        playerAnimCtls.extend(tmpList)
        tmpAnims = AnimControlCollection()
        auto_bind(playerDriver, tmpAnims)
        playerAnimCtls[i] = [None, None]
        # restore animations
        for j in range(tmpAnims.get_num_anims()):
            playerAnimCtls[i][j] = tmpAnims.get_anim(j)

        # restore chaser: through control manager
        pursuerChaser = controlMgr.get_chaser(0)
        # restore animations
        pursuerAnimCtls.extend(tmpList)
        tmpAnims.clear_anims()
        auto_bind(pursuerChaser, tmpAnims)
        pursuerAnimCtls[0] = [None, None]
        for j in range(tmpAnims.get_num_anims()):
            pursuerAnimCtls[0][j] = tmpAnims.get_anim(j)

        # restore sound3ds: through behavior manager
        for sound3d in behaviorMgr.get_sound3ds():
            if sound3d.get_name() == "PlayerSound3d":
                playerSound3d = sound3d
            if sound3d.get_name() == "PursuerSound3d":
                pursuerSound3d = sound3d
        # set sounds looping
        sound = playerSound3d.get_sound_by_name(soundName[4])
        sound.set_loop(True)
        sound.play()
        #
        sound = pursuerSound3d.get_sound_by_name(soundName[2])
        sound.set_loop(True)
        sound.play()

        # restore listeners: through behavior manager
        cameraActivity = behaviorMgr.get_listener(0)
        # reparent listener to the camera
        NodePath.any_path(cameraActivity).reparent_to(app.camera)
        
        # set creation parameters as strings before other drivers creation
        print("\n" + "Current creation parameters:")
        setParametersBeforeCreation()

    # # first option: start the default update task for all plug-ins
    behaviorMgr.start_default_update()
    playerSound3d.set_update_callback(sound3dCallback)
    pursuerSound3d.set_update_callback(sound3dCallback)
    cameraActivity.set_update_callback(listenerCallback)
    globalClock = ClockObject.get_global_clock()

    # # second option: start the custom update task for all plug-ins
    app.taskMgr.add(updateControls, "updateControls", 10, appendTask=True)

    # write to bam file on exit
    app.win.set_close_request_event("close_request_event")
    app.accept("close_request_event", writeToBamFileAndExit, [bamFileName])

    # get player dims for kinematic ray cast
    modelDims = LVecBase3f() 
    modelDeltaCenter = LVector3f()
    controlMgr.get_bounding_dimensions(playerNP, modelDims, modelDeltaCenter)
    playerHeightRayCast = LVector3f(0.0, 0.0, modelDims.get_z())

    # player will be driven by arrows keys
    app.accept("arrow_up", movePlayer, [forwardMove]) 
    app.accept("arrow_up-up", movePlayer, [forwardMoveStop]) 
    app.accept("arrow_left", movePlayer, [leftMove]) 
    app.accept("arrow_left-up", movePlayer, [leftMoveStop]) 
    app.accept("arrow_down", movePlayer, [backwardMove]) 
    app.accept("arrow_down-up", movePlayer, [backwardMoveStop]) 
    app.accept("arrow_right", movePlayer, [rightMove]) 
    app.accept("arrow_right-up", movePlayer, [rightMoveStop])

    # place camera
    trackball = app.trackball.node()
    trackball.set_pos(0.0, 120.0, 5.0)
    trackball.set_hpr(0.0, 10.0, 0.0)
   
    # app.run(), equals to do the main loop in C++
    app.run()
