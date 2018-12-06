mouseDisplay = true;
mouseLock = false;
maxHealth = 10
_health = 10
_damageAmount = 1
enemyName = ""
emptySoilPrefab = "EmptySoil"
gridWidth = 4
gridHeight = 4
startingGrid = Vector3:new(-54.253,-4.672,-310.197)
gridSize = 182
tiles = {}
tilesSprite = {}
_checkX = 0
_checkY = 0
_checkResult = false
_checkX2 = 0
_checkY2 = 0
_checkResult2 = false
_oldIndex = 9999000
_soilHelperCounter = 0

function Initialize()
	_health = maxHealth
	_entity = this:FindEntity(enemyName)
	DisplayMouse(mouseDisplay)
	MouseLock(mouseLock)
	for i = 0, gridWidth-1 do
		for j = 0, gridHeight-1 do
			pos = Vector3:new()
			pos.x = startingGrid.x + j * gridSize
			pos.z = startingGrid.z + i * gridSize
			pos.y = startingGrid.y
			soil = this:CreateEntity()
			if(soil:LoadPrefab(emptySoilPrefab)) then
				soil:GetComponent("Transform"):Set_Position(pos)
				soil:GetComponent("Script"):SetFloatVariable("_x",i);
				soil:GetComponent("Script"):SetFloatVariable("_y",j);
				-- Create the helper
				soil:GetComponent("Script"):CallLuaFunction("CreateHelper")
			end
			tiles[i*gridHeight + j] = soil
		end
	end
	player = this:FindEntity("player")
	_playerScript = player:GetComponent("Script")
	_playerXform = player:GetComponent("Transform")
	uiManager = this:FindExternalEntity("UIManager")
	uiScript = uiManager:GetComponent("Script")
end

function Update(dt)
	local currPos = _playerXform.Position - startingGrid
	currPos.x = currPos.x + gridSize / 2
	currPos.z = currPos.z + gridSize / 2
	if(currPos.x > 0 and currPos.z > 0 and currPos.x < gridSize * gridHeight and
	currPos.x < gridSize * gridWidth) then
		currPos.x = currPos.x / gridSize
		currPos.z = currPos.z / gridSize
		local index = Floor( currPos.z ) * gridHeight + Floor(currPos.x)
		if(oldIndex ~= index) then
			if(oldIndex ~= 9999000) then
				_playerScript:CallLuaFunction("SoilCollisionExit")
			end
			_playerScript:SetVariable("soilObject", tiles[index])
			_playerScript:CallLuaFunction("SoilCollisionEnter")
			oldIndex = index
		end
		if (oldIndex == index and oldIndex ~= 9999000) then
			_playerScript:CallLuaFunction("SoilCollisionStay")
		end
	elseif(oldIndex ~= 9999000) then
		_playerScript:CallLuaFunction("SoilCollisionExit")
		oldIndex = 9999000
	end
end


function MinusHealth()
	if(_health > 0) then
		_health = _health - _damageAmount
		print("Enemy Health: ")
		print(_health)
		ubHealth = uiScript:GetVariable("UpdateBossHealthUI")
		ubHealth(_health)
		if(_health <= 0) then
			--_entity:GetComponent("Script"):SetBoolVariable("awake", false)
			--_entity:GetComponent("Script"):CallLuaFunction("ParentAllSubBoss")
			_entity:GetComponent("Script"):CallLuaFunction("Death")
			--_entity:Destroy()
			print("Enemy Dead")
			this:FindEntity("player"):GetComponent("Script"):SetStringVariable("enemyName", "")
		end
	end
end

function CheckEnemyHealth()
	if(_health <= 0) then
		return true
	else
		return false
	end
end

function CheckTileStatus()
	if(_checkX < 0 or _checkX >= gridWidth or _checkY < 0 or _checkY >= gridHeight) then
		_checkResult = false
	else
		ent = tiles[_checkX*gridHeight + _checkY]
		_checkResult = ent:GetComponent("Script"):GetBoolVariable("tileUsed")
	end
	if(_checkResult) then
		tiles[_checkX*gridHeight + _checkY]:GetComponent("Script"):CallLuaFunction("UpdateGroundTile")
	end
end

function CheckTileStatus2()
	if(_checkX2 < 0 or _checkX2 >= gridWidth or _checkY2 < 0 or _checkY2 >= gridHeight) then
		_checkResult2 = false
	else
		ent = tiles[_checkX2*gridHeight + _checkY2]
		_checkResult2 = ent:GetComponent("Script"):GetBoolVariable("tileUsed")
	end
end