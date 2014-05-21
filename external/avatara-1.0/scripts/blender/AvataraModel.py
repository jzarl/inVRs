#!BPY

""" 
Name: 'Avatara Model (.mdl)...'
Blender: 237
Group: 'Export'
Tooltip: 'Export to MDL file format (.mdl).'
"""

__author__ = ["Helmut Garstenauer", "Martin Garstenauer", "Roland Landertshamer"]
__url__ = ("blender", "avatara", "http://vrc.zid.jku.at")
__version__ = "0.1"
__bpydoc__ = """\

Avatara Model Exporter

This script Exports a MDL file from blender.

"""

# CHANGES by RLANDER from 0.1 to 0.2:
# - updated script for compatibility with python version 2.5
# - added GUI for export
# - export of bone-transformation rewritten:
#  - works with armatures that have the same origin as the mesh

######################################################
# Importing modules
######################################################

import Blender
from Blender import *
from Blender import Draw, BGL
from Blender.NMesh import *
from Blender.Mathutils import *
import struct
from struct import *

print "Avatara Model Export Script"

sFilename = ""
sArmatureName = ""
sMeshName = ""

cByteOrder = '<'

tag = "AVATARA\0"
id = "MODEL\0\0\0\0\0\0\0"
version = 1


#==================================================#
# New name based on old with a different extension #
#==================================================#
def newFName(ext):
	return Blender.Get('filename')[: -len(Blender.Get('filename').split('.', -1)[-1]) ] + ext


#==================================================#
#  ExportSelection class			   #
#==================================================#
class ExportSelection:
  
  objSel = 1
  armSel = 1
  boneSel = 1
  objMenu = None
  armMenu = None
  boneMenu = None

  meshObjects = None
  armatureObjects = None
  rootBones = None
  sObjSelection = ""
  sArmSelection = ""
  sBoneSelection = "Bones %t"

  def __init__(self):
    allObjects = Blender.Object.Get()
    self.meshObjects = {}
    index = 0
    self.sObjSelection = "Objects %t"
    for object in allObjects:
      if object.getType() == "Mesh":
        self.meshObjects[index] = object
        index = index + 1
        self.sObjSelection = self.sObjSelection + "|" + object.name + " %" + "x" + ("%i" % index)
    print self.sObjSelection
    
    allArmatures = Blender.Armature.Get()
    self.armatureObjects = {}
    index = 0
    self.sArmSelection = "Armatures %t"
    for armature in allArmatures:
      self.armatureObjects[index] = armature
      index = index + 1
      self.sArmSelection = self.sArmSelection + "|" + armature + " %" + "x" + ("%i" % index)
    if index > 0:
        self.CalculateRootBones(1)
    print self.sArmSelection

  def CalculateRootBones(self, armIndex):
    sArmature = self.armatureObjects[armIndex-1]
    armature = Blender.Armature.Get(sArmature)
    bones = armature.bones.values()
    self.rootBones = {}
    index = 0
    self.sBoneSelection = "Bones %t"
    for bone in bones:
      if not bone.hasParent():
        self.rootBones[index] = bone
        index = index + 1
        self.sBoneSelection = self.sBoneSelection + "|" + bone.name + " %" + "x" + ("%i" % index)
    
  def event(self, evt, val):    # the function to handle input events
    if evt == Draw.ESCKEY:
      Draw.Exit()                 # exit when user presses ESC
      return

  def button_event(self, evt):  # the function to handle Draw Button events
    if evt == 1:
      self.objSel = self.objMenu.val
      Draw.Redraw(1)
    if evt == 2:
      self.armSel = self.armMenu.val
      self.boneSel = 1
      self.CalculateRootBones(self.armSel)
      Draw.Redraw(1)
    if evt == 3:
      self.boneSel = self.boneMenu.val
      Draw.Redraw(1)
    if evt == 4:
      if self.objSel == 0 or self.armSel == 0 or self.boneSel == 0:
        Draw.PupMenu("Error: You have to select an object,armature and root bone!")
      else:
        exportFile(self.meshObjects[self.objSel-1].name, self.armatureObjects[self.armSel-1], self.rootBones[self.boneSel-1].name)
        Draw.Exit()
    if evt == 5:
      Draw.Exit()	# exit when user presses Cancel

  def gui(self):              # the function to draw the screen
    BGL.glClearColor(0,0,0.5,1)
    BGL.glClear(BGL.GL_COLOR_BUFFER_BIT)
    BGL.glColor3f(1,1,1)
    
    BGL.glRasterPos2i(20,200)
    Draw.Text("Select the object to export:", "normal")
    self.objMenu = Draw.Menu(self.sObjSelection, 1, 20, 170, 200, 20, self.objSel, "Select the animated object.")
    
    BGL.glRasterPos2i(20,150)
    Draw.Text("Select the armature to export:", "normal")
    self.armMenu = Draw.Menu(self.sArmSelection, 2, 20, 120, 200, 20, self.armSel, "Select the matching armature.")
    
    BGL.glRasterPos2i(20,100)
    Draw.Text("Select the root bone of the armature:", "normal")
    self.boneMenu = Draw.Menu(self.sBoneSelection, 3, 20, 70, 200, 20, self.boneSel, "Select the Root Bone of the armature.")
    
    Draw.PushButton("Export", 4, 250, 150, 50, 20, "Export File")
    Draw.PushButton("Cancel", 5, 250, 110, 50, 20, "Cancel")
    
    BGL.glRasterPos2i(72, 16)
    string = "Selections: " + ("%s" % self.objSel) + " / " + ("%s" % self.armSel) + " / " + ("%s" % self.boneSel)
    
    Draw.Text(string, "small")


