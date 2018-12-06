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

_btnIndex = 0
_upKeys = {}
_downKeys = {}
_enterKey = "ENTER"


function Awake()
    btnManager = this:GetComponent("ButtonManagerC")
    controller = this:GetComponent("Controller")
    _btnIndex = btnManager:GetIndex()
end

function Initialize()
    -- Capture the number of keys available
    -- Moving up
    _upKeys[1] = "W"
    _upKeys[2] = "UP"
    -- Moving down
    _downKeys[1] = "S"
    _downKeys[2] = "DOWN"
    size = #_upKeys
end

function Update(dt)
    for i=1,size do
        if controller:GetKeyDown(_upKeys[i]) then
            btnManager:MoveUp()
        elseif controller:GetKeyDown(_downKeys[i]) then
            btnManager:MoveDown()
        end 
    end

    if (controller:GetKeyDown(_enterKey)) then
        btnManager:ExecuteButton()
    end
end

function CollisionEnter (ent)

end

function CollisionExit (ent)

end

function CollisionStay (ent)

end
