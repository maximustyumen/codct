-------------------------------
-- Компьютер
--------------------------------

comp = {}

comp.params = {}
comp.params[1] = {min=30,max=35,value=35}
comp.params[2] = {min=60,max=65,value=60}
comp.params[3] = {min=40,max=50,value=45}
comp.params[4] = {min=20,max=25,value=25}
comp.params[5] = {min=90,max=160,value=90}
comp.params[6] = {min=6,max=9,value=9}
comp.params[7] = {min=10,max=50,value=50}
comp.params[8] = {min=20,max=60,value=20}
comp.params[9] = {min=20,max=80,value=20}
comp.params[10] = {min=1,max=3,value=1, text={"Шариковые","Цилиндрические","Конические"}}

comp.colorset = {}
comp.colorset[0] = {0.5,0.5,1}
comp.colorset[1] = {0,1,0}
comp.colorset[2] = {0,1,1}
comp.colorset[3] = {1,0,0}
comp.colorset[4] = {1,0,1}
comp.colorset[5] = {1,1,0}
comp.colorset[6] = {1,1,1}

comp.color=0

comp.show_text = function()
	for i=1,10 do
		SetText("comp_text_"..i, comp.params[i].value)
	end
	SetText("comp_text_10",comp.params[10].text[comp.params[10].value])
end

comp.hide_text = function()
	for i=1,10 do
		SetText("comp_text_"..i, "")
	end	
end

comp.init_text = function()
	local x,y,z = 0,0,0
	
	for i=1,10 do
		x,y,z = AGetPosition("button"..(i*2-1))
		AddTextObject("comp_text_"..i,"35",false)
		SetTextSize("comp_text_"..i, 0.5)
		SetTextColor("comp_text_"..i,0,0,0,255)
		SetTextFont("comp_text_"..i,getFileNameFromOther("gost_a_.ttf"))
		SetTextPosition("comp_text_"..i,x-1.4,y+0.01,z-0.16)
		SetTextRotate2("comp_text_"..i, math.pi, 0, 0, 1, -math.pi/2, 1, 0, 0)
		SetText("comp_text_"..i, "")
	end
	SetText("comp_text_10", "")
end

comp.change_param = function(index, step)
	if (comp.params[index].value + step <= comp.params[index].max ) and (comp.params[index].value + step>=comp.params[index].min) then
		comp.params[index].value = comp.params[index].value + step
		SetText("comp_text_"..index, comp.params[index].value)
	end
end

