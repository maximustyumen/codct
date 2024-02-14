-- ���� - ������������������ ��������.
CCinema = {
	-- ����� ������(��������).
	frames = {},
	
	-- ������� ����.
	curFrame = 1,
	
	-- ��������� �� ���������� ����.
	cmdEnd = '',
	
	-- ������, ������ ���� �����.
	timer = 0,
	
	-- ���������� ����������.
	cycles = 1,
	
	Create = function()
		local sender = AbstractClassObject.Create()
		sender:Inherit(CCinema)
		return sender
	end,
	
	-- �������� �������.
	AddFrame = function(sender, cmd, len)
		table.insert(sender.frames, {cmd = cmd, len = len})
	end,
	
	-- �������� ������.
	AddTimer = function(sender, cmd, len)
		table.insert(sender.frames, {cmd = cmd, len = len, isTimer = true})
	end,

	Clear = function(sender)
		sender.frames = {}
		sender.curFrame = 1
		sender.cmdEnd = ''
	end,

	Play = function(sender)
		local mustStop = false
		local doing = true
		
		while doing do
			doing = false
			-- ���� ���������, �������� ���������� ��������. ���� ��� ����, �� ������ �������.
			if sender.curFrame > #sender.frames then
				sender.cycles = sender.cycles - 1
				if sender.cycles > 0 then
					sender.curFrame= 1					
				else
					mustStop = true
				end
			end			

			-- ���� ���� ������������, �� �������� ������� ������ � �����������.
			if mustStop then
				if sender.cmdEnd ~= nil then
					load('return function(sender) '..sender.cmdEnd..' end')()(sender)
				end
			else
				local frame = sender.frames[sender.curFrame]
				-- ���� ���� - ������, �� ������� ������ � ��������.
				if frame.isTimer == true then
					Global.AddTimer(frame.cmd, frame.len, 0, sender.self..':Play()')
				else -- ���� ������ ����, ������� �������.
					if (frame.cmd ~= nil) then
						load('return function(sender) '..frame.cmd..' end')()(sender)
					end
					
					if frame.len == nil then
						doing = true
					else
						Global.AddTimer('', frame.len, 0, sender.self..':Play()')
					end					
				end				
				sender.curFrame = sender.curFrame + 1
			end
		end
	end		
}