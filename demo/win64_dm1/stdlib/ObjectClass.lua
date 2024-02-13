-- # class: Абстрактный класс. Создает имя своего экземпляра по полю self.
Inheritable = {
	self = nil,
	countStatic = 0,
	
	-- #meth: Конструктор объекта.
	-- #self: имя создаваемого объекта, которое можно использовать в любом месте программы.
	Create = function(self)
		this = {}
		AppendObjectFields(this, Inheritable)
		Inheritable.InheritClass(this, Inheritable)
		if (self == nil) then self = GetNextName("stdlib_object") end
		this.self = self
		if (self ~= nil) then load(self .. " = this")() end
		return this
	end,
	
	-- #meth: Добавляет поля объекта source к полям sender.
	-- #sender: Объект, к которому будут добавлены поля.
	-- #source: Объект, поля которого будут использоваться для добавления.
	InheritClass = function(sender, source)
		if source == nil then
			print("AbstractClassObject: cannot inherit nil class.")
		else
			-- Копировать поля (рекурсивно)
			-- Дописать _base
			-- Cделать setmetatable()
			AppendObjectFields(sender, source)
			if not sender._base then sender._base = {} end -- Добавим список наследников.
			sender._base[source] = source -- Добавим класс в список наследования.
			sender.__index = function(k, v)
				local _base
				local func = nil
				for _base in pairs(k._base) do
					func = _base[v] or func
				end
				return func
			end
			setmetatable(sender, sender)
		end
	end
}
-- Синоним для InheritClass.
AbstractClassObject = Inheritable
AbstractClassObject.Inherit = AbstractClassObject.InheritClass


-- # class: Абстрактный класс изменения парамеров геометрических объектов. Хранит и изменяет геометрические параметры 3D объекта target.
-- Влияние смещений на входе, на смещения объекта задаются в A.
AbstractClassMotion = {
	-- #prop: Параметр X объекта.
	x = 0,
	-- #prop: Параметр Y объекта.
	y = 0,
	-- #prop: Параметр Z объекта.
	z = 0,
	
	innerX  = 0, -- Внутренний параметр  X объекта.
	innerZ  = 0, -- Внутренний параметр  Y объекта.
	innerY  = 0, -- Внутренний параметр  Z объекта.

	-- #prop: Использовать ли данный манипулятор объекта.
	enabled = false,

	-- # meth: Конструктор объекта.
	-- #self: имя создаваемого объекта, которое можно использовать в любом месте программы.
	Create = function(self)
		local this = AbstractClassObject.Create(self)
		AppendObjectFields(this, AbstractClassMotion)
		return this
	end,
	
	-- # meth: Изменить параметры объекта на dx, dy, dz.
	-- #sender: экземпляр данного класса.
	-- #target: смещаемый объект.
	-- #dx: изменение параметра X.
	-- #dy: изменение параметра Y.
	-- #dz: изменение параметра Z. 
	RChange = function (sender, dx, dy, dz)
		sender.innerX = sender.innerX + dx
		sender.innerY = sender.innerY + dy
		sender.innerZ = sender.innerZ + dz
		sender.x = sender.innerX
		sender.y = sender.innerY
		sender.z = sender.innerZ
	end,

	-- # meth: Установить параметры объекта равными  x, y, z.
	-- #sender: экземпляр данного класса.
	-- #x: новый параметры объекта  X.
	-- #y: новый параметры объекта  Y.
	-- #z: новый параметры объекта  Z. 
	AChange = function (sender, x, y, z)
		sender.innerX = x
		sender.innerY = y
		sender.innerZ = z
		sender.x = sender.innerX
		sender.y = sender.innerY
		sender.z = sender.innerZ
	end,
	
	Refresh = function (sender)
		local vector = Algebra.VectorToChrVector(sender)
		sender:AChange(vector.x, vector.y, vector.z)
	end,
	
	-- Установка всех координат объекта равными value.
	Reset = function(sender, value)
		sender.innerX = value
		sender.innerY = value
		sender.innerZ = value
		sender.x = value
		sender.y = value
		sender.z = value
	end
}

-- Класс положения объекта. Хранит и изменяет положение 3D объекта target.
-- Влияние смещений на входе, на смещения объекта задаются в A.
ClassPosition = {
	-- Конструктор объекта.
	-- self: имя создаваемого объекта, которое можно использовать в любом месте программы.
	Create = function(self)
		local this = AbstractClassMotion.Create(self)
		this:InheritClass(ClassPosition)
		return this
	end,
	
	-- Сместить объект на dx, dy, dz.
	-- sender: экземпляр данного класса.
	-- mTarget: группа смещаемых объектов.
	-- dx: смещение по оси X.
	-- dy: смещение по оси Y.
	-- dz: смещение по оси Z. 
	RChange = function (sender, mTarget, dx, dy, dz)
		V = {dx, dy, dz}
		sender:AChange(mTarget, sender.x + V[1], sender.y + V[2], sender.z + V[3])
		--AbstractClassMotion.RChange(sender, target, dx, dy, dz)
	end,
	
	-- Сместить объект в точку x, y, z.
	-- sender: экземпляр данного класса.
	-- mTarget: группа смещаемых объектов.
	-- x: новая координата X.
	-- y: новая координата Y.
	-- z: новая координата Z. 
	AChange = function (sender, mTarget, x, y, z)
		V = {x, y, z}
		V = Algebra.VectorSubVector(V, {sender.innerX, sender.innerY, sender.innerZ})
		for	k,v in pairs(mTarget) do
			RSetPosition(v, V[1], V[2], V[3])
		end
		-- inherited
		AbstractClassMotion.AChange(sender, x, y, z)
	end	
}

