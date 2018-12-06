_playingWalkSound = false
_walkSoundPlayed = false
moveSpeed = 40
speedLimit = 60
jumpForce = 25
walkSoundID = 0
jumpSoundID = 0
ploughSoundID = 1
plantSoundID = 2
taugeyRiseSoundID = 3
attackSoundID = 4
tossCarrotSoundID = 5
weaponDropSoundID = 6
getHitSoundID = 7
bossHitSoundID = 8
combatBGMSoundID = 1
leftKey = "LEFT"
rightKey = "RIGHT"
upKey = "UP"
downKey = "DOWN"
interactKey = "E"
jumpKey = "SPACE"
lmouseKey = "LMOUSE"
rmouseKey = "RMOUSE"
_forwardVector = Vector3:new()
_leftVector = Vector3:new()
_currPlant = 0
_numPlants = 3
plantPrefab1 = ""
plantPrefab2 = ""
plantPrefab3 = ""
plantPrefab4 = ""
soilPrefab = ""
_health = 8
maxHealth = 8
attackPower = 1
attackRange = 1
attackAngle = 90
enemyName = ""
_animDuration = 1.0
_animTimer = 0.0
_animPlaying = false
carrotSwordBoost = 1
_holdingSword = false
carrotSwordPrefab = ""
carrotSwordOffset = Vector3:new()
throwStrength = 30
healIncrement = 3
isDay = true
maskOn = false
dayMaterial = ""
nightMaterial = ""
enemies = {}
_numEnemies = 4
_selecting = false
_tilesCollided = 0
_ploughing = false
plants = StringList:new()
ploughAnimPercentage = 0.5
attackAnimPercentage = 0.5
_attacking = false
_restrictMovement = true
_throwing = false
throwAnimPercentage = 0.75
_pickingUp = false
pickupAnimPercentage = 0.5
_scrollAmt = 0
_flinching = false
flinchSpeed = 1000
_startFightTrigger = false
_attackRecover = false
dayAnim = "CompiledAssets/Models/d_player_hoe.mdl"
nightAnim = "CompiledAssets/Models/n_player_hoe.mdl"
nightAnimKris = "CompiledAssets/Models/n_player_kris.mdl"
soilObject = {}
_toChangeModel = false
_modelToChange = 0
_dodging = false
dodgeAnimPercentage = 0.8
dodgeSpeed = 8000
lungeSpeed = 1000
maxJumpVec = 400
minPlayerPos = 0.5
_particleTimer = 0

function Awake()
	controller = this:GetComponent("Controller")
	rb = this:GetComponent("RigidBody")
	sound = this:GetComponent("Sound")
	transform = this:GetComponent("Transform")
	player = this:GetComponent("Player")
	camera = this:FindEntity("Camera"):GetComponent("Camera")
	gm = this:FindEntity("GameManager"):GetComponent("Script")
	--enemy = this:FindEntity(enemyName)
	sk = this:GetComponent("SkeletalMesh")
	skr = this:GetComponent("SkeletalMeshRenderer")
	enemies[0] = this:FindEntity("BossStone1")
	enemies[1] = this:FindEntity("BossStone2")
	enemies[2] = this:FindEntity("BossStone3")
	enemies[3] = this:FindEntity("BossStone4")
	enemies[4] = this:FindEntity("BossStone5")
	PreloadPrefab(plantPrefab1)
	PreloadPrefab(plantPrefab2)
	PreloadPrefab(plantPrefab3)
	PreloadPrefab(plantPrefab4)
	PreloadPrefab(carrotSwordPrefab)
	PreloadPrefab(soilPrefab)

	-- Update the UI Component
	uiManager = this:FindExternalEntity("UIManager")
	uiScript = uiManager:GetComponent("Script")
	uiScript:SetVariable("_itemMat", plants)
end

function Initialize()
	uiScript:CallLuaFunction("UpdatePlantUI")

    rb:SetSpeedLimit(speedLimit)
	_health = maxHealth
	hover = this:CreateEntity()
	if(hover:LoadPrefab("HoverTile")) then
		newPos = Vector3:new(-10000,-10000,-10000)
		hover:GetComponent("Transform"):Set_Position(newPos)
	end
end

