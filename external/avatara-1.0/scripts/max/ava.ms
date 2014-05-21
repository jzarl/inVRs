-- ava.ms			export script for avatara viewer
-- viewer and fileformat 	helmut garstenauer & martin garstenauer
-- scriptcode by 		marlene hochrieser 
-- mail 			enel@gmx.at
-- changed 			28.07.2005
-- versions 			3ds max 5, 6, 7, 9 edition

utility avatar "AVATARA EXPORT" rolledUp:false (
-- -----------------------------------local-begin-------------------------------------------------
-- file locals
local fMdlB							-- filePointer binary-mode to .mdl file
local fAniB							-- filePointer binary-mode to .ani file
local fMdlNameB						-- name + extension of file xxxB.mdl
local fAniNameB						-- name + extension of file xxxB.ani
local pathN = ""
local fileFull = ""

-- geometry locals
local selBone						-- selected bone
local selMesh						-- selected mesh
local vCount = 0					-- # vertices
local fCount = 0					-- # faces
local bCount = 0					-- # bones
local pCount = 0					-- # poses
local boneArr = #()					-- bone array
local boneName = #()				-- look-up-table for bonenames to get the index for boneArr
local boneArrFirst = #()			-- save first transform matrix of every bone in same order as boneArr

-- name and string locals
local arm = "ArmUp.R"
local head = "Head"
local spine = "Spine"
local myArm = ""
local myHead = ""
local mySpine = ""

-- animation locals
local curFrame
local frameArr = #()				-- frame array for exporting all keyframes

-- modifier locals
local skinMod						-- skin modifier of selected object

-- gui locals
local gChoice = 1
local gFrom = (int) animationRange.start
local gTo = (int) animationRange.end
local gStep = 10
-- -----------------------------------local-end---------------------------------------------------

-- ----------------------------------function-begin-----------------------------------------------
-- export modell file
fn exportMdl = (
	-- header begin
	WriteString fMdlB "AVATARA"				-- file tag (8 bytes)
	WriteString fMdlB "MODEL"				-- file type (12 bytes)
	WriteShort  fMdlB 0
	WriteLong 	fMdlB 0
	WriteLong   fMdlB 2						-- file version 2 for 3ds max
	WriteLong   fMdlB vCount
	WriteLong 	fMdlB fCount
	WriteLong   fMdlB bCount 
	WriteLong   fMdlB 0						-- 3*4 Byte reserved
	WriteLong   fMdlB 0
	WriteLong   fMdlB 0
	-- header end
	
	-- vertex list begin
	max modify mode							-- skinOps need modify panel open and modifier selected
	select selMesh
	modPanel.setCurrentObject $.Skin
	
	for i = 1 to vCount do (
		cpt = (getVert selMesh i)
		WriteFloat fMdlB cpt.x				-- coord (x,y,z)
		WriteFloat fMdlB cpt.y
		WriteFloat fMdlB cpt.z
		nv = (getNormal selMesh i)
		WriteFloat fMdlB nv.x				-- normal vektor (x,y,z)
		WriteFloat fMdlB nv.y
		WriteFloat fMdlB nv.z
		vwc = skinOps.GetVertexWeightCount $.Skin i
		WriteLong fMdlB vwc					-- # of affecting bones
		for j = 1 to vwc do (
			id = skinOps.GetVertexWeightBoneID $.Skin i j 				-- get id, from the n-th bone
			vw = skinOps.GetVertexWeight $.Skin i j
			t = (findItem boneName (skinOps.GetBoneName $.Skin id 1))-1 --map id to index of boneArr
			WriteLong fMdlB t				-- index of affecting bone
			WriteFloat fMdlB vw				-- weight of affecting bone
		)
	)
	-- vertex list end
	
	-- face list begin
	for i = 1 to fCount do (
		cf = (getFace selMesh i)
		ctf = (getTVFace selMesh i)
		WriteLong fMdlB (cf.x-1)			-- vertex-index (vert1, vert2, vert3)
		WriteLong fMdlB (cf.y-1)			-- saved as point3
		WriteLong fMdlB (cf.z-1)
		
		WriteLong fMdlB 1					-- for smooth write 1
	
		nv = (getFaceNormal selMesh i)
		WriteFloat fMdlB nv.x				-- nv (x,y,z) for each face
		WriteFloat fMdlB nv.y
		WriteFloat fMdlB nv.z
		
		ctp1 = getTVert selMesh ctf.x		-- get coords(x, y, z) for vert1 from texture mesh
		ctp2 = getTVert selMesh ctf.y		-- get coords(x, y, z) for vert2 from texture mesh
		ctp3 = getTVert selMesh ctf.z		-- get coords(x, y, z) for vert3 from texture mesh
		
		WriteByte fMdlB 128				    --vertex red for x
		WriteByte fMdlB 128					--vertex green
		WriteByte fMdlB 128					--vertex blue
		WriteByte fMdlB 255					--vertex alpha

		WriteByte fMdlB 128					--vertex red for y
		WriteByte fMdlB 128					--vertex green
		WriteByte fMdlB 128					--vertex blue  
		WriteByte fMdlB 255					--vertex alpha
		
		WriteByte fMdlB 128				    --vertex red for z
		WriteByte fMdlB 128					--vertex green 
		WriteByte fMdlB 128					--vertex blue 
		WriteByte fMdlB 255				    --vertex alpha
		
		WriteFloat fMdlB ctp1.x				-- u coord for x
		WriteFloat fMdlB ctp1.y				-- v coord
		
		WriteFloat fMdlB ctp2.x				-- u coord for y
		WriteFloat fMdlB ctp2.y				-- v coord
		
		WriteFloat fMdlB ctp3.x				-- u coord for z
		WriteFloat fMdlB ctp3.y				-- v coord
	)
	-- face list end

	-- bone list begin
	for i = 1 to bCount do (
		selBone = boneArr[i]								-- select next bone from bone-array
		selParent = findItem boneArr boneArr[i].parent		-- find parent bone in array
		-- write parent bone ID, beginning with 0, -1 for no parent, parent before child
		WriteLong fMdlB (selParent-1)			
		if selBone.name == myArm then (		-- special vr-kinematik bone
			WriteString fMdlB "ArmUp.R"		-- takes 8 bytes 
			WriteLong	fMdlB 0
			WriteLong 	fMdlB 0				-- takes 8 bytes = 16 bytes
		)else if selBone.name == myHead then (
			WriteString fMdlB "Head"		-- takes 5 bytes 
			WriteLong	fMdlB 0
			WriteLong	fMdlB 0				-- takes 8 bytes
			WriteShort	fMdlB 0				-- takes 2 bytes
			WriteByte	fMdlB 0				-- takes 1 byte = 16 bytes
		)else if selBone.name == mySpine then (
			WriteString fMdlB "Spine"		-- takes 6 bytes 
			WriteLong	fMdlB 0
			WriteLong	fMdlB 0				-- takes 8 bytes
			WriteShort	fMdlB 0				-- takes 2 bytes = 16 bytes
		)else(
			cnt = selBone.name.count
			if cnt > 15 then (				-- if bonename > 15 chars, then cut
				WriteString fMdlB (substring selBone.name 1 15)	
			) else (
				WriteString fMdlB selBone.name
				for i = cnt to 14 do (		-- string of 15 bytes + 0\
					WriteByte fMdlB 0								
				)
			)
		)
		append boneArrFirst selBone.transform	-- get restpose for all bones
		WriteFloat fMdlB selBone.transform.row1.x	
		WriteFloat fMdlB selBone.transform.row1.y
		WriteFloat fMdlB selBone.transform.row1.z
			
		WriteFloat fMdlB selBone.transform.row2.x	
		WriteFloat fMdlB selBone.transform.row2.y
		WriteFloat fMdlB selBone.transform.row2.z
			
		WriteFloat fMdlB selBone.transform.row3.x	
		WriteFloat fMdlB selBone.transform.row3.y
		WriteFloat fMdlB selBone.transform.row3.z
				
		WriteFloat fMdlB selBone.transform.row4.x	
		WriteFloat fMdlB selBone.transform.row4.y
		WriteFloat fMdlB selBone.transform.row4.z
			
		WriteFloat fMdlB selBone.length
	)
	-- bone list end
)--fn exportMdl


-- write rotation, position and scaling for all bones of one frame
fn writePose frame = ( 
	for i = 1 to bCount do(			
		
		parid = findItem boneArr boneArr[i].parent
		if parid > 0 then (
			mx = inverse(boneArrFirst[parid]) * boneArr[parid].transform		-- get local transformation of parent bone
			mt = boneArr[i].transform * inverse(mx) * inverse(boneArrFirst[i]) 	-- get local transformation without parent
		)else(
			mt = boneArr[i].transform * inverse(boneArrFirst[i]) 				-- get local transformation
		)
		
		WriteFloat fAniB mt.rotationpart.w				--rotation as quaternion (s, i, j, k)
		WriteFloat fAniB (mt.rotationpart.x * -1)
		WriteFloat fAniB (mt.rotationpart.y * -1)
		WriteFloat fAniB (mt.rotationpart.z * -1)
				
		WriteFloat fAniB mt.translationpart.x		-- location (x, y, z)
		WriteFloat fAniB mt.translationpart.y		 
 		WriteFloat fAniB mt.translationpart.z
		
		WriteFloat fAniB mt.scalepart.x				-- scale (sx, sy, sz)
		WriteFloat fAniB mt.scalepart.y
		WriteFloat fAniB mt.scalepart.z
	)
)

--export animation file
fn exportAni = (
	-- header begin
	WriteString  fAniB "AVATARA"						-- file tag (8 bytes)
	WriteString  fAniB "ANIMATION"						-- file type (12 bytes)
	WriteShort   fAniB 0
	WriteLong 	 fAniB 1								-- file version 
	WriteLong 	 fAniB bCount							-- # bones
	WriteLong 	 fAniB pCount							-- # poses
	WriteLong 	 fAniB 0								-- 2*4 Byte reserved
	WriteLong 	 fAniB 0
	-- header end

	-- pose list begin
	case gChoice of(
		1: ( 
			for i = gFrom to gTo by gStep do (
				sliderTime = i										-- set slider to fetched frame
				curFrame = i										-- get frame from stepsize
				--WriteLong fAniB (((curFrame as integer) - gFrom) * frameRate/1000 as float)
				WriteLong fAniB (((curFrame as integer) - gFrom) * 1000/frameRate as float)
				writePose curFrame ) 								-- export pose for all bones
		)
		2: (  
			for j = 1 to pCount do (					
				slidertime = frameArr[j]							-- set slider to fetched frame
				curFrame = frameArr[j]								-- first curFrame is animation.start
				--WriteLong fAniB ((curFrame as integer) * frameRate/1000 as float)
				WriteLong fAniB ((curFrame as integer) * 1000/frameRate as float)
				select selMesh
				writePose curFrame 									-- export pose for all bones
				select $* )
		)
	)
	sliderTime = animationRange.start					-- reset Slider
	select selMesh
	-- pose list end
) --animation export end

-- is everything ready for export (selection, path,...)??
fn init = (
	set coordsys parent
	-- user input done ???
	if fileFull.count < 1 then (
		messagebox "insert filename"
		return false
	)
	if not selection.count == 1 then (
		messagebox "select object"
		return false
	)
	if classOf selection[1].baseObject != Editable_Mesh then (
		messagebox "select mesh"
		return false
	)
	if ((myArm.count < 1) or (myHead.count < 1) or (mySpine.count < 1)) then (
		messagebox "don't forget to assign vr-bones"
		return false
	)
	
	-- assign vars
	selMesh = selection[1]								-- save mesh-selection
	vCount = selMesh.numverts							-- # vertices
	fCount = selMesh.numfaces							-- # faces
	if rFrames == 3 then fCount = 0 
	boneArr = #()										-- reset array
	skinBool = false							
	for i = 1 to selMesh.modifiers.count do(
		if (selMesh.modifiers[i].name == "Skin") then skinBool = true	-- is there a skin modifier in the stack?
	)
	if not(skinBool) then (
		messagebox "skin modifier needed"
		return false
	)
	
	-- manage framelist
	if gChoice == 1 then (
		if gFrom > gTo then (
			messagebox "something wrong with from/to ?"
			return false
		) else (
			pCount = (floor ((gTo-gFrom)/gStep)) + 1				-- set # of poses
		)
	) else if gChoice == 2 then (
		if gFrom > gTo then (
			messagebox "something wrong with from/to ?" 
			return false 
		) else (
			addnewKey selMesh.controller gTo
			addnewKey selMesh.controller gFrom
			sliderTime = gFrom								-- beginn with frame set in spinner from
			curFrame = gFrom
			select $*										-- select all object to get each set keyframe				
			while curFrame < gTo do (						-- until frame to is reached
					sliderTime = curFrame 			-- set slider to fetched frame
					append frameArr sliderTime				-- add sliderTime to frameArr
					curFrame = trackbar.GetNextKeyTime() 	-- get frame from next set keyframe
			)
			sliderTime = curFrame 		
			append frameArr sliderTime
			sliderTime = animationRange.start
			pCount = frameArr.count
		)
	)
	
	-- add all bones to boneArr beginning with the root bone and store parent before child
	boneArr = #()
	for selBone in objects  
		where ((classOf selBone == BoneGeometry) and ((selBone.parent == undefined) or (classOf selBone.parent != BoneGeometry)))  
		 	do (
		 			join boneArr selBone
		 	)
	-- since in for-loop children were added, which are not of class BoneGeometry, we delete any other objects from boneArr
	bCount = boneArr.count
	boneLast = boneArr[bCount]
	boneTmp = boneArr[1] 
	i = 1 
	while boneTmp != boneLast do( 
		if (classOf boneTmp == BoneGeometry) then ( 
			i = i+1 
			boneTmp = boneArr[i] 
		)else( 
			deleteItem boneArr i
			boneTmp = boneArr[i]				-- after deleting boneArr[i] boneTmp is invalid (pointer)
			boneLast = boneArr[boneArr.count]	-- same for boneLast
		)
	)
	bCount = boneArr.count
	
	for i = 1 to bCount do(
		join boneName #(boneArr[i].name)
	)
	
	sliderTime = 0
	return true
)
-- ----------------------------------function-end-------------------------------------------------

-- -------------------------------------gui-begin-------------------------------------------------
-- GUI start
group "Avatara"(
	-- 4 bones for inverse kinematik
	listbox kinList "VR Kinematik:" items:#(arm, head, spine) height:3 width:75
	label lArm "..." pos:[110,45]
	label lHead "..." pos:[110,60]
	label lSpine "..." pos:[110,75]
	button bKinAdd "Assign" pos:[55,95]
	button bKinRem "Clear" pos:[60,120]
)

group "Frames"(
	radiobuttons rFrame labels:#("from/to (from/to/step)", "all (from/to)", "none") default:1
	spinner sFrom "from " width:70 type:#integer range:[animationRange.start, animationRange.end, 0]
	spinner sTo   "  to " width:70 type:#integer range:[animationRange.start, animationRange.end, animationRange.end]
	spinner sStep "step " width:70 type:#integer range:[animationRange.start, animationRange.end, 10]
)

group "File/Path"(
	label lPath "..."
	button bSelect_path "Browse"
	edittext eName "Filename: " fieldwidth:60
 )

group "Export"(
    label lWhatIdo "..."
	button bExp "Export"
)
-- -------------------------------------gui-end---------------------------------------------------

-- -----------------------------------handler-begin-----------------------------------------------
on rFrame changed theState do ( gChoice = rFrame.state )
on sFrom changed theState do ( gFrom = sFrom.value )
on sTo changed theState do ( gTo = sTo.value )
on sStep changed theState do ( gStep = sStep.value )

--handler start
on bKinAdd pressed do(
	if selection.count < 1 then messagebox "no object selected"
	else if classOf selection[1].baseObject != BoneGeometry then messagebox "select bone"
	else (
		case kinList.selection of(
			1: ( 
				myArm = selection[1].name
				if ((myArm == myHead) or (myArm == mySpine))then (
					messagebox "bone cannot be assigned to different vr-bones"
					myArm = ""
				)else lArm.caption = myArm 
				)
			2: ( 
			 	myHead = selection[1].name 
				if ((myHead == myArm) or (myHead == mySpine))then (
					messagebox "bone cannot be assigned to different vr-bones"
					myHead = ""
				)else lHead.caption = myHead 
				)
			3: ( 
				mySpine = selection[1].name 
				if ((mySpine == myArm) or (mySpine == myHead))then (
					messagebox "bone cannot be assigned to different vr-bones"
					mySpine = ""
				)else lSpine.caption = mySpine 
				)
		)
	)
)

on bKinRem pressed do(
	case kinList.selection of(
			1: (
				lArm.caption = "..."
				myArm = "")
			2: (
				lHead.caption = "..."
				myHead = "")
			3: (
				lSpine.caption = "..."
				mySpine = "")
		)		
)

on bSelect_path pressed do(
	pathN = getSavePath()									-- select filepath
	if pathN == undefined then pathN = ""
	if pathN.count > 3 then pathN= pathN + "\\"
	fileFull = pathN + eName.text
	lPath.caption = fileFull
)

on eName entered text do (
 	fileFull = pathN + eName.text
	lPath.caption = fileFull
)

on bExp pressed do(
		if (init()) then (
			fMdlNameB = fileFull + ".mdl"
			fMdlB = createFile fMdlNameB 					-- create file for binary output
			close fMdlB
			fMdlB = fopen fMdlNameB "wb"	
			exportMdl()										-- call modell export function
			fclose fMdlB
			
			fAniNameB = fileFull + ".ani"
			fAniB = createFile fAniNameB					-- create file for binary output
			close fAniB
			fAniB = fopen fAniNameB "wb"				
			exportAni()										-- call animation export function
			fclose fAniB	
		)
)
-- -----------------------------------handler-end-------------------------------------------------
) -- avatara