timeBeforeDeath = 1.0

function Awake()

end

function Initialize()

end

function Update(dt)
    timeBeforeDeath = timeBeforeDeath - dt
    if(timeBeforeDeath < 0) then
        this:Destroy()
    end
end

function CollisionEnter (ent)

end

function CollisionExit (ent)

end

function CollisionStay (ent)

end