-- Класс поворота объекта. Хранит и изменяет поворот 3D объекта target.
-- Влияние смещений на входе, на поворот объекта задаются в A.
ClassRotation = {
	-- Конструктор объекта.
	-- self: имя создаваемого объекта, которое можно использовать в любом месте программы.
	Create = function(self)
		local this = AbstractClassMotion.Create(self)
		this:InheritClass(ClassRotation)
		return this
	end,
	
	-- Повернуть объект на указанный вектор.
	-- sender: экземпляр класса ObjectClass, который вызывает событие.
	-- mTarget: Поворачиваемый объект.
	-- dx: поворот по оси X.
	-- dy: поворот по оси Y.
	-- dz: поворот по оси Z. 
	RChange = function (sender, mTarget, dx, dy, dz)
		--V = {dx, dy, dz}
		V = {dx, dy, dz}
		sender:AChange(mTarget, sender.x + V[1], sender.y + V[2], sender.z + V[3])
	end,

	-- Установить углы поворота объекта равными x, y, z.
	-- sender: экземпляр данного класса.
	-- mTarget: Поворачиваемый объект.
	-- x: новый угол X.
	-- y: новый угол Y.
	-- z: новый угол Z. 
	AChange = function (sender, mTarget, x, y, z)
		V = {x, y, z}
		V = Algebra.VectorSubVector(V, {sender.innerX, sender.innerY, sender.innerZ})
		for k,v in pairs(mTarget) do
			RSetRotate(v, V[1], V[2], V[3])
		end

		AbstractClassMotion.AChange(sender, x, y, z)
	end	
}

-- Класс масштаба объекта. Хранит и изменяет масштаб 3D объекта target.
-- Влияние смещений на входе, на масштаб объекта задаются в A.
ClassScale = {
	-- Конструктор объекта.
	-- self: имя создаваемого объекта, которое можно использовать в любом месте программы.
	Create = function(self)
		local this = AbstractClassMotion.Create(self)
		--AppendObjectFields(this, ClassScale)
		this:Inherit(ClassScale)
		this.innerX = 1
		this.innerY = 1
		this.innerZ = 1
		this.x = 1
		this.y = 1
		this.z = 1
		return this
	end,
	
	-- Масштабировать объект на указанный вектор.
	-- sender: экземпляр класса ObjectClass, который вызывает событие.
	-- dx: Масштаб по оси X.
	-- dy: Масштаб по оси Y.
	-- dz: Масштаб по оси Z. 
	RChange = function (sender, mTarget, dx, dy, dz)
		V = {dx, dy, dz}
		V = {sender.x*V[1], sender.y*V[2], sender.z*V[3]}
		sender:AChange(mTarget, V[1], V[2], V[3])
	end,

	-- Установить масштаб объекта по осям равным x, y, z.
	-- sender: экземпляр данного класса.
	-- target: смещаемый объект.
	-- x: Новый масштаб X.
	-- y: Новый масштаб Y.
	-- z: Новый масштаб Z. 
	AChange = function (sender, mTarget, x, y, z)		
		if (x < 0.001) then x = 0.001 end
		if (y < 0.001) then y = 0.001 end
		if (z < 0.001) then z = 0.001 end		
		
		V = {x, y, z}
		
		local V = Algebra.VectorDivVector(V, {sender.innerX, sender.innerY, sender.innerZ})
		
		if (V[1] <= 0) then V[1] = 1 end
		if (V[2] <= 0) then V[2] = 1 end
		if (V[3] <= 0) then V[3] = 1 end
		
		for k,v in pairs(mTarget) do
			RSetScale(v, V[1], V[2], V[3])
		end
		AbstractClassMotion.AChange(sender, x, y, z)
	end	
}


