plantTime = 3
_timer = 0
_planting = false
plantPrefab = ""
_growDist = 0
_origScale = Vector3:new()
_x = 0
_y = 0
cornerPrefab = "CompiledAssets/Models/cornered_plot.mdl"
connectPrefab = "CompiledAssets/Models/connect_plot.mdl"
-- Soil Helper variables
helperPrefab = "SoilHelper"
_toggleHelper = false
_plantName = ""
_helperTexture = "popup_plough.dds"
_prevHelperTexture = _helperTexture
_helperNum = 0
_helperName = "SoilHelper"
_state = "Empty"

function Initialize()
	tileUsed = false
	groundTile = nil
	sk = this:GetComponent("StaticMesh")
	--gm = this:FindEntity("GameManager"):GetComponent("Script")
end

function Update(dt)
	if(_planting) then
		if(_growDist == 0) then
			_growDist = entScript:GetFloatVariable("endY") - entScript:GetFloatVariable("startY")
			if(_growDist ~= 0) then
				position = this:GetComponent("Transform").Position
				position.y = position.y + entScript:GetFloatVariable("startY")
				ent:GetComponent("Transform"):Set_Position(position)
			end
		else
			_timer = _timer + dt
			plantTfm = ent:GetComponent("Transform")
			pos = plantTfm.Position
			pos.y = pos.y + _growDist/(plantTime/dt)
			plantTfm:Set_Position(pos)
			scale = plantTfm.Scale
			scale.x = scale.x + _origScale.x / (plantTime/dt)
			scale.y = scale.y + _origScale.y / (plantTime/dt)
			scale.z = scale.z + _origScale.z / (plantTime/dt)
			plantTfm:Set_Scale(scale)
		end
		if(_timer > plantTime) then
			_timer = 0
			_planting = false
			--GrowPlant()
			ent:GetComponent("Script"):CallLuaFunction("ReadyPlant")
		end
	end
end

function CheckUsed()
	if(tileUsed) then
		return true
	else
		return false
	end
end

function CollisionEnter (ent)
	if(tileUsed and this.name == "EmptySoil") then
		if(ent.name == "GroundTile" or ent.name == "GroundTileSolo" or ent.name == "GroundTileCorner") then
			if(ent:GetComponent("Transform").Position.x == this:GetComponent("Transform").Position.x and ent:GetComponent("Transform").Position.z == this:GetComponent("Transform").Position.z) then
				ent:GetComponent("Identifier"):Update_ID("123")
				groundTile = this:FindEntity("123")
				groundTile:GetComponent("Identifier"):Update_ID("Soil")
			end
		end
	end
end

function CollisionStay (ent)
	if(tileUsed and this.name == "EmptySoil" and groundTile == nil) then
		if(ent.name == "GroundTile" or ent.name == "GroundTileSolo" or ent.name == "GroundTileCorner") then
			if(ent:GetComponent("Transform").Position.x == this:GetComponent("Transform").Position.x and ent:GetComponent("Transform").Position.z == this:GetComponent("Transform").Position.z) then
				ent:GetComponent("Identifier"):Update_ID("123")
				groundTile = this:FindEntity("123")
				groundTile:GetComponent("Identifier"):Update_ID("Soil")
			end
		end
	end
end

function StartPlant()
	if(plantPrefab ~= "None" and PrefabExists(plantPrefab)) then
		position = this:GetComponent("Transform").Position
		position.y = position.y - 30
		_planting = true
		ent = this:CreateEntity()
		ent:LoadPrefab(plantPrefab)
		tfm = ent:GetComponent("Transform")
		tfm:Set_Position(position)
		_origScale = tfm.Scale
		scale = Vector3:new(0,0,0)
		tfm:Set_Scale(scale)
		entScript = ent:GetComponent("Script")
	end
end

function GrowPlant()
	--[[ent = this:CreateEntity()
	position = transform.Position
	position.y = position.y + 2.8
	if(ent:LoadPrefab(_plantPrefab)) then
		ent:GetComponent("Transform"):Set_Position(position)
	end]]--
end