#==================================================#
#  BoneCollector class  			   #
#==================================================#
class BoneCollector:
    """Writes bone-data of an armature."""    
        
    armature = None
    noOfBones = 0
    boneMap = None
    rootBoneName = ""
    
    
    def __init__(self, armature, sRootBone):
        self.armature = armature
        self.noOfBones = 0
        self.boneMap = {}
        self.rootBoneName = sRootBone
        self.CollectBones()
    

    def GetRootBones(self):
        bones = self.armature.bones.values()
        # Attention: Older Blender versions of getBones()
        # return only the root bone.
        # Newer versions return all bones!
        # --> We want only the root bone.
        if len(bones) > 0:
            # Assume that first bone in list is root bone.
            rootBone = []
            for bone in bones:
                if bone.name == self.rootBoneName:
                    rootBone = [bone]
            bones = rootBone
            #bones = [bones[0]]
        else:
            bones = []
        return bones
        
    def CollectBones(self, bones = None):        
        if bones == None:   
            bones = self.GetRootBones()         
        print "Found Bone with name "
        for bone1 in bones:
            print bone1.name
        for bone in bones:
        #print "boneMap[", bone.name,"] = ", self.noOfBones
            self.boneMap[bone.name] = self.noOfBones
            self.noOfBones = self.noOfBones + 1
            if bone.hasChildren():
                self.CollectBones(bone.children)
                        

    def IndexOfBone(self, name):
        return self.boneMap[name]


    def WriteBones(self, file):       
        # Write rest-state of armature
        self.WriteRestState(file)
        

    def WritePoses(self, file, poses = None):
        if poses == None:
            poses = []
               
        # Write poses
        lastFrame = Blender.Get('curframe')
        for pose in poses:
            Blender.Set('curframe', pose)
            Blender.Redraw()
            
            # Write frame time to file
            time = (pose - poses[0]) * nMillisecondsPerFrame
            file.write(pack(cByteOrder+"i", time))
            
            # Write bones to file
            self.WritePose(file)
        Blender.Set('curframe', lastFrame)
        

    def WriteRestState(self, file, bones = None):
        if bones == None:
            bones = self.GetRootBones()
        for bone in bones:
            # Determine parent
            parentBone = -1
            if bone.hasParent():
                parentBone = self.IndexOfBone(bone.parent.name)
            # Parent bone
            file.write(pack(cByteOrder+"i", parentBone))
            # Bone name
            name = bone.name
            if len(name) > 15:
                name = name[:15]
            file.write(name)
            rest = 16 - len(name)
            for x in range(rest):
                file.write("\0")
            #
            ##########################################
            # X-Axis = red
            # Y-Axis = green 
            # Z-Axis = blue
            ##########################################
            #
            armature = Blender.Object.Get(sArmatureName)
            mesh = Blender.Object.Get(sMeshName)
            meshMatrix = mesh.matrix
            meshMatrix.invert()
            armatureMatrix = armature.matrix * meshMatrix
            ##########################################
            # Bone head
            if bone.hasParent():
                parentBoneLength = Matrix((1,0,0,0),(0,1,0,0),(0,0,1,0),(0,bone.parent.length,0,1))
                parentTailboneMatrix = parentBoneLength * bone.parent.matrix['ARMATURESPACE']
                parentTailboneMatrix.invert()
                boneOffsetMatrix = bone.matrix['ARMATURESPACE'] * parentTailboneMatrix 
                boneHead = boneOffsetMatrix.translationPart()
            else:
            	boneHead = bone.head['ARMATURESPACE'] * armatureMatrix
            file.write(pack(cByteOrder+"fff", boneHead[0], boneHead[1], boneHead[2]))
            # Bone tail
            if bone.hasParent():
                parentBoneLength = Matrix((1,0,0,0),(0,1,0,0),(0,0,1,0),(0,bone.parent.length,0,1))
                parentTailboneMatrix = parentBoneLength * bone.parent.matrix['ARMATURESPACE']
                parentTailboneMatrix.invert()
                boneLength = Matrix((1,0,0,0),(0,1,0,0),(0,0,1,0),(0,bone.length,0,1))
                tailboneMatrix = boneLength * bone.matrix['ARMATURESPACE']
                boneOffsetMatrix = tailboneMatrix * parentTailboneMatrix
                boneTail = boneOffsetMatrix.translationPart() 
            else:
                boneTail = bone.tail['ARMATURESPACE'] * armatureMatrix
            file.write(pack(cByteOrder+"fff", boneTail[0], boneTail[1], boneTail[2]))
            # Bone roll
            file.write(pack(cByteOrder+"f", bone.roll['BONESPACE']*3.14159265/180))
            # Children
            print "----------------------------------"
            print "BONE ", bone.name
            print "  "
            print bone.options
            print "Head = ", boneHead
            print "Tail = ", boneTail
            print "Roll = ", bone.roll['BONESPACE']
            print "Matrix = "
            
            if bone.hasChildren():
                self.WriteRestState(file, bone.children)


    def WritePose(self, file, bones = None):
        if bones == None:
            bones = self.GetRootBones()
        for bone in bones:
            # Bone Quaternion
            file.write(pack(cByteOrder+"ffff", bone.getQuat()[0], bone.getQuat()[1], bone.getQuat()[2], bone.getQuat()[3]))
            # Bone location
            file.write(pack(cByteOrder+"fff", bone.getLoc()[0], bone.getLoc()[1], bone.getLoc()[2]))
            #Bone size
            file.write(pack(cByteOrder+"fff", bone.getSize()[0], bone.getSize()[1], bone.getSize()[2]))
            # Children
            self.WritePose(file, bone.getChildren())


