--------------------------------------------------------------------
-- Крутилка v 0.2
--------------------------------------------------------------------
--  tonatos 13.10.2007
--    в отличие от предыдущего класса работает только в радианах
--
--------------------------------------------------------------------

c_volume = {}
mt_c_volume = {}
mt_c_volume.__index = c_volume

--------------------------------------------------------------------
--
-- Конструктор
--
--    name - имя объекта и класса
--
--    min  - (по умолчанию 0, радианы) 		минимальное значение поворота
--    max  - (по умолчанию 2\3 Pi, радианы)	максимальное значение поворота
--    sens - (по умолчанию 6)				чувствительность при управлении
--    enabled - (по умолчанию true)	доступен для управления
--    value - значение по умолчанию (для min <0)
--------------------------------------------------------------------

function c_volume:new(name, min, max, sens, enabled, value)
	local result = setmetatable({name=name,min=min or 0,max=max or math.rad(270),sens=sens or 6,value=value or 0, enabled = enabled or true, os={x=0,y=0,z=1}}, mt_c_volume)
	local s = ""
	s = s .. "function " .. name .. "_DRAG(mouseX, mouseY, OldXmouse, OldYmouse)"
	s = s .. name..":drag(mouseX,OldXmouse)"
	s = s .. "end"
	load(s)()
	rti:register(name,0)
	return  result
end

function c_volume:set_os(x,y,z)
	self.os.x = x
	self.os.y = y
	self.os.z = z
end


function c_volume:drag(x,old_x)
	if self.enabled == true then
		local delta = ((x - old_x) * self.sens)
		self.value = self.value+delta
	
		if self.value<self.min then delta=delta-self.value+self.min; self.value=self.min end
		
		if self.value>self.max then delta=delta-self.value+self.max; self.value=self.max end
	
		RSetRotate2(self.name,-delta,self.os.x, self.os.y, self.os.z)
	
		rti:change(self.name, self.value)

		return -delta
	end
	return 0
end

function c_volume:get()
	return self.value
end

function c_volume:set(new_v)

	if new_v > self.max then new_v = self.max end
	if new_v < self.min then new_v = self.min end

	local delta = self.value - new_v
 	self.value = new_v
	
	rti:change(self.name, self.value)
	
	RSetRotate2(self.name, delta, self.os.x, self.os.y, self.os.z)
	return delta
end
