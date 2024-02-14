--------------------------------
-- “ормоз :)
--------------------------------

tormoz = {}

function tormoz.init() 
	ruchka = c_volume:new("ruchka",0,40,50)
end

function tormoz.timer()
	if rti:is_change("ruchka") then
		local value = rti:get("ruchka")
		
		--—мещение ручки
		local delta = (value.value - value.old_value)/160
		
		RSetPosition("spring_box01",-delta,0,0)
		RSetPosition("spring_box02",-delta,0,0)
		RSetPosition("spring",-delta,0,0)
		
		--скалим пружину и ремень
		RSetScale("spring",(0.837+delta/2)/0.837,1,1)
		RSetScale("remen_scale",(2.47+delta/2)/2.47,1,1)
		
		--фиксируем изменени€ ручки
		rti:commit("ruchka")
		
		--считаем момент на торможе
		if bt_power:get()==true then
			strelka_tormoz_J:set(value.value)
		end
		
	end
end
