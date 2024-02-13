-- # func: Копировать объект. Копируется таблица со всеми полями. Для каждого поля создается копия.
function CopyObject(object)
	if (type(object) == "table") then 
		local copy = {}
		-- Пробегаем по всем полям таблицы и копируем их.
		for i,j in pairs(object) do
			copy[i] = CopyObject(j)
		end
		
		return copy
	else
		return object
	end
end

-- # func: Копировать объект. Копируется таблица со всеми полями. Для каждого поля создается копия.
function CopyObject2(object, inClass)
	-- Если таблица, то копируем ее поля.
	if (type(object) == "table") then
		-- мы внутри класса и Это какой-то класс, просто поставим на него ссылку. По правилам, нужно его создавать отдельно (в конструкторе).
		if inClass and object.Inherit then
			return object
			
		else -- Мы снаружи класса или внутри класса, но это просто таблица, поэтому ее надо копировать.
			local copy = {}
			-- Пробегаем по всем полям таблицы и копируем их.
			for i, j in pairs(object) do
				if i ~= '_base' then -- Не копировать весь базовый класс.
					if (type(object) == 'table') then
						copy[i] = CopyObject2(j, true)
					else
						copy[i] = j
					end
				end
			end
			
			return copy
		end
	else
		return object
	end
end

-- #func: Добавляет поля объекта source к полям sender.
-- #sender: Объект, к которому будут добавлены поля.
-- #source: Объект, поля которого будут использоваться для добавления.
function AppendObjectFields(sender, source)
	if (type(sender) == "table") then
		local copy = CopyObject2(source) -- Делаем копию, и копируем из нее данные в sender.
		for i,j in pairs(copy) do
			sender[i] = j
		end
	end
end


-- Переопределение команды проигрывания звука так, чтобы она с ошибкой не выпадала.
if RealPlaySound == nil then
	RealPlaySound = PlaySound
	PlaySound = function(name)
		if (string.sub(name, 1, 1) == '#') or (Global.emulateSound) then
			print('PlaySound: '..name)
		elseif GetParent(name) == nil then
			print('Sound not foud: '..name)
		elseif GetSoundState(name) ~= 'PLAYING' then
			RealPlaySound(name)
		end
	end
end

-- Остановить звук.
StopSound = function(soundName)
	PauseSound(soundName)
	SetSoundOFFSET(soundName, 0)
end


-- #func: Преобразует таблицу в массив строк.
-- #packingTable: Таблица, которая будет обрабатываться.
-- #result: Строка, в которую будет помещен результат.
-- #prefix: Приставка, которая будет добавляться к каждой строке выходного масива.
function TableToStringArray(packingTable, result, prefix)
	for i,j in pairs(packingTable) do
		if (type(j) == 'table') then
			
			local newPrefix = ""
			
			if (type(i) == 'number') then newPrefix = prefix .. "[" .. i .. "]"
			else newPrefix = prefix .. "['" .. i .. "']" end
			
			table.insert(result, newPrefix .. " = {}")

			TableToStringArray(j, result, newPrefix)			
		else
			cmd = prefix

			if (type(i) == 'number') then cmd = cmd .. "[" .. i .. "]="
			else cmd = cmd .. "['" .. i .. "']=" end

			if ((type(j) == 'number') or (type(j) == 'boolean')) then cmd = cmd .. tostring(j)
			else cmd = cmd .. "'" .. tostring(j) .. "'" end

			table.insert(result, cmd)
		end
	end
end

-- # func: Сохранить таблицу в файл. Сохраняются только числовые, строковые значения и вложенные таблицы.
-- #pakingTable: пакуемая таблица.
-- #filename: имя файла для сохранения.
function SaveTableToFile(pakingTable, file, prefix)
	if (file == nil) then
		print("File is nil in SaveTableToFile")
		return false
	end
	
	local pocket = {}
	TableToStringArray(pakingTable, pocket, prefix)

	for i,j in pairs(pocket) do
		file:write(j .. "\n")
	end
end

-- #func: Загрузка таблицы из файла. Загружаются только числовые, строковые значения и вложенные таблицы.
-- #unpackingTableName: имя таблицы в которую будет развернут файл.
-- #filename: имя файла для загрузки.
function LoadTableFromFile(unpackingTableName, file)
	if (file == nil) then return false end
	
	-- Создаем таблицу дял загрузки.
	if (unpackingTableName ~= nil) then
		load(unpackingTableName .. "= {}")()
	else
		unpackingTableName = ""
	end
	
	for line in file.lines(file) do
		load(unpackingTableName .. line)()
	end
	
	return true
end

-- #func: Проверить существует ли объект с заданным именем.
-- #objectName: имя проверяемого объекта.
function ObjectExists(objectName)
	return _G[objectName] ~= nil
