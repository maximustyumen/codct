-- �����, ������� ��������� ������ � ���� � ������ �� ������, ���� ����� ����� ���������� �� ��������� period.
Undead = {
	-- ����� � ��������, ����� ����������� �������� ���� ��������� ������.
	period = 10,
	
	-- ������.
	data  ={},
	
	-- �������������.
	Initialize = function(sender, filename, period)
		--AppendObjectFields(sender, Undead)
		filename = filename or "play_undead.tmp"
		Undead.filename = GetUserTempDir() .. "/" ..filename
		
		if (period ~= nil) then Undead.period = period end
	end,
	
	-- ��������� ������.
	Save = function(sender)
		local f = io.open(sender.filename, 'w+')
		f:write(os.time()..'\n')
		for k,v in pairs(sender.data) do
			if (type(v) ~= 'table') and (type(v) ~= 'function') then
				if (type(v) == 'boolean') then
					if v then v = 'true' else v = 'false' end 										
				elseif (type(v) == 'string') then
					v = '"'..v..'"'
				end
				
				f:write('Undead.data.'..k..'='..v..'\n')
			end
		end
		f:close()
	end,
	
	-- ��������� ������.
	Load = function(sender)
		local f = io.open(sender.filename, 'r')
		if f then
			local last_time = f:read("*line")
			if ((last_time == nil) or ((os.time() - last_time) < 0) or ((os.time() - last_time) > sender.period) ) then
				f:close()
				return
			end
			
			load(f:read("*all"))()
			f:close()
		end
	end,
	
	-- �������� ��� ������.
	Reset = function(sender)
		sender.data = {}
	end,
	
	-- ���������� �������� ���������.
	Set = function(sender, name, value)
		sender.data[name] = value
	end,
	
	-- ��������� �������� ���������.
	Get = function(sender, name)
		return sender.data[name]
	end		
}