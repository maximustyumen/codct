-------------------------------------
-- Станок
-------------------------------------
stanok = {}

stanok.init = function()

	oborudovanie_group = c_line_move:new("oborudovanie_group", 8, 0, 15)
	oborudovanie_group:set_axsis(0,0,-1)
	
	stoleshniza = c_line_move:new("stoleshniza", 20, 0, 21.2)
	stoleshniza:set_axsis(0,1,0)
	stoleshniza.sound = "s2"
	
	--перемещаем в ну
	oborudovanie_group:set(oborudovanie_group.max)
	stoleshniza:set(stoleshniza.max)
	
	stoleshniza.fix_index = function()
		PauseSound(stoleshniza.sound)
		stoleshniza.old_delta = 0

		if stoleshniza.position == stoleshniza.min then
			oborudovanie_group:prev()
		end
	end
	
	oborudovanie_group.fix_index = function()
		PauseSound(oborudovanie_group.sound)
		oborudovanie_group.old_delta = 0

		if oborudovanie_group.position ==oborudovanie_group.max then
			stoleshniza:next()
		elseif oborudovanie_group.position ==oborudovanie_group.min then
			bt_power.disabled = false
		end
	end
	
	
	stanok.kpd = 0
	stanok.start = os.clock()
	
	maslo = c_line:new("maslo", -0.2, 0.2, 0, false,0)
	maslo_scale = c_scale:new("maslo",0.1,2,1)
	el_dvig_group = c_volume:new("el_dvig_group", 0, math.rad(5),false)
	el_dvig_group:set_os(0,1,0)
	
	porshen_shtir = c_line:new("porshen_shtir",0,0.53,false,0)
end

stanok.timer = function()
	local power = rti:get("bt_power")
	
	--изменилось состояние питания
	if rti:is_change("bt_power") then
		
		PlaySound("s1")
		--PlaySound("s5")
		--питание выключено, все стопорим
		if power.value == false then
			PauseSound("s5")
			strelka_w:set(0)
			strelka_tormoz:set(0)
			strelka_p:set(0)
			strelka_J:set(0)
			strelka_tormoz_J:set(0)
			stanok.kpd = 0
		else
			PlaySound("s5")
			--синхронизируем ручку
			strelka_tormoz_J:set(ruchka:get())
		end
		
		
		rti:commit("bt_power")
	end
	
	--питание включено, выполняем все процессы
	if rti:get("bt_power").value==true then
	
		--индикация оборотов двигателя
		local w = (2.5*reg_power:get()/math.rad(270-45))+0.5
		strelka_w:set(w)	
		
		-- обороты тормоза		
		CWNumEdit1 = comp.params[3]
		CWNumEdit2 = comp.params[4]
		CWNumEdit3 = comp.params[1]
		CWNumEdit4 = comp.params[2]		
		CWNumEdit5 = comp.params[6]
		CWNumEdit6 = comp.params[5]
		CWNumEdit7 = comp.params[7]
		CWNumEdit8 = comp.params[8]
		CWNumEdit9 = comp.params[9]
		
		local w_t = w * CWNumEdit3.value / CWNumEdit4.value * CWNumEdit2.value / CWNumEdit1.value
		strelka_tormoz:set(w_t)
		

		Modul = (2 * CWNumEdit6.value) / (CWNumEdit3.value + CWNumEdit4.value)
		Vokr = Modul * CWNumEdit3.value * w * 1000 * 0.0000537		
		Ftr = 1.25 * (0.075 - 0.027 * math.log(1 + Vokr * 0.7))		
		KPDzub = 1 - 2.3 * Ftr * (1 / CWNumEdit3.value + 1 / CWNumEdit4.value)		
		Modul_ = (2 * CWNumEdit6.value) / (CWNumEdit2.value + CWNumEdit1.value)		
		Vokr_ = Modul_ * CWNumEdit2.value * w * 1000 * 0.0000547 * (CWNumEdit3.value / CWNumEdit4.value)
		Ftr_ = 1.25 * (0.075 - 0.027 * math.log(1 + Vokr_ * 0.7))
		KPDzub_ = 1 - 2.3 * Ftr * (1 / CWNumEdit2.value + 1 / CWNumEdit1.value)
		K_zub = 1 - (1 - KPDzub * KPDzub_) * (1 + (CWNumEdit5.value - 7.2) / 3)
		f_ = 0.002*comp.params[10].value
		f__ = f_ * (1 + (CWNumEdit5.value - 6) / 3)
		KPD_podsh = (1 - 1.7 * f__)
		
		i = w / w_t
		Pi_30 = math.pi / 30
		
		Pgidr = (0.04 * Modul * CWNumEdit3.value * CWNumEdit8.value) * (w * Pi_30*1000) * math.sqrt((2 * Vokr * CWNumEdit7.value * 0.000001) / (CWNumEdit3.value + CWNumEdit4.value))
		Pgidr_ = (0.04 * Modul_ * CWNumEdit2.value * CWNumEdit9.value) * (w * Pi_30*1000 * CWNumEdit3.value / CWNumEdit4.value) * math.sqrt((2 * Vokr_ * CWNumEdit7.value * 0.000001) / (CWNumEdit2.value + CWNumEdit1.value))

		P2 = ruchka:get() * math.pi / 30000 * w_t * 1000
		
		P1 = (Pgidr + Pgidr_) / 1000 + P2 / (K_zub * KPD_podsh * KPD_podsh* KPD_podsh)
		
		if P1 == 0 then
			KPD = 0
		else		
			KPD = P2 / P1
		end

		poteri = 1 - KPD
		
		poteri_ = poteri * (0.98 + math.random() * 0.04)
		poteri_3 = (P1 - P2) * 1000 * (0.99 + math.random() * 0.02) + (2 - math.random() * 4)

		KPD = 1 - poteri_
		gKPD = KPD * 100

		--print("KPD = "..KPD)
		stanok.kpd = KPD
		s_p = ruchka:get() * w_t * math.pi / 30 * KPD

		strelka_p:set(s_p)
		
		if w==0 then
			s_J = 0
		else
			s_J = 100*s_p / w * Pi_30
		end
		
		strelka_J:set(s_J)
		
	--питание выключено
	else
	
	end
	
	-- Отображение процессов	
	local dt = strelka_w.zveno.delta_t
	local rot_val1 = (strelka_w:get()*1000/60)*(2*math.pi)*dt
	local rot_val2 = ((strelka_w:get()*1.392/3.59)*1000/60)*(2*math.pi)*dt
	local rot_val3 = (strelka_tormoz:get()*1000/60)*(2*math.pi)*dt
	
	RSetRotate("val1_group",0,rot_val1,0)
	RSetRotate("val2_group",0,-rot_val2,0)
	RSetRotate("val3_group",0,rot_val3,0)
	
	--колеблем масло
	local a = (strelka_w:get())/3;
	local sin_a = math.sin((os.clock()-stanok.start)*15*a)*a/2 
	maslo:set(sin_a*maslo.min)
	--if (sin_a>=0 and maslo_scale.value<0) or (sin_a<=0 and maslo_scale.value>0) then
	--end
	maslo_scale:set(0.6-sin_a)
	
	--крутим двигатель
	local r = strelka_J:get()/15
	
	el_dvig_group:set(math.rad(5*r))
	porshen_shtir:set(0.53*r)
	
end