end

-- #func: Возвращает экемпляр объекта по имени.
-- #objectName: имя искомого объекта.
function GetObjectByName(objectName)
	return _G[objectName]
end

-- #func: Получить следующее имя экземпляра данного класса. Иям имеет вид <Имя класса><Номер экземпляра>. Нумерация ведется с едиицы.
-- #className: имя класса, для которого подбирается имя.
function GetNextName(className)
	i = 1
	
	while ObjectExists(className .. i) do
		i = i + 1
	end

	return className .. i
end


-- #func: Добавить функцию в глобальный таймер. Функия будет вызываться в таймере.
-- #functionName: имя функции.
-- #params: Параметры, которые будут переданы в функцию.
function AddFunctionToMainTimer(functionName, params)
	-- Если нет параметров, то помещаем в переменную пустую строку. 
	if params == nil then params = '' end

	-- Обработчик таймера.
	local newTimerFunc = nil
	
	-- Если функции таймера не было, то не подкрепляемся к ней.
	if (STDLib_Timer ~= nil) then
		newTimerFunc = GetNextName("STDLib_Timer")
		load(newTimerFunc .. " = STDLib_Timer")()
	end
	
	local s = "STDLib_Timer = function()\n"
	-- Если функции таймера не было, то не вызываем ее.
	if (STDLib_Timer ~= nil) then s = s .. newTimerFunc .. "()\n" end
	s = s .. functionName .. "(" .. params .. ")\n"
	s = s .. "end\n"
	load(s)()
end

-- #func: Найти элемент таблицы по некоторому полю.
-- #searchTable: таблица для поиска.
-- #fieldName: имя поля по которому будет производиться поиск.
-- #fieldValue: значение поля искомого элемента.
function table.GetByField(searchTable, fieldName, fieldValue)
	for key,value in pairs(searchTable) do
		if (value[fieldName] == fieldValue) then
			return value
		end
	end
	return nil
end

-- #func: Найти элемент таблицы по некоторому полю.
-- #searchTable: таблица для поиска.
-- #fieldName: имя поля по которому будет производиться поиск.
-- #fieldValue: значение поля искомого элемента.
function table.FindKeyByItem(searchTable, item)
	for key,value in pairs(searchTable) do
		if (value == item) then	return key end
	end
	return nil
end

-- #func: Проверяет, содержит ли таблица заданный ключ.
-- #searchTable: таблица для поиска.
-- #keyName: имя поля по которому будет производиться поиск.
-- #returns: true, если содержит, false, если нет.
function table.ContainsKey(searchTable, keyName)
	for key,value in pairs(searchTable) do
		if (key == keyName) then
			return true
		end
	end
	return false
end

-- #func: Проверяет, содержит ли таблица заданное значение.
-- #searchTable: таблица для поиска.
-- #valueName: имя поля по которому будет производиться поиск.
-- #returns: true, если содержит, false, если нет.
function table.ContainsValue(searchTable, valueName)
	for key,value in pairs(searchTable) do
		if (value == valueName) then
			return true
		end
	end
	return false
end

-- #func: Выполнить команду переданную в строке.
-- #cmd: строка с командой или массив строк с командами.
function ExecuteCmd(cmd)
	if cmd ~= nil then
		if type(cmd) == 'table' then
			for i,j in pairs(cmd) do
				if j ~= nil then load(j)() end
			end
		else
			load(cmd)()
		end
	end
end

-- # func: Выятгивает строковое представление числа до указанной длинны. Если число длиннее len, то ничего не происходит.
-- #num: число.
-- #len: требуемая длина строки.
function StretchNum(num, len)
	if (num == math.floor(num)) then
		num = string.rep("0", len - string.len(num)) .. num
	else
		num = num .. string.rep("0", len - string.len(num))
	end
	
	return num
end

-- #func: Разбивает строку на массив подстрок, используя в качестве разделителя separator.
-- #str: Строка для разбивания.
-- #separator: Строка, используемая в качестве разделителя.
-- #returns: Полученный в результате разбивания строки массив подстрок.
function string.Split(str, separator)
	local len = string.len(separator)
	local result = {}
	
	while(string.find(str, separator) ~= nil) do
		local i = string.find(str, separator)
		table.insert(result, string.sub(str, 1, i - 1))
		str = string.sub(str, i + len)
	end
	
	table.insert(result, str)
	
	return result
end

-- #func: Соединяет массив строк в одну, используя в качестве разделителя separator.
-- #mas: Массив строк для слияния.
-- #separator: Строка, используемая в качестве разделителя.
-- #returns: Полученную в результате слияния строка.
function string.Join(mas, separator)
	return table.concat(mas, separator)
end
