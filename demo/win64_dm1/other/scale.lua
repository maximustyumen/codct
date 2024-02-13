--------------------------------------------------------------------
-- Масштабировнание объета v 0.2
--------------------------------------------------------------------
--  tonatos 15.10.2007
--
--------------------------------------------------------------------

c_scale = {}
mt_c_scale = {}
mt_c_scale.__index = c_scale

--------------------------------------------------------------------
--
-- Конструктор
--
--    name - имя объекта и класса
--
--    min  - (по умолчанию 1) 			минимальное значение масштаба
--    max  - (по умолчанию 2)			максимальное значение масштаба
--    value - текущее значение (по умолчанию min)
--------------------------------------------------------------------

function c_scale:new(name, min, max, value)
	
	-- определяем умолчания
	min = min or 0.1
	max = max or 2
	value = value or min
		
	return setmetatable({name=name, min=min,max=max,value=value, os={x=0,y=0,z=1}}, mt_c_scale)
end

--------------------------------------------------------------------
-- Установка осей
--------------------------------------------------------------------
function c_scale:set_os(x,y,z)
	self.os.x = x
	self.os.y = y
	self.os.z = z
end

function c_scale:get()
	return self.value
end

function c_scale:set(new_v)

	if new_v > self.max then new_v = self.max end
	if new_v < self.min then new_v = self.min end

	local k = self.value/new_v
 	self.value = new_v
	
	local x,y,z = 1,1,1
	if self.os.x ~= 0 then x = self.os.x/k end
	if self.os.y ~= 0 then y = self.os.y/k end
	if self.os.z ~= 0 then z = self.os.z/k end
	
	RSetScale(self.name, x, y, z)
	return k
end