function StartBattle()
	
	if(gm:CallLuaFunction("CheckEnemyHealth") ~= true) then
		--Print("StartBattle")
		smokeParticle = this:FindEntity("TransformSmokeParticle")
		smokeParticle:GetComponent("ParticleEmitter"):EmitOnce();
		smokeParticle:GetComponent("Transform").Position = this:GetComponent("Transform").Position

		this:FindEntity("Enemy"):GetComponent("Script"):SetBoolVariable("awake", true)
		this:FindEntity("Enemy"):GetComponent("Script"):CallLuaFunction("SpawnBoss")
		this:FindEntity("Light"):GetComponent("PointLight").Light_Diffuse = Vector4:new(22, 27, 45, 255) * (1 / 255)				
		this:FindEntity("Light"):GetComponent("PointLight").Light_Intensity = 3
		--change point light colour that is on player during night time
		this:FindEntity("PlayerLight"):GetComponent("PointLight").Light_Diffuse =  Vector4:new(0, 255, 240, 255) * (1 / 255)
		this:FindEntity("PlayerLight"):GetComponent("PointLight").Light_Specular =  Vector4:new(1, 52, 43, 255) * (1 / 255)
		this:SetFogColor(Vector4:new(0,0,0,0.2))
		--this:FindEntity("Light"):GetComponent("PointLight").Light_Specular = Vector4:new(59, 32, 66, 255) * (1 / 255)

		this:SetSkybox("Left", "Resources/Texture/sky/PurpleSky_Left.png")
		this:SetSkybox("Right", "Resources/Texture/sky/PurpleSky_Right.png")
		this:SetSkybox("Down", "Resources/Texture/sky/PurpleSky_Bottom.png")
		this:SetSkybox("Up", "Resources/Texture/sky/PurpleSky_Top.png")
		this:SetSkybox("Back", "Resources/Texture/sky/PurpleSky_Back.png")
		this:SetSkybox("Front", "Resources/Texture/sky/PurpleSky_Front.png")
		isDay = false
		if(not maskOn) then
			ChangeModel(1)
			maskOn = true

		end
		_startFightTrigger = false
		_tilesCollided = 0
		newPos = Vector3:new(-10000,-10000,-10000)
		hover:GetComponent("Transform"):Set_Position(newPos)
		if soil then
			local soil = soilObject:GetComponent("Script")
			soil:SetVariable("_toggleHelper", false)
			soil:CallLuaFunction("DisplayHelper")
		end
		sound:Stop_BGM()
		sound:Play_BGM(1)
	end
end

