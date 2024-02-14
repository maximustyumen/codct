-- Hands (Start).
-- ����� ����. ��������� ����� �������� � ����. ��� ������ �������� � ���� �� ����������.
-- ��� ������� ����� �������, ������� ���������� � ����, ������� �������� ��� ���.
-- � ����� ����� ���������� ������ ����� �������.
CHand = {
	-- ������ � ����.
	objectIn = nil,
	
	-- ������� �� ����.
	enabled = true,
	
	-- ���������� ������ ��� ���������� �� ���.
	showOnRelease = false,
	
	Create = function()
		local this = EventAttributable.Create()
		this:InheritClass(CHand)
		return this
	end,
	
	Take = function(sender, object)
		if sender.enabled and (sender:IsFree() or object == sender.objectIn) and (object ~= nil)  then
			sender.objectIn = object
			sender.objectIn:Show(false)
			sender:RaiseEvent("ETake", {sender = object})
			return true
		else
			return false
		end
	end,
	
	-- ���������� ��� ������� Use ��������.
	HTake = function(sender, e)
		sender:Take(e.sender)
	end,
	
	Release = function(sender)
		local object = sender.objectIn		
		sender.objectIn = nil
		
		if object ~= nil then
			if sender.showOnRelease then object:Show() end
			sender:RaiseEvent("ERelease", object)
		end
		
		return object
	end,
	
	IsFree = function(sender)
		return sender.objectIn == nil
	end,
	
	IsBusy = function(sender)
		return sender.objectIn ~= nil
	end,
	
	-- ���� ������ ������ target.
	Keeps = function(sender, target)
		return (sender.objectIn == target)
	end,

	-- ���� ������ ������ � ����� className ������ className.
	KeepsClass = function(sender, className)
		return sender:IsBusy() and (sender.objectIn.className == className)
	end	
}
-- Hands (End).