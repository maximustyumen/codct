--------------------------------------------------------------------
-- Стрелочный прибор v 0.2
--------------------------------------------------------------------
--  tonatos 15.10.2007
--    имитация стрелочного прибора
--
--------------------------------------------------------------------

c_strelka = {}
mt_c_strelka = {}
mt_c_strelka.__index = c_strelka

--------------------------------------------------------------------
--
-- Конструктор
--
--    name - имя объекта и класса
--
--- физические параметры (3D)
--    min  - (по умолчанию 0, радианы) 		минимальное значение поворота
--    max  - (по умолчанию 2\3 Pi, радианы)	максимальное значение поворота
--
--- математические параметры
--    min_value   - минимальное значение шкалы
--    max_value   - максимальное  значение шкалы
--------------------------------------------------------------------

function c_strelka:new(name, min, max, min_value, max_value)

	min = min or 0
	max = max or math.rad(270)

	min_value = min_value or min
	max_value = max_value or max

	local zveno = Zveno_a1_sum:new(min, 0.2)
	zveno:start()
	
	rti:register(name, min_value, false, true)
	
	return setmetatable({name=name, min=min, max=max, min_value=min_value, max_value=max_value, value=min_value, value_3d=min, zveno=zveno}, mt_c_strelka)
end

function c_strelka:get()
	return self.zveno.output
end

function c_strelka:set(new_v)
	self.zveno:set_y(new_v)
end

function c_strelka:timer()
	self.value  = self.zveno:execute()

	if self.value > self.max_value then self.value = self.max_value end
	if self.value < self.min_value then self.value = self.min_value end
	
	local k = 1
	
	if self.max - self.min ~= 0 then
		k = (self.max_value - self.min_value)/(self.max - self.min)
	end
	
	local delta = self.value/k-self.value_3d
	
	if delta == 0 then return 0  end
	
	self.value_3d = self.value_3d+delta

	RSetRotate2(self.name,-delta,0,0,1)

	return -delta
end