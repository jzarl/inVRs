#!BPY

""" 
Name: 'Avatara Animation (.ani)...'
Blender: 237
Group: 'Export'
Tooltip: 'Export to ANI file format (.ani).'
"""

__author__ = ["Helmut Garstenauer", "Martin Garstenauer", "Roland Landertshamer"]
__url__ = ("blender", "avatara", "http://vrc.zid.jku.at")
__version__ = "0.1"
__bpydoc__ = """\

Avatara Animation Exporter

This script Exports an ANI file from blender.

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
from Blender.Armature import *
from Blender.Armature.NLA import *
from Blender.NMesh import *
from Blender.Mathutils import *
import struct
from struct import *

print "Avatara Animation Export Script"

sFilename = ""

cByteOrder = '<'

tag = "AVATARA\0"
id = "ANIMATION\0\0\0"
version = 1

#==================================================#
# New name based on old with a different extension #
#==================================================#
def newFName(ext):
	return Blender.Get('filename')[: -len(Blender.Get('filename').split('.', -1)[-1]) ] + ext

class ExportSelection:
  
  objSel = 1
  armSel = 1
  boneSel = 1
  startAnim = 1
  endAnim = 1
  timeSel = 40
  objMenu = None
  armMenu = None
  boneMenu = None
  startMenu = None
  endMenu = None
  timeMenu = None

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
	frames = range(self.startAnim, self.endAnim)
	exportFile(self.meshObjects[self.objSel-1].name, self.armatureObjects[self.armSel-1], self.rootBones[self.boneSel-1].name, frames, self.timeSel)
	Draw.Exit()
    if evt == 5:
      Draw.Exit()	# exit when user presses Cancel
    if evt == 6:
      self.startAnim = self.startMenu.val
      if self.startAnim > self.endAnim:
	self.endAnim = self.startAnim
	Draw.Redraw(1)
    if evt == 7:
      self.endAnim = self.endMenu.val
      if self.endAnim < self.startAnim:
	self.startAnim = self.endAnim
	Draw.Redraw(1)
    if evt == 8:
      self.timeSel = self.timeMenu.val

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
    
    BGL.glRasterPos2i(250, 200)
    Draw.Text("Select start- and end-frame:", "normal")
    self.startMenu = Draw.Number("", 6, 250, 170, 60, 20, self.startAnim, 1, 20000, "Select the start frame")
    self.endMenu = Draw.Number("", 7, 330, 170, 60, 20, self.endAnim, 1, 20000, "Select the end frame")
    
    BGL.glRasterPos2i(250, 150)
    Draw.Text("Select msecs per frame:", "normal")
    self.timeMenu = Draw.Number("", 8, 280, 120, 60, 20, self.timeSel, 1, 1000, "Select the number of milliseconds per frame")
    
    Draw.PushButton("Export", 4, 260, 70, 50, 20, "Export File")
    Draw.PushButton("Cancel", 5, 320, 70, 50, 20, "Cancel")
    
    BGL.glRasterPos2i(72, 16)
    string = "Selections: " + ("%s" % self.objSel) + " / " + ("%s" % self.armSel) + " / " + ("%s" % self.boneSel)
    Draw.Text(string, "small")

class BoneCollector:
    """Writes bone-data of an armature."""    
    
    version = 12
    armature = None
    noOfBones = 0
    boneMap = None
    pose = None
    rootBoneName = ""
    
    
    def __init__(self, armature, sRootBone, pose):
	self.armature = armature
	self.pose = pose
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
	    print "Found Root Bone with name "
	    for bone1 in bones:
		    print bone1.name
        for bone in bones:
            self.boneMap[bone.name] = self.noOfBones
            self.noOfBones = self.noOfBones + 1
	    if bone.hasChildren():
            	self.CollectBones(bone.children)
                        

    def IndexOfBone(self, name):
        return self.boneMap[name]


    def WriteBones(self, file):
        # Write rest-state of armature
        self.WriteRestState(file)
        

    def WritePoses(self, file, sArmature, nMillisecondsPerFrame,  poses = None):
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
            self.WritePose(file, sArmature)
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
            armatureMatrix = armature.matrix
            ##########################################
            # Bone head
            boneHead = bone.head['ARMATURESPACE']
            if bone.hasParent():
            # WORKS WHEN ARMATURE STARTS AT (0,0)
            	boneHead = boneHead - bone.parent.tail['ARMATURESPACE']
            	parentBoneMatrix = armatureMatrix * bone.parent.matrix['ARMATURESPACE']
            	parentBoneMatrix = Matrix(parentBoneMatrix).invert().rotationPart()
            	boneHead = boneHead * parentBoneMatrix
            # TEST FOR TRANSLATED ARMATURES
            #	parentBoneMatrix =  armatureMatrix.rotationPart() * bone.parent.matrix['ARMATURESPACE'].rotationPart()
            #	parentBoneMatrix = parentBoneMatrix.invert()
            #	boneHead = boneHead * parentBoneMatrix
            boneHead = boneHead * armatureMatrix
            file.write(pack(cByteOrder+"fff", boneHead[0], boneHead[1], boneHead[2]))
            # Bone tail
            boneTail = bone.tail['ARMATURESPACE'] - bone.head['ARMATURESPACE']
            if bone.hasParent():
            # WORKS WHEN ARMATURE STARTS AT (0,0)
            	parentBoneMatrix = armatureMatrix * bone.parent.matrix['ARMATURESPACE']
            	parentBoneMatrix = Matrix(parentBoneMatrix).invert().rotationPart()
            	boneTail = boneTail * parentBoneMatrix + boneHead
            # TEST FOR TRANSLATED ARMATURES
            #	parentBoneMatrix = armatureMatrix.rotationPart() * bone.parent.matrix['ARMATURESPACE'].rotationPart()
            #	parentBoneMatrix = parentBoneMatrix.invert()
            #	boneTail = boneTail * parentBoneMatrix
            boneTail = boneTail * armatureMatrix
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

    def GetLocalPoseMatrix(self, armObject, bone, posebone):
	if not bone.hasParent():
		return posebone.localMatrix
	else:
		parentBone = bone.parent
		parentPosebone = self.pose.bones[bone.parent.name]
		newMatrix = posebone.localMatrix * Matrix(parentPosebone.poseMatrix).invert() * parentBone.matrix['ARMATURESPACE']
		return bone.matrix['ARMATURESPACE'] * newMatrix * Matrix(bone.matrix['ARMATURESPACE']).invert()
	

    def WritePose(self, file, sArmature, bones = None):
	armObject = Blender.Object.Get(sArmature)
	if bones == None:
		bones = self.GetRootBones()
	for bone in bones:
		posebone = self.pose.bones[bone.name]

		bonematrix = self.GetLocalPoseMatrix(armObject, bone, posebone)

#		print "Final Bonematrix (", bone.name,"):"
#		print bonematrix
#		print "--------------------------------------"

		rotation = bonematrix.rotationPart()
		trans = bonematrix.translationPart()

		quat = rotation.toQuat()
		file.write(pack(cByteOrder+"ffff", quat[0], quat[1], quat[2], quat[3]))
		# Bone location
		file.write(pack(cByteOrder+"fff", trans[0], trans[1], trans[2]))
		#Bone size
		file.write(pack(cByteOrder+"fff", 1, 1, 1))
            # Children
		if bone.hasChildren():
			self.WritePose(file, sArmature, bone.children)

# Animation export
######################################################
# EXPORT
######################################################
def save_ani(sMeshFilename):
	global sFilename
	
	sFilename = sMeshFilename
	
	es = ExportSelection()
	Draw.Register(es.gui, es.event, es.button_event)  # registering the 3 callbacks
	
def exportFile(sObjectName, sArmature, sRootBone, frames, nMillisecondsPerFrame):
#	try:
	armature = Blender.Armature.Get(sArmature)
	pose = Blender.Object.Get(sArmature).getPose()
	if not armature:
	    print "Armature ", sArmature, " NOT found!"
	else:
	    if not pose:
		    print "Pose from ", sArmature, " NOT found!"
	    print "Armature ", sArmature, " found!"
	    file = open(sFilename, 'wb')
	
	    # Header
	    file.write(tag)
	    file.write(id)
	    file.write(pack(cByteOrder+"i", version))

	    bc = BoneCollector(armature, sRootBone, pose)

	    # Number of bones
	    file.write(pack(cByteOrder+"i", bc.noOfBones))

	    # Number of poses
	    file.write(pack(cByteOrder+"i", len(frames)))        

	    # Reserved bytes
	    file.write(pack(cByteOrder+"ii", 0, 0))
	    
	    # Poses (Animation)
	    bc.WritePoses(file, sArmature, nMillisecondsPerFrame, frames)
	    file.close()
	    
#	except:
#	    print sArmature, " NOT found!"

Blender.Window.FileSelector(save_ani, "Export Avatara ANI", newFName('ani'))