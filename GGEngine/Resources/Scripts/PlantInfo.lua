startY = -15
endY = 30
plantName = ""
_landPrefab = "RaisedLand"
_risingUp = false
_risingDown = false
_riseTime = 1
_timer = 0
_riseSpeed = 4.1
_fallSpeed = 8.2
_riseStayTime = 3
_isRising = false
_ready = false
_swordDrawn = false
_swordRegenTime = 3
_swordRegen = false
_healUsed = false
_healRegenTime = 3
carrotDrawnPrefab = "CompiledAssets/Models/emptycarrot.mdl"
_origMesh = ""
_origY = 0
taugeHeight = 4.1

function Initialize()
	_origMesh = this:GetComponent("StaticMesh"):GetStaticMesh()
	if(this.name == "Plant1") then
		plantName = "Taugeh"
	elseif(this.name == "Plant2") then
		plantName = "Tomato"
	elseif(this.name == "Plant3") then
		plantName = "Carrot"
	elseif(this.name == "Plant4") then
		plantName = "Cabbage"
	end
	_riseSpeed = taugeHeight
	_fallSpeed = taugeHeight * 2
end

function Update(dt)
	if(_risingUp and _risingDown) then
		_timer = _timer + dt
		if(_timer > _riseStayTime) then
			_risingUp = false
			_timer = 0
		end
	elseif(_risingUp) then
		_timer = _timer + dt
		position = ent:GetComponent("Transform").Position
		position.y = position.y + _riseSpeed
		ent:GetComponent("Transform"):Set_Position(position)
		position = this:GetComponent("Transform").Position
		position.y = position.y + _riseSpeed
		this:GetComponent("Transform"):Set_Position(position)
		if(_timer > _riseTime) then
			_risingDown = true
			_timer = 0
		end
	elseif(_risingDown) then
		_timer = _timer + dt
		position = ent:GetComponent("Transform").Position
		position.y = position.y - _fallSpeed
		ent:GetComponent("Transform"):Set_Position(position)
		position = this:GetComponent("Transform").Position
		position.y = position.y - _fallSpeed
		this:GetComponent("Transform"):Set_Position(position)
		if(_timer * (_fallSpeed/_riseSpeed) > _riseTime) then
			_risingDown = false
			_isRising = false
			_timer = 0
			ent:Destroy()
			position = this:GetComponent("Transform").Position
			position.y = _origY
			this:GetComponent("Transform"):Set_Position(position)
		end
	end
	if(_swordDrawn and _swordRegen) then
		_timer = _timer + dt
		if(_timer > _swordRegenTime) then
			_swordDrawn = false
			_timer = 0
			this:GetComponent("StaticMesh"):SetStaticMesh(_origMesh)
			--position = this:GetComponent("Transform").Position
			--position.y = position.y + 15
			--this:GetComponent("Transform"):Set_Position(position)
			_swordRegen = false
		end
	end
	if(_healUsed) then
		_timer = _timer + dt
		if(_timer > _healRegenTime) then
			_healUsed = false
			_timer = 0
			--position = this:GetComponent("Transform").Position
			--position.y = position.y + 25
			--this:GetComponent("Transform"):Set_Position(position)
		end
	end
end

function RiseUp()
	if(_ready) then
		ent = this:CreateEntity()
		position = this:GetComponent("Transform").Position
		position.y = position.y - 120
		if(ent:LoadPrefab(_landPrefab)) then
			ent:GetComponent("Transform"):Set_Position(position)
		end
		_risingUp = true
		_isRising = true
		_origY = this:GetComponent("Transform").Position.y
		print("Taugeh Skill Activated")
	end
end

function ReadyPlant()
	_ready = true
end

function DrawSword()
	if(_ready) then
		_swordDrawn = true
		this:GetComponent("StaticMesh"):SetStaticMesh(carrotDrawnPrefab)
		--position = this:GetComponent("Transform").Position
		--position.y = position.y - 15
		--this:GetComponent("Transform"):Set_Position(position)
		print("Carrot Skill Activated")
		--RegenSword()
	end
end

function RegenSword()
	_swordRegen = true
end

function Heal()
	if(_ready) then
		_healUsed = true
		--position = this:GetComponent("Transform").Position
		--position.y = position.y - 25
		--this:GetComponent("Transform"):Set_Position(position)
		print("Tomato Skill Activated")
	end
end