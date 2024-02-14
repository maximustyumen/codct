------------------------------------------------------------------------
--  RTI
------------------------------------------------------------------------

c_rti = {}
mt_c_rti = {}
mt_c_rti.__index = c_rti

function c_rti:new()
	return setmetatable({params={}, timers={}}, mt_c_rti)
end

function c_rti:register(name, value, changed, timered)
	local param = {name=name, value = value or 0, changed = changed or false, timered = timered or false}
	
	if timered == true then		
		local f = load(name..":timer()")
		table.insert(self.timers, f)
	end
	
	self.params[name] = param
end

function c_rti:change(name, value)
	if self.params[name] then
		if self.params[name].value ~= value then
			if self.params[name].changed == false then
				self.params[name].old_value = self.params[name].value
			end
			self.params[name].value = value
			self.params[name].changed = true
		end
	end
end

function c_rti:is_change(name)
	if self.params[name] then
		return self.params[name].changed
	end
	return false
end

function c_rti:commit(name)
	if self.params[name] then
		self.params[name].changed = false
	end
end

function c_rti:get(name)
	if self.params[name] then
		return self.params[name]
	end
	return null
end

function c_rti:timer()
	for key,value in pairs(self.timers) do
		value()
	end
end