SetUserPositionAndRotation(0, 0, 0, 0, 180, 0)
UserData = {
langindex = 0,
CurrentObjectName = "",
CurrentObjectIntersectsCoordWORLD_X = 0,
CurrentObjectIntersectsCoordWORLD_Y = 0,
CurrentObjectIntersectsCoordWORLD_Z = 0,
CurrentObjectIntersectsCoordLOCAL_X = 0,
CurrentObjectIntersectsCoordLOCAL_Y = 0,
CurrentObjectIntersectsCoordLOCAL_Z = 0,
HeadYaw=0,
HeadPitch=0,
HeadRoll=0,
X=0,
Y=0,
Z=0
}

dofile("./stdlib/stdlib.lua")
local files = {}
function getFileNameFromOther(obj)
return files[obj]
end

translateTB = {}
function tr(word)
if UserData.langindex==0 then return word end
return translateTB[word][UserData.langindex]
end

-- Local timers
function LocalTimers()
end
if Error==false then print (Message) end

-- Local initializations
function LocalInits()
end
if Error==false then print (Message) end

-- Section: Base3DObject
Add3DObject('./osg/dm1.osg')
Add3DObject('./osg/komnata_light.osg')

-- Section: BaseFile
files["lib.lua"] = "./other/lib.lua"
files["volume.lua"] = "./other/volume.lua"
files["tau.lua"] = "./other/tau.lua"
files["strelka.lua"] = "./other/strelka.lua"
files["button.lua"] = "./other/button.lua"
files["rti.lua"] = "./other/rti.lua"
files["comp.lua"] = "./other/comp.lua"
files["tormoz.lua"] = "./other/tormoz.lua"
files["line.lua"] = "./other/line.lua"
files["stanok.lua"] = "./other/stanok.lua"
files["scale.lua"] = "./other/scale.lua"
files["monik"] = "./other/monik_black.bmp"
files["gost_a_.ttf"] = "./other/gost_a_.ttf"

-- Section: BaseSoundObject
AddSoundObject('sreda', false)
LoadSound('sreda', './wav/sreda.wav')
LoopSound('sreda', true)
PositionSound('sreda', 0, 0, 0)

AddSoundObject('s1', false)
LoadSound('s1', './wav/s1.wav')
LoopSound('s1', false)
PositionSound('s1', 0, 0, 0)

AddSoundObject('s5', false)
LoadSound('s5', './wav/s5.wav')
LoopSound('s5', true)
PositionSound('s5', 0, 0, 0)

AddSoundObject('s2', false)
LoadSound('s2', './wav/s2.wav')
LoopSound('s2', true)
PositionSound('s2', 0, 0, 0)

AddSoundObject('s6', false)
LoadSound('s6', './wav/s6.wav')
LoopSound('s6', false)
PositionSound('s6', 0, 0, 0)


-- Section: inits

-- Section: timers

--обработчик события ГЛОБАЛЬНОГО таймера. Нет агрументов.
function MainTimer ()
	rti:timer()
	
	--таймер тормоза
	tormoz.timer()

	--таймер компьютера
	comp.timer()

	--таймер процесса
	stanok.timer()

	error_timer()

end
-- Стандартные библиотеки
dofile(getFileNameFromOther("lib.lua"))
dofile(getFileNameFromOther("tau.lua"))
dofile(getFileNameFromOther("rti.lua"))

-- Стандартные библиотеки (std_lib)
dofile(getFileNameFromOther("volume.lua"))
dofile(getFileNameFromOther("strelka.lua"))
dofile(getFileNameFromOther("button.lua"))
dofile(getFileNameFromOther("line.lua"))
dofile(getFileNameFromOther("scale.lua"))

-- Библиотеки самого процесса
dofile(getFileNameFromOther("comp.lua"))
dofile(getFileNameFromOther("tormoz.lua"))
dofile(getFileNameFromOther("stanok.lua"))

function INIT ()
	rti = c_rti:new()
	comp.init()
	tormoz.init()
	stanok.init()

	--LoopSound("sreda",true)
	PauseSound("s2")
end





