--#class: Класс служит для анимимрования объекта
Animatable = {

--[[
AnimState = {
	pos={x,y,z,rel},
	rot={x,y,z,rel},
	scl={x,y,z,rel},
	--col={r,g,b,a},
	--vis=bool,
	len=float,
	cmdProcess=string,
	cmdEnd=string
}
]]--

	animOldState = {},
	
	animNewState = {},

	animEnabled = false,
	
	animTime = 0,
	
	animLastClock = 0,
	
	GetAnimState = function(sender)
		local result = {}
		result.pos = {sender.pos.x, sender.pos.y, sender.pos.z}
		result.rot = {sender.rot.x, sender.rot.y, sender.rot.z}
		result.scl = {sender.scl.x, sender.scl.y, sender.scl.z}
		
		return result
	end,
	
	-- #meth: Анимировать объект к данному состоянию за даное время.
	-- #sender:  объект, вызывающий событие,
	-- #state: состояние в которое нужно перейти (  {pos={x,y,z}, rot={x,y,z}, scl={x,y,z}, col={r,g,b,a}, vis=true/false} ).
	-- #length: Время аимации.
	-- #cmdEnd: Строка с командой, которая выполниться после завершения анимации. В ней доступны sender, state, timer.
	AnimateToState = function(sender, state, length, cmdEnd)
		if type(state) ~= 'table' then
			if sender.states[state] == nil then
				print('AnimateToState: State ' .. tostring(state) .. ' not found.')
			else
				sender.animNewStateIndex = state
				state = sender.states[state]				
			end
		else
			sender.animNewStateIndex = nil
		end
		
		Global.AddTimer(sender.self .. ":HAnimTick(timer)", length)
		
		-- Переопределение команды завершения.
		if cmdEnd ~= nil then state.cmdEnd = cmdEnd end
		
		sender.animOldState = sender:GetAnimState()
		sender.animNewState = state
		sender.animTime = 0
		sender.animLength = length
		sender.animLastClock = os.clock()
		sender.animEnabled = true
		sender.enabled = false
	end,
	
	HAnimTick = function(sender, timer)
		if sender.animEnabled then
			sender.animEnabled = false
			
			local old = sender.animOldState
			local new = sender.animNewState
			local animTime = timer.elapsed
		
			if timer.elapsed >= timer.len then animTime = timer.len end

			if new.pos ~= nil then
				local pos = Algebra.GetMidValueV(0, timer.len, old.pos, new.pos, animTime)
				sender:Move(pos[1], pos[2], pos[3], true)
			end

			if new.rot ~= nil then
				local rot = Algebra.GetMidValueV(0, timer.len, old.rot, new.rot, animTime)
				sender:Rotate(rot[1], rot[2], rot[3], true)
			end
			
			if new.scl ~= nil then
				local scl = Algebra.GetMidValueV(0, timer.len, old.scl, new.scl, animTime)
				sender:Scale(scl[1], scl[2], scl[3], true)
			end

			if (timer.elapsed >= timer.len) then			
				sender.enabled = true
				if sender.animNewStateIndex ~= nil then 
					sender:SetState(sender.animNewStateIndex)
				end

				if (new.cmdEnd ~= nil) then
					local func = load('return function(sender, state, timer) ' .. new.cmdEnd .. '  end')()
					func(sender, new, timer)
				end
			end
			sender.animEnabled = true
		end		
end,
	
	Create = function (self)
		local this = AbstractClassObject.Create(self)
		this:InheritClass(Animatable)
		
		return this
	end	
}
 
