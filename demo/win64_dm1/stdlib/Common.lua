-- # func: ���������� ������. ���������� ������� �� ����� ������. ��� ������� ���� ��������� �����.
function CopyObject(object)
	if (type(object) == "table") then 
		local copy = {}
		-- ��������� �� ���� ����� ������� � �������� ��.
		for i,j in pairs(object) do
			copy[i] = CopyObject(j)
		end
		
		return copy
	else
		return object
	end
end

-- # func: ���������� ������. ���������� ������� �� ����� ������. ��� ������� ���� ��������� �����.
function CopyObject2(object, inClass)
	-- ���� �������, �� �������� �� ����.
	if (type(object) == "table") then
		-- �� ������ ������ � ��� �����-�� �����, ������ �������� �� ���� ������. �� ��������, ����� ��� ��������� �������� (� ������������).
		if inClass and object.Inherit then
			return object
			
		else -- �� ������� ������ ��� ������ ������, �� ��� ������ �������, ������� �� ���� ����������.
			local copy = {}
			-- ��������� �� ���� ����� ������� � �������� ��.
			for i, j in pairs(object) do
				if i ~= '_base' then -- �� ���������� ���� ������� �����.
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

-- #func: ��������� ���� ������� source � ����� sender.
-- #sender: ������, � �������� ����� ��������� ����.
-- #source: ������, ���� �������� ����� �������������� ��� ����������.
function AppendObjectFields(sender, source)
	if (type(sender) == "table") then
		local copy = CopyObject2(source) -- ������ �����, � �������� �� ��� ������ � sender.
		for i,j in pairs(copy) do
			sender[i] = j
		end
	end
end


-- ��������������� ������� ������������ ����� ���, ����� ��� � ������� �� ��������.
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

-- ���������� ����.
StopSound = function(soundName)
	PauseSound(soundName)
	SetSoundOFFSET(soundName, 0)
end


-- #func: ����������� ������� � ������ �����.
-- #packingTable: �������, ������� ����� ��������������.
-- #result: ������, � ������� ����� ������� ���������.
-- #prefix: ���������, ������� ����� ����������� � ������ ������ ��������� ������.
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

-- # func: ��������� ������� � ����. ����������� ������ ��������, ��������� �������� � ��������� �������.
-- #pakingTable: �������� �������.
-- #filename: ��� ����� ��� ����������.
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

-- #func: �������� ������� �� �����. ����������� ������ ��������, ��������� �������� � ��������� �������.
-- #unpackingTableName: ��� ������� � ������� ����� ��������� ����.
-- #filename: ��� ����� ��� ��������.
function LoadTableFromFile(unpackingTableName, file)
	if (file == nil) then return false end
	
	-- ������� ������� ��� ��������.
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

-- #func: ��������� ���������� �� ������ � �������� ������.
-- #objectName: ��� ������������ �������.
function ObjectExists(objectName)
	return _G[objectName] ~= nil
end

-- #func: ���������� �������� ������� �� �����.
-- #objectName: ��� �������� �������.
function GetObjectByName(objectName)
	return _G[objectName]
end

-- #func: �������� ��������� ��� ���������� ������� ������. ��� ����� ��� <��� ������><����� ����������>. ��������� ������� � ������.
-- #className: ��� ������, ��� �������� ����������� ���.
function GetNextName(className)
	i = 1
	
	while ObjectExists(className .. i) do
		i = i + 1
	end

	return className .. i
end


-- #func: �������� ������� � ���������� ������. ������ ����� ���������� � �������.
-- #functionName: ��� �������.
-- #params: ���������, ������� ����� �������� � �������.
function AddFunctionToMainTimer(functionName, params)
	-- ���� ��� ����������, �� �������� � ���������� ������ ������. 
	if params == nil then params = '' end

	-- ���������� �������.
	local newTimerFunc = nil
	
	-- ���� ������� ������� �� ����, �� �� ������������� � ���.
	if (STDLib_Timer ~= nil) then
		newTimerFunc = GetNextName("STDLib_Timer")
		load(newTimerFunc .. " = STDLib_Timer")()
	end
	
	local s = "STDLib_Timer = function()\n"
	-- ���� ������� ������� �� ����, �� �� �������� ��.
	if (STDLib_Timer ~= nil) then s = s .. newTimerFunc .. "()\n" end
	s = s .. functionName .. "(" .. params .. ")\n"
	s = s .. "end\n"
	load(s)()
end

-- #func: ����� ������� ������� �� ���������� ����.
-- #searchTable: ������� ��� ������.
-- #fieldName: ��� ���� �� �������� ����� ������������� �����.
-- #fieldValue: �������� ���� �������� ��������.
function table.GetByField(searchTable, fieldName, fieldValue)
	for key,value in pairs(searchTable) do
		if (value[fieldName] == fieldValue) then
			return value
		end
	end
	return nil
end

-- #func: ����� ������� ������� �� ���������� ����.
-- #searchTable: ������� ��� ������.
-- #fieldName: ��� ���� �� �������� ����� ������������� �����.
-- #fieldValue: �������� ���� �������� ��������.
function table.FindKeyByItem(searchTable, item)
	for key,value in pairs(searchTable) do
		if (value == item) then	return key end
	end
	return nil
end

-- #func: ���������, �������� �� ������� �������� ����.
-- #searchTable: ������� ��� ������.
-- #keyName: ��� ���� �� �������� ����� ������������� �����.
-- #returns: true, ���� ��������, false, ���� ���.
function table.ContainsKey(searchTable, keyName)
	for key,value in pairs(searchTable) do
		if (key == keyName) then
			return true
		end
	end
	return false
end

-- #func: ���������, �������� �� ������� �������� ��������.
-- #searchTable: ������� ��� ������.
-- #valueName: ��� ���� �� �������� ����� ������������� �����.
-- #returns: true, ���� ��������, false, ���� ���.
function table.ContainsValue(searchTable, valueName)
	for key,value in pairs(searchTable) do
		if (value == valueName) then
			return true
		end
	end
	return false
end

-- #func: ��������� ������� ���������� � ������.
-- #cmd: ������ � �������� ��� ������ ����� � ���������.
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

-- # func: ���������� ��������� ������������� ����� �� ��������� ������. ���� ����� ������� len, �� ������ �� ����������.
-- #num: �����.
-- #len: ��������� ����� ������.
function StretchNum(num, len)
	if (num == math.floor(num)) then
		num = string.rep("0", len - string.len(num)) .. num
	else
		num = num .. string.rep("0", len - string.len(num))
	end
	
	return num
end

-- #func: ��������� ������ �� ������ ��������, ��������� � �������� ����������� separator.
-- #str: ������ ��� ����������.
-- #separator: ������, ������������ � �������� �����������.
-- #returns: ���������� � ���������� ���������� ������ ������ ��������.
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

-- #func: ��������� ������ ����� � ����, ��������� � �������� ����������� separator.
-- #mas: ������ ����� ��� �������.
-- #separator: ������, ������������ � �������� �����������.
-- #returns: ���������� � ���������� ������� ������.
function string.Join(mas, separator)
	return table.concat(mas, separator)
end
