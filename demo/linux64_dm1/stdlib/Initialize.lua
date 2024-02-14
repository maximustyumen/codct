-- ������ ���� ������������� ��� ���������� ������� ��������� � ��������� ������ ��������, 
-- ������� ����� ��������� � ����� �������� STDLib, �� ����� �������� ����.

-- ������ ���� ��������� ���������� � �������� ����� ��� � STDLib-�������� (STD_Box310:Hide())
_unknowns = {}
_Gmt = {
	__index = function(k, v)
		-- ������������ ������� ������������ � STD_
		if (_unknowns[v] == nil) and (string.sub(v, 1, 4) == 'STD_') then
			local v2 = string.sub(v, 5)
			if (GetParent(v2) ~= '') then					
				_G[v] = Global.CreateObject(v2)
				return _G[v]
			else
				_unknowns[v] = true -- �� ������ �������� ������ - ������� ��� ����.
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

-- ������� ��� �������� _PUSH �� ���������.
_PUSH = function()
	if (OLD_PUSH) then OLD_PUSH() end
	
	local counter = 50
	-- ���� ������ ��� �������� ��������� ����� _DRAG, �� � ����� ��� ������������ � ������ ����.
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
	-- ������������ �������. ���� ������� ����������� �����-�� �������, �� � �������� ��� �� ������.
	if Global.isEventHandled ~= true then
		local obj = GetParent(UserData.CurrentObjectName)
		while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do			
			counter = counter - 1
			local push = _G[obj..'_PUSH']
			if push then
				--Global.lastPushedObject = obj
				push()
				-- ���� ������� ���-�� ����������, �� �� ���������� ��� ����.
				if Global.isEventHandled == true then break end
				
			end		
			obj = GetParent(obj)
		end
	end

	Global.isEventHandled = false
end

-- ������� ��� �������� _RELEASE �� ���������.
function _RELEASE()
	if (OLD_RELEASE) then OLD_RELEASE() end
	
	Global.lastPushedObject = nil
	local counter = 50
	-- ������������ �������. ���� ������� ����������� �����-�� �������, �� � �������� ��� �� ������.
	if Global.isEventHandled ~= true then
		local obj = GetParent(UserData.CurrentObjectName)
		while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
			counter = counter - 1
			local push = _G[obj..'_RELEASE']
			if push then
				push()
				-- ���� ������� ���-�� ����������, �� �� ���������� ��� ����.
				if Global.isEventHandled == true then break end
			end
			obj = GetParent(obj)
		end
	end
	Global.isEventHandled = false
end

-- ������� ��� �������� _USE �� ���������.
function _USE()
	if (OLD_USE) then OLD_USE() end
	local counter = 50
	-- ������������ �������. ���� ������� ����������� �����-�� �������, �� � �������� ��� �� ������.
	if Global.isEventHandled ~= true then
		local obj = GetParent(UserData.CurrentObjectName)
		while obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
			counter = counter - 1
			local push = _G[obj..'_USE']
			if push then
				push()
				-- ���� ������� ���-�� ����������, �� �� ���������� ��� ����.
				if Global.isEventHandled == true then break end
			end
			obj = GetParent(obj)
		end
	end
end

-- ������� ��� �������� _DRAG �� ���������.
function _DRAG(x1, y1, x2, y2)
	if (OLD_DRAG) then OLD_DRAG(x1, y1, x2, y2) end
	local counter = 50
	-- ������� ���������� ��� ������ �� ������� ������ ������.
	if Global.lastPushedObject then
		local func = _G[Global.lastPushedObject..'_DRAG']		
		if func then func(x1, y1, x2, y2) end
		
		-- ������������ �������. ���� ������� ����������� �����-�� �������, �� � �������� ��� �� ������.
		if Global.isEventHandled ~= true then
			local obj = GetParent(Global.lastPushedObject)
			while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
				counter = counter - 1
				local func = _G[obj..'_DRAG']
				if func then
					if func then func(x1, y1, x2, y2) end
					-- ���� ������� ���-�� ����������, �� �� ���������� ��� ����.
					if Global.isEventHandled == true then break end
				end
				obj = GetParent(obj)
			end
		end	
	
		--local func = _G[Global.lastPushedObject..'_DRAG']		
	end
end

function STDLIB_INIT()
	Global.Initialize() -- ������������� ����������� ����� Global.
	Hand = CHand.Create() -- ������� ���������� ����.
	User = CUser.Create()
end
