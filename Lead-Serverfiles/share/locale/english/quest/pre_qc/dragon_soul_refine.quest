quest dragon_soul_refine begin
	state start begin
		when 20001.chat.gameforge.dragon_soul_refine._010_npcChat with ds.is_qualified() != 0 begin
			say_title(mob_name(20001))
			say (gameforge.dragon_soul_refine._020_say)
			ds.open_refine_window()
		end
	end
end