function Update(dt)
	UpdateCamera()
	if(_animPlaying) then
		_animTimer = _animTimer + dt
		if(_ploughing) then
			if(_animTimer > _animDuration * ploughAnimPercentage) then
				_ploughing = false
				PloughSoil2(ploughPosition)
			end
		end
		if(_attacking) then
			if(_animTimer > _animDuration * attackAnimPercentage) then
				_attacking = false
				if(_holdingSword) then
					if(not _attackRecover) then
						_attackRecover = true
						atkAnim = "Attack2"
					elseif(atkAnim == "Attack3") then
						_attackRecover = false
					else
						atkAnim = "Attack3"
					end
				end
				Attack2()
			elseif(_attackRecover) then
				rb:AddDirectionForce(atkRot, lungeSpeed)
			end
		elseif(_attackRecover) then
			if(controller:GetKeyDown(lmouseKey)) then
				Attack()
			end
		end
		if(_throwing) then
			if(_animTimer > _animDuration * throwAnimPercentage) then
				_throwing = false
				DiscardWeapon()
			end
		end
		if(_pickingUp) then
			if(_animTimer > _animDuration * pickupAnimPercentage) then
				_pickingUp = false
				CreateWeapon()
			end
		end
		if(_flinching) then
			rb:AddDirectionForce(backwardVector, flinchSpeed)
			if(_animTimer > _animDuration) then
				_flinching = false
			end
		end
		if(_dodging) then
			rb:AddDirectionForce(dodgeRot, dodgeSpeed)
			if(_animTimer > _animDuration * dodgeAnimPercentage) then
				_dodging = false
			end
		end
		if(_animTimer > _animDuration) then
			_animTimer = 0
			_animPlaying = false
			_restrictMovement = true
			_attackRecover = false
			if(_toChangeModel) then
				ChangeModel(_modelToChange)
				_toChangeModel = false
			end
			if(controller:GetKeyHold(upKey)) then
				ChangeAnim("Run")
				rb:AddImpulseDirectionForce(_forwardVector, moveSpeed)
			elseif(controller:GetKeyHold(downKey)) then
				ChangeAnim("Run")
				rb:AddImpulseDirectionForce(_forwardVector, -moveSpeed)
			elseif(controller:GetKeyHold(leftKey)) then 
				ChangeAnim("Run")
				rb:AddImpulseDirectionForce(_leftVector, moveSpeed)
			elseif(controller:GetKeyHold(rightKey)) then
				ChangeAnim("Run")
				rb:AddImpulseDirectionForce(_leftVector, -moveSpeed)
			else
				ChangeAnim("Idle")
			end
		end
	end
	if(ent and controller:GetKeyDown("V")) then
		Print(ent:GetComponent("Transform").Position)
	end
	if(not maskOn) then
		if(controller:GetKeyDown("Q")) then
			_selecting = true
			Print("Enter Seed Selection")
		elseif(controller:GetKeyUp("Q")) then
			_selecting = false
			Print("Exit Seed Selection")
		end

		if(controller:GetKeyHold("Q")) then
			printOut = false
			if(controller:GetKeyDown(lmouseKey)) then
				_currPlant = _currPlant - 1
				if(_currPlant < 0) then
					_currPlant = _numPlants
				end
				printOut = true
			elseif(controller:GetKeyDown(rmouseKey)) then
				_currPlant = _currPlant + 1
				if(_currPlant > _numPlants) then
					_currPlant = 0
				end
				printOut = true
			end
			if(printOut) then
				--uiScript:SetFloatVariable("newSelect", _currPlant)
				--uiScript:CallLuaFunction("UpdatePlantUI")
				if (_currPlant == 0) then
					Print("Current Seed Selected: None")
				elseif(_currPlant == 1) then
					Print("Current Seed Selected: Taugeh")
				elseif(_currPlant == 2) then
					Print("Current Seed Selected: Tomato")
				elseif(_currPlant == 3) then
					Print("Current Seed Selected: Carrot")
				elseif(_currPlant == 4) then
					Print("Current Seed Selected: Cabbage")
				end
			end
		end
		uiScript:CallLuaFunction("UpdatePlantUI")
	end
	if(not _animPlaying or not _restrictMovement) then
		if(controller:GetKeyUp(upKey) or controller:GetKeyUp(downKey) or controller:GetKeyUp(leftKey) or controller:GetKeyUp(rightKey)) then
			playingWalkSound = false;
			if(_restrictMovement and sk.Model_Directory ~= idleAnim and not controller:GetKeyHold(upKey) and not controller:GetKeyHold(downKey) and not controller:GetKeyHold(leftKey) and not controller:GetKeyHold(rightKey)) then
				ChangeAnim("Idle")
			end
		end
		if(controller:GetKeyDown(upKey)) then
			rb:AddImpulseDirectionForce(_forwardVector, moveSpeed)
			playingWalkSound = true
			if(_restrictMovement) then
				ChangeAnim("Run")
			end
		elseif(controller:GetKeyHold(upKey)) then
			--rb:AddDirectionForce(_forwardVector, moveSpeed)
			playingWalkSound = true
			if(controller:GetKeyHold(leftKey)) then
				ChangeDirection(_forwardVector, 45)
				rb:AddDirectionForce(_forwardVector, moveSpeed/2)
			elseif(controller:GetKeyHold(rightKey)) then
				ChangeDirection(_forwardVector, -45)
				rb:AddDirectionForce(_forwardVector, moveSpeed/2)
			else
				ChangeDirection(_forwardVector, 0)
				rb:AddDirectionForce(_forwardVector, moveSpeed)
			end
			EmitRunParticles(dt)
		end
		if(controller:GetKeyDown(downKey)) then
			rb:AddImpulseDirectionForce(_forwardVector, -moveSpeed)
			playingWalkSound = true
			if(_restrictMovement) then
				ChangeAnim("Run")
			end
		elseif(controller:GetKeyHold(downKey)) then
			--rb:AddDirectionForce(_forwardVector, -moveSpeed)
			playingWalkSound = true
			if(controller:GetKeyHold(leftKey)) then
				ChangeDirection(_forwardVector, 135)
				rb:AddDirectionForce(_forwardVector, -moveSpeed/2)
			elseif(controller:GetKeyHold(rightKey)) then
				ChangeDirection(_forwardVector, -135)
				rb:AddDirectionForce(_forwardVector, -moveSpeed/2)
			else
				ChangeDirection(_forwardVector, 180)
				rb:AddDirectionForce(_forwardVector, -moveSpeed)
			end
			EmitRunParticles(dt)
		end
		if(controller:GetKeyDown(leftKey)) then
			rb:AddImpulseDirectionForce(_leftVector, moveSpeed)
			playingWalkSound = true
			if(_restrictMovement) then
				ChangeAnim("Run")
			end
		elseif(controller:GetKeyHold(leftKey)) then
			--rb:AddDirectionForce(_leftVector, moveSpeed)
			playingWalkSound = true
			if(not controller:GetKeyHold(upKey) and not controller:GetKeyHold(downKey)) then
				ChangeDirection(_forwardVector, 90)
				rb:AddDirectionForce(_leftVector, moveSpeed)
			else
				rb:AddDirectionForce(_leftVector, moveSpeed/2)
			end
			EmitRunParticles(dt)
		end
		if(controller:GetKeyDown(rightKey)) then
			rb:AddImpulseDirectionForce(_leftVector, -moveSpeed)
			playingWalkSound = true
			if(_restrictMovement) then
				ChangeAnim("Run")
			end
		elseif(controller:GetKeyHold(rightKey)) then
			--rb:AddDirectionForce(_leftVector, -moveSpeed)
			playingWalkSound = true
			if(not controller:GetKeyHold(upKey) and not controller:GetKeyHold(downKey)) then
				ChangeDirection(_forwardVector, -90)
				rb:AddDirectionForce(_leftVector, -moveSpeed)
			else
				rb:AddDirectionForce(_leftVector, -moveSpeed/2)
			end
			EmitRunParticles(dt)
		end
		if(controller:GetKeyDown(interactKey)) then
		end
		if(controller:GetKeyDown(jumpKey) and _restrictMovement) then
			rb:AddImpulseUpForce(jumpForce)
			sound:Play_SFX(jumpSoundID)
			ChangeAnim("Jump")
			_animPlaying = true
			_restrictMovement = false
		end
		if(controller:GetKeyDown(lmouseKey) and maskOn) then
			Attack()
		end
		if(controller:GetKeyUp(rmouseKey) and _holdingSword and not _selecting and not _animPlaying) then
			--DiscardWeapon()
			ChangeAnim("StaticThrow")
			_animPlaying = true
			_throwing = true
		end
		if(controller:GetKeyDown("LSHIFT") and maskOn) then
			ChangeAnim("Dodge")
			_animPlaying = true
			_dodging = true
			dodgeRot = Vector3:new()
			dodgeRot.x = Cosine(transform.Rotation.y-90)
			dodgeRot.z = -1 * Sine(transform.Rotation.y-90)
			rb:AddImpulseDirectionForce(dodgeRot,5000)
		end
	end
	if(isDay and not _animPlaying) then
		if(controller:GetKeyDown("N")) then
			maskOn = not maskOn
			if(maskOn) then
				print("Mask On")

				smokeParticle = this:FindEntity("TransformSmokeParticle")
				smokeParticle:GetComponent("ParticleEmitter"):EmitOnce();
				smokeParticle:GetComponent("Transform").Position = this:GetComponent("Transform").Position

				ChangeModel(1)
				
		
			else
				print("Mask Off")

				smokeParticle = this:FindEntity("SmokeParticle")
				smokeParticle:GetComponent("ParticleEmitter"):EmitOnce();
				smokeParticle:GetComponent("Transform").Position = this:GetComponent("Transform").Position

				DropWeapon()
				ChangeModel(0)
		
			end
		end

		if(controller:GetKeyDown("M")) then
			uiScript:CallLuaFunction("NightMode")
			StartBattle()
		end
		if(_startFightTrigger) then
			uiScript:CallLuaFunction("NightMode")
			StartBattle()
		end
	end
	
	if(not playingWalkSound) then
		StopWalkSound()
	else
		PlayWalkSound()
	end
	
	if(_holdingSword and carrotSword ~= nil) then
		pos = transform.Position
		pos.x = pos.x + carrotSwordOffset.x
		pos.y = pos.y + carrotSwordOffset.y
		pos.z = pos.z + carrotSwordOffset.z
		carrotSword:GetComponent("Transform"):Set_Position(pos)
	end
	if(transform.Position.y < minPlayerPos) then
		rePos = Vector3:new()
		rePos.x = transform.Position.x
		rePos.z = transform.Position.z
		rePos.y = minPlayerPos
		transform:Set_Position(rePos)
	end
	if(rb.Velocity.y > maxJumpVec) then
		reVec = Vector3:new()
		reVec.x = rb.Velocity.x
		reVec.y = maxJumpVec
		reVec.z = rb.Velocity.z
		rb:SetVelocity(reVec)
	end
