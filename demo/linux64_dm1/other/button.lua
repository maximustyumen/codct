--------------------------------------------------------------------
-- Кнопки v 0.2
--------------------------------------------------------------------
--  tonatos 16.10.2007
--
--------------------------------------------------------------------

c_button = {}
mt_c_button = {}
mt_c_button.__index = c_button

--------------------------------------------------------------------
--
-- Конструктор
--
--    name - имя объекта и класса
--
--    value - (по умолчанию false)		соcтояние кнопки (включено/выключено)
--    flip - (по умолчанию false) 		залипание, тоесть остается кнопка нажатой или возвращается в исходное состояние
--
--------------------------------------------------------------------

function c_button:new(name, value, flip)

	local result = setmetatable({name=name, flip=flip or false, value = value or false, disabled = false}, mt_c_button)

	local s = "function " .. name .. "_PUSH()"
	s = s .. name..":push()"
	s = s .. "end"
	load(s)()

	s = "function " .. name .. "_RELEASE()"
	s = s .. name..":release()"
	s = s .. "end"
	load(s)()
	
	rti:register(name, value)
	
	return  result
end

function c_button:push()
	if self.disabled == true then return end

	-- если кнопка не залипающая то
	if self.flip == false then
		-- если кнопка не нажата
		if self.value == false then
			self.value = true
			
			rti:change(self.name, self.value)
			
			RSetRotate(self.name, 0, 0, math.pi)
		end		
	else

		self.value = not self.value

		rti:change(self.name, self.value)

		RSetRotate(self.name, 0, 0, math.pi)
			
	end
	
	if self.onClick then
		self.onClick();
	end
	
end

function c_button:release()
	if self.disabled == true then return end
	
	-- если кнопка не залипающая то
	if self.flip == false then
	
		-- если кнопка нажата то отпустить ее
		if self.value == true then
			self.value = false
			
			rti:change(self.name, self.value)
			
			RSetRotate(self.name, 0, 0, math.pi)
		end		
	end
end


function c_button:get()
	return self.value
end

function c_button:set(new_v)

	if self.disabled == true then return end
	
	if self.flip == true then
		if self.value ~=new_v then
			self:push()
		end
	end
	
end
