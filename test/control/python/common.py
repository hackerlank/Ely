'''
Created on Jun 20, 2016

@author: consultit
'''

from panda3d.core import load_prc_file_data, GeoMipTerrain, PNMImage, \
                Filename, TextureStage, TexturePool, BitMask32, CardMaker, \
                NodePath, WindowProperties, AnimControlCollection, auto_bind, \
                LVecBase3f, LVector3f, LPoint3f, PartGroup, ClockObject
from p3ai import AIManager, ValueList_float
from direct.showbase.ShowBase import ShowBase
import sys, random

# global data
dataDir = "../../data"
app = None
mask = BitMask32(0x10)
toggleDebugFlag = False
terrain = None
terrainRootNetPos = None
DEFAULT_MAXVALUE = 1.0
# models and animations
vehicleFile = ["eve.egg", "ralph.egg", "sparrow.egg", "ball.egg", "red_car.egg"]
vehicleAnimFiles = [["eve-walk.egg", "eve-run.egg"],
                  ["ralph-walk.egg", "ralph-run.egg"],
                  ["sparrow-flying.egg", "sparrow-flying2.egg"],
                  ["", ""],
                  ["red_car-anim.egg", "red_car-anim2.egg"]]
animRateFactor = [1.20, 4.80]
# obstacle model
obstacleFile = "plants2.egg"
# bame file
bamFileName = "plug_in.boo"

# # functions' declarations and definitions

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
    props.setTitle("p3ai: " + msg)
    app.win.requestProperties(props)
 
    #common callbacks     
    #
    return app

def loadPlane(name, widthX = 30.0, widthY = 30.0):
    """load plane stuff"""
    
    cm = CardMaker("plane")
    cm.set_frame(-widthX / 2.0, widthX / 2.0, -widthY / 2.0, widthY / 2.0)
    plane = NodePath(cm.generate())
    plane.set_p(-90.0)
    plane.set_z(0.0)
    plane.set_color(0.15, 0.35, 0.35)
    plane.set_collide_mask(mask)
    plane.set_name(name)
    return plane

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

def loadTerrain(name, widthScale = 0.5, heightScale = 10.0):
    """load terrain stuff"""

    global app, terrain, terrainRootNetPos
        
    steerMgr = AIManager.get_global_ptr()

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
    terrain.get_root().reparent_to(steerMgr.get_reference_node_path())
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

def getCollisionEntryFromCamera():
    """throws a ray and returns the first collision entry or nullptr"""    
    
    global app
    # get steer manager
    steerMgr = AIManager.get_global_ptr()
    # get the mouse watcher
    mwatcher = app.mouseWatcherNode
    if mwatcher.has_mouse():
        # Get to and from pos in camera coordinates
        pMouse = mwatcher.get_mouse()
        #
        pFrom, pTo = (LPoint3f(), LPoint3f())
        if app.camLens.extrude(pMouse, pFrom, pTo):
            # Transform to global coordinates
            pFrom = app.render.get_relative_point(app.cam, pFrom)
            pTo = app.render.get_relative_point(app.cam, pTo)
            direction = (pTo - pFrom).normalized()
            steerMgr.get_collision_ray().set_origin(pFrom)
            steerMgr.get_collision_ray().set_direction(direction)
            steerMgr.get_collision_traverser().traverse(app.render)
            # check collisions
            if steerMgr.get_collision_handler().get_num_entries() > 0:
                # Get the closest entry
                steerMgr.get_collision_handler().sort_entries()
                return steerMgr.get_collision_handler().get_entry(0)
    return None

def printCreationParameters():
    """print creation parameters"""
    
    steerMgr = AIManager.get_global_ptr()
    #
    valueList = steerMgr.get_parameter_name_list(AIManager.STEERPLUGIN)
    print("\n" + "OSSteerPlugIn creation parameters:")
    for name in valueList:
        print ("\t" + name + " = " + 
               steerMgr.get_parameter_value(AIManager.STEERPLUGIN, name))
    #
    valueList = steerMgr.get_parameter_name_list(AIManager.STEERVEHICLE)
    print("\n" + "OSSteerVehicle creation parameters:")
    for name in valueList:
        print ("\t" + name + " = " + 
               steerMgr.get_parameter_value(AIManager.STEERVEHICLE, name))

def handleVehicleEvent(name, vehicle):
    """handle vehicle's events"""
    
    vehicleNP = NodePath.any_path(vehicle)
    print ("got " + name + " event from '"+ vehicleNP.get_name() + "' at " + str(vehicleNP.get_pos()))

def getRandomPos(modelNP):
    """return a random point on the facing upwards surface of the model"""
    
    # collisions are made wrt render
    steerMgr = AIManager.get_global_ptr()
    # get the bounding box of scene
    modelDims, modelDeltaCenter = (LVecBase3f(), LVector3f())
    # modelRadius not used
    steerMgr.get_bounding_dimensions(modelNP, modelDims, modelDeltaCenter)
    # throw a ray downward from a point with z = double scene's height
    # and x,y randomly within the scene's (x,y) plane
    # set the ray origin at double of maximum height of the model
    zOrig = ((-modelDeltaCenter.get_z() + modelDims.get_z() / 2.0) + modelNP.get_z()) * 2.0
    while True:
        x = modelDims.get_x() * (random.uniform(0.0, 1.0) - 0.5) - modelDeltaCenter.get_x() + modelNP.get_x()
        y = modelDims.get_y() * (random.uniform(0.0, 1.0) - 0.5) - modelDeltaCenter.get_y() + modelNP.get_y()
        gotCollisionZ = steerMgr.get_collision_height(LPoint3f(x, y, zOrig))
        if gotCollisionZ.get_first():
            break
    return LPoint3f(x, y, gotCollisionZ.get_second())

def readFromBamFile(fileName):
    """read scene from a file"""
    
    return AIManager.get_global_ptr().read_from_bam_file(fileName)

def writeToBamFileAndExit(fileName):
    """write scene to a file (and exit)"""
    
    AIManager.get_global_ptr().write_to_bam_file(fileName)
    # # this is for testing explicit removal and destruction of all elements
    steerMgr = AIManager.get_global_ptr()
    # remove steer vehicles from steer plug-ins
    for plugInTmp in steerMgr.get_steer_plug_ins():
        for vehicleTmp in plugInTmp:
            # remove vehicleTmp
            plugInTmp.remove_steer_vehicle(NodePath.any_path(vehicleTmp))
    # destroy steer vehicles
    for vehicleTmp in steerMgr.get_steer_vehicles():
        # destroy vehicleTmp
        steerMgr.destroy_steer_vehicle(NodePath.any_path(vehicleTmp))
    # destroy steer plug-ins
    for plugInTmp in steerMgr.get_steer_plug_ins():
        # destroy vehicleTmp
        steerMgr.destroy_steer_plug_in(NodePath.any_path(plugInTmp))
    # #
    #
    sys.exit(0)
