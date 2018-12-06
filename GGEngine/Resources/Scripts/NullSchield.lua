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

function Awake()

end

function Initialize()
    smr = this:GetComponent("StaticMeshRenderer")
    smr.Transparency = 0.0
end

function Update(dt)
    if(smr.Transparency > 0)then
        smr.Transparency = smr.Transparency - 0.5 * dt
    end
end

function Shield()
    smr.Transparency = 0.5
end

function CollisionEnter (ent)

end

function CollisionExit (ent)

end

function CollisionStay (ent)

end
