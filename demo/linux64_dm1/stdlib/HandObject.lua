-- Hands (Start).
-- Класс рука. Позволяет брать предметы в руки. При взятии предмета в руки он скрывается.
-- При попытке взять предмет, который находиться в руке, предмет береться еще раз.
-- В руках может находиться только однин предмет.
CHand = {
	-- Объект в руке.
	objectIn = nil,
	
	-- Активна ли рука.
	enabled = true,
	
	-- Показывать объект при выпускании из рук.
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
	
	-- Обработчик для события Use объектов.
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
	
	-- Рука держит объект target.
	Keeps = function(sender, target)
		return (sender.objectIn == target)
	end,

	-- Рука держит объект с полем className равным className.
	KeepsClass = function(sender, className)
		return sender:IsBusy() and (sender.objectIn.className == className)
	end	
}
-- Hands (End).