interactKey = "E"
_animTime = 0
_animTimeInterval = 0.0175
_maxHeight = 150
_minHeight = 80

function Awake()
	controller = this:FindEntity("player"):GetComponent("Controller")
end

function Initialize()
	textChild = this:GetChild(0)
	--textChildSprite = textChild:GetComponent("Sprite2D")
	textChild:Disable()

end

function Update(dt)
	if not textChild:IsDisabled() then
		_animTime = _animTime + _animTimeInterval
		-- Sanity Check for out of normalized
		if _animTime > 1.0 then
			_animTime = 1.0
			_animTimeInterval = _animTimeInterval * -1.0
		elseif _animTime < 0 then
			_animTime = 0
			_animTimeInterval = _animTimeInterval * -1.0
		end

		-- Perform linear interpolation
		local transform = textChild:GetComponent("Transform")
		local pos = transform.Position
		pos.y = LerpFloat(_minHeight, _maxHeight, _animTime)
		transform:Set_Position(pos)
	end
end

function CollisionStay (ent)
	if(ent.name == "Player") then
		if(controller:GetKeyDown(interactKey)) then
			this:FindEntity("player"):GetComponent("Script"):SetBoolVariable("_startFightTrigger", true)
			--textChild:Disable()
			--position2 = Vector3:new(0,-1000,0)
			--this:GetComponent("Transform"):Set_Position(position2)
			this:Disable()
		end
	end
end


function CollisionEnter (ent)
	if(ent.name == "Player") then
		textChild:Enable()
	end
end

function CollisionExit (ent)
	if(ent.name == "Player") then
		textChild:Disable()
	end
end