end

function PlayWalkSound()
	if(not walkSoundPlayed) then
		sound:Play_LSFX(walkSoundID)
		walkSoundPlayed = true
	end
end

function StopWalkSound()
	if(walkSoundPlayed) then
		sound:Stop_LSFX(walkSoundID)
		walkSoundPlayed = false
	end
end

function UpdateCamera()
	target_pos = camera:GetTargetPosition()
	camera_pos = camera:GetCameraPosition()
	camera_up = camera:GetUp()
	_forwardVector.x = target_pos.x - camera_pos.x
	_forwardVector.y = target_pos.y - camera_pos.y
	_forwardVector.z = target_pos.z - camera_pos.z
	_forwardVector = _forwardVector:Unit()
	_forwardVector.y = 0
	_leftVector.x = camera_up.y * _forwardVector.z - camera_up.z * _forwardVector.y
	_leftVector.y = camera_up.z * _forwardVector.x - camera_up.x * _forwardVector.z
	_leftVector.z = camera_up.x * _forwardVector.y - camera_up.y * _forwardVector.x
	_leftVector = _leftVector:Unit()
	_leftVector.y = 0
end

function PloughSoil(position)
	if(not _animPlaying) then
		if(isDay) then
			ChangeAnim("Plough")

			smokeParticle = this:FindEntity("SmokeParticle")
			smokeParticle:GetComponent("Transform").Position = position
			smokeParticle:GetComponent("ParticleEmitter"):EmitOnce()
		end
		_animPlaying = true
		playingWalkSound = false;
		--_animDuration = this:GetComponent("SkeletalMeshRenderer"):AnimationDuration(0)
		_ploughing = true
		ploughPosition = position
		sound:Play_SFX(ploughSoundID)
	end
end

