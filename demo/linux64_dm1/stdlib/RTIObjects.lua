-- Статический класс. Хранит данные для работы объектов RTI_Variable. Обеспечивает взаимодействие с RTI.
RTI_Variables = {
	isDebug = false,
	items = {},		

	Add = function(item)
		RTI_Variables.items[item.RTI_Name] = item
	end,

	Delete = function(item)
		table.remove(RTI_Variables.items, item.RTI_Name)
	end,

	UpdateValue = function(objectName, attrName, value)
		local RTI_Name = objectName .. "." .. attrName
		
		if (table.ContainsKey(RTI_Variables.items, RTI_Name)) then
			RTI_Variables.items[RTI_Name]:RTI_Set(value)
		end
	end
}

-- Переменная RTI (способная взаимодействовать по сети). Класс позволяет создавать переменную, которая будет использоваться несколькими
-- пользователями в сети. Для работы используются методы Set, Get и событие EChange(value).
RTI_Variable = {
	objectName = nil,

	attrName = nil,

	value = nil,

	Create = function(objectName, attrName, initValue)
		self = EventAttributable.Create()
		self:InheritClass(RTI_Variable)
		self.objectName = objectName
		self.attrName = attrName
		self.value = initValue
		self.RTI_Name = "ObjectRoot." .. objectName .. "." .. attrName

		RTI_Variables.Add(self)

		return self
	end,

	Get  = function(sender, value)
		return sender.value
	end,


	RTI_Set = function(sender, value)
		sender:RaiseEvent("EChange", {value = value})
		sender.value = value

	end,

	Set = function(sender, value)
		sender:RTI_Set(value)
		RTI_UpdateAttributeValues("ObjectRoot." .. sender.objectName, sender.attrName, tostring(value))
	end	
}

function RTI_CHANGE (objectname, attributname, newvalue)
	RTI_Variables.UpdateValue(objectname, attributname, newvalue)
	if (RTI_Variables.isDebug) then
		print(tostring(objectname) .. '.' .. tostring(attributname) .. '=' .. tostring(newvalue))
	end
end
