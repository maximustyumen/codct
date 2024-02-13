-- Глобальный объект для работы с пользователем.
CUser = {
	-- Разрешить прыгать на Shift+Q.
	isJumpingEnabled = true,

	Create = function()
		local sender = Eventable.Create()
		sender:Inherit(CUser)
		-- При нажатии Shift+Q пользователь прыгет к тому месту в которое указывает мышка, сохраняя поворот головы.
		Global:ConnectEA('EKeyPush', sender ,'Jump',
			'('..sender.self..'.isJumpingEnabled == true) and (e.key==81) and (e.shift == 1)')
		return sender
	end,
	
	Pos = function(sender, is2D)
		local userPos = {UserData.X, UserData.Y, UserData.Z}
		if is2D then userPos[3] = 0 end
		return userPos
	end,
	
	-- Расстояние до объекта(центра) объект stdlib или имя объекта в сцене.
	DistanceTo = function(sender, target, is2D)
		local x,y,z
		if (type(target) == 'table') then
			x,y,z = AGetPosition(target.mTarget[1])
		else
			x,y,z = AGetPosition(target)			
		end
		return sender:DistanceToPos({x=x, y=y, z=z}, is2D)
	end,
	
	-- Расстояние до точки pos. is2D - считать расстояние на плоскости (не учитывать Z).
	DistanceToPos = function(sender, pos, is2D)
		pos = Algebra.VectorToChrVector(pos) -- Переводим данные к виду {x=, y=, z=}
		local userPos = {x = UserData.X, y = UserData.Y, z = UserData.Z}
		local dx = pos.x - userPos.x
		local dy = pos.y - userPos.y
		local dz = pos.z - userPos.z
		
		if is2D then
			return math.sqrt(dx*dx + dy*dy)
		else
			return math.sqrt(dx*dx + dy*dy + dz*dz)
		end
	end,
	
	-- Поворот пользователя вокург оси Z в радианах всегда от 0 до 2*PI.
	HeadYaw = function(sender)
		local yaw = math.modf(UserData.HeadYaw, 360)
		if yaw < 0 then yaw = yaw + 360 end
		return math.rad(yaw)
	end,
	
	-- Реальное смещение курсора, с учетом положения пользователя.
	RealDPos = function(sender, dx, dy)
		local yaw = sender:HeadYaw()			
		local sz = math.sin(yaw)
		local cz = math.cos(yaw)
		local sx = math.sin(math.rad(360 - UserData.HeadPitch))
		local cx = math.cos(math.rad(360 - UserData.HeadPitch))
		
		-- Матрица поворота
		local M = {
			{cz,	-sz,	0},
			{cx*sz,	cx*cz,	-sx},
			{sx*sz,	sx*cz,	cx}
		}
		
		-- Готовим вектор исходного перемещения.
		local v = {dx, 0, dy}
		-- Поворачиваем вектор.
		v = Algebra.VectorMultMatrix(v, M)
		
		return Algebra.VectorToChrVector(v)
	end,
	
	-- Переместить пользователя в точку.
	MoveToPos = function(sender, x, y, z)
		if type(x) == 'table' then
			x = Algebra.VectorToNumVector(x)
			y = x[2]
			z = x[3]
			x = x[1]
		end
		SetUserPositionAndRotation(x, y, z, UserData.HeadPitch, UserData.HeadYaw, UserData.HeadRoll)
		return sender
	end,
	
	-- Поместить пользователя к объекту.
	MoveTo = function(sender, target)
		if type(target) == 'table' then
			target = target.mTarget[1]
		end
		sender:MoveToPos(AGetPosition(target))
		return sender
	end,
	
	-- Переместить пользователя на указазнное кол-во единиц.
	Move = function(sender, x, y, z)
		if type(x) == 'table' then
			x = Algebra.VectorToNumVector(x)
			y = x[2]
			z = x[3]
			x = x[1]
		end
		sender:MoveToPos(UserData.X + x, UserData.Y + y, UserData.Z + z)
		return sender
	end,
	
	
	Jump = function()
		
		local x = UserData.CurrentObjectIntersectsCoordWORLD_X
		local y = UserData.CurrentObjectIntersectsCoordWORLD_Y
		local z = UserData.CurrentObjectIntersectsCoordWORLD_Z + 37
		
		SetUserPositionAndRotation(x, y, z, UserData.HeadPitch, UserData.HeadYaw, UserData.HeadRoll)
		return sender
	end	
}