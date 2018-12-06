IdleLimit = 1
AttackLimit = 1.0
AttackMovementSpeed = 10
AttackCooldown = 3
DropTimer = 1
DropCooldown = 0
DropBaseCooldown = 10
SpinCooldown = 0
SpinBaseCooldown = 15
GeneralCooldown = 3
DropChargeTime = 1
GeneralCooldown = 0
SpinDelayTime = 1.5
SpinSpeed = 10
DropdownHeight = 100
SpinMode = false
SpinPrep = false
Spinning = false
DropMode = false
DropCharge = false
Dropping = false
Stones = {}
awake = false

dead = false

_Timer = 0
_StoneIndex = 0
_SpinStart = Vector3:new()
_SpinEnd = Vector3:new()
_SpinStartRot = Vector4:new()
_SinEndRot = Vector4:new()

maxHP = 10

timer = 0
NumChildDead = 0

pangosLanded = 0
PangosOnPoint = 0
pangosOnPoint = 0
state = 0.0
lastState = 0.0
mamaPosition = Vector3:new()
bigPangPos = Vector3:new()

function Preparation()
end

function Seperated()
	if(timer > 6)then
		if(lastState ~= 3.0)then
			--dropping
			Print("Boss change to drop")
			NextState(3.0)
		else
			--joinedup
			Print("Boss change to join")
			NextState(1.0)
		end
	end
end

function JoinedUp()
	if(pangosOnPoint ~= NumChildren)then
		if(pangosOnPoint == 0 and PangosOnPoint > pangosOnPoint)then
			-- 	spawnedDustCloud = Instantiate(dustCloud, mamaPosition, Quaternion.identity);
		elseif PangosOnPoint == NumChildren then
			-- 	Destroy(spawnedDustCloud, 1);
			bossJoinParticle = this:FindEntity("BossJoinParticle")
			bossJoinParticle:GetComponent("ParticleEmitter").emiting = false
			bigPango:Enable()
			bigPango:GetChild(0):Enable()
			bigPango:GetComponent("Script"):CallLuaFunction("EnableComs")
			bigPango:GetComponent("Transform"):Set_Position(mamaPosition + Vector3:new(0,80,0))
		end
	end
	pangosOnPoint = PangosOnPoint
	-- if(pangosOnPoint == 0 && value > pangosOnPoint)
	-- {
	-- }
	-- else if(value == pangoList.Count)
	-- {

	-- 	bigPango.transform.position = mamaPosition + Vector3.up * 2;
	-- 	bigPango.gameObject.SetActive(true);

	-- }            
	-- pangosOnPoint = value;
end

function DropS()
	--Print(tostring(pangosLanded))
	if(pangosLanded == NumChildren)then
		--seperate
		Print("Time to seperate")
		NextState(2.0)
	end
end

function SeperatedIni()
end

function JoinedUpIni()
end

function DropSIni()
end

function Awake()
end	

function NextStatesPreparation()
end

function NextStatesSingular()
	FormIntoBigPango()
end

function NextStatesSeperated()
end

function NextStatesDropping()
	TellAllTheBoisToJump()
	pangosLanded = 0
end

local States =
{
	[0.0] = Preparation,
	[1.0] = JoinedUp,
	[2.0] = Seperated,
	[3.0] = DropS
}
local StateInitializes =
{
	[0.0] = SeperatedIni,
	[1.0] = JoinedUpIni,
	[2.0] = DropSIni,
}
local NextStates = 
{
	[0.0] = NextStatesPreparation,
	[1.0] = NextStatesSingular,
	[2.0] = NextStatesSeperated,
	[3.0] = NextStatesDropping
}

function Initialize()
	transform = this:GetComponent("Transform")
	-- Ground = this:FindEntity("Ground")
	Player = this:FindEntity("Player")
	-- controller = Player:GetComponent("Controller")
	NumChildren = this:GetChildrenCount()
	for i = 0, NumChildren-1 do
		Stones[i] = this:GetChild(i)
		Stones[i]:GetComponent("Script"):CallLuaFunction("SetParent")
		Stones[i]:GetComponent("Script"):SetFloatVariable("Index",i)
	end
		--Print(AttackLimit)
	for var = 0,NumChildren-1 do
		Stones[var]:GetComponent("Script"):SetFloatVariable("AttackRangeLimit", AttackLimit)
		Stones[var]:Disable()
		--Print(tostring(Stones[var]:GetComponent("Script"):GetFloatVariable("AttackRangeLimit")))
	end
	for i = 0, NumChildren-1 do
		--Print(Stones[i].name)
	end

	--bigPango = this:FindEntity("BigPango")
	--bigPango:SetActive(false)
	--bigPango:GetComponent("Script"):CallLuaFunction("SetParent")

	this:ClearChildren()
	bigPango = this:FindEntity("BigPango")
	this:AddChild(bigPango:GetEntity())
	bigPango:GetComponent("Script"):CallLuaFunction("SetParent")
	bigPango:GetComponent("Script"):CallLuaFunction("DisableComs")
	this:ClearChildren()
	bigPango:GetChild(0):Disable()
	bigPango:Disable()
	--Print("AWAKESTART")
	--rb = this:GetComponent("RigidBody")
	if(awake == true)then
		StartBossFight()
	end
