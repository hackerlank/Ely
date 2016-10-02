'''
Created on Jun 26, 2016

@author: consultit
'''

from panda3d.core import TextNode, ClockObject, AnimControlCollection, \
        auto_bind
from p3ai import GameAIManager, ValueList_string
#
from common import startFramework, toggleDebugFlag, toggleDebugDraw, mask, \
        loadPlane, printCreationParameters, handleVehicleEvent, \
        changeVehicleMaxForce, changeVehicleMaxSpeed, getVehicleModelAnims, \
        animRateFactor, writeToBamFileAndExit, readFromBamFile, bamFileName
import sys
        
# # specific data/functions declarations/definitions
sceneNP = None
vehicleNPs = []
vehicleAnimCtls = []
steerPlugIn = None
steerVehicles = []
#
def setParametersBeforeCreation():
    """set parameters as strings before plug-ins/vehicles creation"""
    
    steerMgr = GameAIManager.get_global_ptr()
    valueList = ValueList_string()
    # set plug-in type
    steerMgr.set_parameter_value(GameAIManager.STEERPLUGIN, "plugin_type",
            "low_speed_turn")

    # set vehicle type, mass, speed
    steerMgr.set_parameter_value(GameAIManager.STEERVEHICLE, "vehicle_type",
            "low_speed_turn")
    steerMgr.set_parameter_value(GameAIManager.STEERVEHICLE, "mass",
            "2.0")
    steerMgr.set_parameter_value(GameAIManager.STEERVEHICLE, "speed",
            "0.01")

    # set vehicle throwing events
    valueList.clear()
    valueList.add_value("move@move-event@0.5")
    steerMgr.set_parameter_values(GameAIManager.STEERVEHICLE,
            "thrown_events", valueList)
    #
    printCreationParameters()

def toggleSteeringSpeed():
    """toggle steering speed"""
    
    global steerVehicles
    if steerVehicles[0].get_steering_speed() < 4.9:
        steerVehicles[0].set_steering_speed(5.0)
    else:
        steerVehicles[0].set_steering_speed(1.0)
    print(str(steerVehicles[0]) + "'s steering speed is " + str(steerVehicles[0].get_steering_speed()))

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

    msg = "'low speed turn'"
    app = startFramework(msg)
      
    # # here is room for your own code
    # print some help to screen
    text = TextNode("Help")
    text.set_text(
            msg + "\n\n"      
            "- press \"d\" to toggle debug drawing\n"
            "- press \"s\"/\"shift-s\" to increase/decrease vehicle's max speed\n"
            "- press \"f\"/\"shift-f\" to increase/decrease vehicle's max force\n"
            "- press \"t\" to toggle steering speed\n")
    textNodePath = app.aspect2d.attach_new_node(text)
    textNodePath.set_pos(-1.25, 0.0, 0.9)
    textNodePath.set_scale(0.035)
    
    # create a steer manager; set root and mask to manage 'kinematic' vehicles
    steerMgr = GameAIManager(0, app.render, mask)

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
        sceneNP = loadPlane("SceneNP")
        # and reparent to the reference node
        sceneNP.reparent_to(steerMgr.get_reference_node_path())
        
        # set sceneNP's collide mask
        sceneNP.set_collide_mask(mask)

        # set creation parameters as strings before plug-in/vehicles creation
        print("\n" + "Current creation parameters:")
        setParametersBeforeCreation()
        
        # create the default plug-in (attached to the reference node)
        plugInNP = steerMgr.create_steer_plug_in()
        steerPlugIn = plugInNP.node()
    
        # get steer vehicles, models and animations   
        #1: get the models and attach animations to them
        #2: create the steer vehicles (attached to the reference node)
        #3: set steer vehicles' positions
        #4: attach the models to steer vehicles
        #5: add the steer vehicles to the plug-in
        for i in range(2):
            if i % 2 == 0:
                moveType = "opensteer"
            else:
                moveType = "kinematic"
            getVehicleModelAnims(0.35, i, moveType, sceneNP, steerPlugIn, 
                           steerVehicles, vehicleAnimCtls)
    else:
        # valid bamFile
        # restore plug-in: through steer manager
        steerPlugIn = GameAIManager.get_global_ptr().get_steer_plug_in(0)
        # restore sceneNP: through panda3d
        sceneNP = GameAIManager.get_global_ptr().get_reference_node_path().find("**/SceneNP")
        # reparent the reference node to render
        GameAIManager.get_global_ptr().get_reference_node_path().reparent_to(app.render)
    
        # restore steer vehicles
        NUMVEHICLES = GameAIManager.get_global_ptr().get_num_steer_vehicles()
        tmpList = [None for i in range(NUMVEHICLES)]
        steerVehicles.extend(tmpList)
        vehicleAnimCtls.extend(tmpList)
        for i in range(NUMVEHICLES):
            # restore the steer vehicle: through steer manager
            steerVehicles[i] = GameAIManager.get_global_ptr().get_steer_vehicle(i)
            # restore animations
            tmpAnims = AnimControlCollection()
            auto_bind(steerVehicles[i], tmpAnims)
            vehicleAnimCtls[i] = [None, None]
            for j in range(tmpAnims.get_num_anims()):
                vehicleAnimCtls[i][j] = tmpAnims.get_anim(j)

    # show the added vehicles
    print("Vehicles added to plug-in:")
    for vehicle in steerPlugIn:
        print("\t- " + str(vehicle))

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

    # increase/decrease last inserted vehicle's max speed
    app.accept("s", changeVehicleMaxSpeed, ["s", steerVehicles])
    app.accept("shift-s", changeVehicleMaxSpeed, ["shift-s", steerVehicles])
    # increase/decrease last inserted vehicle's max force
    app.accept("f", changeVehicleMaxForce, ["f", steerVehicles])
    app.accept("shift-f", changeVehicleMaxForce, ["shift-f", steerVehicles])
    
    # handle OSSteerVehicle(s)' events
    app.accept("move-event", handleVehicleEvent, ["move-event"])
    
    # write to bam file on exit
    app.win.set_close_request_event("close_request_event")
    app.accept("close_request_event", writeToBamFileAndExit, [bamFileName])

    # 'low speed turn' specific: toggle steering speed
    app.accept("t", toggleSteeringSpeed)
    
    # place camera
    trackball = app.trackball.node()
    trackball.set_pos(0.0, 50.0, 0.0)
    trackball.set_hpr(0.0, 20.0, 0.0)
   
    # app.run(), equals to do the main loop in C++
    app.run()