function UpdateModel()
	gm = this:FindEntity("GameManager"):GetComponent("Script")
	gm:SetFloatVariable("_checkX",_x-1)
	gm:SetFloatVariable("_checkY",_y)
	gm:CallLuaFunction("CheckTileStatus")
	result1 = gm:GetBoolVariable("_checkResult")
	gm:SetFloatVariable("_checkX",_x+1)
	gm:SetFloatVariable("_checkY",_y)
	gm:CallLuaFunction("CheckTileStatus")
	result2 = gm:GetBoolVariable("_checkResult")
	if(result1 or result2) then
		pfb = connectPrefab
		if((result1 and not result2) or (not result1 and result2)) then
			pfb = cornerPrefab
		end
		Print(pfb)
		sk:SetStaticMesh(pfb)
		if(result1 and not result2) then
			rot = this:GetComponent("Transform").Rotation
			rot.y = 180
			this:GetComponent("Transform"):Set_Rotation(rot)
		end
	end
end

function UpdateModelSelf()
	gm = this:FindEntity("GameManager"):GetComponent("Script")
	gm:SetFloatVariable("_checkX2",_x-1)
	gm:SetFloatVariable("_checkY2",_y)
	gm:CallLuaFunction("CheckTileStatus2")
	result1 = gm:GetBoolVariable("_checkResult2")
	gm:SetFloatVariable("_checkX2",_x+1)
	gm:SetFloatVariable("_checkY2",_y)
	gm:CallLuaFunction("CheckTileStatus2")
	result2 = gm:GetBoolVariable("_checkResult2")
	if(result1 or result2) then
		pfb = connectPrefab
		if((result1 and not result2) or (not result1 and result2)) then
			pfb = cornerPrefab
		end
		Print(pfb)
		sk:SetStaticMesh(pfb)
		if(result1 and not result2) then
			rot = this:GetComponent("Transform").Rotation
			rot.y = 180
			this:GetComponent("Transform"):Set_Rotation(rot)
		end
	end
end

function UpdateGroundTile()
	if(groundTile ~= nil) then
		groundTile:GetComponent("Script"):CallLuaFunction("UpdateModelSelf")
	else
		Print("Couldn't find tile!")
	end
end

function CreateHelper()
	spriteHelper = this:CreateEntity()
	if spriteHelper:LoadPrefab(helperPrefab) then
		-- Set the position to be above the soil
		local soilPos = this:GetComponent("Transform").Position
		soilPos.y = soilPos.y + 100
		local helperPos = Vector3:new(soilPos.x, soilPos.y, soilPos.z)
		spriteHelper:GetComponent("Transform"):Set_Position(helperPos)
		spriteHelper:GetComponent("Sprite2D"):SetVisibility(false)
		spriteHelper:GetComponent("Sprite2D"):SetTexture(_helperTexture)
		local gameMgr = this:FindEntity("GameManager"):GetComponent("Script")
		local counter = Floor(gameMgr:GetVariable("_soilHelperCounter"))
		local idName = spriteHelper:GetComponent("Identifier").ID
		idName = idName .. counter
		_helperNum = counter
		spriteHelper:GetComponent("Identifier"):Update_ID(idName)
		-- Increment the counter
		counter = counter + 1
		gameMgr:SetVariable("_soilHelperCounter", counter)
	end
end

function DisplayHelper()
	local fullName = _helperName .. Floor(_helperNum)
	spriteHelper = this:FindEntity(fullName)
	local sprite = spriteHelper:GetComponent("Sprite2D")

	if _toggleHelper then
		sprite:SetVisibility(true)
	else
		sprite:SetVisibility(false)
	end
end

function UpdateTexture()
	-- Pre-req ->_helperTexture must be updated
	local fullName = _helperName .. Floor(_helperNum)
	spriteHelper = this:FindEntity(fullName)
	local sprite = spriteHelper:GetComponent("Sprite2D")

	-- Sanity Check if we should update on every frame
	if _helperTexture ~= _prevHelperTexture then
		if tileUsed then
			if StringLen(_helperTexture) ~= 0 then
				sprite:SetVisibility(true)
				sprite:SetTexture(_helperTexture)
			else
				sprite:SetVisibility(false)
			end
		elseif _helperTexture == "" then
			sprite:SetVisibility(false)
		else
			sprite:SetTexture(_helperTexture)
		end
		
		_prevHelperTexture = _helperTexture
	end
end