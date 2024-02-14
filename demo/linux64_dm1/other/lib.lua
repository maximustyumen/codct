--------------------------------
--  print_object(obj)
-- 
--  описание:   
--   выводит объект(таблицу) на печать в виде
--   параметр = значение
--
--  пример:
--
--	use.status = 0
--	use.windex = -1
--	use.sindex = -1
--	use.inuse = {id=2,nam="test", value=100}
--
--	print_object(use)
--
--  результат:
--
--      table: {
--          windex = -1
--          inuse: 
--              id = 2
--              nam = test
--              value = 100
--          sindex = -1
--          status = 0
--      }
--
--------------------------------------------------------
function print_object(obj)
if type(obj)=="table" then
	print("table: {");
	tprint(obj);
	print("}");
else
	print(obj)
end
end

-- функция выводит на консоль таблицу
function tprint (t, indent, done)
done = done or {}
indent = indent or 4  
local space = string.rep (" ", indent)
for key, value in pairs (t) do
	if type(value) == "table" and not done [value] then
		done [value] = true      
		print(space..tostring(key)..": ")
		tprint (value, indent + 4, done)    
	else
		print(space..tostring(key).." = "..tostring(value))
	end  
end
end
--------------------------------------------------------

function out(text)
	SetText("out",text)
end


error_time = 0

function error(text)
	SetText("error",text)
	SetTextColor("error",1,0,0,1)
	error_time=os.clock()
	
end


function error_timer()
	if error_time~=0 then
		local delta = os.clock()-error_time
		if delta>3 then
			SetTextColor("error",1,0,0, 4-delta)
			
			if delta>=4 then
				error_time=0			
				SetText("error","")
				
			end
		end
	end
end

isdebug = true

function debug(text)

	if isdebug==true then
		error("debug: "..text)
	end

end

---------------------------------------------------------------
function init_timer()
	tstart = os.clock()
	tstatus=false	

	SetTextPosition("msec",2.7-6.3,-121.85,31.046)
	SetTextRotate2("msec",3.14/2,1,0,0,-3.14,0,0,1,0,0,0,0)

	SetTextPosition("sec",3.7-6.3,-121.85,31.046)
	SetTextRotate2("sec",3.14/2,1,0,0,-3.14,0,0,1,0,0,0,0)

	SetTextPosition("div",4-6.3,-121.85,31.15)
	SetTextRotate2("div",3.14/2,1,0,0,-3.14,0,0,1,0,0,0,0)

	SetTextPosition("min",5-6.3,-121.85,31.046)
	SetTextRotate2("min",3.14/2,1,0,0,-3.14,0,0,1,0,0,0,0)
	SetText("tTimer", "Таймер: 00:00.00")
	
end

function timer_timer()
	if tstatus then
	local delta = os.clock()-tstart
	local msec  = math.mod(math.floor(delta*100),100)
	local sec   = math.mod(math.floor(delta),60)
	local min   = math.floor(delta/60)
	SetText("msec",string.format("%#02d", msec))
	SetText("sec", string.format("%#02d", sec))
	SetText("min", string.format("%#02d", min))
	SetText("tTimer", "Таймер: "..string.format("%#02d", min)..":"..string.format("%#02d", sec).."."..string.format("%#02d", msec))
	end
end

function start_timer()
	tstatus=true
	tstart=os.clock()
end

function stop_timer()
	tstatus=false
end

-- рекурсивная функция показывающая/скрывающая объекты
function object_visible(obj, value)
--[[
	if obj.inobject then
		for i,v in ipairs(obj.inobject) do 
			object_visible(v, value)
		end
	end
]]--
	RSetVisible(obj.nam, value)
end

function check_attach(obj)
	-- сравниваем на столкновения
	if obj.inobject then
		for i,v in ipairs(obj.inobject) do 
			if use.inuse.nam ~= v.nam then
				local est = RTestIntersects(use.inuse.nam,v.nam,0.75)
				if est then
					object_visible(use.inuse, false)
					SetText("out", "На объекте "..obj.fullnam.." недостаточно места");
	
					return false
				end
			end
		end
	end
	
	if obj.fnCheckAttach then
		if obj.fnCheckAttach(use.inuse)==false then
			object_visible(use.inuse, false)
			return false
		end
	end
	
	return true
end

-- прикрепить к объекту
function object_attach(obj)
	object_visible(use.inuse, true)


	RConnectObjectTo(use.inuse.nam, obj.action)	
	
	if use.inuse.fix_pivot then
		print("pivot")
		use.inuse.fix_pivot()
	end
	
	x,y,z=AGetPosition(use.inuse.nam)
	
	if obj.attach == 2 then
		-- прикрепление по горизонтали
		RSetPosition(use.inuse.nam,
			CurrentObjectIntersectsCoordWORLD_X-x, 0, 0)
			
		if obj.fnAttach then
			obj.fnAttach(use.inuse)
		end
			
		
		if check_attach(obj)==false then
			return false
		end
		
	elseif obj.attach==3 then
		-- прикрепление по вертикали
		RSetPosition(use.inuse.nam,
			0, CurrentObjectIntersectsCoordWORLD_Y-y, 0)	

		if obj.fnAttach then
			obj.fnAttach(use.inuse)
		end
			
	elseif obj.attach==4 then
		-- прикрепление по обоим направлениям
		RSetPosition(use.inuse.nam,
			CurrentObjectIntersectsCoordWORLD_X-x, CurrentObjectIntersectsCoordWORLD_Y-y, 0)	

		-- функция дополнительных действий при прикреплении
		if obj.fnAttach then
			obj.fnAttach(use.inuse)
		end
		
		-- проверка на прикрепление
		if check_attach(obj)==false then
			return false
		end



	else
		if obj.fnAttach then
			obj.fnAttach(use.inuse)
		end
	end
	
	use.inuse.parent=obj
	
	if obj.inobject == nil then
		obj.inobject = {}
	end
	table.insert(obj.inobject,use.inuse)
	
	return true
end

-- взять объект в руки
function object_get(obj)

	-- скрываем сам объект	
	object_visible(obj,false)

	if obj.parent ~= nil then
		--объект был прикреплен, убрать из списка
		
		--определяем позицию
		local findex = -1
		for i,v in ipairs(obj.parent.inobject) do 
			if v.id==obj.id then 
				findex=i
				break
			end
		end
		if findex==-1 then	SetText("out","Script error! findex = -1") end

		table.remove(obj.parent.inobject,findex)
		
		if obj.parent.fnDetach then obj.parent.fnDetach(obj) end
		
		obj.parent = nil
	end

	SetText("out","Вы взяли: "..obj.fullnam)
end


-- действие на объекте
function object_use(obj)

	if use.inuse == nil then
		--берем данный объект
		if obj.fixed and obj.fixed==true then
			-- данный объект нельзя брать в руки
		else
			object_get(obj)	
			use.inuse  = obj
		end
	else
		-- прикрепляем на данный объект
		if obj.attach == 0 then
			--на данный объект ничего прикрепить нельзя
			SetText("out","На данный объект ничего прикрепить нельзя")
		else
			local est=false
			--проверяем на доступность			
			if obj.allow then
				for i,v in ipairs(obj.allow) do 
					if use.inuse.id==v then
						--Вы можете прикрепить 
						SetText("out","Вы прикрепили: "..use.inuse.fullnam)
						est=true
						
						if object_attach(obj) then
							use.inuse  = nil
							break
						end
					end
				end		
			end
			if not est then 
				SetText("out", "На данный объект нельзя прикрепить: "..use.inuse.fullnam)
			end
		end		
	end
end
