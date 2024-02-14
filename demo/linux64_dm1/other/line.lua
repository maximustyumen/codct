--------------------------------------------------------------------
-- Линейное перемещение v 0.2
--------------------------------------------------------------------
--  tonatos 17.10.2007
--
--------------------------------------------------------------------

c_line = {}
mt_c_line = {}
mt_c_line.__index = c_line

--------------------------------------------------------------------
--
-- Конструктор
--
--    name - имя объекта и класса
--
--    min  - (по умолчанию 0) 		минимальное значение поворота
--    max  - (по умолчанию 10)		максимальное значение поворота
--    sens - (по умолчанию 6)		чувствительность при управлении
--    enabled - (по умолчанию true)	доступен для управления
--    value - значение по умолчанию (для min <0)
--------------------------------------------------------------------

function c_line:new(name, min, max, sens, enabled, value)
	min = min or 0
	max = max or 10
	local result = setmetatable({name=name, min=min, max=max, sens=sens or 6,value=value or min, enabled = enabled or true}, mt_c_line)
	local s = ""
	s = s .. "function " .. name .. "_DRAG(mouseX, mouseY, OldXmouse, OldYmouse)"
	s = s .. name..":drag(mouseX,OldXmouse)"
	s = s .. "end"
	load(s)()
	rti:register(name,min)
	return  result
end


function c_line:drag(x,old_x)
	if self.enabled == true then
		local delta = ((x - old_x) * self.sens)
		self.value = self.value+delta

		if self.value<self.min then delta=delta-self.value+self.min; self.value=self.min end
		
		if self.value>self.max then delta=delta-self.value+self.max; self.value=self.max end

		RSetPosition(self.name,0,0,delta)

		rti:change(self.name, self.value)
		return -delta
	end
	return 0
end

function c_line:get()
	return self.value
end

function c_line:set(new_v)

	if new_v > self.max then new_v = self.max end
	if new_v < self.min then new_v = self.min end

	local delta = self.value - new_v
 	self.value = new_v
	
	rti:change(self.name, self.value)
	
	RSetPosition(self.name, 0, 0, delta)
	return delta
end


-- Линейное перемещение
c_line_move = {}
mt_c_line_move = {}
mt_c_line_move.__index = c_line_move
-----------------------------------------------
--  Конструктор 
--   m_v - скорость перемещения
-----------------------------------------------
function c_line_move:new(name, m_v, m_min, m_max, m_pos, m_sound, axsis)
	m_min = m_min or 0
	rti:register(name, m_min, false, true)
	return setmetatable(
		{old_delta = 0, name = name, v = m_v or 20, position=m_pos or m_min, min=m_min, max=m_max or 100, stop=m_pos or m_min, 
		sound = m_sound or "s2", axsis = axsis or {x=0,y=0,z=1}}, mt_c_line_move)
end

function c_line_move:set_axsis(x,y,z)
	self.axsis = {x=x,y=y,z=z}
end

function c_line_move:move(new_pos)
	if new_pos~=self.position then 	PlaySound(self.sound) end
	self.tstart = os.clock()
	if new_pos>self.max then new_pos = self.max end
	if new_pos<self.min then new_pos = self.min end
	self.stop = new_pos
end

function c_line_move:next(m_index)
	self:move(self.max)
end

function c_line_move:prev(m_index)
	self:move(self.min)
end

function c_line_move:fix_index()
	PauseSound(self.sound)
	self.old_delta = 0
end

function c_line_move:timer()
	
	local delta = self.stop-self.position

	if delta == 0 then 
		if self.old_delta ~= 0 then
			self:fix_index()
		end
		return 0		
	end

	self.old_delta = delta
	
	local dt = os.clock() - self.tstart
	self.tstart = os.clock()
	local s = self.v*dt

	if delta > 0 then
		self.position = self.position + s
		if self.position>self.stop then
			s = s - (self.position-self.stop)
			self.position=self.stop
			self:fix_index()
		end
	elseif delta<0 then
		self.position = self.position - s
		if self.position<self.stop then
			s = s + (self.position-self.stop)
			self.position = self.stop
			self:fix_index()
		end
		s = -s
	end
	
	RSetPosition(self.name,self.axsis.x*s,self.axsis.y*s, self.axsis.z*s)
	
	return s

end

function c_line_move:set(new_pos)

	if new_pos>self.max then new_pos = self.max end
	if new_pos<self.min then new_pos = self.min end

	local s = new_pos-self.position
	
	self.position = new_pos
	self.stop = new_pos
	self.old_delta = 0

	RSetPosition(self.name,self.axsis.x*s,self.axsis.y*s, self.axsis.z*s)
	
	return s
end

function c_line_move:get()
	return self.position
end


function c_line_move:moving()
	return self.stop-self.position
end
-- end Линейное перемещение