function PloughSoil2(position)
	local ent = this:CreateEntity()
	if(ent:LoadPrefab(soilPrefab)) then
		position.y = position.y - 5
		ent:GetComponent("Transform"):Set_Position(position)
		local script = ent:GetComponent("Script")
		script:SetFloatVariable("_x",x)
		script:SetFloatVariable("_y",y)

		script:CallLuaFunction("UpdateModel")
	end
end

function PlantSeed(position)
	if(_currPlant == 1) then
		soil:GetComponent("Script"):SetStringVariable("plantPrefab", plantPrefab1)
		Print("Taugeh Planted")
		PerformPlanting()
		soil:GetComponent("Script"):SetBoolVariable("tileUsed", true)
		soilObject:GetComponent("Script"):SetStringVariable("_plantName", "Tauge")
	elseif(_currPlant == 2) then
		soil:GetComponent("Script"):SetStringVariable("plantPrefab", plantPrefab2)
		Print("Tomato Planted")
		PerformPlanting()
		soil:GetComponent("Script"):SetBoolVariable("tileUsed", true)
		soilObject:GetComponent("Script"):SetStringVariable("_plantName", "Flower")
	elseif(_currPlant == 3) then
		soil:GetComponent("Script"):SetStringVariable("plantPrefab", plantPrefab3)
		Print("Carrot Planted")
		PerformPlanting()
		soil:GetComponent("Script"):SetBoolVariable("tileUsed", true)
		soilObject:GetComponent("Script"):SetStringVariable("_plantName", "Carrot")
	elseif(_currPlant == 4) then
		soil:GetComponent("Script"):SetStringVariable("plantPrefab", plantPrefab4)
		Print("Cabbage Planted")
		PerformPlanting()
		soil:GetComponent("Script"):SetBoolVariable("tileUsed", true)
		soilObject:GetComponent("Script"):SetStringVariable("_plantName", "Cabbage")
	elseif(_currPlant == 0) then
		soil:GetComponent("Script"):SetStringVariable("plantPrefab", "None")
		Print("No seed selected!")
		--PerformPlanting()
	end
end
--Had to add this because sometimes _currPlant is not 1-4 for some reason...
function PerformPlanting()
	soil:GetComponent("Script"):CallLuaFunction("StartPlant")
	sound:Play_SFX(plantSoundID)
	smokeParticle = this:FindEntity("SmokeParticle")
	smokeParticle:GetComponent("ParticleEmitter"):EmitOnce()
	smokeParticle:GetComponent("Transform").Position = soil:GetComponent("Transform").Position
	ChangeAnim("Grow")
	_animPlaying = true
	playingWalkSound = false;
end

function GetDamage(damage, EnPos)
	_health = _health - damage
	ChangeAnim("Flinch")
	_animPlaying = true
	_flinching = true
	local cheat = false

	Print("Player Health: ")
	Print(_health)

	particleComponent = this:GetComponent("ParticleEmitter")
	if(particleComponent ~= nil)then
		particleComponent:EmitOnce()
	end
	rb:ResetAll()
	--backwardVector = Vector3:new()
	--backwardVector.x = Cosine(transform.Rotation.y-90) * -1
	--backwardVector.z = -1 * Sine(transform.Rotation.y-90) * -1
	--backwardVector.y = 0
	--backwardVector = backwardVector:Unit()
	--backwardVector.y = 0.1
	backwardVector = transform.Position - EnPos
	backwardVector.y = 0
	backwardVector = backwardVector:Unit()
	backwardVector.y = 0.1

	rb:AddImpulseDirectionForce(backwardVector, 12500)
	--rb:AddImpulseUpForce(jumpForce * 4.0)
	if(_health == 0) then
		_health = maxHealth
		cheat = true
		Print("Player Dead")
	end
	
	uHealth = uiScript:GetVariable("UpdatePlayerHealthUI")
	uHealth(_health, cheat)
	sound:Play_SFX(getHitSoundID)
end

function Attack()
	if(not _animPlaying) then
		if(not isDay or maskOn) then
			ChangeAnim("Attack")
		end
		_animPlaying = true
		playingWalkSound = false;
		--_animDuration = this:GetComponent("SkeletalMeshRenderer"):AnimationDuration(0)
		_attacking = true
		sound:Play_SFX(attackSoundID)
	elseif(_attackRecover) then
		_animPlaying = false
		ChangeAnim(atkAnim)
		_animPlaying = true
		_attacking = true;
		_animTimer = 0
		atkRot = Vector3:new()
		atkRot.x = Cosine(transform.Rotation.y-90)
		atkRot.z = -1 * Sine(transform.Rotation.y-90)
		rb:AddImpulseDirectionForce(atkRot,5000)
		sound:Play_SFX(attackSoundID)
	end
end