--#class: Класс объединяет функционал классов Attributable и EventedObjectClass.
EventAttributable = 
{
	-- #prop: Таблица содержащая 4 значеня:
	-- sender - объект, вызывающий событие или меняющий атрибут (ссылка),
	-- senderEA - генереруемое событие или изменяемый атрибут (строка),
	-- receiver - объект, принимающий событие или изменение атрибута (ссылка),
	-- receiverEA - обработчик вызываемый в принимающем объекте или изменяемый атрибут (строка).
	-- predicate - предикат(строка с содержащая логическое выражение), при выполнении которого связь будет актвирована(вызван receiverEA).
	EA_connections = {},
	
	-- Пследнее время вызова CalcEA.
	EA_lastClockAttr = os.clock(),

	-- Доступен ли объект для вызовов и расчетов.
	EA_enabled = true,	
	
	Create = function ()
		local this = AbstractClassObject.Create(nil)
		this:InheritClass(EventAttributable)
		
		return this
	end,
	
	
	-- Вызывается либо клиентом для пересчета, либо системой при изменении параметров от которых зависит объект.
	CalcEA = function(sender)
		if sender.EA_enabled then
			sender.EA_enabled = false
			
			local dt = os.clock() - sender.EA_lastClockAttr
			sender.EA_lastClockAttr = os.clock()

			sender:Calculate(dt)
			
			local count = #sender.EA_connections
			for i = 1, count do
				sender:EA_ProcessItem(sender.EA_connections[i])
			end
			
			sender:UpdateGraphics(dt)

			sender.EA_enabled = true
		end
	end,
	
	-- #meth: Пересчет параметров объекта, если нужно. Реализуется программистом.
	-- #sender: Объект вызывающий метод.
	-- #dt: Время прошедшее с последнего вызова.
	Calculate = function(sender, dt)
	end,
	
	-- #meth: Обновление графики (перемецение, масштабирование, ...). Реализуется программистом.
	-- #sender: Объект вызывающий метод.
	-- #dt: Время прошедшее с последнего вызова.
	UpdateGraphics = function(sender, dt)
	end,	
	
	-- #meth: Генерация события.
	-- #sender: Объект вызывающий метод. Всегда добавляется в е (e.sender=sender).
	-- #event: Событие (строка).
	-- #e: Параметры передаваемые в обработчик. Если не таблица, то оборачивается таблицей, где args = e.
	-- #force: Выполнить данный метод не учитывая блокировку. Внимание использовать только при необходимости
	-- так как этот флаг может привести к зацикливанию!
	RaiseEvent = function(sender, event, e, force)	
		if sender.EA_enabled or force then
			if not force then sender.EA_enabled = false end
		
			for i = 1,#sender.EA_connections do
				if sender.EA_connections[i] then
					sender:EA_ProcessItem(sender.EA_connections[i], event, e)
				end
				if sender.isSkipEA == true then break end
			end
			sender.isSkipEA = false
			if not force then sender.EA_enabled = true end
		end
	end,
	
	-- Не выполнять остальных слотов.
	SkipEA = function(sender)
		sender.isSkipEA = true
	end,
	
	EA_ProcessItem = function(sender, item, senderEA, e)
		local iSender = item.sender
		local iSenderEA = item.senderEA
		local iReceiver = item.receiver
		local iReceiverEA = item.receiverEA
		
		-- элемен нужно обрабатывать.
		if iSender == sender and (iSenderEA == senderEA or senderEA == nil) and
			item.predicate(iSender, iReceiver, e) then
		
			-- Запаковка параметров в таблицу.
			local newE = {}
			if type(e) ~= 'table' then newE = {args = e} else newE = e end
			newE.sender = sender
		
			-- Если премник не определен, то просто выполним iReceiverEA(sender, e).
			if iReceiver == nil then
				item.receiverEA2(iSender, newE)
			else
				-- Послано событие, а не атрибут. Если нет такого поля, то считаем что это событие.
				local is1E = (iSender[iSenderEA] == nil)
				-- Принимается событие, а не атрибут.
				local is2E = type(iReceiver[iReceiverEA]) == 'function'
				
				if is1E then
					if is2E then
						-- Вызов обработчика.
						iReceiver[iReceiverEA](iReceiver, newE)
					else
						if newE[iReceiverEA] ~= nil then
							iReceiver[iReceiverEA] = newE[iReceiverEA]
							iReceiver:CalcEA()
						end					
					end
				else
					if is2E then
						newE[iSenderEA] = iSender[iSenderEA]
						-- Вызов обработчика.
						iReceiver[iReceiverEA](iReceiver, newE)
					else
						iReceiver[iReceiverEA] = iSender[iSenderEA]
						iReceiver:CalcEA()					
					end
				end
			end		
			
			-- Установим глобальный флаг, сообщая, что событие уже обработано. Однако можно поставить флаг и проброс не будет оставновлен.
			if (senderEA == 'EPush' or senderEA == 'EUse' or senderEA == 'ERelease')
				and sender.dontStopHandling ~= true then -- !!!!TEMPORARY
				Global.isEventHandled = true
			end
		end
	end,
	
	
	-- #meth: Привязать обработчик к событию.
	-- #sender: объект, вызывающий событие или меняющий атрибут (ссылка),
	-- #senderEA: генереруемое событие или изменяемый атрибут (строка),
	-- #receiver: объект, принимающий событие или изменение атрибута (ссылка),
	-- #receiverEA: обработчик вызываемый в принимающем объекте или изменяемый атрибут (строка).
	-- #predicate: предикат(строка с содержащая логическое выражение), при выполнении которого связь будет актвирована(вызван receiverEA).
	ConnectEA = function(sender, senderEA, receiver, receiverEA, predicate)
		if (receiver ~= nil) and (receiver[receiverEA] == nil) then
			print('Attribute or method ' .. receiverEA .. ' not found.')
		else
			-- если предикат не указан, то всегда возвращать true.
			if predicate == nil then predicate = 'true' end
			-- Событие могут быть указаны через ;
			local senderEAs = string.Split(senderEA, ';')	
			-- Для каждого события создаем свою запись.
			for k,v in pairs(senderEAs) do
				local item = {}
				item.sender = sender 
				item.senderEA = v --senderEA
				item.receiver = receiver
				item.receiverEA = receiverEA

				-- При отсутствии принимающего объекта.
				if receiver == nil then
					item.receiverEA2 = load('return function(sender, e) ' .. receiverEA .. ' end')()
				end
				-- Предикат.
				item.predicate = load('return function(sender, receiver, e) return ' .. predicate .. ' end')()
				
				table.insert(sender.EA_connections, item)
			end
		end
	end,
	
	-- #meth: Отвязать обработчик или атрибут от события или атрибута.
	-- #sender: объект, вызывающий событие или меняющий атрибут (ссылка),
	-- #senderEA: генереруемое событие или изменяемый атрибут (строка),
	-- #receiver: объект, принимающий событие или изменение атрибута (ссылка),
	-- #receiverEA: обработчик вызываемый в принимающем объекте или изменяемый атрибут (строка).
	DisconnectEA = function(sender, senderEA, receiver, receiverEA)
		local i = #sender.EA_connections
		local senderEAs = string.Split(senderEA, ';')
		
		while (i > 0) do
			local item = sender.EA_connections[i]
			if (sender == nil or item.sender == sender) and		
				(senderEA == nil or table.ContainsValue(senderEAs, item.senderEA)) and 
				(item.receiver == receiver or receiver == nil) and 
				(item.receiverEA == receiverEA or receiverEA == nil) then
				table.remove(sender.EA_connections, i)
			end
			i = i - 1
		end
	end,
	
	-- #meth: Привязать атрибут или метод данного объекта к атрибуту или методу объекта sender.
	-- #sender: объект, принимающий событие или изменение атрибута (ссылка),
	-- #senderEA: обработчик вызываемый в принимающем объекте или изменяемый атрибут (строка).
	-- #receiver: объект, вызывающий событие или меняющий атрибут (ссылка),
	-- #receiverEA: генереруемое событие или изменяемый атрибут (строка),
	-- #predicate: предикат(строка с содержащая логическое выражение), при выполнении которого связь будет актвирована(вызван receiverEA).
	AttachEA = function(receiver, receiverAttr, sender, senderAttr, predicate)
		sender:Connect(senderEA, receiver, receiverEA, predicate)
	end,
	
	-- #meth: Отвязать аттрибут или метод данного объекта от атрибута или метода объекта sender.
	-- #sender: объект, принимающий событие или изменение атрибута (ссылка),
	-- #senderEA: обработчик вызываемый в принимающем объекте или изменяемый атрибут (строка).
	-- #receiver: объект, вызывающий событие или меняющий атрибут (ссылка),
	-- #receiverEA: генереруемое событие или изменяемый атрибут (строка),
	-- #predicate: предикат(строка с содержащая логическое выражение), при выполнении которого связь будет актвирована(вызван receiverEA).
	DetachEA = function(receiver, receiverEA, sender, senderEA)
		sender:DisconnectAttr(senderEA, receiver, receiverEA)
	end,
	
	-- #meth: Удалить все связи объекта.
	-- #sender: Объект вызывающий метод.
	ClearEA = function(sender)
		sender.EA_connections = {}
	end
}
-- Синоним для EventAttributable.
Eventable = EventAttributable


