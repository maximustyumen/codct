-- #class: �����, ����������� �������������� �������� ClassObject.
Global = {
	-- # prop: ��������� ����� �������� � �������.
	lastClock = os.clock(),
	
	-- HUD, ������� ���� ��� ��������� ��������.
	HUD = nil,
	
	-- ���� ����� ������ ��� ������ � EditText. [en | ru]
	lang = 'ru',

	-- ��� ������� ��� ��������������.
	editTextCursor = '_',
	
	timeSpeed = 1, -- ���������� �������� ��������. �������� ��� ���� ���������.
	
	-- # prop: ��������� �� ���������.
	default = {
		events = 'use;push;release;drag'
	},
	
	-- ����� ��������� �������. ������ �������� - �������, ������� ����� ���������� � �������. ��������� �������� �������: dt.
	-- ������ �������� - ���������� ������� ����� ������� �������� �������.
	-- ���������� ������ �� ��������� ������� � �������.
	timerItems = {},	

	-- #meth: ������������� �������.
	Initialize = function(sender)
		AddFunctionToMainTimer("Global.OnTimer")

		Global.HUD = Global.CreateObject("HUD")
		
		AddTextObject("Global_InfoText", "", 1)
		-- �� �������� ������ ��� UserData ��� ��� �� ���������.
		Global.kWidth = UserData.WINDOW_WIDTH / 10000
		Global.kHeight = UserData.WINDOW_HEIGHT / 10000
		Global.kScale = math.min(Global.kWidth, Global.kHeight)

		Global.infoText = "Global_InfoText"
		RConnectObjectTo(Global.infoText, "HUD")
		SetTextPosition(Global.infoText, 40*Global.kScale, UserData.WINDOW_HEIGHT - 180*Global.kScale, -10)
		SetTextSize(Global.infoText, 200*Global.kScale)
		SetTextColor(Global.infoText, 0 ,0, 0, 1)
		Global.infoTextColor = {0, 0, 0}

		-- �������� Global ����� �������.
		AppendObjectFields(Global, Eventable)
		if GetParent('Global_InfoText_plane') ~= '' then
			Global.infoTextPlane = Global.CreateObject('Global_InfoText_plane')
			Global.infoTextPlane:AttachTo('HUD')
			Global.infoTextPlane:Move(0, UserData.WINDOW_HEIGHT, -20, true)
		end
		math.randomseed(os.time())				
		
		-- ������� ���� ������ � ��� �������� �������.
		Global.keyCodes = {}
		Global.keyCodes[39]=221
		Global.keyCodes[44]=193
		Global.keyCodes[58]=198
		Global.keyCodes[59]=198
		Global.keyCodes[60]=193
		Global.keyCodes[62]=222
		Global.keyCodes[65]=212
		Global.keyCodes[66]=200
		Global.keyCodes[67]=209
		Global.keyCodes[68]=194
		Global.keyCodes[69]=211
		Global.keyCodes[70]=192
		Global.keyCodes[71]=207
		Global.keyCodes[72]=208
		Global.keyCodes[73]=216
		Global.keyCodes[74]=206
		Global.keyCodes[75]=203
		Global.keyCodes[76]=196
		Global.keyCodes[77]=220
		Global.keyCodes[78]=210
		Global.keyCodes[79]=217
		Global.keyCodes[80]=199
		Global.keyCodes[81]=201
		Global.keyCodes[82]=202
		Global.keyCodes[83]=219
		Global.keyCodes[84]=197
		Global.keyCodes[85]=195
		Global.keyCodes[86]=204
		Global.keyCodes[87]=214
		Global.keyCodes[88]=215
		Global.keyCodes[89]=205
		Global.keyCodes[90]=223
		Global.keyCodes[91]=213
		Global.keyCodes[93]=218
		
		-- �������� ������ ���������� ������� �������.
		if KEY_PUSH and (KEY_PUSH ~= Global.KeyPush) then
			OLD_KEY_PUSH = KEY_PUSH	
		end
		-- ������� ����� ����������.
		KEY_PUSH = Global.KeyPush		
		
		-- ������ ��� ���������� Over � Out.
		Global.AddTimer('Global:TestOverOut()')
		Global.AddTimer('Global:OnEditTextTimer()', 0, 0.5)
	end,
	
	-- showtime - ����� � �������� ����� �������� ��������� ����� ������.
	-- effect - ������ ���������� ������ (hide - ������, blind - ������ ������, blink - ������� � �������� 1)
	ShowMessage = function(message, isutf8, showtime, effect)
		if GetParent(Global.infoText) then
			local itc = Global.infoTextColor
			Global.ITSetOpacity(1)
			if Global.infoTextTimer then
				Global.DelTimer(Global.infoTextTimer) -- ������ ������ ������, ���� �� ���.
				Global.infoTextTimer = nil
			end
			if message == nil then message = '' end -- ������ �� ��������� ��� nil � ������.
			message = tostring(message)
			-- Wrap �����
			local cnt = UserData.WINDOW_WIDTH / (120 * Global.kScale)
			if isutf8 then
				message = Global.Wrap(message, 2*cnt)
			else
				message = Global.Wrap(message, cnt)
			end
			
			-- ������� �����.
			SetText(Global.infoText, message, (isutf8 == true))
			
			-- ���� ���� ��������, �� ������� �� ��� ����. ������ �������� 1*1 ��� ��� ���������� ������ �� �������.
			if Global.infoTextPlane then
				local tmp, lines = message:gsub("\n", '')
				Global.infoTextPlane:Show(message ~= '')
				Global.infoTextPlane:Scale(UserData.WINDOW_WIDTH, ((lines + 1)*200 + 40)*Global.kScale, 1, true)
			end
		end
		
		-- ������ ��������� ������.
		if showtime then
			local cmd = ''		
			if effect == 'hide' then -- �������.
				cmd = ''
			elseif effect == 'blind' then -- ������� �������.
				cmd = 'Global.ITSetOpacity(1 - timer.elapsed/timer.len)'
			elseif effect == 'blink' then  -- ��������
				cmd = 'Global.ITSetOpacity(math.ceil(math.modf(timer.elapsed, 2) - 1))'
			elseif effect == 'blink-blind' then  -- ������� ��������
				cmd = 'Global.ITSetOpacity(math.abs(1 - 2*math.modf(timer.elapsed, 1)))'
			elseif effect == 'blink-blind1' then  -- ������� �������� 0.5 - 1
				cmd = 'Global.ITSetOpacity(0.2 + 0.8*math.abs(1 - math.modf(timer.elapsed, 2)))'
			elseif effect == 'blink-blind2' then  --  ������� �������� �������-����������
				cmd = 'Global.ITSetOpacity(1 - math.modf(timer.elapsed, 1))'
			elseif effect == 'blink-blind3' then  -- ������� �������� ����������
				cmd = 'Global.ITSetOpacity(math.modf(timer.elapsed, 1))'
			end
			
			Global.infoTextTimer = Global.AddTimer(cmd, showtime, 0, 'Global.infoTextTimer = nil; Global.ShowMessage()')
		end
	end,
	
	
	-- ������ � �������� ������� Global � timerItems (������ �����).	
	-- ����� ��������� � ������� ������� ��������� � ������������ ��� ���������������� �������.
	OnTimer = function()
		-- ������ ������� ����������� � ������ ������� �����.
		Global.isEventHandled = false

		-- ����� ��������� � ��������� ������
		local dt = os.clock() - Global.lastClock
		Global.lastClock = os.clock()
		
		dt = dt*Global.timeSpeed

		-- ���������� ��������.
		local i = #Global.timerItems
		
		-- ��������� ���� ���������������� ��������.
		while i > 0 do
			-- ��������� i-�� �������.
			local item = Global.timerItems[i]

			if (item ~= nil) and (not item.paused) then				
				-- ���������� ��������� ������� �������.
				local elapsedBef = item.elapsed;
				-- ���������� �������� ���������� �������.
				item.elapsed = item.elapsed + dt*item.speed
				
				-- ������������� ������������.
				local needRemove = false;
				--local elapsedBef = item.elapsed;
				-- ��������� ��������� �������.
				if ((item.elapsed <= 0) and (item.speed < 0)) or ((item.elapsed >= item.len) and (item.speed > 0)) then
					item.cycles = item.cycles - 1;
					
					-- ����� ��� ��������.
					if ( (item.cycles > 0) or (item.cycles == nil) or (item.len == 0) ) then						
						if (item.reverse) then item.speed = -item.speed; end -- ������.
						
						local el_ind;
						local re_ind;
						local del; -- delta elapsed
						local nel; -- new elapsed
						
						if (item.elapsed > 0) then el_ind = 1 else el_ind = -1 end
						if (item.reverse) then re_ind = 1 else re_ind = -1 end
						
						if (el_ind < 0) then del = -item.elapsed else del = item.elapsed - item.len end
						if (re_ind*el_ind == -1) then nel = del else nel = item.len - del end
						
						item.elapsed = nel
					else
						needRemove = true;
					end
				end

				-- ���������� �������� �������.
				item.dt = item.elapsed - elapsedBef;
				
				-- ��� ������������� ������������ �������� ������� ������ ��� �������� ������� ������ period.
				if (item.period > 0) then
					item.period_dt = item.period_dt + dt -- ����������� � �������� ������� �����.
					if (item.period_dt >= item.period) then
						local itel = item.elapsed
						local itdt = item.dt
						item.dt = item.period
						--item.elapsed = 
						item.func(item)
						--item.elapsed = itel 
						item.dt = itdt
						item.period_dt = item.period_dt - item.period
					end						
				else -- ���������� ����������.
					item.func(item)
				end					
				
				if needRemove then
					table.remove(Global.timerItems, i)
					ExecuteCmd(item.cmdEnd)				
					--Global.DelTimer(item)
				end
			end
			i = i - 1
		end
	end,


	-- #meth: �������� ������.
	-- #func: ������ � �������� ������� ����� ����������� ������������. �������� �� ���� timer{dt, elapsed}.
	-- #len: ����� ������ ������� ������� � ��������. ���� 0, �� �������������.
	-- #period: ������ �������, ����� ������� ����� ����������� ������ � ��������. ���� 0, �� �������������.
	-- #cmdEnd: �������, ������� ����������� ����� ���������� ������ �������.
	-- #params: �������������� ���������� �������
	-- #returns: ������ ������.
	AddTimer = function(func, len, period, cmdEnd, params)
		if func == nil then func = '' end
		if len == nil then len = 0 end
		if period == nil then period = 0 end
		
		local item = {}
		item.func = load('return function(timer) ' .. tostring(func) .. ' end')()
		item.len = len
		item.period = period
		item.cmdEnd = cmdEnd
		item.dt = 0
		item.elapsed = 0
		item.speed = 1
		item.cycles = 1
		item.reverse = false
		item.paused = false
		item.period_dt = 0
		
		if (params ~= nil) then AppendObjectFields(item, params) end -- ����������� ��� ��������� � ������.
		
		table.insert(Global.timerItems, item)
		return item
	end,
	
	-- #meth: �������� ������.
	AddTimer2 = function(func, params)
		return Global.AddTimer(func, 0, 0, nil, params)
	end,
	
	-- �������� ������� - ������� ������� ����� ��������� ����� len ������.
	AddTimeout = function(len, cmdEnd)	
		Global.AddTimer(nil, len, 0, cmdEnd)
	end,	
	
	-- #meth: ������� ������.
	-- #timer: ��������� ������. 
	DelTimer = function(timer)
		-- ����� ����� ��� ������� �������.
		local key = table.FindKeyByItem(Global.timerItems, timer)
		-- �������� ������.
		if (key ~= nil) then table.remove(Global.timerItems, key)
		else print('Global.DelTimerItem: Item not found') end
	end,
	-- ������ � �������� ������� Global � timerItems (����� �����).
	
	-- # meth: ������� ������� ��� ������� �������. ������ ������������� ����������� � Global.objects.
	-- #manName: ��� �������. ���� nil, �� ����������� ������������� ��� ��� ������� eTarget+'_man'.
	-- #eTarget: ��� ��� ����� ���� 3D ��������, � �������� ���������� �����������. ������ ������ ����� ��������� �������.
	-- #mTarget: ��� ��� ����� ���� 3D ��������, ������� ����� ��������� ��������: ��������������, ���������. ������ ������������,
	-- ����� ��� ����� �� ������ ����� �������������� �������.
	-- #events: �������� ���� �������, ������� ����� ������������� ��� ������� ������� ����������� ";" (drag, click, use, timer).
	-- #returns: ��������� �������� ClassObject.
	CreateObjectSeam = function(manName, eTarget, mTarget, events)
		-- ���� ���� ������ �� ������, �� ������� �� ��� ���� ������ (���������, ��� ��� ��� �����������).
		if type(eTarget) ~= 'string' then return eTarget end

		local objectName = ''

		-- ������ �������� �������.
		if type(eTarget) == 'string' then
			objectName = string.Split(eTarget, ';')[1]
			--eTarget = string.Split(eTarget, ';')
		--else
			--objectName = eTarget[1]			
		end
		
		-- ������ events.
		if events == nil then events = Global.default.events end
		
		-- ������������ ����� ������������.
		if manName == nil then manName = objectName .. "_man" end
		
		-- ���� ������ ������������ ���, �� ������� ���.
		if ObjectExists(manName) == false then
			print("Global: created object - " .. manName)
			return ClassObject.Create(manName, eTarget, mTarget, events)
		else
			print("Global: object already exists - " .. manName)
			return GetObjectByName(manName)
		end
	end,

	-- #meth: ������� ������� ��� ������� �������. ������ ������������� ����������� � Global.objects.
	-- #eTarget: ��� ��� ����� ���� 3D ��������, � �������� ���������� �����������. ������ ������ ����� ��������� �������.
	-- ���� ������� ��� ��������� ������, �� �� �������� ��� ���������.
	-- #mTarget: ��� ��� ����� ���� 3D ��������, ������� ����� ��������� ��������: ��������������, ���������. ������ ������������,
	-- ����� ��� ����� �� ������ ����� �������������� �������.
	-- #events: �������� ���� �������, ������� ����� ������������� ��� ������� ������� ����������� ";" (drag, click, use, timer).
	-- #returns: ��������� �������� ClassObject.
	CreateObject = function(eTarget, mTarget, events)
		return Global.CreateObjectSeam(nil, eTarget, mTarget, events)
	end,
	
	-- ������� ���� ������� ����� Global.CreateObject. 3D �� �����������.
	CreateClone = function(object, suffix)
		if object == nil then return nil end
		
		local cloned = object:CloneName(suffix)
		local new = Global.CreateObject(cloned[1], cloned[2])
		return new
	end,
	
	-- �������� ���������� ��� � ��������� name.
	GetUniqueName = function(name)
		local sender = Global
		if sender.last_unique_name_index == nil then sender.last_unique_name_index = 1 end
		sender.last_unique_name_index = sender.last_unique_name_index + 1
		if name == nil then name = 'unique' end
		return name..sender.last_unique_name_index
	end,

	ITSetFont = function(name)
		local fileName = getFileNameFromOther(name)
		if fileName then SetTextFont(Global.infoText, fileName) end
	end,

	ITSetColor = function(r, g, b)
		Global.infoTextColor = {r, g, b}
		SetTextColor(Global.infoText, r, g, b, 1)
	end,

	ITSetOpacity = function(a)
		local gitc = Global.infoTextColor
		SetTextColor(Global.infoText, gitc[1], gitc[2], gitc[3], a)
	end,


	-- ������� ��� ������������� ������� Over � Out ��� ��������.
	TestOverOut = function(sender)
		local curObj = UserData.CurrentObjectName
		if (curObj == 'not using hands') then curObj = nil end 
		
		local counter = 100
		
		-- ��������� ������������ �������� �������.
		if (sender.lastObj ~= curObj) then
			-- ��������� ������ � �������� OUT ��� �������.
			if sender.lastObj and (tonumber(sender.lastObj) == nil) then
				local func = _G[sender.lastObj..'_OUT']			
				if func then func() end
				if _OUT then _OUT(sender.lastObj) end
				
				-- ������������ �������. ���� ������� ���������� � �����-�� �������, �� � �������� ��� �� ������.
				if Global.isEventHandled ~= true then
					local obj = GetParent(sender.lastObj)
					while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do			
						counter = counter - 1
						local func = _G[obj..'_OUT']
						if func then
							func()
							-- ���� ������� ���-�� ����������, �� �� ���������� ��� ����.
							if Global.isEventHandled == true then break end
							
						end
						
						obj = GetParent(obj)												
					end
				end			
			end	
			
			sender.lastObj = curObj -- ���������� ��������� ������ � ������.
			
			-- ��������� ������ � �������� OVER ��� �������.
			if sender.lastObj and (tonumber(sender.lastObj) == nil)then 
				local func = _G[sender.lastObj..'_OVER']
				if func then func() end
				if _OVER then _OVER(sender.lastObj) end

				-- ������������ �������. ���� ������� ���������� � �����-�� �������, �� � �������� ��� �� ������.
				if Global.isEventHandled ~= true then
					local obj = GetParent(sender.lastObj)
					while counter > 0 and obj and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
						counter = counter - 1
						local func = _G[obj..'_OVER']
						if func then
							--Global.lastPushedObject = obj
							func()
							-- ���� ������� ���-�� ����������, �� �� ���������� ��� ����.
							if Global.isEventHandled == true then break end
							
						end		
						obj = GetParent(obj)												
					end
				end
			end
		end		
	end,
	
	-- ����� ����� �� ������ ������ �� ������������� limit.
	Wrap = function(str, limit, indent, indent1)
		indent = indent or ""
		indent1 = indent1 or indent
		limit = limit or 72
		local here = 1-#indent1
		return indent1..str:gsub("(%s+)()(%S+)()",
			function(sp, st, word, fi)
				if fi-here > limit then
				here = st - #indent
				return "\n"..indent..word
				end
			end
		)
	end,

	-- ����� ����� �� ������ ������ �� linesPerPage ����� ������ � signsPerLine ��������.
	Paginate = function(text, signsPerLine, linesPerPage)
		text = Global.Wrap(text, signsPerLine)

		local blocks = {}
		local curLine = 1
		local curBlock = 1
		for line in string.gmatch(text, "([^\n]*)\n?") do
			if blocks[curBlock] then blocks[curBlock] = blocks[curBlock].."\n" end
			blocks[curBlock] = (blocks[curBlock] or '') .. line
			curLine = curLine + 1
			if curLine > linesPerPage then
				curLine = 1
				curBlock = curBlock + 1
			end
		end
		return blocks
	end,
	
	-- ������������� (������� � �����)
	EditText = function(textObject, maxLength)
		if Global.editedText then
			Global.editTextCursorVisible = ''
			Global:OnEditTextTimer()
		end	

		if textObject == nil then
			Global.editedText = nil
		else
			Global.editedText = Global.CreateObject(textObject)
			if Global.editedText.text == nil then Global.editedText.text = '' end
			
			if maxLength == nil then			
				Global.editedTextMaxLength = Global.editedText.maxLength
			else
				Global.editedTextMaxLength = maxLength
			end		
			
			Global.editTextCursorVisible = Global.editTextCursor
		end
	end,


	-- ���������� ������� ������ �� ����������.
	KeyPush = function(key, c, a, shift, raw)
		-- �������� ������ ����������, ���� �� ���.
		if OLD_KEY_PUSH then
			OLD_KEY_PUSH(key, c, a, shift, raw)
		end
		
		local char = ''
		
		-- ������� ������� ����.
		if Global.lang == 'ru' then
			if (key > 97) and (key < 122) then key = key - 32 end
			if  Global.keyCodes[key] then
				char = string.char(Global.keyCodes[key] + 32*(1 - shift))
			end
		-- ������� ���������� ����.
		else
			if (key > 64) and (key < 91) then			
				char = string.char(key + 32*(1 - shift))
			end		
		end
		
		if char == '' then
			-- �����.
			if (key > 48) and (key < 57) then
				char = key - 48
			-- ���� �������.
			elseif (key > 13) and (key < 65) then
				char = string.char(key)
			end
		end
		
		
		-- ���� ���� ����� ��� ��������������, �� ���������� ���.
		if Global.editedText ~= nil then	
			Global.editedText.text = tostring(Global.editedText.text)
			
			-- ������� 1 ������.
			if key == 16777219 then
				Global.editedText.text = Global.editedText.text:sub(1, Global.editedText.text:len() - 1)
			-- Enter
			elseif (key == 16777220) then	
				--char = 13
			end
			
			if (Global.editedTextMaxLength == nil) or (Global.editedText.text:len() < Global.editedTextMaxLength) then
				Global.editedText.text = Global.editedText.text .. char
				SetText(Global.editedText.mTarget[1], Global.editedText.text)
			end
		end
		
		Global:RaiseEvent('EKeyPush', {key = key, char = char, shift = shift})
	end,

	OnEditTextTimer = function(sender)	
		if Global.editedText then		
			SetText(Global.editedText.mTarget[1], Global.editedText.text..Global.editTextCursorVisible)

			if Global.editTextCursorVisible == '' then
				Global.editTextCursorVisible = Global.editTextCursor
			else
				Global.editTextCursorVisible = ''
			end
		end
	end,
	
	-- ���� ������ ������, �� ���������� ���, ����� ������� � ����������.
	AsObject = function(name)
		-- ���� ���� ������ �� ������, �� ������� �� ��� ���� ������ (���������, ��� ��� ��� �����������).
		if type(name) ~= 'string' then
			return name
		else
			return _G[string.Split(name, ';')[1]..'_man'] or Global.CreateObject(name)
		end	
	end	
}