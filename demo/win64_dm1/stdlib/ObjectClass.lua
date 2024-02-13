-- # class: ����������� �����. ������� ��� ������ ���������� �� ���� self.
Inheritable = {
	self = nil,
	countStatic = 0,
	
	-- #meth: ����������� �������.
	-- #self: ��� ������������ �������, ������� ����� ������������ � ����� ����� ���������.
	Create = function(self)
		this = {}
		AppendObjectFields(this, Inheritable)
		Inheritable.InheritClass(this, Inheritable)
		if (self == nil) then self = GetNextName("stdlib_object") end
		this.self = self
		if (self ~= nil) then load(self .. " = this")() end
		return this
	end,
	
	-- #meth: ��������� ���� ������� source � ����� sender.
	-- #sender: ������, � �������� ����� ��������� ����.
	-- #source: ������, ���� �������� ����� �������������� ��� ����������.
	InheritClass = function(sender, source)
		if source == nil then
			print("AbstractClassObject: cannot inherit nil class.")
		else
			-- ���������� ���� (����������)
			-- �������� _base
			-- C������ setmetatable()
			AppendObjectFields(sender, source)
			if not sender._base then sender._base = {} end -- ������� ������ �����������.
			sender._base[source] = source -- ������� ����� � ������ ������������.
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
-- ������� ��� InheritClass.
AbstractClassObject = Inheritable
AbstractClassObject.Inherit = AbstractClassObject.InheritClass


-- # class: ����������� ����� ��������� ��������� �������������� ��������. ������ � �������� �������������� ��������� 3D ������� target.
-- ������� �������� �� �����, �� �������� ������� �������� � A.
AbstractClassMotion = {
	-- #prop: �������� X �������.
	x = 0,
	-- #prop: �������� Y �������.
	y = 0,
	-- #prop: �������� Z �������.
	z = 0,
	
	innerX  = 0, -- ���������� ��������  X �������.
	innerZ  = 0, -- ���������� ��������  Y �������.
	innerY  = 0, -- ���������� ��������  Z �������.

	-- #prop: ������������ �� ������ ����������� �������.
	enabled = false,

	-- # meth: ����������� �������.
	-- #self: ��� ������������ �������, ������� ����� ������������ � ����� ����� ���������.
	Create = function(self)
		local this = AbstractClassObject.Create(self)
		AppendObjectFields(this, AbstractClassMotion)
		return this
	end,
	
	-- # meth: �������� ��������� ������� �� dx, dy, dz.
	-- #sender: ��������� ������� ������.
	-- #target: ��������� ������.
	-- #dx: ��������� ��������� X.
	-- #dy: ��������� ��������� Y.
	-- #dz: ��������� ��������� Z. 
	RChange = function (sender, dx, dy, dz)
		sender.innerX = sender.innerX + dx
		sender.innerY = sender.innerY + dy
		sender.innerZ = sender.innerZ + dz
		sender.x = sender.innerX
		sender.y = sender.innerY
		sender.z = sender.innerZ
	end,

	-- # meth: ���������� ��������� ������� �������  x, y, z.
	-- #sender: ��������� ������� ������.
	-- #x: ����� ��������� �������  X.
	-- #y: ����� ��������� �������  Y.
	-- #z: ����� ��������� �������  Z. 
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
	
	-- ��������� ���� ��������� ������� ������� value.
	Reset = function(sender, value)
		sender.innerX = value
		sender.innerY = value
		sender.innerZ = value
		sender.x = value
		sender.y = value
		sender.z = value
	end
}

-- ����� ��������� �������. ������ � �������� ��������� 3D ������� target.
-- ������� �������� �� �����, �� �������� ������� �������� � A.
ClassPosition = {
	-- ����������� �������.
	-- self: ��� ������������ �������, ������� ����� ������������ � ����� ����� ���������.
	Create = function(self)
		local this = AbstractClassMotion.Create(self)
		this:InheritClass(ClassPosition)
		return this
	end,
	
	-- �������� ������ �� dx, dy, dz.
	-- sender: ��������� ������� ������.
	-- mTarget: ������ ��������� ��������.
	-- dx: �������� �� ��� X.
	-- dy: �������� �� ��� Y.
	-- dz: �������� �� ��� Z. 
	RChange = function (sender, mTarget, dx, dy, dz)
		V = {dx, dy, dz}
		sender:AChange(mTarget, sender.x + V[1], sender.y + V[2], sender.z + V[3])
		--AbstractClassMotion.RChange(sender, target, dx, dy, dz)
	end,
	
	-- �������� ������ � ����� x, y, z.
	-- sender: ��������� ������� ������.
	-- mTarget: ������ ��������� ��������.
	-- x: ����� ���������� X.
	-- y: ����� ���������� Y.
	-- z: ����� ���������� Z. 
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

-- ����� �������� �������. ������ � �������� ������� 3D ������� target.
-- ������� �������� �� �����, �� ������� ������� �������� � A.
ClassRotation = {
	-- ����������� �������.
	-- self: ��� ������������ �������, ������� ����� ������������ � ����� ����� ���������.
	Create = function(self)
		local this = AbstractClassMotion.Create(self)
		this:InheritClass(ClassRotation)
		return this
	end,
	
	-- ��������� ������ �� ��������� ������.
	-- sender: ��������� ������ ObjectClass, ������� �������� �������.
	-- mTarget: �������������� ������.
	-- dx: ������� �� ��� X.
	-- dy: ������� �� ��� Y.
	-- dz: ������� �� ��� Z. 
	RChange = function (sender, mTarget, dx, dy, dz)
		--V = {dx, dy, dz}
		V = {dx, dy, dz}
		sender:AChange(mTarget, sender.x + V[1], sender.y + V[2], sender.z + V[3])
	end,

	-- ���������� ���� �������� ������� ������� x, y, z.
	-- sender: ��������� ������� ������.
	-- mTarget: �������������� ������.
	-- x: ����� ���� X.
	-- y: ����� ���� Y.
	-- z: ����� ���� Z. 
	AChange = function (sender, mTarget, x, y, z)
		V = {x, y, z}
		V = Algebra.VectorSubVector(V, {sender.innerX, sender.innerY, sender.innerZ})
		for k,v in pairs(mTarget) do
			RSetRotate(v, V[1], V[2], V[3])
		end

		AbstractClassMotion.AChange(sender, x, y, z)
	end	
}

-- ����� �������� �������. ������ � �������� ������� 3D ������� target.
-- ������� �������� �� �����, �� ������� ������� �������� � A.
ClassScale = {
	-- ����������� �������.
	-- self: ��� ������������ �������, ������� ����� ������������ � ����� ����� ���������.
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
	
	-- �������������� ������ �� ��������� ������.
	-- sender: ��������� ������ ObjectClass, ������� �������� �������.
	-- dx: ������� �� ��� X.
	-- dy: ������� �� ��� Y.
	-- dz: ������� �� ��� Z. 
	RChange = function (sender, mTarget, dx, dy, dz)
		V = {dx, dy, dz}
		V = {sender.x*V[1], sender.y*V[2], sender.z*V[3]}
		sender:AChange(mTarget, V[1], V[2], V[3])
	end,

	-- ���������� ������� ������� �� ���� ������ x, y, z.
	-- sender: ��������� ������� ������.
	-- target: ��������� ������.
	-- x: ����� ������� X.
	-- y: ����� ������� Y.
	-- z: ����� ������� Z. 
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


--#class: ����� ������ ��� ������������� �������
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
	
	-- #meth: ����������� ������ � ������� ��������� �� ����� �����.
	-- #sender:  ������, ���������� �������,
	-- #state: ��������� � ������� ����� ������� (  {pos={x,y,z}, rot={x,y,z}, scl={x,y,z}, col={r,g,b,a}, vis=true/false} ).
	-- #length: ����� �������.
	-- #cmdEnd: ������ � ��������, ������� ����������� ����� ���������� ��������. � ��� �������� sender, state, timer.
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
		
		-- ��������������� ������� ����������.
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
 
--#class: ����� ���������� ���������� ������� Attributable � EventedObjectClass.
EventAttributable = 
{
	-- #prop: ������� ���������� 4 �������:
	-- sender - ������, ���������� ������� ��� �������� ������� (������),
	-- senderEA - ������������ ������� ��� ���������� ������� (������),
	-- receiver - ������, ����������� ������� ��� ��������� �������� (������),
	-- receiverEA - ���������� ���������� � ����������� ������� ��� ���������� ������� (������).
	-- predicate - ��������(������ � ���������� ���������� ���������), ��� ���������� �������� ����� ����� �����������(������ receiverEA).
	EA_connections = {},
	
	-- �������� ����� ������ CalcEA.
	EA_lastClockAttr = os.clock(),

	-- �������� �� ������ ��� ������� � ��������.
	EA_enabled = true,	
	
	Create = function ()
		local this = AbstractClassObject.Create(nil)
		this:InheritClass(EventAttributable)
		
		return this
	end,
	
	
	-- ���������� ���� �������� ��� ���������, ���� �������� ��� ��������� ���������� �� ������� ������� ������.
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
	
	-- #meth: �������� ���������� �������, ���� �����. ����������� �������������.
	-- #sender: ������ ���������� �����.
	-- #dt: ����� ��������� � ���������� ������.
	Calculate = function(sender, dt)
	end,
	
	-- #meth: ���������� ������� (�����������, ���������������, ...). ����������� �������������.
	-- #sender: ������ ���������� �����.
	-- #dt: ����� ��������� � ���������� ������.
	UpdateGraphics = function(sender, dt)
	end,	
	
	-- #meth: ��������� �������.
	-- #sender: ������ ���������� �����. ������ ����������� � � (e.sender=sender).
	-- #event: ������� (������).
	-- #e: ��������� ������������ � ����������. ���� �� �������, �� ������������� ��������, ��� args = e.
	-- #force: ��������� ������ ����� �� �������� ����������. �������� ������������ ������ ��� �������������
	-- ��� ��� ���� ���� ����� �������� � ������������!
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
	
	-- �� ��������� ��������� ������.
	SkipEA = function(sender)
		sender.isSkipEA = true
	end,
	
	EA_ProcessItem = function(sender, item, senderEA, e)
		local iSender = item.sender
		local iSenderEA = item.senderEA
		local iReceiver = item.receiver
		local iReceiverEA = item.receiverEA
		
		-- ������ ����� ������������.
		if iSender == sender and (iSenderEA == senderEA or senderEA == nil) and
			item.predicate(iSender, iReceiver, e) then
		
			-- ��������� ���������� � �������.
			local newE = {}
			if type(e) ~= 'table' then newE = {args = e} else newE = e end
			newE.sender = sender
		
			-- ���� ������� �� ���������, �� ������ �������� iReceiverEA(sender, e).
			if iReceiver == nil then
				item.receiverEA2(iSender, newE)
			else
				-- ������� �������, � �� �������. ���� ��� ������ ����, �� ������� ��� ��� �������.
				local is1E = (iSender[iSenderEA] == nil)
				-- ����������� �������, � �� �������.
				local is2E = type(iReceiver[iReceiverEA]) == 'function'
				
				if is1E then
					if is2E then
						-- ����� �����������.
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
						-- ����� �����������.
						iReceiver[iReceiverEA](iReceiver, newE)
					else
						iReceiver[iReceiverEA] = iSender[iSenderEA]
						iReceiver:CalcEA()					
					end
				end
			end		
			
			-- ��������� ���������� ����, �������, ��� ������� ��� ����������. ������ ����� ��������� ���� � ������� �� ����� �����������.
			if (senderEA == 'EPush' or senderEA == 'EUse' or senderEA == 'ERelease')
				and sender.dontStopHandling ~= true then -- !!!!TEMPORARY
				Global.isEventHandled = true
			end
		end
	end,
	
	
	-- #meth: ��������� ���������� � �������.
	-- #sender: ������, ���������� ������� ��� �������� ������� (������),
	-- #senderEA: ������������ ������� ��� ���������� ������� (������),
	-- #receiver: ������, ����������� ������� ��� ��������� �������� (������),
	-- #receiverEA: ���������� ���������� � ����������� ������� ��� ���������� ������� (������).
	-- #predicate: ��������(������ � ���������� ���������� ���������), ��� ���������� �������� ����� ����� �����������(������ receiverEA).
	ConnectEA = function(sender, senderEA, receiver, receiverEA, predicate)
		if (receiver ~= nil) and (receiver[receiverEA] == nil) then
			print('Attribute or method ' .. receiverEA .. ' not found.')
		else
			-- ���� �������� �� ������, �� ������ ���������� true.
			if predicate == nil then predicate = 'true' end
			-- ������� ����� ���� ������� ����� ;
			local senderEAs = string.Split(senderEA, ';')	
			-- ��� ������� ������� ������� ���� ������.
			for k,v in pairs(senderEAs) do
				local item = {}
				item.sender = sender 
				item.senderEA = v --senderEA
				item.receiver = receiver
				item.receiverEA = receiverEA

				-- ��� ���������� ������������ �������.
				if receiver == nil then
					item.receiverEA2 = load('return function(sender, e) ' .. receiverEA .. ' end')()
				end
				-- ��������.
				item.predicate = load('return function(sender, receiver, e) return ' .. predicate .. ' end')()
				
				table.insert(sender.EA_connections, item)
			end
		end
	end,
	
	-- #meth: �������� ���������� ��� ������� �� ������� ��� ��������.
	-- #sender: ������, ���������� ������� ��� �������� ������� (������),
	-- #senderEA: ������������ ������� ��� ���������� ������� (������),
	-- #receiver: ������, ����������� ������� ��� ��������� �������� (������),
	-- #receiverEA: ���������� ���������� � ����������� ������� ��� ���������� ������� (������).
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
	
	-- #meth: ��������� ������� ��� ����� ������� ������� � �������� ��� ������ ������� sender.
	-- #sender: ������, ����������� ������� ��� ��������� �������� (������),
	-- #senderEA: ���������� ���������� � ����������� ������� ��� ���������� ������� (������).
	-- #receiver: ������, ���������� ������� ��� �������� ������� (������),
	-- #receiverEA: ������������ ������� ��� ���������� ������� (������),
	-- #predicate: ��������(������ � ���������� ���������� ���������), ��� ���������� �������� ����� ����� �����������(������ receiverEA).
	AttachEA = function(receiver, receiverAttr, sender, senderAttr, predicate)
		sender:Connect(senderEA, receiver, receiverEA, predicate)
	end,
	
	-- #meth: �������� �������� ��� ����� ������� ������� �� �������� ��� ������ ������� sender.
	-- #sender: ������, ����������� ������� ��� ��������� �������� (������),
	-- #senderEA: ���������� ���������� � ����������� ������� ��� ���������� ������� (������).
	-- #receiver: ������, ���������� ������� ��� �������� ������� (������),
	-- #receiverEA: ������������ ������� ��� ���������� ������� (������),
	-- #predicate: ��������(������ � ���������� ���������� ���������), ��� ���������� �������� ����� ����� �����������(������ receiverEA).
	DetachEA = function(receiver, receiverEA, sender, senderEA)
		sender:DisconnectAttr(senderEA, receiver, receiverEA)
	end,
	
	-- #meth: ������� ��� ����� �������.
	-- #sender: ������ ���������� �����.
	ClearEA = function(sender)
		sender.EA_connections = {}
	end
}
-- ������� ��� EventAttributable.
Eventable = EventAttributable


-- #class: ����� �����������. �������� ������ �� ������ ������ � ������� Initialize,
-- ������� ������� ������ 3D ������� � ������-������������. ������������ EventAttributable.
ClassObject = {
	-- ��� ������.
	className = "ClassObject",
	
	-- #prop: ������ ���� �������� ������� ����� ��������� �������,
	eTarget = {},
	
	-- #prop: ������ ���� ��������, ������� ����� ��������� �������.
	mTarget = {},
	
	pos = 0,
	rot = 0,
	scl = 0,
	vis = true,
	
	-- #prop: ������� �� �����, ����� �� �� ������������ ������� �����, ������������� � �.�.
	enabled = true,

	states = {},
	stateIndex = 1,
	
	-- #prop: ������ �� ������� ������������ ������������ ������ �������� �������, ���������� ����� ����� � �������.
	-- �������: use, click, drag, timer, push, release.
	events = '',
	
	-- �������� ������� ��� ��������� ������ OnTimer.
	lastClock = 0,

	-- #prop: ������� ���������� ��� ����� �� �������. ������� Push.
	Click = nil,
	
	-- #prop: ������� ���������� ��� ������� �� �������.
	Push = nil,

	-- #prop: ������� ���������� ��� ���������� ������.
	Release = nil,
	
	-- #prop: ������� ���������� ��� ������������� �������.
	Use = nil,
	
	-- ������� ���������� ��� �������������� �������.
	Drag = nil,
	
	-- ���-�������, ������� ��������� ����� ����� ��������� � �������������. ('method = object')
	methodsHash = {},
	
	-- #meth: ����������� �������.
	-- #self: ��� ������������ �������, ������� ����� ������������ � ����� ����� ���������.
	-- #eTarget: ��� ��� ����� ���� 3D ��������, � ������� ���������� �����������. ������ ������ ����� ��������� �������.
	-- #mTarget: ��� ��� ����� ���� 3D ��������, ������� ����� ��������� ��������: ��������������, ���������. ������ ������������,
	-- ����� ��� ����� �� ������ ����� �������������� ������ ��������.
	-- #events: ������ ���� �������, ������� ����� ������������� ��� ������� �������.
	Create = function (self, eTarget, mTarget, events)
		if (self == nil) then self = GetNextName("ClassObject_") end

		-- ���� ������ ������, � �� ������� � �������, �� �������, ��� ����� ������ � ������ ����� ;
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
			-- ������������ �������.
			this:InheritClass(ClassObject)
			this:InheritClass(EventAttributable)
			this:InheritClass(Animatable)			
			
			this.eTarget = eTarget
			this.mTarget = mTarget
			if events ~= nil then this.events = events end
			
			-- �������� �������.
			this:CreateEvents(events)
			
			-- �������� ��� ������ � ����������, ���������, ���������.
			this.pos = ClassPosition.Create()
			this.pos.enabled = false

			this.rot = ClassRotation.Create()
			this.rot.enabled = false
			
			this.scl = ClassScale.Create()
			this.scl.enabled = false
			
			-- ���������.
			vis = true
		end
		
		return this		
	end,
	
	-- ���������� ����� ������ ��� ��������� ������ method ������� �������.
	RedirectMethod = function(sender, method, newName)
		sender.methodsHash[method] = Global.CreateObject(newName)
	end,

	-- #meth: ������� ��� ����������� ���� �������� � ������ ������� sender.
	-- #sender: ������ ��� �������� ����� ������� �������.
	-- #events: ������, ���������� ������������ ���� ������� ����� ";",  ������� ����� �������������.
	CreateEvents = function(sender, events)
		for i = 1, #sender.eTarget do
			sender:CreateObjectEvents(sender.eTarget[i], events)
		end
	end,
	
	-- #meth: ������� ����������� ������� ��������� � ������ �������� � ��� �������������.
	-- #target: ������ ��� �������� ����� ������� �������.
	-- #events: ������, ���������� ������������ ���� ������� ����� ";",  ������� ����� �������������.
	-- #meth: ������� ����������� ������� ��������� � ������ �������� � ��� �������������.
	CreateObjectEvents = function (sender, target, events)
		events = string.Split(events, ';')
		
		local s = ''
		
		local needsDrag = table.ContainsValue(events, 'drag')
		local needsPush = table.ContainsValue(events, 'push')
		local needsRelease = table.ContainsValue(events, 'release')
		local needsUse = table.ContainsValue(events, 'use')
		local needsTimer = table.ContainsValue(events, 'timer')
		
		if needsDrag then
			-- ���������� ��������.
			s = "function " .. target
			s = s .. "_DRAG(mouseX, mouseY, OldXmouse, OldYmouse)\n"
			s = s .. "\t" .. sender.self .. ".OnDrag(" .. sender.self .. ","
			s = s .. "mouseX - OldXmouse, mouseY - OldYmouse, 0)\n"
			s = s .. "end"
			load(s)()
		end

		if needsPush then
			-- ���������� �������.
			s = "function " .. target .. "_PUSH()\n"
			s = s .. "\t" .. sender.self .. ".OnPush(" .. sender.self .. ")\n"
			s = s .. "end"
			load(s)()
		end
		
		if needsRelease then
			--���������� ����������.
			s = "function " .. target .. "_RELEASE()\n"
			s = s .. "\t" .. sender.self .. ".OnRelease(" .. sender.self .. ")"
			s = s .. "end"
			load(s)()
		end

		if needsUse then
			-- ���������� �������������.
			s = "function " .. target .. "_USE()\n"
			s = s .. "\t" .. sender.self .. ".OnUse(" .. sender.self .. ")\n"
			s = s .. "end"
			load(s)()
		end

		-- ���������� �������� ������� (����������)  ����.
		s = "function " .. target .. "_OVER()\n"
		--s = s .. "\t" .. sender.self .. "RaiseEvent('"..sender.self.."', 'EOver')\n"
		s = s .. "\t" .. sender.self .. ":RaiseEvent('EOver')\n"
		s = s .. "\tGlobal:RaiseEvent('EOver', {target = "..sender.self.."})\n"
		s = s .. "end"
		load(s)()

		-- ���������� ������ ������ ����.
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
	
	
	-- # meth: �������� ������ (������� ��������������).
	-- �������: EDrag
	OnDrag = function (sender, dx, dy, dz)
		if (sender.enabled) then
			if (sender.pos.enabled) then sender:Move(dx, dy, dz, false)	end
			if (sender.rot.enabled) then sender:Rotate(dx, dy, dz, false) end
			if (sender.scl.enabled) then sender:Scale(dx, dy, dz, false) end
			
			if (sender.Drag ~= nil) then sender.Drag(sender, dx, dy, dz) end
			
			sender:RaiseEvent("EDrag", {x=dx, y=dy, z=dz})
		end		
	end,	

	-- # meth: ������� �� ������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	OnPush = function (sender)
		if (sender.enabled) then
			if (sender.Push ~= nil) then sender.Push(sender) end
			sender:RaiseEvent("EPush", sender)
		end
	end,
	
	-- # meth: ���������� ������ �� �������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	OnRelease = function (sender)
		if (sender.enabled) then
			if (sender.Release ~= nil) then sender.Release(sender) end
			sender:RaiseEvent("ERelease", sender)
		end
	end,
	
	-- ������������� �������.
	-- sender:  ��������� ������ ObjectClass, ������� �������� �������.
	OnUse = function (sender)
		if (sender.enabled) then
			if (sender.Use ~= nil) then sender.Use(sender) end
			sender:RaiseEvent("EUse", sender)
		end
	end,	
	
	
	-- #meth: ����������� ������ �� ��������� ������.
	-- #sender: ��������� ������ ObjectClass, ������� �������� �������.
	-- #dx: �������� �� ��� X.
	-- #dy: �������� �� ��� Y.
	-- #dz: �������� �� ��� Z. 
	-- #absolute: � ���������� �����������.
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
	
	-- #meth: ��������� ������ �� ��������� ������.
	-- #sender: ��������� ������ ObjectClass, ������� �������� �������.
	-- #dx: ������� �� ��� X.
	-- #dy: ������� �� ��� Y.
	-- #dz: ������� �� ��� Z. 
	-- #absolute: � ���������� �����������.
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
	
	-- #meth: ����������������� ������ �� ��������� ������.
	-- #sender: ��������� ������ ObjectClass, ������� �������� �������.
	-- #dx: ��������������� �� ��� X.
	-- #dy: ��������������� �� ��� Y.
	-- #dz: ��������������� �� ��� Z. 
	-- #absolute: � ���������� �����������.
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
	
	-- #meth: ����������� ��� ������� �������.
	-- #sender: ��������� ������, ������� �������� �������.	
	-- #visible: ������� ������ �������.
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

	-- ������ ������.
	Hide = function(sender)
		sender:Show(false)
	end,

	-- ������������ ������ ������� ��������� (enabled = true).
	Enable = function(sender, enabled)
		sender.enabled = (enabled ~= false)
	end,

	-- �������������� ������ ������� ����������� (enabled = false).
	Disable = function(sender)
		sender.enabled = false	
	end,

	
	-- #meth: ���������� ���� �������.
	-- #sender: ��������� ������, ������� �������� �������.	
	-- #color: ��������������� ���� r, g, b, a. �������� �� 0 �� 1.
	SetColor = function(sender, r, g, b, a)
		for k,v in pairs(sender.mTarget) do
			RSetColor(v, r, g, b, a)
		end
	end,
	
	-- #meth: ����� ��������� �������. ��� ������ �� ���������, �������� ������ ���������� ������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	ResetMatrix = function (sender)
		sender.pos:Reset(0)
		sender.rot:Reset(0)
		sender.scl:Reset(1)
	end,
	
	-- #meth: ���������� ������ ������ � ������� �������. ��������� ������ ������ �����������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	-- #object: ������-����������� � �������� ����� �������� ������.
	-- #dPos: �������� ����� ��������.
	-- #dAng: ������� ����� ��������.	
	AttachTo = function(sender, object, dPos, dAng)
		-- ���� ������ ������. �� ����� �� ���� ������ �����������.
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
	

	-- ������ � �����������(������ �����)
	
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
	
	-- #meth: ��������� ���������� ��������� �� ������ ��������� ������� �������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	SetNextState = function (sender)
		sender.SetState(sender, sender.stateIndex + 1)
	end,
	
	-- #meth: ��������� ����������� ��������� �� ������ ��������� ������� �������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	SetPrevState = function (sender)	
		sender.SetState(sender, sender.stateIndex - 1)
	end,

	-- #meth: ��������� ���������� ��������� �������, ���� ��������� ������� �� ������� ������� ���������, �� ��������������� 1-� ���������.
	-- #sender:  ��������� ������ ObjectClass, ������� �������� �������.
	-- #stateIndex: ������ ������ ��������� �� ������� ��������� ������� �������.
	SetState = function (sender, state)
		Sender = sender
		--lastStateIndex = sender.stateIndex
		object = sender.target
		
		-- ��������� ������ ���������. (������)
		local newState
		local newStateIndex
		
		-- ������ ���������.
		if (type(state) == 'table') then			
			newState = state			
		-- ����� ������( ����� ��� ��������� ���) ��������� �������.
		else
			newStateIndex = state			
			-- ������������ ��������� ������� ���������. ���� ������ ��������, �� ���������� � 0, ���� ������ 0, �� ���������� � ����������.
			if (type(state) == 'number') then
				if (state > #sender.states) then
					newStateIndex = 1
				elseif (state < 1) then
					newStateIndex = #sender.states
				end
			end
			-- ���� ������ ��������� �� ����.
			newState = sender.states[newStateIndex]
		end
		-- ��������� ������ ���������. (�����)	
		
		if (newState ~= nil) then
			-- ��������� �� ������� � ������ ���������.
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
			-- ��������� ��������� ������� (������ �����)
				sender.stateIndex = newStateIndex
			
				-- ��������� ���������.
				if (newState.pos ~= nil) then
					local posV = Algebra.VectorToNumVector(newState.pos)
					sender.Move(sender, posV[1], posV[2], posV[3], true)
					sender.pos.enabled = newState.pos.enabled
				end

				-- ��������� ��������.
				if (newState.rot ~= nil) then
					local rotV = Algebra.VectorToNumVector(newState.rot)
					sender.Rotate(sender, rotV[1], rotV[2], rotV[3], true)
					sender.rot.enabled = newState.rot.enabled
				end
				
				-- ��������� ��������.
				if (newState.scl ~= nil) then
					local sclV = Algebra.VectorToNumVector(newState.scl)
					sender.Scale(sender, sclV[1], sclV[2], sclV[3], true)
					sender.scl.enabled = newState.scl.enabled
				end

				-- ��������� ���������.
				if (newState.vis ~= nil) then sender.Show(sender, newState.vis) end
				
				-- ���������� ������������� �������.
				if (newState.cmd ~= nil) then
					load('return function(sender) '..newState.cmd.. ' end')()(sender)
					--ExecuteCmd(newState.cmd)
				end
			-- ��������� ��������� ������� (����� �����) 
			else
				-- ���������� ������������� �������.
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

	-- �������� ������� ��������� � ������.
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
	
	-- ������� ������� ���������.
	-- sender:  ��������� ������ ObjectClass, ������� �������� �������.
	DelState = function(sender)
		if (sender.stateIndex <= #sender.states) then
			table.remove(sender.states, sender.stateIndex)
			
			if (sender.stateIndex > 1) then
				sender.stateIndex = sender.stateIndex - 1
			end
		end
	end,
	
	-- �������� ��� ��������� ���������
	EnableLightning = function(sender, enable)
		for k,v in pairs(sender.mTarget) do
			SetLightingEnable(v, enable)
		end
	end,
	
	-- �������� ������ sender � ������� target(�� pivot point), � ����� ��� �������� �� dPos.
	MoveTo = function(sender, target, dPos)
		local vis_sen = sender.vis
		sender:Show(true)
		local vis_tgt = nil
		local tgt = target -- ��� ������� � �������� ���������.
		
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
	
	-- �������� ������ (�� pivot point) � ��������� (x,y,z). x ����� ���� ��������.
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
	
	-- ��������� ��� mTarget � ����� ������ � ���������� {eTargets - ������ ����� eTarget, mTargets - ������ ����� mTarget, new_name - ��������� �������}.
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

	-- ���������� ����� ������������� �������� {eTargets - ������ ����� eTarget, mTargets - ������ ����� mTarget}.
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

	-- ������� ���� ������� ����� Global.CreateObject. 3D �� �����������.
	CreateClone = function(object, suffix)
		if object == nil then return nil end
		
		local cloned = object:CloneName(suffix)
		local new = Global.CreateObject(cloned[1], cloned[2])
		return new
	end
	
}