end

function StartBossFight()

	if (state ~= 0.0) then return end

	for i = 0, NumChildren-1 do
		Stones[i]:GetComponent("Script"):SetFloatVariable("NextState", 1.0)
		Stones[i]:GetComponent("Script"):CallLuaFunction("UpdateState")
		Stones[i]:Enable()
	end

	NextState(2.0)
	Print("AWAKEEND")
	-- foreach (PangoWango pan in pangoList)
	-- {
	-- 	pan.NextState(PangoWango.State.Chase);
	-- }

	-- NextState(State.Seperated);
end

function NextState(theNextState)
	lastState = state
	timer = 0
	func = NextStates[theNextState]
	if(func)then
		func()
	else
		Print("NextState out of switch statement")
	end
	state = theNextState
end

function SpawnBoss()
Print("SpawningBoss")
	StartBossFight()
end

function Update(dt)
	if(dead == false)then
		if(awake == true)then
			timer = timer + dt
			func = States[state]
			--Print(tostring(state))
			if(func) then
				func()
			else
			end
		end
	else
		if(NumChildDead == 5)then
			Print("Destruction")
			ParentAllSubBoss()
			bigPango:Destroy()
			this:Destroy()
		end
	end
end

function FormIntoBigPango()
	center = Vector3:new()
	for i = 0, NumChildren-1 do 
		center.x = center.x + Stones[i]:GetComponent("Transform").Position.x
		center.z = center.z + Stones[i]:GetComponent("Transform").Position.z
	end
	center.x = center.x / NumChildren
	center.y = center.y + Stones[0]:GetComponent("Transform").Position.y
	center.z = center.z / NumChildren

	mamaPosition = center

	for i = 0, NumChildren-1 do 
		Stones[i]:GetComponent("Script"):SetVector3Variable( "MamaCenter", center)
		Stones[i]:GetComponent("Script"):CallLuaFunction("MoveToBigMama")
	end
end

function BreakUpBigPango()
	Print("SPLIT")
	pangosOnPoint = 0
	PangosOnPoint = 0

	for i = 0, NumChildren-1 do
		pan = Stones[i]
		--pan.gameObject.SetActive(true);
		pan:Enable()
		pan:GetComponent("Transform"):Set_Position(bigPangPos)
		local panScript = pan:GetComponent("Script")
		panScript:SetVector3Variable("toBeLocation",bigPangPos)
		--chase
		pan:GetComponent("Script"):CallLuaFunction("Stunned")
		pan:GetComponent("Script"):SetFloatVariable("NextState", 4.0)
		pan:GetComponent("Script"):CallLuaFunction("UpdateState")
		pan:GetComponent("Script"):CallLuaFunction("EnableComs")
		--seperate
		NextState(2.0)
	end
end

function TellAllTheBoisToJump()
	for i = 0, NumChildren-1 do
		Stones[i]:GetComponent("Script"):SetFloatVariable("NextState", 6.0)
		--Print("4")
		Stones[i]:GetComponent("Script"):CallLuaFunction("UpdateState")
	end
end

function CheckListPos(pango)
	for i = 0, NumChildren-1 do
		
	end
	-- int i = 0;
	-- foreach(PangoWango pan in pangoList)
	-- {
	-- 	if (pango == pan)
	-- 		return i;
	-- 	i++;
	-- }
	-- return i;
end

function MinusHealth()
	if(_damageAmount < 0) then
		
	end
end

function Death()
	dead = true
	for i = 0, NumChildren-1 do 
		Stones[i]:GetComponent("Script"):CallLuaFunction("Death")
	end
end


function ParentAllSubBoss()
	for i = 0, NumChildren-1 do
		this:AddChild(Stones[i]:GetEntity())
	end
end

function UnparentAllSubBoss()
	this:ClearChildren()
end

function IncreasePongosOnPoint()
	PangosOnPoint = PangosOnPoint + 1
	Print(tostring(PangosOnPoint))
end
function IncreasePongosLanded()
	pangosLanded = pangosLanded + 1
end
function IncreasePongosDead()
	NumChildDead = NumChildDead + 1
end