State = 0.0
JumpState = 0.0
NextState = 0.0
MamaCenter = Vector3:new()
Index = 0

dir = Vector3:new()
draw = Vector3:new()
draw2 = Vector3:new()
draw3 = Vector3:new()

--Quaternion lastRot

anticipateTime = 2.0
coolDownTime = 2.0
attackDist = 30

attackTarget = Vector3:new()
timer = 0
chaseSpeed = 10
attackSpeed = 30
chaseAccel = 8
attackAccel = 40

bigMamaPosition = Vector3:new()
toBeLocation = Vector3:new()

jumpStateTimer = 0

jumpAttackExplosion = false

curDT = 0

moveUpdatePos = true
moveDestPos = Vector3:new()
moveSpeed = 0
moveAccel = 0
moveStopDist = 0
moveReverse = false
moveSpeedLimit = 500
DropSpeed = 600
stun = false

awakeDrop = false

dead = false
deadAnimFinish = false

dropFxName = ""

anim_charge_dash = ""
anim_charge_idle = ""
anim_charge_start = ""
anim_death1 = ""
anim_death2 = ""
anim_idle = ""
anim_run = ""
anim_run_start = ""
anim_stun = ""
anim_flinch = ""
_animTimer = 0
_animDuration = 0


function Initialize()
	transform = this:GetComponent("Transform")
	Player = this:FindEntity("Player")
	rb = this:GetComponent("RigidBody")
	sk = this:GetChild(0):GetComponent("SkeletalMeshRenderer")
	moveSpeed = chaseSpeed
	moveAccel = chaseAccel
	--nma = GetComponent<NavMeshAgent>();
	-- nma.speed = chaseSpeed;
	-- nma.acceleration = chaseAccel;
	-- anticipationAnim = transform.GetChild(2).GetComponent<Animator>();
	-- anim = GetComponent<Animator>();
	-- hitSphereCollider.enabled = false;
	--Print("stoneindi transform")
	lastRot = transform.Rotation
	rb:SwitchGravity(false)
	sk.Loop = true
	--ChangeAnim(anim_idle)
	_animTimer = 0
	--dropFX = this:FindEntity("dropAimFx")
	dropFX = this:FindEntity(dropFxName)
end

function JumpAnticipate()
	if(jumpStateTimer > 1.0)then
		JumpState = 1.0
		rb:SwitchGravity(false)
		this:GetComponent("AABBCollider"):SetTrigger(true)
	end
end

function JumpJump()
	newPosY = transform.Position.y + curDT * 400
	transform:Set_Position(Vector3:new(transform.Position.x,
							newPosY,
							transform.Position.z))
	--Print("if check")
	if(transform.Position.y >= 400) then
		JumpState = 2.0

		--Print("raycast check")
		hitPoint = RayCastPoint(transform.Position, Vector3:new(0,-1,0),1000)
		_downVec = (hitPoint - transform.Position)
		if(_downVec:Length() <= 1050)then
		-- RaycastHit hit2;
		-- // instantiate aiming thing
		-- if (Physics.Raycast(transform.position, Vector3.down, out hit2, 100, canDropOntoMask))
		-- {
		-- 	dropAimFx_spawned = Instantiate(dropAimFx, hit2.point, Quaternion.Euler(-90,0,0));
		-- 	jumpStateTimer = 0;
		-- }
			jumpStateTimer = 0
		end
		--Print("end check")
	end
end

function JumpAim()
	newPos = transform.Position
	newPos.y = Player:GetComponent("Transform").Position.y
	newPos = LerpVector3(newPos, Player:GetComponent("Transform").Position, 2*curDT)
	newPos.y = transform.Position.y
	transform:Set_Position(newPos)
	-- RaycastHit hit;
	hitPoint = RayCastPoint(transform.Position, Vector3:new(0,-1,0),1000)
	_downVec = (hitPoint - transform.Position)
	if(_downVec:Length() <= 1000)then
		-- 	dropAimFx_spawned.transform.position = hit.point;
		dropFX:GetComponent("Transform").Position = hitPoint
		--Print(tostring(dropFX:GetComponent("Script"):GetBoolVariable("active")))
		if(dropFX:GetComponent("Script"):GetBoolVariable("active") == false)then
			dropFX:GetComponent("Script"):CallLuaFunction("Enlarge")
			if(jumpStateTimer > 2 * Index + 1)then
				dropFX:GetComponent("Script"):CallLuaFunction("Activate")
			end
		end
	end

	if(jumpStateTimer > 2 * Index + 3)then
		JumpState = 3.0
		--dropFX:GetComponent("Script"):CallLuaFunction("Activate")
		--hitBoxCollider.enabled = false;
	end
