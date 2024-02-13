-- #class: Класс преобразует объект так, чтобы его поведение соответствовало выбранной модели. Первым параметром
-- всех методов обычно является манипулятор, который будет поддерживать выбранную модель поведеия. Остальные параметры
-- зависят от конкретной модели поведения объекта.
LooksLike = {
	-- #meth: Модель лампочки. Объект имеет 2 состояния, отличающиеся поворотом. 
	-- Дополнительные методы Switch(on) включает или выключает лампочку, SwitchOn, SwitchOff, SwitchSwap - меняет состояние на противоположное.
	-- #man: Преобразуемый манипулятор.
	-- #dRot: Поворот во включенном состоянии.
	Lamp = function(man, dRot)
		man = Global.CreateObject(man)
		man.states = {}
		man.states['LampOn1'] = {rot = {0, 0, 0}}
		man.states['LampOn2'] = {rot = dRot}
		man.on = false		
		
		man.Switch = function(sender, on)
			if on then sender:SetState('LampOn2') else sender:SetState('LampOn1') end
			sender.on = on
		end

		man.SwitchOn = function(sender)
			sender:Switch(true)
		end

		man.SwitchOff = function(sender)
			sender:Switch(false)
		end

		man.SwitchSwap = function(sender)
			sender:Switch(not sender.on)
		end
		
		return man
	end,

	-- #meth: Кнопка. Объект имеет 2 состояния, отличающиеся смещением.
	-- Дополнительные методы: нет. Дополнительные события EClick - при переходе кнопки в нажатое состояние, 
	-- EClickBack - при возврате кнопки в исходное состояние.
	-- #man: Преобразуемый манипулятор.
	-- #dPos: Смещение во включенном состоянии.
	-- #predicate: [optional] Предикат, ограничивающийработу кнопки.
	Button = function(man, dPos, predicate)
		man = Global.CreateObject(man)
		
		man:InheritClass(Animatable)
		
		man.HButtonClick = function(sender, e)
			sender.enabled = false
			sender.states["LButton1"] = {pos = {0, 0, 0}, 
				cmdEnd = 'sender.enabled = true sender:RaiseEvent("EClickBack")'}
				
			sender.states["LButton2"] = {pos = dPos, 
				cmdEnd = 'sender:AnimateToState("LButton1", 0.1) sender:RaiseEvent("EClick")'}		
				
			sender:AnimateToState("LButton2", 0.2)
		end
		
		man:ConnectEA("EPush", man, "HButtonClick", predicate)
		
		return man
	end,
	
	-- #meth: Модель поворотной ручки. Объект имеет обрабатывает событие Motion и посылает сообщение EChange(value), 
	-- где value - значение на крутилке. Значение всегда от 0 до 1. Дополнительное поле value - текущее значение параметра.
	-- #man: Преобразуемый манипулятор.
	-- #dPos: [optional] Смещение в состоянии максимального поворота.
	-- #dRot: [optional] Поворот в состоянии максимального поворота.
	-- #speed: [optional] Скорость поворота ручки.
	-- #predicate: [optional] Предикат, ограничивающий работу объекта.
	Ruchka = function(man, dPos, dRot, speed, predicate)
		man = Global.CreateObject(man)
		
		if dPos == nil then dPos = {0, 0, 0} end
		if dRot == nil then dRot = {0, 0, 0} end
		if speed == nil then speed = 1 end
		
		man.states["RuchkaS1"] = {pos = {0, 0, 0}, rot = {0, 0, 0}}
		man.states["RuchkaS2"] = {pos = dPos, rot = dRot}
		man.value = 0
		man.speed = speed

		man.OnRuchkaRotate = function(sender, e)
			sender.value = sender.value + e.x * sender.speed
			if sender.value < 0 then sender.value = 0 elseif sender.value > 1 then sender.value = 1 end
			sender:SetMidState('RuchkaS1', 'RuchkaS2', sender.value)
			sender:RaiseEvent("EChange", {value = sender.value}, true)			
		end
		
		man:ConnectEA("EDrag", man, "OnRuchkaRotate", predicate)
		
		return man
	end,
	
	-- Модель вращающейся ручки.
	-- sender - объект сцены или stdlib. dPos, dRot - смещение и поворот в конечном положении. speed - коэффициент смещения при движении курсора вдоль реальных осей {kx,ky,kz}.
	-- predicate - условие, при котором ручка будет действовать.
	Ruchka2 = function(sender, dPos, dRot, speed, predicate)
		sender = Global.CreateObject(sender)
		if dPos == nil then dPos = {0, 0, 0} end
		if dRot == nil then dRot = {0, 0, 0} end
		if speed == nil then speed = {1, 0, 0} end

		sender.states["RuchkaS1"] = {pos = {0, 0, 0}, rot = {0, 0, 0}}
		sender.states["RuchkaS2"] = {pos = dPos, rot = dRot}
		sender.value = 0
		sender.speed = Algebra.VectorToChrVector(speed) -- {dx -> dv, dy -> dv, dz -> dv)
		
		sender.HDragMRuchka = function(sender, e)
			v = User:RealDPos(e.x, e.y)
			sender.value = sender.value + v.x * sender.speed.x + v.y * sender.speed.y + v.z * sender.speed.z
			if sender.value < 0 then sender.value = 0 elseif sender.value > 1 then sender.value = 1 end
			sender:SetMidState('RuchkaS1', 'RuchkaS2', sender.value)
			sender:RaiseEvent('EChange', {value = sender.value}, true)		
		end
		
		sender._SetValue = function(sender, value)
			sender.value = value
			if sender.value < 0 then sender.value = 0 elseif sender.value > 1 then sender.value = 1 end
			sender:SetMidState('RuchkaS1', 'RuchkaS2', sender.value)
			sender:RaiseEvent("EChange", {value = sender.value})	
		end
		
		sender:ConnectEA('EDrag', sender, 'HDragMRuchka', predicate)
		
		return sender
	end,	
	
	-- Стрелка.
	-- sender - объект сцены или ыевдши, dPos, dRot - смещение и поворот в конечном состоянии, range - пользовательский диапазон значений {min, max}, 
	-- speed[int] - скорость перехода стрелки до указанного значения при вызове SetValueA(value).
	Strelka = function(sender, dPos, dRot, range, speed, dreif)
		sender = Global.CreateObject(sender)
		if dPos == nil then dPos = {0, 0, 0} end
		if dRot == nil then dRot = {0, 0, 0} end
		if range == nil then range = {0, 1} end		
		if speed == nil then speed = 1 end
		if dreif == nil then dreif = 0 end
		
		sender.range = range -- Диапазон значений стрелки.
		sender.speed = speed -- Скорость перехода в значение.
		sender._value = 0 -- Текущее нормированное положение [0,1]
		sender._newvalue = 0 -- Целевое нормированное значение в диапазоне [0,1]	
		sender.dreif = dreif -- Дрейф (дребезжание стрелки) около значения.
		
		sender.states["StrelkaS1"] = {pos = {0, 0, 0}, rot = {0, 0, 0}}
		sender.states["StrelkaS2"] = {pos = dPos, rot = dRot}	
		
		sender.timer = Global.AddTimer(sender.self..':OnTimer(timer)')
		
		sender.OnTimer = function(sender, timer)		
			if (math.abs(sender._newvalue - sender._value) < 0.01) then
				sender:_SetValue(sender._newvalue)		
			elseif (sender._newvalue > sender._value) then
				sender:_SetValue(sender._value + timer.dt*sender.speed)
			elseif (sender._newvalue < sender._value) then
				sender:_SetValue(sender._value - timer.dt*sender.speed)
			end
		end
		
		-- Установить нормированное значение стрелки.
		sender._SetValue = function(sender, _value)
			if _value < 0 then _value = 0 end
			if _value > 1 then _value = 1 end
			
			-- Дрейф.	
			local dreifed = _value
			if (_value > 0) and (sender.dreif > 0)then			
				local dreif = Algebra.GetMidValueN(sender.range[1], sender.range[2], 0, 1, sender.dreif)
				dreifed = _value + math.random()*dreif - dreif/2
				if dreifed < 0 then dreifed = 0 elseif dreifed > 1 then dreifed = 1 end
			end
			
			sender._value = _value
			sender:SetMidState('StrelkaS1', 'StrelkaS2', dreifed) --sender._value)
			sender:RaiseEvent("EChange", {_value = sender._value, value = sender:Value()})
		end

		-- Установить значение из диапазона.
		sender.SetValue = function(sender, value)
			local _value = Algebra.GetMidValueN(sender.range[1], sender.range[2], 0, 1, value)
			sender._newvalue = _value
			sender:_SetValue(_value)
		end
		
		-- Плавно установить значение.
		sender.SetValueA = function(sender, value)
			local _value = Algebra.GetMidValueN(sender.range[1], sender.range[2], 0, 1, value)
			sender._newvalue = _value
		end

		-- Текущее значение из диапазона.
		sender.Value = function(sender)	
			return Algebra.GetMidValueN(0, 1, sender.range[1], sender.range[2], sender._value)
		end
		
		return sender		
	end,	
	
	-- # meth: Модель электронных часов. Объект имеет обрабатывает событие Timer.
	-- События: Start, Stop, Reset - Запустить, остаовить, сбросить.
	-- Дополнительное поле clock - текущее значение времени - секунд.
	-- #man: Преобразуемый манипулятор.
	-- #tip: Отображаемое время 1 - сек, 2 - мин:сек, 3 - час:мин:сек.
	Clock = function (man, tip, dights)
		man.tip = tip
		man.lastClock = os.clock()
		man.clock = 0
		man.enabled = false
		man.dights = dights		
		
		man.CreateEvents(man, 'timer')
		
		man.Timer = function(sender, dt)
			if sender.enabled then
				sender.clock = sender.clock + dt
				
				local sec = sender.clock

				local h = math.floor(sec / (60*60))
				sec = sec - h*60*60
				
				local m = math.floor(sec / 60)
				sec = sec - h*60*60
				
				local s =  math.floor(sec)
				
				local newText = StretchNum(s, 2)
				if sender.tip > 1 then newText = StretchNum(m, 2) .. ':' .. newText end
				if sender.tip > 2 then newText = StretchNum(h, 2) .. ':' .. newText end
				
				sender.SetText(sender, newText)
			end
		end
		
		man.Start = function(sender)
			sender.enabled = true
		end
		
		man.Stop = function(sender)
			sender.enabled = false
		end 
		
		man.Reset = function(sender)
			sender.clock = 0
		end 
	end,
	
	-- # meth: Модель электронных весов. Объект обрабатывает событие Timer.
	-- Метод SetValue(weight) - установить вес. События LLClick, LLUse. Свойство weight - вес, dislpayWeight - отображаемый вес.
	-- Добавляется текст (имя манипулятора_DisplayText).
	-- #man: Преобразуемый манипулятор.
	-- #precision: Число на которое производиться округление веса при отображении (10 - 1 знаск после запятой, 100 - 2, ...)
	Vesi = function (man, precision)
		man.className = "LL.Vesi"
		man.weight = 0	
		man.displyWeight = 0
		man.m_precision = precision
		man.m_dights = 5
		man.m_weightTextName = man.self .. "_DisplayText"
		man.holdingObject = nil
		
		man.LLClick = nil
		man.LLUse = nil
		
		-- Для того, чтобы не добавлять текст 2 раза.
		if ObjectExists("LooksLike."..man.m_weightTextName) then
		else
			AddTextObject(man.m_weightTextName, "", 0)
			load("LooksLike."..man.m_weightTextName.."=1")()
		end	
		
		SetTextSize(man.m_weightTextName, 0.8)
		SetTextColor(man.m_weightTextName, 0, 0, 0, 1)	
		
		xo, yo, zo = AGetPosition(man.mTarget[1])
		SetTextPosition(man.m_weightTextName, xo + 2, yo + 3.2, zo + 0.4)
		SetTextRotate(man.m_weightTextName, 3.14, 0, 0, 1)
		
		-- Мтеоды.
		man.SetWeight = function(sender, weight)
			sender.weight = weight
			sender.displyWeight = 
				StretchNum(math.floor(sender.weight * man.m_precision) / man.m_precision, sender.m_dights)
				
			SetText(sender.m_weightTextName, sender.displyWeight)
		end
	
		man.IncWeight = function(sender, weight)
			sender:SetWeight(sender.weight + weight)
		end
		
		man.DecWeight = function(sender, weight)
			sender:SetWeight(sender.weight - weight)
		end
	
		-- Создание событий.
		man.CreateEvents(man, 'click;use')
		
		-- Создание обработчиков событий.
		-- man.Timer = function(sender, dt)
			-- SetText(sender.m_weightTextName, sender.displyWeight)
		-- end
	
		-- Положить объект на весы.
		man.PutObject = function(sender, object)
			sender.holdingObject = object
			
			object:AttachTo(sender)
			object:Show(true)
			
			sender:IncWeight(object:GetWeight())
		end
		
		-- Снять объект с весов.
		man.TakeObject = function(sender, object)
			--object:AttachTo(sender)
			object:Show(false)

			local object = sender.holdingObject
			sender.holdingObject = nil
			sender:DecWeight(object:GetWeight())
			
			sender:RaiseEvent("ETakeObject", object)
		end
		
		-- Установка веса = 0.
		man:SetWeight(0)		
		
		return man			
	end,	
	
	-- # meth: Модель поверхности объема вещества. Обычно используется совместно со стаканом или подобным объектом.
	-- Поля: value - текущее значение времени - секунд.
	-- Методы: SetValue(value) - установить объем value за время procTime.
	-- События: EEmpty, EFull, EChange(value).
	-- #man: Преобразуемый манипулятор.
	-- #minV: Минимум значения.
	-- #maxV: Максимум значения.
	-- #minZ: Минимум Z.
	-- #maxZ: Максимум Z.
	-- #minScl: Масштаб в минимуме.
	-- #maxScl: Масштаб в максимуме.	
	VolumePlane = function (man, minV, maxV, minZ, maxZ, minScl, maxScl)
		man.className = "LL.VolumePlane"
		man.minV = minV
		man.maxV = maxV
		man.minZ = minZ
		man.maxZ = maxZ
		man.minScl = minScl
		man.maxScl = maxScl
		man.anim = nil
		man.value = 0
		
		man.SetValue = function(sender, value)
			-- Проверка выхода значения за диапазон стакана.
			if (value < sender.minV) then value = sender.minV
			elseif (value > sender.maxV) then value = sender.maxV end
			
			local lenV = (sender.maxV - sender.minV)
			local lenZ = (sender.maxZ - sender.minZ)
			local lenScl = (sender.maxScl - sender.minScl)
			
			local relV = value - sender.minV
			local relZ = relV * (lenZ / lenV)
			local relScl = relV * (lenScl / lenV)
			
			local absZ = relZ + minZ
			local absScl = relScl + minScl
			
			sender.value = value

			sender:Move(sender.pos.x, sender.pos.y, absZ, true)
			sender:Scale(absScl, absScl, absScl, true)
			
			if (value > sender.minV) then sender:Show(true) else sender:Show(fasle) end
			
			-- Событие изменения значения.
			sender:RaiseEvent("EChange", value)

			-- События граничных положений значения.
			if (value <= sender.minV) then sender:RaiseEvent("EEmpty")
			elseif (value >= sender.maxV) then sender:RaiseEvent("EFull") end
		end
		
		man:SetValue(0)

		return man
	end,
	
	-- # meth: Модель цилиндра объема вещества. Обычно используется совместно со стаканом или подобным объектом.
	-- Поля: value - текущее значение времени - секунд.
	-- Методы: SetValue(value) - установить объем value за время procTime.
	-- События: EEmpty, EFull, EChange(value).
	-- #man: Преобразуемый манипулятор.
	-- #minV: Минимум значения.
	-- #maxV: Максимум значения.
	-- #minScl: Масштаб в минимуме.
	-- #maxScl: Масштаб в максимуме.	
	VolumeCylinder = function (man, minV, maxV, minScl, maxScl)
		man.className = "LL.VolumeCylinder"
		man.minV = minV
		man.maxV = maxV
		man.minScl = minScl
		man.maxScl = maxScl
		man.anim = nil
		man.value = maxV
		
		man.SetValue = function(sender, invalue)
			-- Проверка выхода значения за диапазон стакана.
			local value = invalue
			if (value < sender.minV) then value = sender.minV
			elseif (value > sender.maxV) then value = sender.maxV end
			
			-- Делаем пересчет, только при реальном изменении значения.
			if (sender.value ~= value) then
				local sv = sender.value

				local lenV = (sender.maxV - sender.minV)
				local lenScl = (sender.maxScl - sender.minScl)
				
				local relV = value - sender.minV
				local relScl = relV * (lenScl / lenV)
				
				local absScl = minScl + relScl
				
				sender:Scale(1, 1, absScl, true)
				if (value > sender.minV) then sender:Show(true) else sender:Show(fasle) end
			
				sender.value = value
			
				-- Событие изменения значения.
				sender:RaiseEvent("EChange", value)				
			end
			
			-- События граничных положений значения.
			if (value <= sender.minV) then sender:RaiseEvent("EEmpty")
			elseif (value >= sender.maxV) then sender:RaiseEvent("EFull") end
		end
		
		man:SetValue(0)
		
		return man
	end,	
	
	-- # meth: Модель стакана. Для работы может использовать VolumePlane или VolumeCylinder.
	-- Обычно с ними и исапользуется. в качестве контейнера.
	-- Методы: SetValue(value) - установить значение объема равным vallue. GetValue - получить значение.
	-- SetVolumePlane(man[LL.VolumePlane]) - установить VolumePlane. Производиться перепривязка объекта.
	-- SetVolumeCylinder(man[LL.VolumeCylinder]) - установить VolumeCylinder. Производиться перепривязка объекта.
	-- AddValue(value), SubValue(value) - добавить\удбавить value.
	-- События: LLFull, LLEmpty(от дочерних объектов), EChange(value), EAddValue(value), ESubValue(value).
	-- #man: Преобразуемый манипулятор.
	-- #bodyWeight: Собственный вес стакана.
	-- #length:Длина стакана.
	-- #width: Ширина стакана.
	-- #height: Высота  стакана.
	Cap = function(man, bodyWeight, length, width, height)
		man.className = "LL.Cap"
		
		man.VolumeObject = nil
		man.bodyWeight = bodyWeight
		man.density = 1
		man.CreateEvents(man, 'click;use;drag')
		
		man.readyForSanding = false
		
		
		man.ClearChilds = function(sender)
			if sender.VolumeObject ~= nil then
				sender.VolumeObject:Disconnect("EFull", sender, "SeamEFull")
				sender.VolumeObject:Disconnect("EEmpty", sender, "SeamEEmpty")
				sender.VolumeObject:Disconnect("EChange", sender, "SeamEChange")
		
				sender.VolumeObject = nil
			end
		end
		
		man.SetVolumeObject = function(sender, man2)
			RConnectObjectTo(man2.mTarget[1], sender.mTarget[1])
			man2:ResetMatrix()
			
			if sender.VolumeObject ~= nil then
				local value = sender.VolumeObject.value
				sender.VolumeObject.value = sender.VolumeObject.maxV
				man2:SetValue(value)
			end

			sender:ClearChilds()
			
			man2:Connect("EFull", sender, "SeamEFull")
			man2:Connect("EEmpty", sender, "SeamEEmpty")
			man2:Connect("EChange", sender, "SeamEChange")

			sender.VolumeObject = man2
		end
		
		man.SeamEFull = function(sender)
			sender:RaiseEvent("EFull")
		end

		man.SeamEEmpty = function(sender)
			sender:RaiseEvent("EEmpty")
		end

		man.SeamEChange = function(sender, value)
			sender:RaiseEvent("EChange", value)
			sender:RaiseEvent("EChangeWeight", sender:GetWeight())
		end
		
		man.SetValue = function(sender, value)			
			if sender.VolumeObject ~= nil then sender.VolumeObject:SetValue(value) end
			
			sender:RaiseEvent("EChange", value)
			
			-- Событие окончаия высыпания, при опустошении стакана.
			if sender:GetValue() == 0 then
				sender:RaiseEvent("EWantsSanding", false)
			end
		end
		
		man.GetValue = function(sender)
			if sender.VolumeObject ~= nil then return sender.VolumeObject.value
			else return 0 
			end
		end

		man.GetWeight = function(sender)
			return sender.bodyWeight + sender:GetValue() * sender.density
		end
		
		man.IncValue = function(sender, value)
			sender:SetValue(sender:GetValue() + value)
			sender:RaiseEvent("EIncValue", value)
		end
		
		man.DecValue = function(sender, value)
			if sender:GetValue() < value then value = sender:GetValue() end
			
			sender:SetValue(sender:GetValue() - value)
			sender:RaiseEvent("EDecValue", value)
		end
		
		--#e: {target, dPos[3] dAng[3], axis[3], angRange[2], kz}
		man.BeginSanding = function(sender, e)
			sender.target = e.target
			sender.dPos = e.dPos
			sender.dAng = e.dAng
			sender.axis = e.axis
			sender.angRange = e.angRange
			sender.ang = 0
			sender.kz = e.kz
			sender.sandingSpeed = 0
			
			if sender.anim == nil then
				sender.anim = AnimationItemClass.CreateProcess(1, 
					"Animation.sender:DecValue(Animation.dt*Animation.sender.sandingSpeed)")
			--Animation.sender:DecValue(Animation.dt)
				sender.anim.sender = sender
			-- sender.anim.removeAfter = false
			-- sender.anim.cmdProcess = 
			--anim = AnimationItemClass.CreateEmpty(10)
				AnimationClass.AddAnimationItem(sender.anim)
			end
			
			sender:AttachTo(e.target, e.dPos, e.dAng)			

			sender:Connect("EDrag", sender, "MoveWhileSanding")
			sender:Connect("EEmpty", sender, "BreakSanding")
			sender:Connect("EDecValue", sender.target, "IncValue")
		end
		
		man.EndSanding = function(sender)
			-- sender:AttachTo(e.target, e.dPos)
			-- sender:Rotate(e.dAng[1], 0, 0, true)
			-- sender:Rotate(0, e.dAng[2], 0, true)
			-- sender:Rotate(0, 0, e.dAng[3], true)
			sender:RaiseEvent("EWantsSanding", false)
			if sender.anim ~= nil then
				sender.anim:Stop()
			end
					
			sender:Disconnect("EDrag", sender, "MoveWhileSanding")
			sender:Disconnect("EEmpty", sender, "BreakSanding")
			sender:Disconnect("EDecValue", sender.target, "IncValue")
		end
		
		man.MoveWhileSanding = function(sender, e)
			local newAng = sender.ang + e.dx
			if (newAng > sender.angRange[1]) and (newAng < sender.angRange[2]) then
				
				sender:Rotate(sender.axis[1]*newAng, sender.axis[2]*newAng, sender.axis[3]*newAng, true)
				--sender:Move(sender.dPos[1], sender.dPos[2], sender.dPos[3] + newAng*sender.kz, true)				
				sender:Move(0, 0, newAng*sender.kz, true)
				
				local midAng = sender.angRange[3]
				local oldAng = sender.ang				
				
				-- Событие запуска и остановки высыпания.
				if (midAng > oldAng) and (midAng < newAng) and (sender:GetValue() > 0) then 
					sender:RaiseEvent("EWantsSanding", true)
					sender.anim:Start()					
				elseif (midAng > newAng) and (midAng < oldAng) then 
					sender:RaiseEvent("EWantsSanding", false)
					sender.anim:Stop()
				end
				
				if newAng > midAng then 
					local k = (newAng - midAng) / (sender.angRange[2] - midAng)
					sender.sandingSpeed = sender:GetValue() * k / 2
				end
				
				--Global.ShowMessage(newAng)
				
				sender.ang = newAng
			end			
		end
		
		man.BreakSanding = function(sender)
			sender.anim:Stop()
		end
		
		return man
	end,

	
	-- # meth: Модель высыпания материала по дуге. Обычно используется со стаканом.
	-- Методы Start, Stop - запустить/останвить высыпание.
	-- Свойства: deltaPos - смещение объекта дуги относительно высыпающего объекта. 
	-- #man: Преобразуемый манипулятор.
	-- #startRot: Вектор начального поворота.
	-- #endRot: Вектор конечного поворота.
	-- #deltaPos: Смещение объекта дуги относительно высыпающего объекта.
	RadialSanding = function(man, startRot, endRot, deltaPos)
		man.anim = nil
		
		man.states['LL_RS1'] = {}
		man.states['LL_RS1']['rot'] = startRot
		man.states['LL_RS2'] = {}
		man.states['LL_RS2']['rot'] = endRot
		
		man.deltaPos = deltaPos	
		
		man.anim = AnimationItemClass.CreateObject(man, "LL_RS1", "LL_RS2", 0.1, false)
		AnimationClass.AddAnimationItem(man.anim) 
		--man.anim = AnimationClass.AddAnimationObject2(man, "LL_RS1", "LL_RS2", 0.1, false, false) 
	
		man.SetObject = function(sender, target)
			man.attached = target
			RConnectObjectTo(sender.mTarget[1], target.mTarget[1])
			RSetPosition(sender.mTarget[1], sender.deltaPos[1], sender.deltaPos[2], sender.deltaPos[3])		
	
			sender:ResetMatrix()
		end
	
		man.Start = function(sender)--, target
			sender.anim:Restart()
			sender:Show(true)
		end
		
		man.Stop = function(sender)
			sender:Show(false)
			sender.anim:Stop()
		end
		
		man.SetActive = function(sender, active)
			if active then
				sender.anim:Start()
				sender:Show(true)
			else
				sender:Stop()
			end
		end
		
		-- Углы поворота для анимации.
		man.SetAnimRange = function(sender, rotStart, rotEnd)
			sender.states['LL_RS1']['rot'] = CopyObject(rotStart)
			sender.states['LL_RS2']['rot'] = CopyObject(rotEnd)
		end
		
		-- Начальное смещение.
		man.SetDeltaPos = function(sender, pos)
			sender.deltaPos = CopyObject(pos)
		end
		
		man:Stop()
		
		return man
	end	
}