function Attack2()
	if(this:EntityExists("Scarecrow")) then
		scarecrow = this:FindEntity("Scarecrow")
		pos = scarecrow:GetComponent("Transform").Position
		pos.x = pos.x - transform.Position.x
		pos.y = pos.y - transform.Position.y
		pos.z = pos.z - transform.Position.z
		if(pos:Length() < attackRange) then
			pos:Normalize()
			rot = Vector3:new()
			rot.x = Cosine(transform.Rotation.y-90)
			rot.z = -1 * Sine(transform.Rotation.y-90)
			rot:Normalize()
			dot = ACosine(pos.x * rot.x + pos.y * rot.y + pos.z * rot.z)
			dot = RadToDeg(dot)
			if(dot < attackAngle) then
				scarecrow:GetComponent("SkeletalMeshRenderer"):PlayAnimation()
				sound:Play_SFX(bossHitSoundID)
			end
		end
	end
	if(enemyName ~= "") then
		for i = 0, _numEnemies do
			pos = enemies[i]:GetComponent("Transform").Position
			pos.x = pos.x - transform.Position.x
			pos.y = pos.y - transform.Position.y
			pos.z = pos.z - transform.Position.z
			if(pos:Length() < attackRange and enemies[i]:IsDisabled() == false) then
				--Print("enemynearby")
				pos:Normalize()
				rot = Vector3:new()
				rot.x = Cosine(transform.Rotation.y-90)
				rot.z = -1 * Sine(transform.Rotation.y-90)
				rot:Normalize()
				dot = ACosine(pos.x * rot.x + pos.y * rot.y + pos.z * rot.z)
				dot = RadToDeg(dot)
				if(dot < attackAngle) then
					gm:SetFloatVariable("_damageAmount", attackPower)
					enemies[i]:GetComponent("Script"):CallLuaFunction("ActivateHurtEffect")
					gm:CallLuaFunction("MinusHealth")
					sound:Play_SFX(bossHitSoundID)
				end
			end
		end
		if (this:EntityExists("BigPango"))then
			--Print("theres pango")
			local bigPango = this:FindEntity("BigPango")
			pos = bigPango:GetComponent("Transform").Position
			pos.x = pos.x - transform.Position.x
			pos.y = pos.y - transform.Position.y
			pos.z = pos.z - transform.Position.z
			if(pos:Length() < attackRange) then
				pos:Normalize()
				rot = Vector3:new()
				rot.x = Cosine(transform.Rotation.y-90)
				rot.z = -1 * Sine(transform.Rotation.y-90)
				rot:Normalize()
				dot = ACosine(pos.x * rot.x + pos.y * rot.y + pos.z * rot.z)
				dot = RadToDeg(dot)
				if(dot < attackAngle) then
					bigPango:GetComponent("Script"):CallLuaFunction("GetDamage")
					sound:Play_SFX(bossHitSoundID)
				end
			end
		end
	end
end

function CollisionEnter (ent)
	--Print("Player Collision Enter")
	id = ent:GetComponent("Identifier")
	if(ent.name == "BossSub1" and not _animPlaying) then
		GetDamage(1, ent:GetComponent("Transform").Position)
	elseif(ent.name == "BigPango") then
		GetDamage(1, ent:GetComponent("Transform").Position)
	elseif(id:Exists() and id.ID == "CarrotSword" and maskOn and not _holdingSword and not _animPlaying and ent:GetComponent("Script"):GetBoolVariable("_rest")) then
		ent:Destroy()
		carrotSword = nil
		ChangeAnim("Pickup")
		_animPlaying = true
		_pickingUp = true
	end
end

function SoilCollisionEnter()
	--Print("Collision Enter soil")
	if(soilObject.name == "EmptySoil") then
		if(isDay or soilObject:GetComponent("Script"):GetBoolVariable("tileUsed") == true) then
			--hover = this:CreateEntity()
			--if(hover:LoadPrefab("HoverTile")) then
			newPos = soilObject:GetComponent("Transform").Position
			newPos.y = newPos.y + 15
			hover:GetComponent("Transform"):Set_Position(newPos)
			_tilesCollided = _tilesCollided + 1
			local soil = soilObject:GetComponent("Script")
			soil:SetVariable("_toggleHelper", true)
			soil:CallLuaFunction("DisplayHelper")
		end
	end
end
function SoilCollisionExit()
	--Print("Collision Exit soil")
	if(soilObject.name == "EmptySoil") then
		if(isDay or soilObject:GetComponent("Script"):GetBoolVariable("tileUsed") == true) then
			_tilesCollided = _tilesCollided - 1
			if(_tilesCollided == 0) then
				newPos = Vector3:new(-10000,-10000,-10000)
				hover:GetComponent("Transform"):Set_Position(newPos)
			elseif(_tilesCollided < 0) then
				_tilesCollided = 0
			end
			--newPos = ent:GetComponent("Transform").Position
			--hoverPos = hover:GetComponent("Transform").Position
			--if(newPos.x == hoverPos.x and newPos.z == hoverPos.z) then
			
				--hover:GetComponent("Transform").Position = Vector3:new(newPos.x, -99999999999, newPos.z)
			--end
			local soil = soilObject:GetComponent("Script")
			soil:SetVariable("_toggleHelper", false)
			soil:CallLuaFunction("DisplayHelper")
		end
	end