end

function JumpDrop()
	--Print("drop?")
	newPos2 = transform.Position - Vector3:new(0,curDT * DropSpeed,0)
	--Print("raycasting?")
	hitPoint = RayCastPoint(transform.Position, Vector3:new(0,-1,0),curDT * DropSpeed)
	local entityHit = RayCast(transform.Position,Vector3:new(0,-1,0),curDT*DropSpeed)
	--Print("Raycast check")
	hitLength = (hitPoint - transform.Position):Length()
	--if(entityHit:CheckNullBool())then
	--Print(tostring(entityHit.name))
	--end
	--if(hitLength <= curDT * DropSpeed and entityHit.name == "Ground")then
	if(hitLength <= curDT * DropSpeed )then
		--dropFx:Destroy()
		rb:SwitchGravity(true)
		--Print(tostring(hitLength))
		--newPos2 = hit3.point;
		newPos2 = hitPoint
		--anim.SetTrigger("Jump");
		JumpState = 4.0

		--Destroy(dropAimFx_spawned);
		jumpStateTimer = 0
		--Print("JUMPMODE")
		mama:GetComponent("Script"):CallLuaFunction("IncreasePongosLanded")
		--nma.updatePosition = true;
		moveUpdatePos = true

		--FxHandler.inst.PlayPfx("Big SmokePfx", newPos2);
		--FxHandler.inst.PlaySfx("Smash", newPos2);

		--hitSphereCollider.enabled = true;

		bossDropParticle = this:FindEntity("SmokeParticle")
		bossDropParticle:GetComponent("Transform").Position = this:GetComponent("Transform").Position
		bossDropParticle:GetComponent("ParticleEmitter"):EmitOnce()
		

		jumpAttackExplosion = true;
		this:GetComponent("AABBCollider"):SetTrigger(false)
		--play sfx
		Player:GetComponent("Sound"):Play_SFX(9)

	end
	transform:Set_Position(newPos2)
end

function JumpWait()
	if(jumpAttackExplosion == true)then
		jumpAttackExplosion = false
	end

	-- if(!hitBoxCollider.gameObject.activeInHierarchy)
	-- 	hitBoxCollider.enabled = true;
	-- if (hitSphereCollider.gameObject.activeInHierarchy)
	-- 	hitSphereCollider.enabled = false;
end

local JumpStates =
{
	[0.0] = JumpAnticipate,
	[1.0] = JumpJump,
	[2.0] = JumpAim,
	[3.0] = JumpDrop,
	[4.0] = JumpWait
}

function Prepare()
end

function Chase()
	LookAtPlayer(false)
	MoveToPlayer()
	if(_animTimer > _animDuration)then
		--Print("start chase")
		sk.Loop = true
		ChangeAnim(anim_run)
	end
	if (Player:GetComponent("Transform").Position - transform.Position ):Length() 
	< attackDist * 0.5 then
		NextState = 2.0
		UpdateState()
	end
end

function Anticipate()

	--chargeParticle = this:FindEntity("ChargeParticle")
	--chargeParticle:GetComponent("Transform").Position = transform.Position
	--chargeParticle:GetComponent("ParticleEmitter"):EmitOnce()


	--Print("anti")
	LookAtPlayer(false)
	--nma.SetDestination(transform.position + dir);
	moveDestPos = transform.Position - dir
	
	-- hover = this:CreateEntity()
	-- if(hover:LoadPrefab("BananaTree")) then
	-- 	hover:GetComponent("Transform"):Set_Position(moveDestPos)
	-- end

	timer = timer - curDT

	if timer <= 0 then
		chargeParticle = this:CreateEntity()
		chargeParticle:LoadPrefab("ChargeParticle")
		chargeParticle:GetComponent("Transform").Position = transform.Position
		--nma.SetDestination(transform.position);
		moveDestPos = transform.Position
		--attack
		NextState = 3.0
		UpdateState()
		this:GetComponent("AABBCollider"):SetTrigger(true)
		rb:SwitchGravity(false)

		--FxHandler.inst.PlaySfx("PangoCharge", transform.position, 0.3f);
	end
