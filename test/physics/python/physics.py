'''
Created on Oct 09, 2016

@author: consultit
'''

from panda3d.core import load_prc_file_data, WindowProperties, BitMask32, \
        PNMImage, NodePath, AnimControlCollection, auto_bind, PartGroup, \
        ClockObject, TextNode, LPoint3f, LVecBase3f, GeoMipTerrain, \
        Filename, TextureStage, TexturePool, GeomNode, TransformState, \
        GeomVertexArrayFormat, InternalName, Geom, GeomVertexFormat, \
        GeomVertexData, GeomVertexWriter, LVector3f, GeomTriangles, \
        RopeNode
from direct.showbase.ShowBase import ShowBase
from panda3d.bullet import Z_up, X_up, Y_up, BulletSoftBodyConfig
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
# rope specifics
ropeSoftBody = None
softBodyCBCount = 0.0

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

def setParametersBeforeCreation(objectName, upAxis = "z"):
    """set parameters as strings before rigid_bodies/soft_bodies creation"""
    
    physicsMgr = GamePhysicsManager.get_global_ptr()
    # set rigid_body's parameters
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "body_mass", "10.0")
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "collide_mask", "0x10")
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "object", objectName)
    physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
            "shape_up", upAxis)

    # set soft_body's parameters
    physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, 
            "", "")
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
    # destroy soft_bodies
    for soft_bodyTmp in physicsMgr.get_soft_bodies():
        # destroy soft_bodyTmp
        physicsMgr.destroy_soft_body(NodePath.any_path(soft_bodyTmp))
    #
    sys.exit(0)

def loadPlane(name, width=30.0, depth=30.0, upAxis=Z_up,
                       texture="dry-grass.png"):
    """load plane stuff centered at (0,0,0)"""
    
    global app
    # Vertex Format
    arrayFormat = GeomVertexArrayFormat()
    arrayFormat.add_column(InternalName.make("vertex"), 3,
                          Geom.NT_float32, Geom.C_point)
    arrayFormat.add_column(InternalName.make("normal"), 3,
                          Geom.NT_float32, Geom.C_vector)
    arrayFormat.add_column(InternalName.make("texcoord"), 2,
                          Geom.NT_float32, Geom.C_texcoord)
    vertexFormat = GeomVertexFormat()
    vertexFormat.add_array(arrayFormat)
    # Pre-defined vertex formats
#         vertexFormatAdded = GeomVertexFormat.get_v3n3t2()
    vertexFormatAdded = GeomVertexFormat.register_format(vertexFormat)
    # Vertex Data
    vertexData = GeomVertexData("plane", vertexFormatAdded, Geom.UH_static)
    vertex = GeomVertexWriter(vertexData, "vertex")
    normal = GeomVertexWriter(vertexData, "normal")
    texcoord = GeomVertexWriter(vertexData, "texcoord")
    # compute coords and normal according to up axis
    # 3------2      ^y           ^z           ^x
    # |      |      |            |            |
    # |      | +d   | Z_up   OR  | X_up   OR  | Y_up
    # |      |      |            |            |
    # 0------1      -----.x     -----.y     -----.z
    #    +w   
    w = abs(width) / 0.5
    d = abs(depth) / 0.5
    # default: Z_up
    n = LVector3f(0.0, 0.0, 1.0)
    v0 = LPoint3f(-w, -d, 0.0)
    v1 = LPoint3f(w, -d, 0.0)
    v2 = LPoint3f(w, d, 0.0)
    v3 = LPoint3f(-w, d, 0.0)
    if upAxis == X_up:
        n = LVector3f(1.0, 0.0, 0.0)
        v0 = LPoint3f(0.0, -w, -d)
        v1 = LPoint3f(0.0, w, -d)
        v2 = LPoint3f(0.0, w, d)
        v3 = LPoint3f(0.0, -w, d)
    elif upAxis == Y_up:
        n = LVector3f(0.0, 1.0, 0.0)
        v0 = LPoint3f(-d, 0.0, -w)
        v1 = LPoint3f(d, 0.0, -w)
        v2 = LPoint3f(d, 0.0, w)
        v3 = LPoint3f(-d, 0.0, w)
    # normalize
    n.normalize()
    # fill-up vertex data (plane)
    # vertex 0
    vertex.add_data3f(v0)
    normal.add_data3f(n)
    texcoord.add_data2f(0.0, 0.0)
    # vertex 1
    vertex.add_data3f(v1)
    normal.add_data3f(n)
    texcoord.add_data2f(1.0, 0.0)
    # vertex 2
    vertex.add_data3f(v2)
    normal.add_data3f(n)
    texcoord.add_data2f(1.0, 1.0)
    # vertex 3
    vertex.add_data3f(v3)
    normal.add_data3f(n)
    texcoord.add_data2f(0.0, 1.0)
    # Creating the GeomPrimitive objects for plane
    planeTriangles = GeomTriangles(Geom.UH_static)
    # lower triangle
    planeTriangles.add_vertices(0, 1, 3)
    # higher triangle
    planeTriangles.add_vertices(2, 3, 1)
    # Putting your new geometry in the scene graph
    planeGeom = Geom(vertexData)
    planeGeom.add_primitive(planeTriangles)
    planeNode = GeomNode(name + "Node")
    planeNode.add_geom(planeGeom)
    planeNP = NodePath(planeNode)
    # apply texture
    tex = app.loader.load_texture(texture)
    planeNP.set_texture(tex)
    #
    return planeNP