end
function SoilCollisionStay()
	--Print("Collision Stay soil")
	if(soilObject.name == "EmptySoil") then
		-- About to Plough
		if(controller:GetKeyDown(lmouseKey) and not _animPlaying and not maskOn and not _selecting) then
			if(soilObject:GetComponent("Script"):GetBoolVariable("tileUsed") ~= true) then
				--hover:Destroy()
				--newPos = Vector3:new(-10000,-10000,-10000)
				--hover:GetComponent("Transform"):Set_Position(newPos)
				soilObject:GetComponent("Script"):SetBoolVariable("tileUsed", true)
				PloughSoil(soilObject:GetComponent("Transform").Position)
				x = soilObject:GetComponent("Script"):GetFloatVariable("_x")
				y = soilObject:GetComponent("Script"):GetFloatVariable("_y")
			end
		-- Not Ploughing
		else
			local soilScript = soilObject:GetComponent("Script")
			local ddsFile = "popup_plough.dds"
			soilScript:SetStringVariable("_helperTexture", ddsFile)
			soilScript:CallLuaFunction("UpdateTexture")
		end

		-- Already have a tile
		if soilObject:GetComponent("Script"):GetBoolVariable("tileUsed") then
			local soilScript = soilObject:GetComponent("Script")
			soilScript:SetStringVariable("_helperTexture", GetHelperTexture(soilObject))
			soilScript:CallLuaFunction("UpdateTexture")
		end
	end
end

function CollisionExit (ent)
	--Print("Player Collision Exit")
end

function CollisionStay (ent)
	--Print("Player Collision Stay")
	if(controller:GetKeyDown(lmouseKey) and not _animPlaying and not maskOn and not _selecting) then
		if(ent.name == "GroundTile" or ent.name == "GroundTileSolo" or ent.name == "GroundTileCorner") then
			if(ent:GetComponent("Script"):GetBoolVariable("tileUsed") ~= true) then
				soil = ent
				PlantSeed(ent:GetComponent("Transform").Position)
			end
		end
	elseif(controller:GetKeyDown(interactKey) and (maskOn or not isDay)) then
		id = ent:GetComponent("Identifier")
		if(id:Exists() and (id.ID == "Plant" or id.ID == "DrawnCarrot")) then
			plantName = ent:GetComponent("Script"):GetStringVariable("plantName")
			if(plantName == "Taugeh") then
				Print("Taugeh")
				if(not ent:GetComponent("Script"):GetBoolVariable("_isRising")) then
					ent:GetComponent("Script"):CallLuaFunction("RiseUp")
					--ChangeAnim("Harvest")
					_animPlaying = true;

					sound:Play_SFX(taugeyRiseSoundID)
				end
			elseif(plantName == "Carrot") then
				Print("Carrot")
				if(not _holdingSword and not ent:GetComponent("Script"):GetBoolVariable("_swordDrawn")) then
					ent:GetComponent("Script"):CallLuaFunction("DrawSword")
					if(ent:GetComponent("Script"):GetBoolVariable("_swordDrawn")) then
						carrotName = ent:GetComponent("Identifier").ID
						ent:GetComponent("Identifier"):Update_ID("DrawnCarrot")
						CreateWeapon()
						carrotSword:GetComponent("Script"):SetBoolVariable("_orphaned", false)
						ChangeAnim("Harvest")
						_animPlaying = true;
					end
				end
			elseif(plantName == "Tomato") then
				Print("Tomato")
				if(not ent:GetComponent("Script"):GetBoolVariable("_healUsed")) then
					ent:GetComponent("Script"):CallLuaFunction("Heal")
					if(ent:GetComponent("Script"):GetBoolVariable("_healUsed")) then
						HealPlayer()
					end
				end
			elseif(plantName == "Cabbage") then
				Print("Cabbage")
			end
		end
	end
end

function ChangeDirection(lookDirection, angle)
	lookDirection:Normalize()
	quat = LookAt(transform.Position, transform.Position + lookDirection)
	eulerAngles = QuaternionToEuler(quat)
	if(lookDirection.x < 0) then
		eulerAngles.y = -eulerAngles.y
	end
	transform.Rotation = Vector3:new(0,RadToDeg(eulerAngles.y) + angle,0)
end

function ChangeAnim(anim)
	if(not _animPlaying) then
		skr:SetAnimation(anim)
		_animDuration = skr:CurrAnimDuration()
	end
end

function ChangeModel(mode)
	if(mode == 0) then
		sk:SetMesh(dayAnim)
		sk:SetAllMaterials(dayMaterial)
	elseif(mode == 1) then
		sk:SetMesh(nightAnim)
		sk:SetAllMaterials(nightMaterial)
		sk:SetMaterial("Rocks", 24)
		sk:SetMaterial("Rocks", 25)
	elseif(mode == 2) then
		sk:SetMesh(nightAnimKris)
		sk:SetAllMaterials(nightMaterial)
		sk:SetMaterial("Orange", 24)
	end
