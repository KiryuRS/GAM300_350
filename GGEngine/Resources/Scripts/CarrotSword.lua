_pickedUp = true
_rest = true
_orphaned = false

function Initialize()

end

function Update(dt)
	if(not _rest and not _pickedUp) then
		rb = this:GetComponent("RigidBody")
		if(rb.Velocity.x == 0 and rb.Velocity.y == 0 and rb.Velocity.z == 0) then
			--this:GetComponent("AABBCollider"):SetTrigger(true)
			--rb:SwitchGravity(false)
			_rest = true
		end
	end
end

function Pickup()
	_pickedUp = true
	this:GetComponent("AABBCollider"):SetTrigger(true)
	--this:GetComponent("RigidBody"):SwitchGravity(false)
end

function CollisionEnter (ent)
	if(not _rest and not _pickedUp) then
		if(ent.name == "BossSub1") then
			Print("Carrot Destroyed")
			carrot = this:FindEntity("DrawnCarrot")
			carrot:GetComponent("Script"):CallLuaFunction("RegenSword")
			carrot:GetComponent("Identifier"):Update_ID("Plant")
			this:FindEntity("GameManager"):GetComponent("Script"):CallLuaFunction("MinusHealth")
			this:Destroy()
		elseif(ent.name ~= "Player") then
			--this:GetComponent("AABBCollider"):SetTrigger(false)
			--this:GetComponent("RigidBody"):SwitchGravity(false)
			_rest = true
		end
	end
end

function Discard()
	_pickedUp = false
	_rest = false
	rb = this:GetComponent("RigidBody")
	this:GetComponent("AABBCollider"):SetTrigger(false)
	--rb:SwitchGravity(true)
	vel = Vector3:new(0,0,0)
	rb:SetVelocity(vel)
end