'''
Created on Jun 26, 2016

@author: consultit
'''

from panda3d.core import TextNode, ClockObject, AnimControlCollection, \
        auto_bind, LPoint3f, LVecBase3f
from p3opensteer import OSSteerManager, ValueList_string, ValueList_LPoint3f, \
        ValueList_float
#
from common import startFramework, toggleDebugFlag, toggleDebugDraw, mask, \
        loadTerrain, printCreationParameters, handleVehicleEvent, \
        changeVehicleMaxForce, changeVehicleMaxSpeed, getVehicleModelAnims, \
        animRateFactor, writeToBamFileAndExit, readFromBamFile, bamFileName, \
        getCollisionEntryFromCamera, obstacleFile, HandleObstacleData, \
        handleObstacles, HandleVehicleData, handleVehicles
import sys, random
        
# # specific data/functions declarations/definitions
sceneNP = None
vehicleAnimCtls = []
steerPlugIn = None
steerVehicles = []
#
def setParametersBeforeCreation():
    """set parameters as strings before plug-ins/vehicles creation"""
    
    steerMgr = OSSteerManager.get_global_ptr()
    valueList = ValueList_string()
    # set plug-in type
    steerMgr.set_parameter_value(OSSteerManager.STEERPLUGIN, "plugin_type",
            "pedestrian")

    # set vehicle's type, mass, speed
    steerMgr.set_parameter_value(OSSteerManager.STEERVEHICLE, "vehicle_type",
            "pedestrian")
    steerMgr.set_parameter_value(OSSteerManager.STEERVEHICLE, "mass",
            "2.0")
    steerMgr.set_parameter_value(OSSteerManager.STEERVEHICLE, "speed",
            "0.01")

    # set vehicle throwing events
    valueList.clear()
    valueList.add_value("avoid_obstacle@avoid_obstacle@1.0:avoid_close_neighbor@avoid_close_neighbor@")
    steerMgr.set_parameter_values(OSSteerManager.STEERVEHICLE,
            "thrown_events", valueList)
    #
    printCreationParameters()

def toggleWanderBehavior():
    """toggle wander behavior of last inserted vehicle"""
    
    global steerVehicles
    if len(steerVehicles) == 0:
        return
    
    if steerVehicles[-1].get_wander_behavior():
        steerVehicles[-1].set_wander_behavior(False)
    else:
        steerVehicles[-1].set_wander_behavior(True)
    print(str(steerVehicles[-1]) + "'s wander behavior is " + str(steerVehicles[-1].get_wander_behavior()))

def updatePlugIn(steerPlugIn, task):
    """custom update task for plug-ins"""
    
    global steerVehicles, vehicleAnimCtls
    # call update for plug-in
    dt = ClockObject.get_global_clock().get_dt()
    steerPlugIn.update(dt)
    # handle vehicle's animation
    for i in range(len(vehicleAnimCtls)):
        # get current velocity size
        currentVelSize = steerVehicles[i].get_speed()
        if currentVelSize > 0.0:
            if currentVelSize < 4.0: 
                animOnIdx = 0
            else:
                animOnIdx = 1
            animOffIdx = (animOnIdx + 1) % 2
            # Off anim (0:walk, 1:run)
            if vehicleAnimCtls[i][animOffIdx].is_playing():
                vehicleAnimCtls[i][animOffIdx].stop()
            # On amin (0:walk, 1:run)
            vehicleAnimCtls[i][animOnIdx].set_play_rate(currentVelSize / animRateFactor[animOnIdx])
            if not vehicleAnimCtls[i][animOnIdx].is_playing():
                vehicleAnimCtls[i][animOnIdx].loop(True)
        else:
            # stop any animation
            vehicleAnimCtls[i][0].stop()
            vehicleAnimCtls[i][1].stop()
    #
    return task.cont
        