######################################################
# EXPORT
######################################################
def save_mdl(sMeshFilename):
	global sFilename
	
	sFilename = sMeshFilename
	
	es = ExportSelection()
	Draw.Register(es.gui, es.event, es.button_event)  # registering the 3 callbacks
	
def exportFile(sObjectName, sArmature, sRootBone):
	global sArmatureName
	global sMeshName
	
	bExportBones = 1
	bc = None
	sArmatureName = sArmature
	sMeshName = sObjectName
	if bExportBones:
	    armature = Blender.Armature.Get(sArmature)
	    if not armature:
	        print "Error: Armature ", sArmature, " NOT found!"
	        # Ignore bones
	        bExportBones = 0
	    else:
	        print "Armature ", sArmature, " found!"
	        bc = BoneCollector(armature, sRootBone)

	# Mesh export
	object = Blender.Object.Get(sObjectName)
	mesh = object.getData()
	#mesh = NMesh.GetRaw(sMeshName)
	
	if not mesh:
	    print "Mesh from object ", sObjectName, " NOT found!"
	else:
	    print "Mesh from object ", sObjectName, " found!"
	    file = open(sFilename, 'wb')

	    # Header
	    file.write(tag)
	    file.write(id)
	    file.write(pack(cByteOrder+"i", version))
	    
	    # Number of vertices    
	    file.write(pack(cByteOrder+"i", len(mesh.verts)))
	    
	    # Number of faces (triangles only)
	    noOfFaces = 0
	    for face in mesh.faces:
	        # Count triangles
	        if len(face.v) == 3:
	            noOfFaces = noOfFaces + 1
	    file.write(pack(cByteOrder+"i", noOfFaces))
	    
	    # Number of bones
	    if bExportBones == 0:
	        file.write(pack(cByteOrder+"i", 0))
	    else:
	        file.write(pack(cByteOrder+"i", bc.noOfBones))
	   
	    # Reserved bytes
	    file.write(pack(cByteOrder+"iii", 0, 0, 0))
	    
	    # Vertices
	    for vertex in mesh.verts:
	        # Coordinate        
	        file.write(pack(cByteOrder+"fff", vertex.co[0], vertex.co[1], vertex.co[2]))
	        # Normal vector        
	        file.write(pack(cByteOrder+"fff", vertex.no[0], vertex.no[1], vertex.no[2]))
	        # Bone, that influence this vertex
	        #print "Groups = ", len(mesh.getVertGroupNames())
		
		if bExportBones == 1:
	            influences = mesh.getVertexInfluences(mesh.verts.index(vertex))
	            # Number of bones
	            file.write(pack(cByteOrder+"i", len(influences)))
	            for influence in influences:
	                bone = influence[0]     # Index of bone
	                weight = influence[1]   # Weight of bone
	                # Bone index and weight
	                file.write(pack(cByteOrder+"i", bc.IndexOfBone(bone)))
	                file.write(pack(cByteOrder+"f", weight))

	    # Faces
	    for face in mesh.faces:
	        # Vertex Indices
	        if len(face.v) != 3:
	            print "Warning: Non-triangle face found!"
	            print "(Number of vertices: ", len(face.v), ")"
	            print "Coordinates: ", face.v[0].co
	        else:
	            for vertex in face.v:
	                file.write(pack(cByteOrder+"i", mesh.verts.index(vertex)))
	            
	            # Smooth
	            if face.smooth == 0:
	                file.write(pack(cByteOrder+"i", 0))
	            else:
	                file.write(pack(cByteOrder+"i", 1))
	                
	            # Normal vector
	            file.write(pack(cByteOrder+"fff", face.normal[0], face.normal[1], face.normal[2]))
	    
	            # Vertex colors
	            #print "Vertex: ", v, " - Vertex Colors: ", face.col(
	            for i in range(0,3):
	                if len(face.col)!=4:
	                    # Write default color           
	                    file.write(pack(cByteOrder+"BBBB", 100, 100, 100, 255))
	                else:
	                    color = face.col[i]
	                    file.write(pack(cByteOrder+"BBBB", color.r, color.g, color.b, color.a))
	            
	            # UV coordinates
	            if len(face.uv) != 3:
	                for i in range(0,3):
	                    file.write(pack(cByteOrder+"ff", 0.0, 0.0))
	            else:
	                for uv in face.uv:
	                    file.write(pack(cByteOrder+"ff", uv[0], uv[1]))
	                    
	    # Bones
	    if bExportBones == 1:
	        bc.WriteBones(file)
	        
	    file.close()

Blender.Window.FileSelector(save_mdl, "Export Avatara MDL", newFName('mdl'))