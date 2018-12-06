State = 0.0
curSpinSpeed = 0.0
maxRollSpinSpeed = 360.0
chargeSpinAccel = 90
dir = Vector3:new()
lastRot = Vector3:new()
distRolled = 0
maxDistToRoll = 15
curHorzSpeed = 40
maxHorzSpeed = 40
hozDecelleration = 20
spinDecelleration = 180
pangoRadius = 2.12
collided = false
restTimer = 0
maxRestTime = 2

soundPlayed = false

function Charge()
	LookAtPlayer()
	Spin(curSpinSpeed)
	curSpinSpeed = curSpinSpeed + chargeSpinAccel * curDT
	if(curSpinSpeed >= maxRollSpinSpeed)then
		NextState(1.0)
		rb:AddForwardForce(maxHorzSpeed * 250)
		--Print(tostring(rb.Velocity:Length()))
		rb.constraintPositionY = true
	end
end

function Roll()
	--Print("Rolling")
	MoveForward()
	if(soundPlayed == false)then
		--FxHandler.inst.PlaySfx("Boss_Roll", transform.position);
        soundPlayed = true;
	end
	if(collided == true)then
		collided = false
		return
	end
	Spin(curSpinSpeed)
	distRolled = distRolled + curHorzSpeed * curDT
	if(distRolled >= maxDistToRoll)then
		--Print("Roll limit reached")
		NextState(2.0)
	end
end

function SlowDown()
	--Print("slowing")
	curHorzSpeed = curHorzSpeed + hozDecelleration * curDT
	MoveForward()
	-- local curVelo = rb.Velocity
	-- local dirViable = VectorDot3(curVelo, dir:Unit())
	-- if(dirViable > 0)then
	-- 	curHorzSpeed = curHorzSpeed * -5.0
	-- else
	-- 	curHorzSpeed = 0
	-- end
	-- Print(tostring(rb.Velocity:Length()))
	if(collided == true)then
		collided = false
		return
	end

	if(curSpinSpeed > 0)then
		curSpinSpeed = curSpinSpeed - spinDecelleration * curDT
	end

	if(curSpinSpeed < 0)then
		curSpinSpeed = 0
	end
	Spin(curSpinSpeed)
	if(curHorzSpeed == 0)then
		NextState(3.0)
		rb.constraintPositionY = false
	end
	soundPlayed = false
end

function Rest()
	restTimer = restTimer + curDT
	if(restTimer >= maxRestTime)then
		NextState(0.0)
	end
end

function BreakUp()
end

Stats = {
	[0.0] = Charge,
	[1.0] = Roll,
	[2.0] = SlowDown,
	[3.0] = Rest,
	[4.0] = BreakUp
}

function NextCharge()
	curSpinSpeed = 0
	rb:SetVelocity(Vector3:new())
end

function NextRoll()
	distRolled = 0
	curHorzSpeed = maxHorzSpeed
end

function NextSlowDown()
end

function NextRest()
	restTimer = 0
	rb:SetVelocity(Vector3:new())
end

function NextBreakUp()
	--Print("Breaking")
	--Print(tostring(mama.name))
	mama:GetComponent("Script"):SetVector3Variable("bigPangPos",transform.Position)
	mama:GetComponent("Script"):CallLuaFunction("BreakUpBigPango")
	-- FxHandler.inst.PlaySfx("Poof", transform.position);
	-- FxHandler.inst.PlaySfx("BossSplit", transform.position);
	State = 0.0
	curSpinSpeed = 0
	DisableComs()
	this:GetChild(0):Disable()
	this:Disable()
end

NextStats = {
	[0.0] = NextCharge,
	[1.0] = NextRoll,
	[2.0] = NextSlowDown,
	[3.0] = NextRest,
	[4.0] = NextBreakUp
}

function NextState(theNextState)
	func = NextStats[theNextState]
	if(func)then
		func()
	else
	end
	State = theNextState
end

function ResetSpeeds()
	rb:SetVelocity(Vector3:new())
end


function Spin(spinSpeed)
	this:GetChild(0):GetComponent("Transform"):Rotate_Object
	(Vector3:new(1,0,0) * spinSpeed * curDT)
end

function LookAtPlayer()
	transform:Look_At(Player:GetComponent("Transform").Position)
	dir = Player:GetComponent("Transform").Position - transform.Position
end

function GetDamage()
	Print("damaged")
	this:GetChild(1):GetComponent("Script"):CallLuaFunction("Shield")
end

function MoveForward()
	--Print("MoveForward now")
	rb.Velocity.y = 0
	rb:AddForwardForce(curHorzSpeed)
	rb.Force.y = 0
	rb.Velocity.y = 0

	-- Vector3 nextPos = transform.position 
	-- 					+ transform.rotation 
	-- 						* Vector3.forward 
	-- 						* curHorzSpeed
	-- 						* Time.deltaTime;
	-- RaycastHit hit;
	-- if(Physics.SphereCast(transform.position,
	-- 					pangoRadius, 
	-- 					transform.rotation * Vector3.forward, 
	-- 					out hit, 
	-- 					curHorzSpeed * Time.deltaTime, 
	-- 					thingsThatCantBeRolledOver))
	-- {
	-- 	nextPos = transform.position
	-- 					+ transform.rotation
	-- 						* Vector3.forward 
	-- 						* hit.distance;
	-- 	if (hit.collider.gameObject.layer == 15)
	-- 	{
	-- 		NextState(State.BreakUp);
	-- 	}
	-- 	else
	-- 	{
	-- 		NextState(State.Rest);
	-- 	}
	-- 	collided = true;
	-- }
	-- if (Physics.SphereCast(transform.position,
	-- 		pangoRadius,
	-- 		transform.rotation * Vector3.forward,
	-- 		out hit,
	-- 		Vector3.Distance(transform.position, nextPos),
	-- 		player))
	-- {
	-- 	PlayerHealth.instance.DecreaseHealth(2);
	-- }
	-- transform.position = nextPos;

end

function SetParent()
	mama = this:GetParent()
end

function DisableComs()
	this:GetComponent("SphereCollider"):SetTrigger(true)
	--rb:SwitchGravity(false)
	rb:SetVelocity(Vector3:new())
end

function EnableComs()
	this:GetComponent("SphereCollider"):SetTrigger(false)
	--rb:SwitchGravity(true)
	rb:SetVelocity(Vector3:new())
end

function Initialize()
	transform = this:GetComponent("Transform")
	Player = this:FindEntity("Player")
	rb = this:GetComponent("RigidBody")
end

function Update(dt)
	curDT = dt
	func = Stats[State]
	--limitations
	local newX = transform.Position.x
	if(newX > 2479.99)then
		newX = 2479
	end
	if(newX < -185.336)then
		newX = -185.3
	end
	local newZ = transform.Position.z
	if(newZ > 1346.92)then
		newZ = 1346
	end
	if(newZ < -1281.61)then
		newZ = -1281
	end
	transform:Set_Position(Vector3:new(newX,transform.Position.y,newZ))
	-- local curVelo = rb.Velocity
	-- if(curVelo:Length() > maxHorzSpeed)then
	-- 	rb.Velocity = curVelo:Unit() * maxHorzSpeed
	-- end
	if(func)then
		func()
	else
		Print("BigPango switch error")
	end
end

function CollisionEnter(ent)
	--need check if other entity is beansprout
	if(ent.name == "RaisedLand")then
		--Print("seperate")
		NextState(4.0)
	elseif(ent.name ~= "Ground")then
		NextState(3.0)
	end
end