end

function CreateWeapon()
	attackPower = attackPower + carrotSwordBoost
	_holdingSword = true
	carrotSword = this:CreateEntity()
	if(carrotSword:LoadPrefab(carrotSwordPrefab)) then
		pos = transform.Position
		pos.x = pos.x + carrotSwordOffset.x
		pos.y = pos.y + carrotSwordOffset.y
		pos.z = pos.z + carrotSwordOffset.z
		carrotSword:GetComponent("Transform"):Set_Position(pos)
		carrotSword:GetComponent("Script"):CallLuaFunction("Pickup")
		carrotSword:GetComponent("Script"):SetBoolVariable("_orphaned", true)
	end
	_toChangeModel = true
	_modelToChange = 2
	sound:Play_SFX(weaponDropSoundID)
	Print("Attack Power Increased")
end

function DiscardWeapon()
	if(_holdingSword) then
		attackPower = attackPower - carrotSwordBoost
		_holdingSword = false
		carrotSword:GetComponent("Script"):CallLuaFunction("Discard")
		rot = Vector3:new()
		rot.x = Cosine(transform.Rotation.y-90)
		rot.z = -1 * Sine(transform.Rotation.y-90)
		pos = transform.Position
		pos.y = pos.y + 50
		pos.x = pos.x + rot.x * 50
		pos.z = pos.z + rot.z * 50
		newRot = Vector3:new(90, transform.Rotation.y + 180, 0)
		carrotSword:GetComponent("Transform"):Set_Rotation(newRot)
		carrotSword:GetComponent("Transform"):Set_Position(pos)
		carrotSword:GetComponent("RigidBody"):AddImpulseDirectionForce(rot, throwStrength)
		--[[if(not carrotSword:GetComponent("Script"):GetBoolVariable("_orphaned")) then
			carrot = this:FindEntity("DrawnCarrot")
			carrot:GetComponent("Script"):CallLuaFunction("RegenSword")
			carrot:GetComponent("Identifier"):Update_ID(carrotName)
		end]]--
		Print("Weapon Discarded")
		--ChangeAnim("StaticThrowTransition")
		--_animPlaying = true;
		if(maskOn) then
			_toChangeModel = true
			_modelToChange = 1
		else
			_toChangeModel = true
			_modelToChange = 0
		end
		sound:Play_SFX(tossCarrotSoundID)
	end
end

function DropWeapon()
	if(_holdingSword) then
		attackPower = attackPower - carrotSwordBoost
		_holdingSword = false
		carrotSword:GetComponent("Script"):CallLuaFunction("Discard")
		rot = Vector3:new()
		rot.x = Cosine(transform.Rotation.y-90)
		rot.z = -1 * Sine(transform.Rotation.y-90)
		pos = transform.Position
		pos.y = pos.y + 50
		pos.x = pos.x + rot.x * 50
		pos.z = pos.z + rot.z * 50
		newRot = Vector3:new(90, transform.Rotation.y + 180, 0)
		carrotSword:GetComponent("Transform"):Set_Rotation(newRot)
		carrotSword:GetComponent("Transform"):Set_Position(pos)
	end
end

function HealPlayer()
	_health = _health + healIncrement
	if(_health > maxHealth) then
		_health = maxHealth
	end
	uHealth = uiScript:GetVariable("UpdatePlayerHealthUI")
	uHealth(_health, false)
	Print("Player Healed")
end

function SetSkyboxes()
	this:SetSkybox("Left", "")
end

function GetHelperTexture(ent)
	local retStr = "popup_plough.dds"
	-- the starting file name of the items should be item_
	local itemFileName = "item_"
	if not maskOn then
		-- the starting file name should be "popup_plant_"
		local plantFileName = "popup_plant_"
		-- Set the texture if the mask is not switched on
		local extracted = ExtractString(plants:GetElem(_currPlant), itemFileName)
		if StringLen(extracted) == 0 then
			retStr = ""
		else
			plantFileName = plantFileName .. extracted
			retStr = plantFileName
		end
	else
		-- the starting file name should be "popup_use_"
		local plantUseName = "popup_use_"
		-- Set the texture if the mask is not switched on
		local plantName = ent:GetComponent("Script"):GetStringVariable("_plantName")
		if StringLen(plantName) == 0 then
			retStr = ""
		else
			plantName = ConvertToLower(plantName)
			-- Append the file
			plantUseName = plantUseName .. plantName .. ".dds"
			retStr = plantUseName
		end
	end
	return retStr
end

function EmitRunParticles(dt)
	_particleTimer = _particleTimer + dt
	if(_particleTimer > 0.1) then
		local runParticlesC = this:CreateEntity()
		runParticlesC:LoadPrefab("PlayerRunParticle")
		runParticlesC:GetComponent("Transform").Position = transform.Position
		_particleTimer = 0
	end
end