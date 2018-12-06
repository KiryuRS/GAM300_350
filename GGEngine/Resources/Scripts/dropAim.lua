--[[
Basic LUA Programming:
- All variables defined or declared are by default "script-global" variables
  . To use only local variables, append a "local" to the variable name. i.e.
    local foo = 5;
	
- LUA arrays start from 1 instead of 0. To get the size of the array, use #<arrayName>
  i.e.
  myArray = {10,11,12,13,14,15}
  local size = #myArray
  
- To allow your variables to not reflect on the engine, append an underscore to your variables
  i.e.
  editorReflect = "Hello World"			-- This will be reflected on the editor
  _noEditorReflect = "Goodbye World"	-- This will not be reflected on the editor
  
- All function parameters do NOT require the type! Only pass in your variables

  
Changelog: 25/11/2018:
1) For available function calls, please refer to engine's component for the list
   of functions available for calling
   - You may do so by hovering on each of the components for that particular entity
 
2) It is possible to grab a variable from other script. A sample code is given below
   someManager = this:FindExternalEntity("EntityThatHoldsScript")
   someScript = someManager:GetComponent("Script")
   intVariable = someScript:GetVariable("intVariableFromOtherScript")
   
3) It is also possible to call for functions with multiple parameters from other script.
   A sample code is given below.
   -- Using the above example
   funcPtr = someScript:GetVariable("FunctionWithTwoParams")
   funcPtr(arg0, arg1)
--]]

minSize = 100.0
maxSize = 500.0
decreaseSpeed = 400.0
active = false

function Awake()

end

function Initialize()
    transform = this:GetComponent("Transform")
end

function Update(dt)
    if(active == true)then
        if(transform.Scale.x > minSize)then
            active = true
            newVec = Vector3:new(transform.Scale.x - decreaseSpeed * dt
            , transform.Scale.y - decreaseSpeed * dt, 1.0)
            transform:Set_Scale(newVec)
        else
            active = false
            transform.Scale.x = 0
            transform.Scale.y = 0
            transform:Set_Scale(Vector3:new(0,0,1))
            if(this:GetComponent("ParticleEmitter"))then
                this:GetComponent("ParticleEmitter").emiting = false
            end
        end
    end
end

function Enlarge()
    transform:Set_Scale(Vector3:new(maxSize,maxSize,1))
    --Print("test")
    if(this:GetComponent("ParticleEmitter"))then
            this:GetComponent("ParticleEmitter").emiting = true
    end
    --Print("test2")
end

function Activate()
    --Print("activate")
    transform:Set_Scale(Vector3:new(maxSize,maxSize,1))
    active = true
    if(this:GetComponent("ParticleEmitter"))then
            this:GetComponent("ParticleEmitter").emiting = true
    end
end

function CollisionEnter (ent)

end

function CollisionExit (ent)

end

function CollisionStay (ent)

end
