-- Plant Information
_showMaxUI = false					-- Determines if the UI should show all of the available options
plantScale = Vector3:new()
_currSelect = 0						-- Determines the current selection in the list of plants
_itemMat = StringList:new()			-- The items for all of the plants available
_itemCount = 0
_displayPlant = true
-- Player's Health Details
_maxHealth = 8
_currHealth = 0
_prevHealth = 0
_healthUpdate = false
_healthMat = IntList:new()
_displayHealth = false
_cheatMode = false
-- Boss' Health Details
_bossHealthZOffset = 35
_bossMaxHealth = 0
_bossCurrHealth = 0
_bossPrevHealth = 0
_bossHealthUpdate = false
_bossDisplayHealth = false
bossPrefab = "BossHealth"
-- Loading Stuff
_loadState = true
_loadTimer = 0
_loadTimerCounter = 2

-- Day / Night Toggle
_dayMode = true

function Awake()
	controller = this:GetComponent("Controller")

	-- Player Health Stuff
	for i=0,_maxHealth-1 do
		_healthMat:PushBack(i)
	end

	-- Plant Stuff
	plantWheel = this:FindEntity("PlantWheel")
	numItems = plantWheel:GetChildrenCount()
	hover = this:FindEntity("Hover")

	-- Player Stuff
	playerManager = this:FindExternalEntity("player")
	playerScript = playerManager:GetComponent("Script")

	-- Boss Stuff (Capturing from the Game Manager)
	bossScript = this:FindExternalEntity("GameManager"):GetComponent("Script")
	_bossMaxHealth = Floor(bossScript:GetFloatVariable("maxHealth"))
	_bossCurrHealth = _bossMaxHealth
	_bossPrevHealth = _bossCurrHealth
	GenerateBossHealth()
end

function Initialize()

	DayToggle()

	_currHealth = playerScript:GetVariable("_health")
	_prevHealth = _currHealth
	_currSelect = Floor(playerScript:GetVariable("_currPlant"))

	-- Plant Section
	DisplayFiveItems()
	SortTransparency()

	-- Loading Screen Stuff
	-- loadingScreen = this:FindEntity("LoadingScreen")
end

function Update(dt)
	-- Loading Screen
	if ObjectsLoaded() then
		this:FindEntity("LoadingScreen"):Disable()
		-- Start our counter
		if _loadState then
			_loadTimer = _loadTimer + dt
			if _loadTimer > _loadTimerCounter then
				_loadState = false
			end
		end
	elseif _loadState then
		this:FindEntity("LoadingScreen"):Enable()
	end

	-- Player's Health here
	if _healthUpdate then
		PlayerHealthUpdate()
		_healthUpdate = false
	elseif _cheatMode then
		PlayerMaxHealthCheat()
		_cheatMode = false
	end

	-- Boss Health here
	if _bossHealthUpdate then
		BossHealthUpdate()
		_bossHealthUpdate = false
	end
end

function DisplayFiveItems()
	-- Depending on the current selection, we will show all of the 5 items
	plantWheel:GetChild(2):GetComponent("Sprite2D"):SetTexture(_itemMat:GetElem(_currSelect))
	local prev0 = Floor(_currSelect) - 1
	local next0 = Floor(_currSelect) + 1
	-- Sanity Checking (prev0 && next0)
	if prev0 < 0 then
		prev0 = _itemCount - 1
	end
	if next0 >= _itemCount then
		next0 = 0
	end

	local prev1 = prev0 - 1
	local next1 = next0 + 1
	-- Sanity Checking (prev1 && next1)
	if prev1 < 0 then
		prev1 = _itemCount - 1
	end
	if next1 >= _itemCount then
		next1 = 0
	end

	-- Display the items
	plantWheel:GetChild(0):GetComponent("Sprite2D"):SetTexture(_itemMat:GetElem(prev1))
	plantWheel:GetChild(1):GetComponent("Sprite2D"):SetTexture(_itemMat:GetElem(prev0))
	plantWheel:GetChild(3):GetComponent("Sprite2D"):SetTexture(_itemMat:GetElem(next0))
	plantWheel:GetChild(4):GetComponent("Sprite2D"):SetTexture(_itemMat:GetElem(next1))
end

function SortTransparency()
	-- when _showMaxUI is false, we only show the left and right side
	if _showMaxUI then
		for i=0,numItems - 1 do
			plantWheel:GetChild(i):GetComponent("Sprite2D"):SetVisibility(true)
			plantWheel:GetChild(i):GetComponent("Sprite2D"):SetTransparency(1)
		end
		hover:GetComponent("Sprite2D"):SetVisibility(true)
	else
		plantWheel:GetChild(0):GetComponent("Sprite2D"):SetVisibility(false)
		plantWheel:GetChild(4):GetComponent("Sprite2D"):SetVisibility(false)
		-- Set the transparency of the 2nd and 4th to be 0.3
		plantWheel:GetChild(1):GetComponent("Sprite2D"):SetTransparency(0.3)
		plantWheel:GetChild(3):GetComponent("Sprite2D"):SetTransparency(0.3)
		hover:GetComponent("Sprite2D"):SetVisibility(false)
	end
end

function PlayerHealthUpdate()
	local incHealth = false
	if _currHealth - _prevHealth > 0 then
		incHealth = true
	end

	if incHealth then
		local diff = _currHealth - _prevHealth
		for i=0,diff-1 do
			local healthNum = Floor(_prevHealth + i)
			local sprite = this:FindEntity("Health" .. healthNum):GetComponent("Sprite2D")
			sprite:SetTexture("health_petal.dds")
			local tint = Vector4:new(255,0,0,120)
			--sprite:SetTint(tint)
			sprite:SetTransparency(1)
		end
	else
		local diff = _prevHealth - _currHealth
		for i=0,diff-1 do
			local healthNum = Floor(_prevHealth - i - 1)
			local sprite = this:FindEntity("Health" .. healthNum):GetComponent("Sprite2D")
			sprite:SetTexture("health_petalnope.dds")
			local tint = Vector4:new(0,0,0,0)
			--sprite:SetTint(tint)
			sprite:SetTransparency(0.5)
		end
	end
	_prevHealth = _currHealth
