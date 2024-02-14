-- #class: Класс, реализующий редактирования объектов ClassObject.
Global = {
	-- # prop: Последнее время обращеия к таймеру.
	lastClock = os.clock(),
	
	-- HUD, оконное меню для помещения объектов.
	HUD = nil,
	
	-- Язык ввода текста для работы с EditText. [en | ru]
	lang = 'ru',

	-- Вид курсора для редактирования.
	editTextCursor = '_',
	
	timeSpeed = 1, -- Глобальная скорость таймеров. Работает для всех таймерова.
	
	-- # prop: Параметры по умолчанию.
	default = {
		events = 'use;push;release;drag'
	},
	
	-- Спиок элементов таймера. Первый параметр - функция, которая будет вызываться в таймере. Доступные праметры функции: dt.
	-- Второй параметр - промежуток времени через который вызывать функцию.
	-- Возвращает ссылку на созданный элемент в таблице.
	timerItems = {},	

	-- #meth: Инициализация объекта.
	Initialize = function(sender)
		AddFunctionToMainTimer("Global.OnTimer")

		Global.HUD = Global.CreateObject("HUD")
		
		AddTextObject("Global_InfoText", "", 1)
		-- Не работает потому что UserData тут еще не определен.
		Global.kWidth = UserData.WINDOW_WIDTH / 10000
		Global.kHeight = UserData.WINDOW_HEIGHT / 10000
		Global.kScale = math.min(Global.kWidth, Global.kHeight)

		Global.infoText = "Global_InfoText"
		RConnectObjectTo(Global.infoText, "HUD")
		SetTextPosition(Global.infoText, 40*Global.kScale, UserData.WINDOW_HEIGHT - 180*Global.kScale, -10)
		SetTextSize(Global.infoText, 200*Global.kScale)
		SetTextColor(Global.infoText, 0 ,0, 0, 1)
		Global.infoTextColor = {0, 0, 0}

		-- Позволим Global слать события.
		AppendObjectFields(Global, Eventable)
		if GetParent('Global_InfoText_plane') ~= '' then
			Global.infoTextPlane = Global.CreateObject('Global_InfoText_plane')
			Global.infoTextPlane:AttachTo('HUD')
			Global.infoTextPlane:Move(0, UserData.WINDOW_HEIGHT, -20, true)
		end
		math.randomseed(os.time())				
		
		-- Перевод кода кнопки в код русского символа.
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
		
		-- Сохраним старый обработчик нажатия клавиши.
		if KEY_PUSH and (KEY_PUSH ~= Global.KeyPush) then
			OLD_KEY_PUSH = KEY_PUSH	
		end
		-- Зацепим новый обработчик.
		KEY_PUSH = Global.KeyPush		
		
		-- Таймер для реализации Over и Out.
		Global.AddTimer('Global:TestOverOut()')
		Global.AddTimer('Global:OnEditTextTimer()', 0, 0.5)
	end,
	
	-- showtime - время в секундах после которого сообщение будет скрыто.
	-- effect - эффект пропадания текста (hide - скрыть, blind - плавно скрыть, blink - моргать с периодом 1)
	ShowMessage = function(message, isutf8, showtime, effect)
		if GetParent(Global.infoText) then
			local itc = Global.infoTextColor
			Global.ITSetOpacity(1)
			if Global.infoTextTimer then
				Global.DelTimer(Global.infoTextTimer) -- Удалим старый таймер, если он был.
				Global.infoTextTimer = nil
			end
			if message == nil then message = '' end -- Защита от выпадения при nil в тексте.
			message = tostring(message)
			-- Wrap текст
			local cnt = UserData.WINDOW_WIDTH / (120 * Global.kScale)
			if isutf8 then
				message = Global.Wrap(message, 2*cnt)
			else
				message = Global.Wrap(message, cnt)
			end
			
			-- Выводим текст.
			SetText(Global.infoText, message, (isutf8 == true))
			
			-- Если есть подложка, то вытянем ее как надо. Размер подложки 1*1 так что вытягивать просто по строкам.
			if Global.infoTextPlane then
				local tmp, lines = message:gsub("\n", '')
				Global.infoTextPlane:Show(message ~= '')
				Global.infoTextPlane:Scale(UserData.WINDOW_WIDTH, ((lines + 1)*200 + 40)*Global.kScale, 1, true)
			end
		end
		
		-- Эффект исчезания текста.
		if showtime then
			local cmd = ''		
			if effect == 'hide' then -- Скрытие.
				cmd = ''
			elseif effect == 'blind' then -- Плавное скрытие.
				cmd = 'Global.ITSetOpacity(1 - timer.elapsed/timer.len)'
			elseif effect == 'blink' then  -- Моргание
				cmd = 'Global.ITSetOpacity(math.ceil(math.modf(timer.elapsed, 2) - 1))'
			elseif effect == 'blink-blind' then  -- Плавное моргание
				cmd = 'Global.ITSetOpacity(math.abs(1 - 2*math.modf(timer.elapsed, 1)))'
			elseif effect == 'blink-blind1' then  -- Плавное моргание 0.5 - 1
				cmd = 'Global.ITSetOpacity(0.2 + 0.8*math.abs(1 - math.modf(timer.elapsed, 2)))'
			elseif effect == 'blink-blind2' then  --  Плавное моргание вспышка-затуханием
				cmd = 'Global.ITSetOpacity(1 - math.modf(timer.elapsed, 1))'
			elseif effect == 'blink-blind3' then  -- Плавное моргание проявление
				cmd = 'Global.ITSetOpacity(math.modf(timer.elapsed, 1))'
			end
			
			Global.infoTextTimer = Global.AddTimer(cmd, showtime, 0, 'Global.infoTextTimer = nil; Global.ShowMessage()')
		end
	end,
	
	
	-- Работа с таймером объекта Global и timerItems (Начало блока).	
	-- Метод крутиться в главном таймере программы и обрабатывает все пользовательские таймеры.
	OnTimer = function()
		-- Разные события выполняемые в начале каждого кадра.
		Global.isEventHandled = false

		-- Время прошедшее с полседнео вызова
		local dt = os.clock() - Global.lastClock
		Global.lastClock = os.clock()
		
		dt = dt*Global.timeSpeed

		-- Количество таймеров.
		local i = #Global.timerItems
		
		-- Обработка всех пользовательских таймеров.
		while i > 0 do
			-- Получение i-го таймера.
			local item = Global.timerItems[i]

			if (item ~= nil) and (not item.paused) then				
				-- Запоминаем последнюю отметку времени.
				local elapsedBef = item.elapsed;
				-- Увеличение занчения прошедшего времени.
				item.elapsed = item.elapsed + dt*item.speed
				
				-- Периодическое срабатывание.
				local needRemove = false;
				--local elapsedBef = item.elapsed;
				-- Обработка граничных условий.
				if ((item.elapsed <= 0) and (item.speed < 0)) or ((item.elapsed >= item.len) and (item.speed > 0)) then
					item.cycles = item.cycles - 1;
					
					-- Циклы еще остались.
					if ( (item.cycles > 0) or (item.cycles == nil) or (item.len == 0) ) then						
						if (item.reverse) then item.speed = -item.speed; end -- Реверс.
						
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

				-- Выполнение цикловой команды.
				item.dt = item.elapsed - elapsedBef;
				
				-- При периодическом срабатывании вызывать функцию только при разности времени больше period.
				if (item.period > 0) then
					item.period_dt = item.period_dt + dt -- Накопленное с прошлого периода время.
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
				else -- Постоянное выполнение.
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


	-- #meth: Добавить таймер.
	-- #func: Строка с функцией которая будет выполняться периодически. Получает на вход timer{dt, elapsed}.
	-- #len: Время работы данного таймера в секундах. Если 0, то неучитывается.
	-- #period: Период времени, через который будет срабатывать таймер в секундах. Если 0, то неучитывается.
	-- #cmdEnd: Команда, которая выполниться после завершения работы таймера.
	-- #params: Дополнительные параметеры таймера
	-- #returns: Объект таймер.
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
		
		if (params ~= nil) then AppendObjectFields(item, params) end -- Скопировать все параметры в объект.
		
		table.insert(Global.timerItems, item)
		return item
	end,
	
	-- #meth: Добавить таймер.
	AddTimer2 = function(func, params)
		return Global.AddTimer(func, 0, 0, nil, params)
	end,
	
	-- Добавить таймаут - команду которую нужно выполнить через len секунд.
	AddTimeout = function(len, cmdEnd)	
		Global.AddTimer(nil, len, 0, cmdEnd)
	end,	
	
	-- #meth: Удалить таймер.
	-- #timer: Удаляемый таймер. 
	DelTimer = function(timer)
		-- Поиск ключа дял данного таймера.
		local key = table.FindKeyByItem(Global.timerItems, timer)
		-- Удаление тамера.
		if (key ~= nil) then table.remove(Global.timerItems, key)
		else print('Global.DelTimerItem: Item not found') end
	end,
	-- Работа с таймером объекта Global и timerItems (Конец блока).
	
	-- # meth: Создать обертку для данного объекта. Объект автоматически добавляется в Global.objects.
	-- #manName: имя обертки. Если nil, то формируется автоматически как имя первого eTarget+'_man'.
	-- #eTarget: Имя или масив имен 3D объектов, к которому привяжется манипулятор. Данный объект будет принимать события.
	-- #mTarget: Имя или масив имен 3D объектов, который будет выполнять операции: поворачиваться, смещаться. Обычно используется,
	-- когда при клике на объект нужно манипулировать группой.
	-- #events: Перечень имен событий, которые нужно сгенерировать для данного объекта разделенных ";" (drag, click, use, timer).
	-- #returns: экземпляр обеъекта ClassObject.
	CreateObjectSeam = function(manName, eTarget, mTarget, events)
		-- Если была подана не строка, то вернуть то что было подано (считается, что это уже манипулятор).
		if type(eTarget) ~= 'string' then return eTarget end

		local objectName = ''

		-- Правка входного объекта.
		if type(eTarget) == 'string' then
			objectName = string.Split(eTarget, ';')[1]
			--eTarget = string.Split(eTarget, ';')
		--else
			--objectName = eTarget[1]			
		end
		
		-- Правка events.
		if events == nil then events = Global.default.events end
		
		-- Автосоздание имени манипулятора.
		if manName == nil then manName = objectName .. "_man" end
		
		-- Если такого манипулятора нет, то создать его.
		if ObjectExists(manName) == false then
			print("Global: created object - " .. manName)
			return ClassObject.Create(manName, eTarget, mTarget, events)
		else
			print("Global: object already exists - " .. manName)
			return GetObjectByName(manName)
		end
	end,

	-- #meth: Создать обертку для данного объекта. Объект автоматически добавляется в Global.objects.
	-- #eTarget: Имя или масив имен 3D объектов, к которому привяжется манипулятор. Данный объект будет принимать события.
	-- Если передан уже созданный объект, то он вернется без изменений.
	-- #mTarget: Имя или масив имен 3D объектов, который будет выполнять операции: поворачиваться, смещаться. Обычно используется,
	-- когда при клике на объект нужно манипулировать группой.
	-- #events: Перечень имен событий, которые нужно сгенерировать для данного объекта разделенных ";" (drag, click, use, timer).
	-- #returns: экземпляр обеъекта ClassObject.
	CreateObject = function(eTarget, mTarget, events)
		return Global.CreateObjectSeam(nil, eTarget, mTarget, events)
	end,
	
	-- Создать клон объекта через Global.CreateObject. 3D не клонируется.
	CreateClone = function(object, suffix)
		if object == nil then return nil end
		
		local cloned = object:CloneName(suffix)
		local new = Global.CreateObject(cloned[1], cloned[2])
		return new
	end,
	
	-- Получить уникальное имя с префиксом name.
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


	-- Функция для генерирования событий Over и Out для объектов.
	TestOverOut = function(sender)
		local curObj = UserData.CurrentObjectName
		if (curObj == 'not using hands') then curObj = nil end 
		
		local counter = 100
		
		-- Проверяем корректность текущего объекта.
		if (sender.lastObj ~= curObj) then
			-- Проверяем данные и вызываем OUT для объекта.
			if sender.lastObj and (tonumber(sender.lastObj) == nil) then
				local func = _G[sender.lastObj..'_OUT']			
				if func then func() end
				if _OUT then _OUT(sender.lastObj) end
				
				-- Пробрасываем событие. Если событие обработано в каком-то объекте, то к родителю оно не пойдет.
				if Global.isEventHandled ~= true then
					local obj = GetParent(sender.lastObj)
					while counter > 0 and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do			
						counter = counter - 1
						local func = _G[obj..'_OUT']
						if func then
							func()
							-- Если событие кем-то обработано, то не генерируем его выше.
							if Global.isEventHandled == true then break end
							
						end
						
						obj = GetParent(obj)												
					end
				end			
			end	
			
			sender.lastObj = curObj -- Запоминаем последний объект в фокусе.
			
			-- Проверяем данные и вызываем OVER для объекта.
			if sender.lastObj and (tonumber(sender.lastObj) == nil)then 
				local func = _G[sender.lastObj..'_OVER']
				if func then func() end
				if _OVER then _OVER(sender.lastObj) end

				-- Пробрасываем событие. Если событие обработано в каком-то объекте, то к родителю оно не пойдет.
				if Global.isEventHandled ~= true then
					local obj = GetParent(sender.lastObj)
					while counter > 0 and obj and obj ~= '' and (obj ~= 'Scene Root') and (obj ~= '__MAINCAMERA__') and (obj ~= 'HUD') do
						counter = counter - 1
						local func = _G[obj..'_OVER']
						if func then
							--Global.lastPushedObject = obj
							func()
							-- Если событие кем-то обработано, то не генерируем его выше.
							if Global.isEventHandled == true then break end
							
						end		
						obj = GetParent(obj)												
					end
				end
			end
		end		
	end,
	
	-- Режет текст на строки длиной не превосходящей limit.
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

	-- Делит текст на массив блоков по linesPerPage строк вблоке и signsPerLine символов.
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
	
	-- Редактировать (вводить с клавы)
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


	-- Обработчик нажатия кнопки на клавиатуре.
	KeyPush = function(key, c, a, shift, raw)
		-- Вызываем старый обработчик, если он был.
		if OLD_KEY_PUSH then
			OLD_KEY_PUSH(key, c, a, shift, raw)
		end
		
		local char = ''
		
		-- Включен русский язык.
		if Global.lang == 'ru' then
			if (key > 97) and (key < 122) then key = key - 32 end
			if  Global.keyCodes[key] then
				char = string.char(Global.keyCodes[key] + 32*(1 - shift))
			end
		-- Включен английский язык.
		else
			if (key > 64) and (key < 91) then			
				char = string.char(key + 32*(1 - shift))
			end		
		end
		
		if char == '' then
			-- Цифры.
			if (key > 48) and (key < 57) then
				char = key - 48
			-- Иные символы.
			elseif (key > 13) and (key < 65) then
				char = string.char(key)
			end
		end
		
		
		-- Если есть текст для редактирования, то реактируем его.
		if Global.editedText ~= nil then	
			Global.editedText.text = tostring(Global.editedText.text)
			
			-- Удалить 1 символ.
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
	
	-- Если объект создан, то возвращает его, иначе создает и возвращает.
	AsObject = function(name)
		-- Если была подана не строка, то вернуть то что было подано (считается, что это уже манипулятор).
		if type(name) ~= 'string' then
			return name
		else
			return _G[string.Split(name, ';')[1]..'_man'] or Global.CreateObject(name)
		end	
	end	
}