def loadTerrainLowPoly(name, widthScale=128, heightScale=64.0,
                       texture="dry-grass.png"):
    """load terrain low poly stuff"""
    
    global app
    terrainNP = app.loader.load_model("terrain-low-poly.egg")
    terrainNP.set_name(name)
    terrainNP.set_transform(TransformState.make_identity())
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
    terrain.get_root().set_transform(TransformState.make_identity())
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
    # get animations if requested
    if modelAnimCtls:
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

def softBodyCallback(softBody):
    """soft body update callback function"""

    global softBodyCBCount, globalClock
    softBodyCBCount += globalClock.get_dt()
    if softBodyCBCount <= 5:
        return
    softBodyCBCount = 0    
    refNP = GamePhysicsManager.get_global_ptr().get_reference_node_path()
    distLS = NodePath.any_path(softBody).get_pos(refNP).length()
    print(str(softBody) + " callback: " + str(globalClock.get_real_time()) +
            " - " + str(distLS))

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
        # # plane
        planeUpAxis = Z_up # Z_up X_up Y_up
        sceneNP = loadPlane("SceneNP", 128.0, 128.0, planeUpAxis)
        # # triangle mesh
#         sceneNP = loadTerrainLowPoly("SceneNP")
        # # heightfield
#         sceneNP = loadTerrain("SceneNP", 1.0, 60.0)
        # set sceneNP transform
        sceneNP.set_pos_hpr(LPoint3f(0.0, 0.0, 0.0), LVecBase3f(45.0, 0.0, 0.0))
        # create scene's rigid_body (attached to the reference node)
        sceneRigidBodyNP = physicsMgr.create_rigid_body("SceneRigidBody")
        # get a reference to the scene's rigid_body
        sceneRigidBody = sceneRigidBodyNP.node()
        # set some parameters
        # plane
#         sceneRigidBody.set_shape_up(planeUpAxis)
#         sceneRigidBody.set_shape_type(GamePhysicsManager.PLANE)
        # triangle mesh
        sceneRigidBody.set_shape_type(GamePhysicsManager.TRIANGLEMESH)
        # heightfield
#         sceneRigidBody.set_shape_type(GamePhysicsManager.HEIGHTFIELD)
#         sceneRigidBody.set_shape_heightfield_file(dataDir + "/heightfield.png")
        # other common parameters
        sceneRigidBody.switch_body_type(BTRigidBody.STATIC)
        sceneRigidBodyNP.set_collide_mask(mask)
        # setup the player's rigid body
        sceneRigidBody.setup(sceneNP)

        # # Rigid Bodies
        # # box
#         physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
#                 "shape_type", "box")
#         # set various creation parameters as string for other rigid bodies
#         setParametersBeforeCreation("PlayerNP")
#         # get a player with anims, reparent to reference node, set transform
#         playerNP = getModelAnims("PlayerNP", 1.2, 4, playerAnimCtls)
#         playerNP.reparent_to(physicsMgr.get_reference_node_path())
#         playerNP.set_pos_hpr(LPoint3f(4.1, 0.0, 100.1),
#                 LVecBase3f(-75.0, 145.0, -235.0))        # create player's rigid_body (attached to the reference node)
#         playerRigidBodyNP = physicsMgr.create_rigid_body("PlayerRigidBody")
#         # get a reference to the player's rigid_body
#         playerRigidBody = playerRigidBodyNP.node()
         
