-- Данный файл инициализиует все глобальные объекты настройки и выполняет другие действия, 
-- которые нужно выполнить в конце загрузки STDLib, но перед запуском лабы.

-- Данный блок позволяет обращаться к объектам сцены как к STDLib-объектам (STD_Box310:Hide())
_unknowns = {}
_Gmt = {
	__index = function(k, v)
		-- Обрабатываем объекты начинающиеся с STD_
		if (_unknowns[v] == nil) and (string.sub(v, 1, 4) == 'STD_') then
			local v2 = string.sub(v, 5)
			if (GetParent(v2) ~= '') then					
				_G[v] = Global.CreateObject(v2)
				return _G[v]
			else
				_unknowns[v] = true -- Не смогли обернуть объект - забудем про него.
				return nil
			end
		end
	end		
}	
setmetatable(_G, _Gmt)


OLD_PUSH = _PUSH
OLD_RELEASE = _RELEASE
OLD_USE = _USE
OLD_DRAG = _DRAG

-- Функции для проброса _PUSH по родителям.
_PUSH = function()
	if (OLD_PUSH) then OLD_PUSH() end
	
	local counter = 50
	-- Ищем объект для которого определен метод _DRAG, он и будет его обрабатывать в случае чего.
	Global.lastPushedObject = nil
	if (_G[UserData.CurrentObjectName..'_DRAG'] == nil) then
		local obj = GetParent(UserData.CurrentObjectName)
		while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
			counter = counter - 1
			if _G[obj..'_DRAG'] ~= nil then
				Global.lastPushedObject = obj
				break;
			else
				obj = GetParent(obj)
			end
		end	
	end
	Global.lastPushedObject = UserData.CurrentObjectName
	
	counter = 50
	-- Пробрасываем событие. Если событие обработанов каком-то объекте, то к родителю оно не пойдет.
	if Global.isEventHandled ~= true then
		local obj = GetParent(UserData.CurrentObjectName)
		while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do			
			counter = counter - 1
			local push = _G[obj..'_PUSH']
			if push then
				--Global.lastPushedObject = obj
				push()
				-- Если событие кем-то обработано, то не генерируем его выше.
				if Global.isEventHandled == true then break end
				
			end		
			obj = GetParent(obj)
		end
	end

	Global.isEventHandled = false
end

-- Функции для проброса _RELEASE по родителям.
function _RELEASE()
	if (OLD_RELEASE) then OLD_RELEASE() end
	
	Global.lastPushedObject = nil
	local counter = 50
	-- Пробрасываем событие. Если событие обработанов каком-то объекте, то к родителю оно не пойдет.
	if Global.isEventHandled ~= true then
		local obj = GetParent(UserData.CurrentObjectName)
		while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
			counter = counter - 1
			local push = _G[obj..'_RELEASE']
			if push then
				push()
				-- Если событие кем-то обработано, то не генерируем его выше.
				if Global.isEventHandled == true then break end
			end
			obj = GetParent(obj)
		end
	end
	Global.isEventHandled = false
end

-- Функции для проброса _USE по родителям.
function _USE()
	if (OLD_USE) then OLD_USE() end
	local counter = 50
	-- Пробрасываем событие. Если событие обработанов каком-то объекте, то к родителю оно не пойдет.
	if Global.isEventHandled ~= true then
		local obj = GetParent(UserData.CurrentObjectName)
		while obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
			counter = counter - 1
			local push = _G[obj..'_USE']
			if push then
				push()
				-- Если событие кем-то обработано, то не генерируем его выше.
				if Global.isEventHandled == true then break end
			end
			obj = GetParent(obj)
		end
	end
end

-- Функции для проброса _DRAG по родителям.
function _DRAG(x1, y1, x2, y2)
	if (OLD_DRAG) then OLD_DRAG(x1, y1, x2, y2) end
	local counter = 50
	-- Событие генерирует тот объект на котором зажали кнопку.
	if Global.lastPushedObject then
		local func = _G[Global.lastPushedObject..'_DRAG']		
		if func then func(x1, y1, x2, y2) end
		
		-- Пробрасываем событие. Если событие обработанов каком-то объекте, то к родителю оно не пойдет.
		if Global.isEventHandled ~= true then
			local obj = GetParent(Global.lastPushedObject)
			while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
				counter = counter - 1
				local func = _G[obj..'_DRAG']
				if func then
					if func then func(x1, y1, x2, y2) end
					-- Если событие кем-то обработано, то не генерируем его выше.
					if Global.isEventHandled == true then break end
				end
				obj = GetParent(obj)
			end
		end	
	
		--local func = _G[Global.lastPushedObject..'_DRAG']		
	end
end

function STDLIB_INIT()
	Global.Initialize() -- Иницалищируем статический класс Global.
	Hand = CHand.Create() -- Создаем глобальную руку.
	User = CUser.Create()
end
