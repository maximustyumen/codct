-----------------------------------------------------
-- Апереодические звенья
-----------------------------------------------------

-- Опереодичское звено первого порядка
Zveno_a1 = {}
mt_Zveno_a1 = {}

function Zveno_a1:new(m_k, m_T)
	return setmetatable({output=0, k = m_k or 0, T = m_T or 0, start_t=0, t=0}, mt_Zveno_a1)
end

mt_Zveno_a1.__index = Zveno_a1

function Zveno_a1:print()
	print_object(self)
end

function Zveno_a1:start(m_k, m_T)
	if  m_k~=nil then self.k = m_k end
	if  m_T~=nil then self.T = m_T end
        self.start_t = os.clock()
        self.t = 0
	self.output=0
	self.started = true
end

function Zveno_a1:execute()
	if self.T==0 or self.paused or self.started==nil then return self.output end
	self.t=os.clock() - self.start_t
	self.output = self.k * (1 - math.exp(-self.t/self.T) )
	return self.output, self.t
end

function Zveno_a1:pause()
	if self.paused then
		local delta = os.clock()-self.paused
		self.start_t = self.start_t + delta
		self.paused = nil
	else
		self.paused = os.clock()
	end
end

function Zveno_a1:stop()
	self.started=nil
	self.start_t=0
	self.t=0
	self.output=0
end

-- Апереодическое звено первого порядка с накоплением
Zveno_a1_sum = {}
mt_Zveno_a1_sum = {}

function Zveno_a1_sum:new(m_k, m_T)
	return setmetatable({m_zveno = Zveno_a1:new(), input=0, output=0, k = m_k or 0, T = m_T or 0, start_t=0, t=0,old_t=0}, mt_Zveno_a1_sum)
end
mt_Zveno_a1_sum.__index = Zveno_a1_sum

function Zveno_a1_sum:start(m_k, m_T)
	if m_k~=nil then self.k = m_k end
	if m_T~=nil then self.T = m_T end
	self.start_t = os.clock()
	self.t = 0
	self.old_t = 0
	self.output = 0
	self.input = 0
	self.started = true
	self.m_zveno:start(self.k, self.T)
end

function Zveno_a1_sum:set_y(new_y)	
	self.k = new_y
	self.input = self.input + self.m_zveno:execute()
	self.m_zveno.k=new_y-self.input
	self.m_zveno:start()
end

function Zveno_a1_sum:execute()
	if self.started and self.paused==nil then 
		self.old_t = self.t		
		self.t = os.clock() - self.start_t
		self.delta_t = self.t-self.old_t
		self.output = self.input+self.m_zveno:execute()
	end
	
	
	
	return self.output, self.t
end

function Zveno_a1_sum:pause()
	if self.paused then
		local delta = os.clock()-self.paused
		self.start_t = self.start_t + delta
		self.paused = nil
	else
		self.paused = os.clock()
	end
	self.m_zveno:pause()
end

function Zveno_a1_sum:stop()
	self.start_t = 0
	self.t = 0
	self.output = 0
	self.input = 0
	self.started = nil
	self.m_zveno:stop()
end


function Zveno_a1_sum:print()
	print_object(self)
end

-- end Апереодические звенья