end

function Attack()
	--Print("ATTACK")
	draw = attackTarget

	distVec = attackTarget - transform.Position
	dist = distVec:Length()
	curSpeed = rb.Velocity:Length()

	if(_animTimer > _animDuration)then
		--Print("start chage")
		sk.Loop = true
		ChangeAnim(anim_run)
	end

	if dist < 0.4 * attackDist and curSpeed > 50 then
		--Print("err")
		--Print(tostring(curSpeed))
		moveSpeed = moveSpeed - curDT * attackAccel
		rb.Velocity = distVec:Unit() * moveSpeed
	end

	--Print("check")
	if(dist < 50 + moveStopDist and rb.Velocity:Length() <= 50)then
		--Print(tostring(dist))
		this:GetComponent("AABBCollider"):SetTrigger(false)
		rb:SwitchGravity(true)
		--Print("cooling")
		NextState = 4.0
		UpdateState()
		rb.constraintPositionY = false
	end
end

function Cooldown()
	LookAtPlayer(true)
	timer = timer - curDT
	if(timer <= 0) then
		NextState = 1.0
		UpdateState()
	end
end

function BigMamaMode()
	mamaDirection = (bigMamaPosition - transform.Position)
	dist = mamaDirection:Length()
	if(_animTimer > _animDuration)then
		sk.Loop = true
		ChangeAnim(anim_run)
		--Print("runrunrun")
	end
	--Print(tostring(dist))
	if(dist <= 80)then
		--Print("MAMAMODE")
		mama:GetComponent("Script"):CallLuaFunction("IncreasePongosOnPoint")
		-- mama.PangosOnPoint++;
		-- nma.ResetPath();
		ResetPath()
		-- gameObject.SetActive(false);
		this:Disable()
		DisableComs()
	end

end

function JumpAttack()
	--Print("JumpAttack")
	--Print(tostring(JumpState))
	jumpStateTimer = jumpStateTimer + curDT
	func = JumpStates[JumpState]
	if(func)then
		func()
	else
		Print("Error in stoneindi jumpattack")
	end
end

local States =
{
	[0.0] = Prepare,
	[1.0] = Chase,
	[2.0] = Anticipate,
	[3.0] = Attack,
	[4.0] = Cooldown,
	[5.0] = BigMamaMode,
	[6.0] = JumpAttack
}



function Awake()
	--Print(tostring(DropSpeed))
	transform = this:GetComponent("Transform")
	--Print(tostring(newPos2))
	newPos2 = transform.Position - Vector3:new(0,curDT * DropSpeed,0)
	--Print(tostring(newPos2))
	hitPoint = RayCastPoint(transform.Position, Vector3:new(0,-1,0),curDT * DropSpeed)
	hitLength = (hitPoint - transform.Position):Length()
	if(hitLength <= curDT * DropSpeed)then
		rb:SwitchGravity(true)
		newPos2 = hitPoint
		JumpState = 4.0

		jumpStateTimer = 0
		mama:GetComponent("Script"):CallLuaFunction("IncreasePongosLanded")
		moveUpdatePos = true
		jumpAttackExplosion = true;
		this:GetComponent("AABBCollider"):SetTrigger(false)
		awakeDrop = true
	end
	--Print(newPos2)
	transform:Set_Position(newPos2)
end

function NextStateChase()
	-- nma.speed = chaseSpeed;
	moveSpeed = chaseSpeed

	-- nma.acceleration = chaseAccel;
	moveAccel = chaseAccel

	-- nma.stoppingDistance = 0;
	moveStopDist = 0
	sk.Loop = false
	ChangeAnim(anim_run_start)
end

function NextStateAnticipate()
	timer = 1
	--anticipationAnim.SetTrigger("Anticipate");
	ResetSpeeds()
	moveReverse = true
	moveDestPos = transform.Position - dir * 1.5
	moveReverseDir = moveDestPos - transform.Position
	ChangeAnim(anim_charge_idle)
	this:GetChild(1):GetComponent("Script"):CallLuaFunction("Activate")
end