end

function PlayerMaxHealthCheat()
	_currHealth = playerScript:GetFloatVariable("_health")
	local total = Floor(_currHealth)
	for i=0,total-1 do
		local sprite = this:FindEntity("Health" .. i):GetComponent("Sprite2D")
		sprite:SetTexture("health_petal.dds")
		local tint = Vector4:new(255,0,0,120)
		--sprite:SetTint(tint)
		sprite:SetTransparency(1)
	end
	_prevHealth = _currHealth
end

function UpdatePlantUI()
	if not _dayMode then
		return
	end

	-- Capturing from the player component
	_itemCount = _itemMat:Size()
	_currSelect = Floor(playerScript:GetFloatVariable("_currPlant"))
	_showMaxUI = playerScript:GetBoolVariable("_selecting")
	--[[
	Pre-Req for this Function:
	1) _currSelect must be updated
	2) _showMaxUI must be updated
	--]]
	_currSelect = _currSelect % _itemCount
	DisplayFiveItems()
	SortTransparency()
end

function UpdatePlayerHealthUI(newHealth, maxAll)
	if _dayMode then
		return
	end

	if not maxAll then
		_currHealth = newHealth
		-- Update our health on the next frame
		_healthUpdate = true
	else
		_cheatMode = true
	end
end

function BossHealthUpdate()
	-- If it has already dropped, don't update
	if _bossCurrHealth < 0 then
		_bossCurrHealth = 0
		_bossPrevHealth = 0
		return
	end

	local incHealth = false
	if _bossCurrHealth - _bossPrevHealth > 0 then
		incHealth = true
	end

	if incHealth then
		local diff = _bossCurrHealth - _bossPrevHealth
		for i=0,diff-1 do
			local healthNum = Floor(_bossCurrHealth + i + 1)
			local sprite = this:FindEntity("BossHealth" .. healthNum):GetComponent("Sprite2D")
			sprite:SetTexture("boss_health.dds")
			local tint = Vector4:new(255,0,0,120)
			--sprite:SetTint(tint)
			sprite:SetTransparency(1)
		end
	else
		local diff = _bossPrevHealth - _bossCurrHealth
		for i=0,diff-1 do
			local healthNum = Floor(_bossPrevHealth - i - 1)
			local sprite = this:FindEntity("BossHealth" .. healthNum):GetComponent("Sprite2D")
			sprite:SetTexture("boss_healthnope2.dds")
			local tint = Vector4:new(0,0,0,0)
			--sprite:SetTint(tint)
			sprite:SetTransparency(0.5)
		end
	end
	_bossPrevHealth = _bossCurrHealth
end

function UpdateBossHealthUI(newHealth)
	if _dayMode then
		return
	end

	_bossCurrHealth = newHealth
	_bossHealthUpdate = true
end

function MaximumBossHealthUI()
	-- This function is to reset back all of the boss' health
	_bossCurrHealth = bossScript:GetFloatVariable("_health")
	local total = Floor(_bossCurrHealth)
	for i=1,total do
		ent = this:FindEntity("BossHealth" .. i)
		ent:GetComponent("Sprite2D"):SetTexture("bossHasHealth")
	end
	_bossPrevHealth = _bossCurrHealth
end

function DayToggle()
	_dayMode = true
	-- If it's a day mode, we will only want to display the plants
	-- Show all plants
	for i=0,4 do
		plantWheel:GetChild(i):GetComponent("Sprite2D"):SetVisibility(true)
	end
	-- Hide all health Stuff
	-- Player
	Print(_bossMaxHealth)
	for i=0,_maxHealth-1 do
		local itemNo = _healthMat:GetElem(i)
		local health = this:FindEntity("Health" .. itemNo)
		health:GetComponent("Sprite2D"):SetVisibility(false)
	end
	-- Boss
	for i=0,_bossMaxHealth-1 do
		local bossHealth = this:FindEntity("BossHealth" .. i)
		bossHealth:GetComponent("Sprite2D"):SetVisibility(false)
	end
end

function NightMode()
	_dayMode = false
	-- If it's a night mode, we will only want to display the health Stuff
	-- Hide all plants
	for i=0,4 do
		plantWheel:GetChild(i):GetComponent("Sprite2D"):SetVisibility(false)
	end
	-- Show all health stuff
	-- Player
	for i=0,_maxHealth-1 do
		local itemNo = _healthMat:GetElem(i)
		local health = this:FindEntity("Health" .. itemNo)
		health:GetComponent("Sprite2D"):SetVisibility(true)
	end
	-- Boss
	for i=0,_bossMaxHealth-1 do
		local bossHealth = this:FindEntity("BossHealth" .. i)
		bossHealth:GetComponent("Sprite2D"):SetVisibility(true)
	end
end

function GenerateBossHealth()
	for i=0,_bossMaxHealth-1 do
		local ent = this:CreateEntity()
		ent:LoadPrefab(bossPrefab)
		-- Update the transform
		local transform = ent:GetComponent("Transform")
		local pos = transform.Position
		pos.z = pos.z + _bossHealthZOffset * i
		transform:Set_Position(pos)
		-- Update the Identifier
		local idName = ent:GetComponent("Identifier").ID
		idName = idName .. i
		ent:GetComponent("Identifier"):Update_ID(idName)
		ent.name = idName
	end
end