comp.init = function ()
	RChangeTexture("monik_grahpik",getFileNameFromOther("monik")) 
	reg_power = c_volume:new("reg_power",0,math.rad(270-45))

	strelka_w = c_strelka:new("strelka_w", 0, math.rad(270), 0, 3)
	strelka_p = c_strelka:new("strelka_p", 0, math.rad(270), 0, 6)
	strelka_J = c_strelka:new("strelka_J", 0, math.rad(270), 0, 15)

	strelka_tormoz = c_strelka:new("strelka_tormoz", 0, math.rad(270), 0, 1.2)
	strelka_tormoz_J = c_strelka:new("strelka_tormoz_J", 0, math.rad(270), 0, 45)

	bt_power = c_button:new("bt_power", false, true)
	
	button1 = c_button:new("button1")
	button2 = c_button:new("button2")
	button3 = c_button:new("button3")
	button4 = c_button:new("button4")
	button5 = c_button:new("button5")
	button6 = c_button:new("button6")
	button7 = c_button:new("button7")
	button8 = c_button:new("button8")
	button9 = c_button:new("button9")
	button10 = c_button:new("button10")
	button11 = c_button:new("button11")
	button12 = c_button:new("button12")
	button13 = c_button:new("button13")
	button14 = c_button:new("button14")
	button15 = c_button:new("button15")
	button16 = c_button:new("button16")
	button17 = c_button:new("button17")
	button18 = c_button:new("button18")
	button19 = c_button:new("button19")
	button20 = c_button:new("button20")
	
	button_ok = c_button:new("button_ok")
	buttom_cancel01 = c_button:new("buttom_cancel01")
	
	-- Инициализация текста на мониторе
	comp.init_text()

	button1.onClick = function()
		PlaySound("s6")
		comp.change_param(1,-1)
	end
	
	button2.onClick = function()
		PlaySound("s6")
		comp.change_param(1,1)
	end

	button3.onClick = function()
		PlaySound("s6")
		comp.change_param(2,-1)
	end
	
	button4.onClick = function()
		PlaySound("s6")
		comp.change_param(2,1)
	end

	button5.onClick = function()
		PlaySound("s6")
		comp.change_param(3,-1)
	end
	
	button6.onClick = function()
		PlaySound("s6")
		comp.change_param(3,1)
	end
	
	button7.onClick = function()
		PlaySound("s6")
		comp.change_param(4,-1)
	end
	
	button8.onClick = function()
		PlaySound("s6")
		comp.change_param(4,1)
	end
	
	button9.onClick = function()
		PlaySound("s6")
		comp.change_param(5,-1)
	end
	
	button10.onClick = function()
		PlaySound("s6")
		comp.change_param(5,1)
	end
	
	button11.onClick = function()
		PlaySound("s6")
		comp.change_param(6,-1)
	end
	
	button12.onClick = function()
		PlaySound("s6")
		comp.change_param(6,1)
	end
	
	button13.onClick = function()
		PlaySound("s6")
		comp.change_param(7,-1)
	end
	
	button14.onClick = function()
		PlaySound("s6")
		comp.change_param(7,1)
	end
	
	button15.onClick = function()
		PlaySound("s6")
		comp.change_param(8,-1)
	end
	
	button16.onClick = function()
		PlaySound("s6")
		comp.change_param(8,1)
	end
	
	button17.onClick = function()
		PlaySound("s6")
		comp.change_param(9,-1)
	end
	
	button18.onClick = function()
		PlaySound("s6")
		comp.change_param(9,1)
	end

	button19.onClick = function()
		PlaySound("s6")
		comp.change_param(10,-1)
		SetText("comp_text_10",comp.params[10].text[comp.params[10].value])
	end
	
	button20.onClick = function()
		PlaySound("s6")
		comp.change_param(10,1)
		SetText("comp_text_10",comp.params[10].text[comp.params[10].value])
	end
	
	buttom_fix = c_button:new("buttom_fix")
	buttom_cancel = c_button:new("buttom_cancel")
	
	rti:register("comp_mode",false, true)
	
	buttom_cancel.onClick = function()
		PlaySound("s6")	
		if oborudovanie_group:moving()==0 and stoleshniza:moving()==0 then
			rti:change("comp_mode", false)
			comp.color=comp.color+1
			if comp.color == 7 then comp.color = 0 end
		end
	end
	
	buttom_cancel01.onClick = function()
		PlaySound("s6")	
		print("RChangeTexture", getFileNameFromOther("monik"))

		RChangeTexture("monik_grahpik",getFileNameFromOther("monik")) 
	end

	button_ok.onClick = function()
		PlaySound("s6")	
		if oborudovanie_group:moving()==0 and stoleshniza:moving()==0 then
			rti:change("comp_mode", true)			
		end
	end
	
	buttom_fix.onClick = function()
		PlaySound("s6")
		comp.setPset(stanok.kpd, ruchka:get())
	end
	
	--buttom_cancel01.onClick()
	
	--RChangeTexture("monik_grahpik",getFileNameFromOther("monik")) 
	
	RSetRotate("monik_rotate",0,0,math.pi)

end

comp.shum = function(value, percent)
	return value - value*percent/2 + percent*math.random()
end

comp.setPset = function(kpd, nagruzka)
	
	--шум
	
	if kpd<0 or kpd==0 then 
		kpd=0 
	else 
		kpd = comp.shum(kpd, 0.025)
		if kpd>1 then kpd=1 end
	end
	
	
	
	
	local y = kpd*347 + (512-433)
	local x = (nagruzka/40)*417+48
	
	local cm = comp.colorset[comp.color]

	RTextureDraw("monik_grahpik",x, y,     cm[1],cm[2],cm[3],1,false)

	RTextureDraw("monik_grahpik",x+1, y+1, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-1, y-1, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x+1, y-1, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-1, y+1, cm[1],cm[2],cm[3],1,false)	

	RTextureDraw("monik_grahpik",x+2, y+2, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-2, y-2, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x+2, y-2, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-2, y+2, cm[1],cm[2],cm[3],1,false)	

	RTextureDraw("monik_grahpik",x+3, y+3, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-3, y-3, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x+3, y-3, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-3, y+3, cm[1],cm[2],cm[3],1,false)	

	RTextureDraw("monik_grahpik",x, y+1, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x, y-1, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x+1, y, cm[1],cm[2],cm[3],1,false)	
	RTextureDraw("monik_grahpik",x-1, y, cm[1],cm[2],cm[3],1,true)		
	
	for var = -2 ,2 do 
		for var2 = -2 ,2 do 
	RTextureDraw("monik_grahpik",x+var, y+var2, cm[1],cm[2],cm[3],1,true)	
	
	
		end
	end 
	
end

comp.timer = function()
	-- Изменился режим 
	if rti:is_change("comp_mode") then

		local value = rti:get("comp_mode")
		
		--Переворачиваем монитор
		RSetRotate("monik_rotate",0,0,math.pi)
		
		--Показываем\скрываем текст
		if value.value ~= true then
			oborudovanie_group:next()
			comp.show_text()
			bt_power:set(false)
			bt_power.disabled=true
		else
			stoleshniza:prev()
			comp.hide_text()
		end
		
		--фиксим измерения
		rti:commit("comp_mode")
	end
end