function NextStateAttack()
	-- transform.GetChild(1).gameObject.SetActive(false);
	-- transform.GetChild(0).gameObject.SetActive(true);
	
	attackTarget = transform.Position + dir * attackDist
	sk.Loop = false
	ChangeAnim(anim_charge_start)
	_animTimer = 0
	rb.constraintPositionY = true

	-- path = new NavMeshPath();
	-- nma.CalculatePath(attackTarget, path);

	-- hover = this:CreateEntity()
	-- if(hover:LoadPrefab("BananaTree")) then
	-- 	hover:GetComponent("Transform"):Set_Position(attackTarget)
	-- end

	--NavMeshHit hit = new NavMeshHit();

	-- if(path.status == NavMeshPathStatus.PathInvalid )
	-- {
	-- 	bool blocked = NavMesh.Raycast(transform.position, attackTarget, out hit, nma.areaMask);

	-- 	//print(blocked);

	-- 	attackTarget = hit.position;

	-- 	draw3 = hit.position;
	-- }

	moveDestPos = attackTarget

	-- nma.SetDestination(attackTarget);
	
	-- nma.speed = attackSpeed;
	moveSpeed = attackSpeed
	-- nma.acceleration = attackAccel;
	moveAccel = attackAccel

	-- nma.stoppingDistance = 0;
	moveStopDist = 0
	moveReverse = false
end

function NextStateCooldown()
	--transform.GetChild(0).gameObject.SetActive(false);
	timer = coolDownTime
	ResetSpeeds()
	sk.Loop = true
	ChangeAnim(anim_idle)
	if(stun == true)then
		ChangeAnim(anim_stun)
		timer = timer + timer
		stun = false
	end
end

function Stunned()
	stun = true
end

function NextStateBigMamaMode()
	ResetSpeeds()
	sk.Loop = true
	ChangeAnim(anim_run)
	--Print("runstart")
	--Print(tostring(_animDuration))
	--FxHandler.inst.PlaySfx("Poof", transform.position, 0.3f);
end

function NextStateJumpAttack()
	ResetSpeeds();
	jumpStateTimer = 0;
	JumpState  = 0.0
	sk.Loop = true
	ChangeAnim(anim_idle)

	--FxHandler.inst.PlaySfx("PangoJump", transform.position, 0.2f);

	--anim.SetTrigger("Jump");
	--nma.updatePosition = false;
	moveUpdatePos = false
end

local NextStates =
{
	[1.0] = NextStateChase,
	[2.0] = NextStateAnticipate,
	[3.0] = NextStateAttack,
	[4.0] = NextStateCooldown,
	[5.0] = NextStateBigMamaMode,
	[6.0] = NextStateJumpAttack
}

function UpdateState()
	func = NextStates[NextState]
	--Print("5")
	--Print(tostring(NextState))
	if(func)then
		func()
	else
		Print("Error in StoneIndi NextState switch statement")
	end
	State = NextState 
end

function ResetSpeeds()
	-- transform.GetChild(1).gameObject.SetActive(false);
	-- transform.GetChild(0).gameObject.SetActive(false);

	-- nma.ResetPath();
	ResetPath()

	-- nma.speed = chaseSpeed;
	moveSpeed = chaseSpeed
	-- nma.acceleration = chaseAccel;
	moveAccel = chaseAccel
	rb:SetVelocity(Vector3:new())
	moveReverse = false
end

function LookAtPlayer(slowTurn)
	dir = Player:GetComponent("Transform").Position - transform.Position 
	dir = dir:Normalize()

	transform:Look_At(Player:GetComponent("Transform").Position)

	draw2 = dir
end

function MoveToPlayer()
	-- nma.SetDestination(PlayerScript.instance.transform.position + dir * 3);
	moveDestPos = Player:GetComponent("Transform").Position + dir * 3
	draw = Player:GetComponent("Transform").Position + dir * 2;
end

function MoveToBigMama()
	NextState = 5.0
	UpdateState()
	--nma.SetDestination(mamaPosition);
	moveDestPos = MamaCenter

	bigMamaPosition = MamaCenter;

	bossJoinParticle = this:FindEntity("BossJoinParticle")
	bossJoinParticle:GetComponent("ParticleEmitter").emiting = true
	bossJoinParticle:GetComponent("Transform").Position = MamaCenter

	
end

function ShowDropZone()
--this function creates the drop circle where the boss will drop
--currently disabled cause the needed functionality is not available
	-- if DropZone ~= nil then
		-- RaycastHit hit;
		-- if (Physics.Raycast(transform.position, (Target - transform.position), out hit, DropZoneLM))
		-- {
			-- // DropZone = GameObject.Instantiate(DropZoneTemplate, Target, Quaternion.Euler(90, 0, 0));

			-- DropZone = Instantiate(darkAura, Target, Quaternion.Euler(-90, 0, 0));
		-- }
	-- end
	--Print("DropZoneAppear")