#         # some clones of player with different shapes
#         # # sphere
#         playerSphereNP = physicsMgr.get_reference_node_path().attach_new_node("playerSphereNP")
#         playerNP.instance_to(playerSphereNP)
#         playerSphereNP.set_pos_hpr(LPoint3f(4.1, 0.0, 130.1),
#                LVecBase3f(145.0, -235.0, -75.0))
#         setParametersBeforeCreation("playerSphereNP")
#         physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
#                "shape_type", "sphere")
#         physicsMgr.create_rigid_body("PlayerRigidBodySphere")
#          
#         # # cylinder
#         playerCylinderNP = physicsMgr.get_reference_node_path().attach_new_node("playerCylinderNP")
#         playerNP.instance_to(playerCylinderNP)
#         playerCylinderNP.set_pos_hpr(LPoint3f(4.1, 0.0, 160.1),
#                LVecBase3f(145.0, -75.0, -235.0))
#         setParametersBeforeCreation("playerCylinderNP", "y")
#         physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
#                "shape_type", "cylinder")
#         physicsMgr.create_rigid_body("PlayerRigidBodyCylinder")
#          
#         # # capsule
#         playerCapsuleNP = physicsMgr.get_reference_node_path().attach_new_node("playerCapsuleNP")
#         playerNP.instance_to(playerCapsuleNP)
#         playerCapsuleNP.set_pos_hpr(LPoint3f(4.1, 0.0, 190.1),
#                LVecBase3f(-235.0, 145.0, -75.0))
#         setParametersBeforeCreation("playerCapsuleNP", "y")
#         physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
#                "shape_type", "capsule")
#         physicsMgr.create_rigid_body("PlayerRigidBodyCapsule")
#          
#         # # cone
#         playerConeNP = physicsMgr.get_reference_node_path().attach_new_node("playerConeNP")
#         playerNP.instance_to(playerConeNP)
#         playerConeNP.set_pos_hpr(LPoint3f(4.1, 0.0, 210.1),
#                LVecBase3f(-235.0, -75.0, 145.0))
#         setParametersBeforeCreation("playerConeNP", "y")
#         physicsMgr.set_parameter_value(GamePhysicsManager.RIGIDBODY,
#                "shape_type", "cone")
#         physicsMgr.create_rigid_body("PlayerRigidBodyCone")

        # # Soft Bodies
        sharedTS0 = TextureStage("sharedTS0")       
        # # rope
        # rope_node: this is a generic RopeNode to which
        # a NurbsCurveEvaluator could be associated.
        rope = RopeNode("Rope")
        rope.set_render_mode(RopeNode.RM_tube)
        rope.set_uv_mode(RopeNode.UV_parametric)
        rope.set_normal_mode(RopeNode.NM_none)
        rope.set_num_subdiv(4)
        rope.set_num_slices(8)
        rope.set_thickness(0.4)
        ropeNP = NodePath(rope)
        # RopeNode texturing
        ropeTex = TexturePool.load_texture(Filename("iron.jpg"))
        ropeNP.set_tex_scale(sharedTS0, 1.0, 1.0)
        ropeNP.set_texture(sharedTS0, ropeTex, 1)
        # create the rope soft body
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_type", "rope")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "points", "-17.75,-17.2,8.8:-17.75,-5.2,8.8")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "res", "8")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "fixeds", "1")
        ropeSoftBodyNP = physicsMgr.create_soft_body("RopeSoftBody")
        ropeSoftBodyNP.set_collide_mask(mask)
        ropeSoftBody = ropeSoftBodyNP.node()
        ropeSoftBody.setup(ropeNP)
        
        # # patch
        # GeomNode: this is a generic GeomNode to which
        # one or more Geoms could be added.
        patch = GeomNode("Patch")
        patchNP = NodePath(patch)
        # GeomNode texturing
        patchTex = TexturePool.load_texture(Filename("panda.jpg"))
        patchNP.set_tex_scale(sharedTS0, 1.0, 1.0)
        patchNP.set_texture(sharedTS0, patchTex, 1)
        # create the patch soft body
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_type", "patch")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "points",
                "-35.2,-15.5,15.8:-29.2,-15.5,15.8:-35.2,-21.5,15.8:-29.2,-21.5,15.8")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "res",
                "31:31")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "fixeds",
                "3")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "gendiags", "true")
        patchSoftBodyNP = physicsMgr.create_soft_body(
                "PatchSoftBody")
        patchSoftBody = patchSoftBodyNP.node()
        patchSoftBodyNP.set_collide_mask(mask)
        patchSoftBody.setup(patchNP)
        # generate bending constraints: must be done after soft body setup()
        patchMaterial = patchSoftBody.append_material()
        patchMaterial.set_linear_stiffness(0.4)
        patchSoftBody.generate_bending_constraints(2, patchMaterial)
        
        # # ellipsoid
        # GeomNode: this is a generic GeomNode to which
        # one or more Geoms could be added.
        ellipsoid = GeomNode("Ellipsoid")
        ellipsoidNP = NodePath(ellipsoid)
        ellipsoidNP.set_color(0.5, 0.0, 0.5, 1.0)
        ellipsoidNP.set_pos(LPoint3f(14.1, -10.0, 50.1))
        # create the ellipsoid soft body
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_type", "ellipsoid")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "points",
                "0.0,0.0,0.0")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "res",
                "128")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY, "radius",
                "1.5,1.5,1.5")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_total_mass", "30.0")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_mass_from_faces", "True")
        ellipsoidSoftBodyNP = physicsMgr.create_soft_body(
                "EllipsoidSoftBody")
        ellipsoidSoftBody = ellipsoidSoftBodyNP.node()
        ellipsoidSoftBodyNP.set_collide_mask(mask)
        ellipsoidSoftBody.setup(ellipsoidNP)
        # other features: must be done after soft body setup()
        ellipsoidSoftBody.get_material(0).set_linear_stiffness(0.1)
        ellipsoidSoftBody.get_cfg().set_dynamic_friction_coefficient(1)
        ellipsoidSoftBody.get_cfg().set_damping_coefficient(0.001)
        ellipsoidSoftBody.get_cfg().set_pressure_coefficient(1500)
        ellipsoidSoftBody.set_pose(True, False)

        # # trimesh
        # get a model: should have one only Geom
        trimeshNP = getModelAnims("trimeshNP", 1.0, 4, None)
        trimeshNP.set_pos(LPoint3f(30.1, -40.0, 20.1))
        trimeshNP.set_p(90)
        trimeshNP.ls()
        # embed model transform
        trimeshNP.flatten_strong()
        trimeshNP.ls()
        # create the trimesh soft body
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_type", "tri_mesh")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "randomize_constraints", "True")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_total_mass", "50.0")
        physicsMgr.set_parameter_value(GamePhysicsManager.SOFTBODY,
                "body_mass_from_faces", "True")
        trimeshSoftBodyNP = physicsMgr.create_soft_body(
                "TrimeshSoftBody")
        trimeshSoftBody = trimeshSoftBodyNP.node()
        trimeshSoftBodyNP.set_collide_mask(mask)
        trimeshSoftBody.setup(trimeshNP)
        # other features: must be done after soft body setup()
        trimeshSoftBody.generate_bending_constraints(2)
        trimeshSoftBody.get_cfg().set_positions_solver_iterations(2)
        trimeshSoftBody.get_cfg().set_collision_flag(
                BulletSoftBodyConfig.CF_vertex_face_soft_soft, True)
        
                
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

    # setup DEBUG DRAWING
    physicsMgr.init_debug()
    app.accept("d", toggleDebugDraw)
            
    # enable collision notify event: BTRigidBody_BTRigidBody_Collision
    physicsMgr.enable_collision_notify(GamePhysicsManager.COLLISIONNOTIFY, 10.0)
    app.accept("BTRigidBody_BTRigidBody_Collision", collisionNotify, 
               ["BTRigidBody_BTRigidBody_Collision"])
    app.accept("BTRigidBody_BTRigidBody_CollisionOff", collisionNotify, 
               ["BTRigidBody_BTRigidBody_CollisionOff"])

    # # first option: start the default update task for all plug-ins
    physicsMgr.start_default_update()
#     playerRigidBody.set_update_callback(rigidBodyCallback)
    ropeSoftBody.set_update_callback(softBodyCallback)
    globalClock = ClockObject.get_global_clock()

    # # second option: start the custom update task for all plug-ins
#     app.taskMgr.add(updateControls, "updateControls", 10, appendTask=True)

    # write to bam file on exit
    app.win.set_close_request_event("close_request_event")
    app.accept("close_request_event", writeToBamFileAndExit, [bamFileName])

    # place camera
    trackball = app.trackball.node()
    trackball.set_pos(10.0, 200.0, 15.0)
    trackball.set_hpr(0.0, 10.0, 0.0)
   
    # app.run(), equals to do the main loop in C++
    app.run()