if __name__ == '__main__':

    msg = "'pedestrian'"
    app = startFramework(msg)
      
    # # here is room for your own code
    # print some help to screen
    text = TextNode("Help")
    text.set_text(
            msg + "\n\n"      
            "- press \"d\" to toggle debug drawing\n"
            "- press \"a\"/\"k\" to add 'opensteer'/'kinematic' vehicle\n"
            "- press \"s\"/\"shift-s\" to increase/decrease last inserted vehicle's max speed\n"
            "- press \"f\"/\"shift-f\" to increase/decrease last inserted vehicle's max force\n"
            "- press \"t\" to toggle last inserted vehicle's wander behavior\n"
            "- press \"o\"/\"shift-o\" to add/remove obstacle\n")
    textNodePath = app.aspect2d.attach_new_node(text)
    textNodePath.set_pos(-1.25, 0.0, -0.5)
    textNodePath.set_scale(0.035)
    
    # create a steer manager; set root and mask to manage 'kinematic' vehicles
    steerMgr = OSSteerManager(app.render, mask)

    # print creation parameters: defult values
    print("\n" + "Default creation parameters:")
    printCreationParameters()

    # load or restore all scene stuff: if passed an argument
    # try to read it from bam file
    if (not len(sys.argv) > 1) or (not readFromBamFile(sys.argv[1])):
        # no argument or no valid bamFile
        # reparent the reference node to render
        steerMgr.get_reference_node_path().reparent_to(app.render)
    
        # get a sceneNP, naming it with "SceneNP" to ease restoring from bam 
        # file
        sceneNP = loadTerrain("SceneNP")
        # and reparent to the reference node
        sceneNP.reparent_to(steerMgr.get_reference_node_path())
        
        # set sceneNP's collide mask
        sceneNP.set_collide_mask(mask)

        # set creation parameters as strings before plug-in/vehicles creation
        print("\n" + "Current creation parameters:")
        setParametersBeforeCreation()
        
        # create the plug-in (attached to the reference node)
        plugInNP = steerMgr.create_steer_plug_in()
        steerPlugIn = plugInNP.node()
    
        # set the pathway
        pointList = ValueList_LPoint3f()
        pointList.add_value(LPoint3f(79.474, 51.7236, 2.0207))
        pointList.add_value(LPoint3f(108.071, 51.1972, 2.7246))
        pointList.add_value(LPoint3f(129.699, 30.1742, 0.720501))
        pointList.add_value(LPoint3f(141.597, 73.496, 2.14218))
        pointList.add_value(LPoint3f(105.917, 107.032, 3.06428))
        pointList.add_value(LPoint3f(61.2637, 109.622, 3.03588))
        #  use single radius pathway
        radiusList = ValueList_float()
        radiusList.add_value(4)
        steerPlugIn.set_pathway(pointList, radiusList, True, True)
    else:
        # valid bamFile
        # restore plug-in: through steer manager
        steerPlugIn = OSSteerManager.get_global_ptr().get_steer_plug_in(0)
        # restore sceneNP: through panda3d
        sceneNP = OSSteerManager.get_global_ptr().get_reference_node_path().find("**/SceneNP")
        # reparent the reference node to render
        OSSteerManager.get_global_ptr().get_reference_node_path().reparent_to(app.render)
    
        # restore steer vehicles
        NUMVEHICLES = OSSteerManager.get_global_ptr().get_num_steer_vehicles()
        tmpList = [None for i in range(NUMVEHICLES)]
        steerVehicles.extend(tmpList)
        vehicleAnimCtls.extend(tmpList)
        for i in range(NUMVEHICLES):
            # restore the steer vehicle: through steer manager
            steerVehicles[i] = OSSteerManager.get_global_ptr().get_steer_vehicle(i)
            # restore animations
            tmpAnims = AnimControlCollection()
            auto_bind(steerVehicles[i], tmpAnims)
            vehicleAnimCtls[i] = [None, None]
            for j in range(tmpAnims.get_num_anims()):
                vehicleAnimCtls[i][j] = tmpAnims.get_anim(j)

        # set creation parameters as strings before other plug-ins/vehicles creation
        print("\n" + "Current creation parameters:")
        setParametersBeforeCreation()

    # # first option: start the default update task for all plug-ins
#     steerMgr.start_default_update()

    # # second option: start the custom update task for all plug-ins
    app.taskMgr.add(updatePlugIn, "updatePlugIn", extraArgs=[steerPlugIn], 
                    appendTask=True)

    # DEBUG DRAWING: make the debug reference node paths sibling of the reference node
    steerMgr.get_reference_node_path_debug().reparent_to(app.render)
    steerMgr.get_reference_node_path_debug_2d().reparent_to(app.aspect2d)
    # enable debug drawing
    steerPlugIn.enable_debug_drawing(app.camera)

    # # set events' callbacks
    # toggle debug draw
    toggleDebugFlag = False
    app.accept("d", toggleDebugDraw, [steerPlugIn])

    # handle addition steer vehicles, models and animations 
    vehicleData = HandleVehicleData(0.7, 0, "opensteer", sceneNP, 
                        steerPlugIn, steerVehicles, vehicleAnimCtls)
    app.accept("a", handleVehicles, [vehicleData])
    vehicleDataKinematic = HandleVehicleData(0.7, 1, "kinematic", sceneNP, 
                        steerPlugIn, steerVehicles, vehicleAnimCtls)
    app.accept("k", handleVehicles, [vehicleDataKinematic])

    # handle obstacle addition
    obstacleAddition = HandleObstacleData(True, sceneNP, steerPlugIn,
                        LVecBase3f(0.03, 0.03, 0.03))
    app.accept("o", handleObstacles, [obstacleAddition])
    # handle obstacle removal
    obstacleRemoval = HandleObstacleData(False, sceneNP, steerPlugIn)
    app.accept("shift-o", handleObstacles, [obstacleRemoval])

    # increase/decrease last inserted vehicle's max speed
    app.accept("s", changeVehicleMaxSpeed, ["s", steerVehicles])
    app.accept("shift-s", changeVehicleMaxSpeed, ["shift-s", steerVehicles])
    # increase/decrease last inserted vehicle's max force
    app.accept("f", changeVehicleMaxForce, ["f", steerVehicles])
    app.accept("shift-f", changeVehicleMaxForce, ["shift-f", steerVehicles])
    
    # handle OSSteerVehicle(s)' events
    app.accept("avoid_obstacle", handleVehicleEvent, ["avoid_obstacle"])
    app.accept("avoid_close_neighbor", handleVehicleEvent, ["avoid_close_neighbor"])
    
    # write to bam file on exit
    app.win.set_close_request_event("close_request_event")
    app.accept("close_request_event", writeToBamFileAndExit, [bamFileName])

    # 'pedestrian' specific: toggle wander behavior
    app.accept("t", toggleWanderBehavior)
    
    # place camera
    trackball = app.trackball.node()
    trackball.set_pos(-128.0, 120.0, -40.0)
    trackball.set_hpr(0.0, 20.0, 0.0)
   
    # app.run(), equals to do the main loop in C++
    app.run()

