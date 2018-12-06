timeElapsed = 0.0
fadingOut = false

function Awake()
end

function Initialize()
	print("Sound Initialize")
	sound = this:GetComponent("Sound")
	sound:FadeIn_BGM(sound.Start_BGM_ID, sound.Fade_Duration)
end

function Update(dt)
	timeElapsed = dt + timeElapsed
	if(timeElapsed > sound.Fade_Duration and not fadingOut) then 
		sound:FadeOut_BGM(sound.Start_BGM_ID, sound.Fade_Duration)
		fadingOut = true
	end
end