end


function ResetPath()
	moveDestPos = Vector3:new()
	moveAccel = 0
	moveSpeed = 0
end

function InitializeStone()
	Awake()
end

function Update(dt)
	if(dead == false)then
		curDT = dt
		if(toBeLocation ~= Vector3:new()) then
			transform:Set_Position(toBeLocation)
			toBeLocation = Vector3:new()
		end
		if(_animTimer < _animDuration)then
			_animTimer = _animTimer + dt
		end
		if(awakeDrop == false) then 
			Awake()
		else
			func = States[State]
			if(func) then
				func()
			else
				Print("StoneIndi switch error")
			end
			Movement()
		end
		if(sk.Tint.w > 0) then
			sk.Tint = Vector4:new(sk.Tint.x, sk.Tint.y, sk.Tint.z, sk.Tint.w - 0.05)
		end
	else
		if(_animTimer < _animDuration)then
			_animTimer = _animTimer + dt
		end
		--Print(_animTimer)
		--Print(_animDuration)
		if(_animTimer >= _animDuration)then
			--Print("DEAD BOSS")
			--Print(tostring(deadAnimFinish))
			deadAnimFinish = true
			mama:GetComponent("Script"):CallLuaFunction("IncreasePongosDead")
		end
	end
end

function GetDeadAnimFinish()
	return deadAnimFinish
end

function CollisionEnter(ent)
	if(ent == Player)then
	end
end

function CollisionStay(ent)

end

function SetParent()
	mama = this:GetParent()
end

function DisableComs()
	this:GetComponent("AABBCollider"):SetTrigger(true)
	rb:SwitchGravity(false)
	rb:SetVelocity(Vector3:new())
end

function EnableComs()
	this:GetComponent("AABBCollider"):SetTrigger(false)
	rb:SwitchGravity(true)
	rb:SetVelocity(Vector3:new())
end

function ChangeAnim(anim)
	--Print(tostring(anim))
	_animTimer = 0
	if(anim == "")then
		return
	end
	--sk:SetMesh(anim)
	--Print(anim)
	sk:SetAnimation(anim)
	_animDuration = sk:AnimNameDuration(anim)
	sk:PlayAnimation()
	--Print("new duration")
	--Print(_animDuration)
end

function Death()
	dead = true
	sk.Loop = false
	ChangeAnim(anim_death1)
end

function Movement()
	if(moveUpdatePos == true)then
		if(moveReverse == false)then
			if(moveDestPos ~= Vector3:new())then
				if(moveDestPos.y ~= transform.Position.y)then
					moveDestPos.y = transform.Position.y
				end
				transform:Look_At(moveDestPos)
				rb:AddForwardForce(moveSpeed )
				curDist = (moveDestPos - transform.Position):Length()
				if(curDist <= moveStopDist)then
					moveDestPos = Vector3:new()
					this:GetComponent("AABBCollider"):SetTrigger(false)
					rb:SwitchGravity(true)
					NextState = 4.0
					UpdateState()
				else
					transform:Look_At(moveDestPos)
					rb:AddForwardForce(moveSpeed )
				end
				curVelo = rb.Velocity
				if(curVelo:Length() > moveSpeedLimit)then
					curVelo:Normalize()
					rb.Velocity = curVelo * moveSpeedLimit
				end
			end
		else
			if(moveDestPos ~= Vector3:new())then
				if(moveDestPos.y ~= transform.Position.y)then
					moveDestPos.y = transform.Position.y
				end
				inverseMoveDestPos = Vector3:new(-moveReverseDir.x,moveReverseDir.y,-moveReverseDir.z)
				transform:Look_At( inverseMoveDestPos * 5 + transform.Position)
				rb:AddForwardForce( -moveSpeed )
				curVelo = rb.Velocity
				if(curVelo:Length() > moveSpeedLimit)then
					curVelo:Normalize()
					rb.Velocity = curVelo * moveSpeedLimit
				end
			end
		end
	end
end

function ActivateHurtEffect()
	particleComponent = this:GetComponent("ParticleEmitter")
	--Print(type(particleComponent))
	if(particleComponent ~= nil)then
		particleComponent:EmitOnce()
	end
	sk.Tint = Vector4:new(1,1,1,1)
end