-- #class: Класс манипулятор. Содержит ссылки на другие классы и функцию Initialize,
-- которая создает связку 3D объекта и класса-манипулятора. Поддерживает EventAttributable.
ClassObject = {
	-- Имя класса.
	className = "ClassObject",
	
	-- #prop: Массив имен объектов которые будут принимать события,
	eTarget = {},
	
	-- #prop: Массив имен объектов, которые будут выполнять команды.
	mTarget = {},
	
	pos = 0,
	rot = 0,
	scl = 0,
	vis = true,
	
	-- #prop: Активен ли объкт, будет ли он обрабатывать события клика, использования и т.д.
	enabled = true,

	states = {},
	stateIndex = 1,
	
	-- #prop: Строка со списком обработчиков используемых данным объектом событий, записанные через точку с запятой.
	-- События: use, click, drag, timer, push, release.
	events = '',
	
	-- Значение таймера при последнем вызове OnTimer.
	lastClock = 0,

	-- #prop: Событие вызываемое при клике по объекту. Синоним Push.
	Click = nil,
	
	-- #prop: Событие вызываемое при нажатии на объекту.
	Push = nil,

	-- #prop: Событие вызываемое при отпускании кнопки.
	Release = nil,
	
	-- #prop: Событие вызываемое при исползованиии объекта.
	Use = nil,
	
	-- Событие вызываемое при перетаскивании объекта.
	Drag = nil,
	
	-- Хеш-таблица, которая описывает связь между объектами и обработчиками. ('method = object')
	methodsHash = {},
	
	-- #meth: Конструктор объекта.
	-- #self: Имя создаваемого объекта, которое можно использовать в любом месте программы.
	-- #eTarget: Имя или масив имен 3D объектов, к которым привяжется манипулятор. Данный объект будет принимать события.
	-- #mTarget: Имя или масив имен 3D объектов, которые будут выполнять операции: поворачиваться, смещаться. Обычно используется,
	-- когда при клике на объект нужно манипулировать другим объектом.
	-- #events: Массив имен событий, которые нужно сгенерировать для данного объекта.
	Create = function (self, eTarget, mTarget, events)
		if (self == nil) then self = GetNextName("ClassObject_") end

		-- Если подана строка, а не таблица с именами, то считаем, что имена заданы в строке через ;
		if type(eTarget) == 'string' then eTarget = string.Split(eTarget, ';') end
		if type(mTarget) == 'string' then mTarget = string.Split(mTarget, ';') end
		if (mTarget == nil) then mTarget = eTarget end		

		if mTarget[#mTarget] == '' then table.remove(mTarget, #mTarget) end
		if eTarget[#eTarget] == '' then table.remove(eTarget, #eTarget) end
		
		local this = nil
		
		if ObjectExists(self) then
			load("thisOld = " .. self)()
			this = thisOld
		else	
			this = AbstractClassObject.Create(self)
			-- Наследование классов.
			this:InheritClass(ClassObject)
			this:InheritClass(EventAttributable)
			this:InheritClass(Animatable)			
			
			this.eTarget = eTarget
			this.mTarget = mTarget
			if events ~= nil then this.events = events end
			
			-- Создание событий.
			this:CreateEvents(events)
			
			-- Свойства для работы с положением, поворотом, масштабом.
			this.pos = ClassPosition.Create()
			this.pos.enabled = false

			this.rot = ClassRotation.Create()
			this.rot.enabled = false
			
			this.scl = ClassScale.Create()
			this.scl.enabled = false
			
			-- Видимость.
			vis = true
		end
		
		return this		
	end,
	
	-- Установить новый объект для обработки метода method данного объекта.
	RedirectMethod = function(sender, method, newName)
		sender.methodsHash[method] = Global.CreateObject(newName)
	end,

	-- #meth: Создает все обработчики всех объектов в данном объекте sender.
	-- #sender: Объект для которого нужно создать события.
	-- #events: Строка, содержащая перечисление имен событий через ";",  которые нужно сгенерировать.
	CreateEvents = function(sender, events)
		for i = 1, #sender.eTarget do
			sender:CreateObjectEvents(sender.eTarget[i], events)
		end
	end,
	
	-- #meth: Создать обработчики событий связанные с данным объектом и его манипулятором.
	-- #target: Объект для которого нужно создать события.
	-- #events: Строка, содержащая перечисление имен событий через ";",  которые нужно сгенерировать.
	-- #meth: Создать обработчики событий связанные с данным объектом и его манипулятором.
	CreateObjectEvents = function (sender, target, events)
		events = string.Split(events, ';')
		
		local s = ''
		
		local needsDrag = table.ContainsValue(events, 'drag')
		local needsPush = table.ContainsValue(events, 'push')
		local needsRelease = table.ContainsValue(events, 'release')
		local needsUse = table.ContainsValue(events, 'use')
		local needsTimer = table.ContainsValue(events, 'timer')
		
		if needsDrag then
			-- Обработчик движения.
			s = "function " .. target
			s = s .. "_DRAG(mouseX, mouseY, OldXmouse, OldYmouse)\n"
			s = s .. "\t" .. sender.self .. ".OnDrag(" .. sender.self .. ","
			s = s .. "mouseX - OldXmouse, mouseY - OldYmouse, 0)\n"
			s = s .. "end"
			load(s)()
		end

		if needsPush then
			-- Обработчик нажатия.
			s = "function " .. target .. "_PUSH()\n"
			s = s .. "\t" .. sender.self .. ".OnPush(" .. sender.self .. ")\n"
			s = s .. "end"
			load(s)()
		end
		
		if needsRelease then
			--Обработчик отпускания.
			s = "function " .. target .. "_RELEASE()\n"
			s = s .. "\t" .. sender.self .. ".OnRelease(" .. sender.self .. ")"
			s = s .. "end"
			load(s)()
		end

		if needsUse then
			-- Обработчик использования.
			s = "function " .. target .. "_USE()\n"
			s = s .. "\t" .. sender.self .. ".OnUse(" .. sender.self .. ")\n"
			s = s .. "end"
			load(s)()
		end

		-- Обработчик получеия курсора (наведенмия)  мыши.
		s = "function " .. target .. "_OVER()\n"
		--s = s .. "\t" .. sender.self .. "RaiseEvent('"..sender.self.."', 'EOver')\n"
		s = s .. "\t" .. sender.self .. ":RaiseEvent('EOver')\n"
		s = s .. "\tGlobal:RaiseEvent('EOver', {target = "..sender.self.."})\n"
		s = s .. "end"
		load(s)()

		-- Обработчик потери фокуса мыши.
		s = "function " .. target .. "_OUT()\n"
		--s = s .. "\t" .. sender.self .. ".RaiseEvent('"..sender.self.."', 'EOut')\n"
		s = s .. "\t" .. sender.self .. ":RaiseEvent('EOut')\n"
		s = s .. "\tGlobal:RaiseEvent('EOut', {target = "..sender.self.."})\n"
		s = s .. "end"
		load(s)()
		
		if needsTimer then
			AddFunctionToMainTimer(sender.self .. ".OnTimer", sender.self)
		end
	end,
	
	
	-- # meth: Движение оъекта (событие перетаскивание).
	-- События: EDrag
	OnDrag = function (sender, dx, dy, dz)
		if (sender.enabled) then
			if (sender.pos.enabled) then sender:Move(dx, dy, dz, false)	end
			if (sender.rot.enabled) then sender:Rotate(dx, dy, dz, false) end
			if (sender.scl.enabled) then sender:Scale(dx, dy, dz, false) end
			
			if (sender.Drag ~= nil) then sender.Drag(sender, dx, dy, dz) end
			
			sender:RaiseEvent("EDrag", {x=dx, y=dy, z=dz})
		end		
	end,	

	-- # meth: Нажатие на объект.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	OnPush = function (sender)
		if (sender.enabled) then
			if (sender.Push ~= nil) then sender.Push(sender) end
			sender:RaiseEvent("EPush", sender)
		end
	end,
	
	-- # meth: Отпускание кнопки на объекте.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	OnRelease = function (sender)
		if (sender.enabled) then
			if (sender.Release ~= nil) then sender.Release(sender) end
			sender:RaiseEvent("ERelease", sender)
		end
	end,
	
	-- Использование объекта.
	-- sender:  экземпляр класса ObjectClass, который вызывает событие.
	OnUse = function (sender)
		if (sender.enabled) then
			if (sender.Use ~= nil) then sender.Use(sender) end
			sender:RaiseEvent("EUse", sender)
		end
	end,	
	
	
	-- #meth: Переместить объект на указанный вектор.
	-- #sender: экземпляр класса ObjectClass, который вызывает событие.
	-- #dx: смещение по оси X.
	-- #dy: смещение по оси Y.
	-- #dz: смещение по оси Z. 
	-- #absolute: в абсолютных координатах.
	Move = function (sender, dx, dy, dz, absolute)
		if sender.methodsHash['move'] ~= nil then
			sender.methodsHash['move']:Move(dx, dy, dz, absolute)
		else
			if (absolute == true) then
				sender.pos:AChange(sender.mTarget, dx, dy, dz)
			else
				sender.pos:RChange(sender.mTarget, dx, dy, dz)
			end		
		end
	end,
	
	-- #meth: Повернуть объект на указанный вектор.
	-- #sender: экземпляр класса ObjectClass, который вызывает событие.
	-- #dx: поворот по оси X.
	-- #dy: поворот по оси Y.
	-- #dz: поворот по оси Z. 
	-- #absolute: в абсолютных координатах.
	Rotate = function (sender, dx, dy, dz, absolute)
		if sender.methodsHash['rotate'] ~= nil then
			sender.methodsHash['rotate']:Rotate(dx, dy, dz, absolute)
		else
			if (absolute == true) then
				sender.rot:AChange(sender.mTarget, dx, dy, dz)
			else
				sender.rot:RChange(sender.mTarget, dx, dy, dz)
			end
		end
	end,
	
	-- #meth: Промасштабировать объект на указанный вектор.
	-- #sender: экземпляр класса ObjectClass, который вызывает событие.
	-- #dx: масштабирование по оси X.
	-- #dy: масштабирование по оси Y.
	-- #dz: масштабирование по оси Z. 
	-- #absolute: в абсолютных координатах.
	Scale = function (sender, dx, dy, dz, absolute)
		if sender.methodsHash['scale'] ~= nil then
			sender.methodsHash['scale']:Scale(dx, dy, dz, absolute)
		else
			if (absolute == true) then
				sender.scl:AChange(sender.mTarget, dx, dy, dz)
			else
				sender.scl:RChange(sender.mTarget, dx, dy, dz)
			end
		end
	end,
	
	-- #meth: Отображение или скрытие объекта.
	-- #sender: экземпляр класса, который вызывает событие.	
	-- #visible: сделать объект видимым.
	Show = function(sender, visible)
		if (visible == nil) then 
			visible = true
		end
		for k,v in pairs(sender.mTarget) do
			RSetVisible(v, visible)
		end
		sender.vis = visible
		return sender
	end,

	-- Скрыть объект.
	Hide = function(sender)
		sender:Show(false)
	end,

	-- Активировать объект сделать доступным (enabled = true).
	Enable = function(sender, enabled)
		sender.enabled = (enabled ~= false)
	end,

	-- Деактивировать объект сделать недоступным (enabled = false).
	Disable = function(sender)
		sender.enabled = false	
	end,

	
	-- #meth: Установить цвет объекта.
	-- #sender: экземпляр класса, который вызывает событие.	
	-- #color: устанавливаемый цвет r, g, b, a. Значения от 0 до 1.
	SetColor = function(sender, r, g, b, a)
		for k,v in pairs(sender.mTarget) do
			RSetColor(v, r, g, b, a)
		end
	end,
	
	-- #meth: Сброс координат объекта. Сам объект не двигается, меняется только внутренние данные.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	ResetMatrix = function (sender)
		sender.pos:Reset(0)
		sender.rot:Reset(0)
		sender.scl:Reset(1)
	end,
	
	-- #meth: Прикрепить данный объект к другому объекту. Крепиться только первый манипулятор.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	-- #object: объект-манипулятор к которому будет привязан данный.
	-- #dPos: Смещение после привязки.
	-- #dAng: Поворот после привязки.	
	AttachTo = function(sender, object, dPos, dAng)
		-- Если подали объект. то берем из него первый манипулятор.
		if (type(object) == 'table') then object = object.mTarget[1] end		
			
		RConnectObjectTo(sender.mTarget[1], object)
		sender:ResetMatrix()

		if (dPos ~= nil) then
			sender:Move(dPos[1], dPos[2], dPos[3], true)
		end

		if (dAng ~= nil) then
			sender:Rotate(dAng[1], dAng[2], dAng[3], true)
		end
		
		sender:ResetMatrix()
		
		return sender
	end,
	

	-- Работа с состояниями(начало блока)
	
	StateToState = function(sender, state)
		if type(state) ~= 'table' then
			if sender.states[state] == nil then
				print('StateToState: State ' .. tostring(state) .. ' not found.')
			else
				state = sender.states[state]
			end
		end
		
		return state
	end,
	
	-- #meth: Установка следующего состояния из списка состояний данного объекта.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	SetNextState = function (sender)
		sender.SetState(sender, sender.stateIndex + 1)
	end,
	
	-- #meth: Установка предыдущего состояния из списка состояний данного объекта.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	SetPrevState = function (sender)	
		sender.SetState(sender, sender.stateIndex - 1)
	end,

	-- #meth: Установка выбранного состояния объекта, если состояние выходит за границы массива состояний, то устанавливается 1-е состояние.
	-- #sender:  экземпляр класса ObjectClass, который вызывает событие.
	-- #stateIndex: индекс нового состояния из массива состояний данного объекта.
	SetState = function (sender, state)
		Sender = sender
		--lastStateIndex = sender.stateIndex
		object = sender.target
		
		-- Получение нового состояния. (начало)
		local newState
		local newStateIndex
		
		-- Подано состояние.
		if (type(state) == 'table') then			
			newState = state			
		-- Подан индекс( номер или строковое имя) состояния объекта.
		else
			newStateIndex = state			
			-- Закольцевать изменение индекса состояния. Если больше минимума, то приравнять к 0, если меньше 0, то приравнять к последнему.
			if (type(state) == 'number') then
				if (state > #sender.states) then
					newStateIndex = 1
				elseif (state < 1) then
					newStateIndex = #sender.states
				end
			end
			-- Если список состояний не пуст.
			newState = sender.states[newStateIndex]
		end
		-- Получение нового состояния. (конец)	
		
		if (newState ~= nil) then
			-- Позволено ли входить в данное состояние.
			local allowed = true
			if (newState.allowed ~= nil) then
				if type(newState.allowed) == 'boolean' then
					allowed = newState.allowed
				else
					load("allowedGlob = " .. newState.allowed)()
					allowed = allowedGlob
				end
			end			
			
			if allowed then
			-- Изменение состояния объекта (Начало блока)
				sender.stateIndex = newStateIndex
			
				-- Изменение положения.
				if (newState.pos ~= nil) then
					local posV = Algebra.VectorToNumVector(newState.pos)
					sender.Move(sender, posV[1], posV[2], posV[3], true)
					sender.pos.enabled = newState.pos.enabled
				end

				-- Изменение поворота.
				if (newState.rot ~= nil) then
					local rotV = Algebra.VectorToNumVector(newState.rot)
					sender.Rotate(sender, rotV[1], rotV[2], rotV[3], true)
					sender.rot.enabled = newState.rot.enabled
				end
				
				-- Изменение масштаба.
				if (newState.scl ~= nil) then
					local sclV = Algebra.VectorToNumVector(newState.scl)
					sender.Scale(sender, sclV[1], sclV[2], sclV[3], true)
					sender.scl.enabled = newState.scl.enabled
				end

				-- Изменение видимости.
				if (newState.vis ~= nil) then sender.Show(sender, newState.vis) end
				
				-- Выполнение прикрепленной команды.
				if (newState.cmd ~= nil) then
					load('return function(sender) '..newState.cmd.. ' end')()(sender)
					--ExecuteCmd(newState.cmd)
				end
			-- Изменение состояния объекта (Конец блока) 
			else
				-- Выполнение прикрепленной команды.
				if (newState.cmdNotAllowed ~= nil) then 
					local func = load('return function(sender) '..newState.cmdNotAllowed.. 'end')()
					func(sender)
				end
			end
		end	
	end,

	SetMidState = function(sender, state1, state2, value)
		state1 = sender:StateToState(state1)
		state2 = sender:StateToState(state2)
		if value < 0 then value = 0 elseif value > 1 then value = 1 end
		
		if (state1.pos ~= nil) and (state2.pos ~= nil) then
			local pos = Algebra.GetMidValueV(0, 1, state1.pos, state2.pos, value)
			sender:Move(pos[1], pos[2], pos[3], true)
		end

		if (state1.rot ~= nil) and (state2.rot ~= nil) then
			local rot = Algebra.GetMidValueV(0, 1, state1.rot, state2.rot, value)
			sender:Rotate(rot[1], rot[2], rot[3], true)
		end
		
		if (state1.scl ~= nil) and (state2.scl ~= nil) then
			local scl = Algebra.GetMidValueV(0, 1, state1.scl, state2.scl, value)
			sender:Rotate(scl[1], scl[2], scl[3], true)
		end
		
		if (state1.col ~= nil) and (state2.col ~= nil) then
			local col = Algebra.GetMidValueV(0, 1, state1.col, state2.col, value)
			sender:SetColor(col[1], col[2], col[3], col[4])
		end
	end,

	-- Добавить текущее состояние в список.
	AddState = function(sender)
		table.insert(sender.states, 
			{
				pos = {sender.pos.x, sender.pos.y, sender.pos.z, enabled = sender.pos.enabled},
				rot = {sender.rot.x, sender.rot.y, sender.rot.z, enabled = sender.rot.enabled},
				scl = {sender.scl.x, sender.scl.y, sender.scl.z, enabled = sender.scl.enabled},
				--col = {sender.col.r, sender.col.g, sender.col.b, sender.col.a},
				vis = sender.vis
			})
			
			sender.stateIndex = #sender.states
	end,
	
	-- Удалить текущее состояние.
	-- sender:  экземпляр класса ObjectClass, который вызывает событие.
	DelState = function(sender)
		if (sender.stateIndex <= #sender.states) then
			table.remove(sender.states, sender.stateIndex)
			
			if (sender.stateIndex > 1) then
				sender.stateIndex = sender.stateIndex - 1
			end
		end
	end,
	
	-- Включить или выключить освещение
	EnableLightning = function(sender, enable)
		for k,v in pairs(sender.mTarget) do
			SetLightingEnable(v, enable)
		end
	end,
	
	-- Сместить объект sender к объекту target(по pivot point), а затем еще сместить на dPos.
	MoveTo = function(sender, target, dPos)
		local vis_sen = sender.vis
		sender:Show(true)
		local vis_tgt = nil
		local tgt = target -- Имя объекта к которому двигаться.
		
		if type(target) == 'table' then
			target:Show(true)
			vis_tgt = target.vis
			tgt = target.mTarget[1]
		end	
		
		local x1, y1, z1 = AGetPosition(sender.mTarget[1])
		local x2, y2, z2 = AGetPosition(tgt)
		sender:Move(x2 - x1, y2 - y1, z2 - z1)
			
		if (dPos ~= nil) then
			dPos = Algebra.VectorToNumVector(dPos)
			sender:Move(dPos[1], dPos[2], dPos[3])
		end
		
		sender:Show(vis_sen)
		if type(target) == 'table' then
			target:Show(vis_tgt)
		end
	end,
	
	-- Сместить объект (по pivot point) в положение (x,y,z). x может быть таблицей.
	MoveToPos = function(sender, x, y, z)
		local vis = sender.vis
		sender:Show(true)
		local x1, y1, z1 = AGetPosition(sender.mTarget[1])		
		if type(x) == "table" then
			x = Algebra.VectorToChrVector(x)
			z = x.z
			y = x.y
			x = x.x
		end
		sender:Move(x - x1, y - y1, z - z1)
		sender:Show(vis)
	end,	
	
	-- Клонирует все mTarget с новым именем и возвращает {eTargets - строка новых eTarget, mTargets - строка новых mTarget, new_name - созданный суффикс}.
	Clone3D = function(sender)
		local suffix = Global.GetUniqueName('_clone')
		local mTargets = ""
		for k,v in pairs(sender.mTarget) do
			mTargets = mTargets..v..suffix..';'
			Clone(v, v..suffix, suffix)
		end

		local eTargets = ""
		for k,v in pairs(sender.eTarget) do
			eTargets = eTargets..v..suffix..';'
		end

		return {eTargets, mTargets, suffix}
	end,

	-- Возвращает имена клонированных объектов {eTargets - строка новых eTarget, mTargets - строка новых mTarget}.
	CloneName = function(sender, suffix)
		local mTargets = ""
		for k,v in pairs(sender.mTarget) do
			mTargets = mTargets..v..suffix..';'
		end

		local eTargets = ""
		for k,v in pairs(sender.eTarget) do
			eTargets = eTargets..v..suffix..';'
		end

		return {eTargets, mTargets}
	end,

	-- Создать клон объекта через Global.CreateObject. 3D не клонируется.
	CreateClone = function(object, suffix)
		if object == nil then return nil end
		
		local cloned = object:CloneName(suffix)
		local new = Global.CreateObject(cloned[1], cloned[2])
		return new
	end
	
}