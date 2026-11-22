-- #################################################################
-- ##                                                             ##
-- ##                        LOCALE.LUA                           ##
-- ##                                                             ##
-- ## Background:                                                 ##
-- ##   This file provides Metin2 with one big array!             ##
-- ##   This array contains some configurations values and a lot  ##
-- ##   of strings to translate. Also every quest-file contains   ##
-- ##   string to translate.                                      ##
-- ##                                                             ##
-- ## Gameforge-Way-To-Do:                                        ##
-- ##   We extract everything we need to translate from this file ##
-- ##   into one big "Key = Value"-file. Simple key-value-files   ##
-- ##   can be managed by our localisation-tool.                  ##
-- ##   Also we extract every string out of every .quest-file and ##
-- ##   generate a lots of new keys. These keys are used in the   ##
-- ##   .quest-files.                                             ##
-- ##                                                             ##
-- ##   !! The key-value-file is called 'translate.lua'           ##
-- ##                                                             ##
-- ## The Advantage:                                              ##
-- ##   1) We can easily translate everything with our tool       ##
-- ##   2) We cant create syntax-errors in this file              ##
-- ##   3) We cant create syntax-errors in the quest-files        ##
-- ##   4) We can compare the quest-files between two languages   ##
-- ##      to find differences if something is broken in only     ##
-- ##      country.                                               ##
-- ##                                                             ##
-- #################################################################

locale = {}
locale.man_postfix = gameforge.locale.man_postfix
locale.woman_postfix = gameforge.locale.woman_postfix
locale.confirm = gameforge.locale.confirm
locale.reward = gameforge.locale.reward
locale.exp = gameforge.locale.exp
locale.count_prefix = gameforge.locale.count_prefix
locale.count_postfix = gameforge.locale.count_postfix
locale.level = gameforge.locale.level
locale.quest_word = gameforge.locale.quest_word
locale.complete = gameforge.locale.complete
locale.start = gameforge.locale.start
locale.chat = gameforge.locale.chat
locale.eliminate_on_begin = gameforge.locale.eliminate_on_begin
locale.eliminate_on_end = gameforge.locale.eliminate_on_end
locale.yes = gameforge.locale.yes
locale.no = gameforge.locale.no
locale.cancel = gameforge.locale.cancel
locale.gold = gameforge.locale.gold
locale.need_item_prefix = gameforge.locale.need_item_prefix
locale.need_item_postfix = gameforge.locale.need_item_postfix
locale.map_name = {
	[61] = gameforge.locale.map_name_1,
	[62] = gameforge.locale.map_name_2,
	[63] = gameforge.locale.map_name_3,
	[64] = gameforge.locale.map_name_4,
}
locale.empire_names = {
	[0] = gameforge.locale.empire_names_1,
	[1] = gameforge.locale.empire_names_2,
	[2] = gameforge.locale.empire_names_3,
	[3] = gameforge.locale.empire_names_4,
}
locale.item_drop_pct = gameforge.locale.item_drop_pct
locale.gold_drop_pct = gameforge.locale.gold_drop_pct
locale.tengold_drop_pct = gameforge.locale.tengold_drop_pct
locale.exp_pct = gameforge.locale.exp_pct
locale.show_guild_ranking_top = gameforge.locale.show_guild_ranking_top
locale.show_guild_ranking_around = gameforge.locale.show_guild_ranking_around
locale.show_guild_cur_score = gameforge.locale.show_guild_cur_score
locale.guild_around_rank_msg = gameforge.locale.guild_around_rank_msg
locale.guild_top_rank_msg = gameforge.locale.guild_top_rank_msg
locale.guild_rank_head = gameforge.locale.guild_rank_head
locale.guild_your_rank1 = gameforge.locale.guild_your_rank1
locale.guild_your_rank2 = gameforge.locale.guild_your_rank2
locale.guild_your_rank3 = gameforge.locale.guild_your_rank3
locale.deviltower_man_chat = gameforge.locale.deviltower_man_chat
locale.deviltower_man_say = gameforge.locale.deviltower_man_say
locale.deviltower_man_say_you_cant = gameforge.locale.deviltower_man_say_you_cant 
locale.deviltower_enter = gameforge.locale.deviltower_enter
locale.deviltower_no_enter = gameforge.locale.deviltower_no_enter
locale.forked_man_chat = gameforge.locale.forked_man_chat
locale.forked_man_say = gameforge.locale.forked_man_say
locale.forked_man_say_cant = gameforge.locale.forked_man_say_cant
locale.forked_enter = gameforge.locale.forked_enter
locale.forked_no_enter = gameforge.locale.forked_no_enter
locale.forked_open_gate = gameforge.locale.forked_open_gate
locale.forked_kill_boss = gameforge.locale.forked_kill_boss
locale.forked_condition = gameforge.locale.forked_condition
locale.forked_condition2 = gameforge.locale.forked_condition2
locale.forked_rule = gameforge.locale.forked_rule
locale.forked_rule_sungzi = gameforge.locale.forked_rule_sungzi
locale.login_notice = gameforge.locale.login_notice
locale.NOTICE_COLOR = color256(255, 230, 186)
locale.NORMAL_COLOR = color256(196, 196, 196)
locale.QUEST_TEMP_REWARD = locale.NOTICE_COLOR .. gameforge.locale.quest_temp_reward_1 .. locale.NORMAL_COLOR .. gameforge.locale.quest_temp_reward_2 

locale.yeonnahwan = {}
locale.yeonnahwan.start_level_begin = 99
locale.yeonnahwan.start_level_end = 99
locale.yeonnahwan.start_probability = 1
locale.yeonnahwan.end_level = 99
locale.yeonnahwan.kill_count = 1
locale.yeonnahwan.event_item = gameforge.locale.yeonnahwan.event_item
locale.yeonnahwan.find_yeonnahwan_label = gameforge.locale.yeonnahwan.find_yeonnahwan_label
locale.yeonnahwan.find_yeonnahwan_text = locale.NOTICE_COLOR .. gameforge.locale.yeonnahwan.find_yeonnahwan_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yeonnahwan.find_yeonnahwan_text_2
locale.yeonnahwan.find_eulduji_label = gameforge.locale.yeonnahwan.find_eulduji_label
locale.yeonnahwan.find_eulduji_text = locale.NOTICE_COLOR .. gameforge.locale.yeonnahwan.find_eulduji_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yeonnahwan.find_eulduji_text_2
locale.yeonnahwan.report_label = gameforge.locale.yeonnahwan.report_label
locale.yeonnahwan.report_text = gameforge.locale.yeonnahwan.report_text
locale.yeonnahwan.kill_unggwi_label = gameforge.locale.yeonnahwan.kill_unggwi_label
locale.yeonnahwan.kill_unggwi_text = locale.NOTICE_COLOR .. gameforge.locale.yeonnahwan.kill_unggwi_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yeonnahwan.kill_unggwi_text_2

locale.huanso = {}
locale.huanso.start_level_begin = 32
locale.huanso.start_level_end = 35
locale.huanso.start_probability = 3
locale.huanso.end_level = 38
locale.huanso.kill_count = 3
locale.huanso.event_item = 30102
locale.huanso.order_label = gameforge.locale.huanso.order_label
locale.huanso.order_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.order_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.order_text_2
locale.huanso.too_late = gameforge.locale.huanso.too_late
locale.huanso.find_label = gameforge.locale.huanso.find_label
locale.huanso.find_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.find_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.find_text_2
locale.huanso.kill_label = gameforge.locale.huanso.kill_label
locale.huanso.kill_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.kill_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.kill_text_2
locale.huanso.make_label = gameforge.locale.huanso.make_label
locale.huanso.make_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.make_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.make_text_2
locale.huanso.deliver_label = gameforge.locale.huanso.deliver_label
locale.huanso.deliver_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.deliver_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.deliver_text_2
locale.huanso.success_label = gameforge.locale.huanso.success_label
locale.huanso.success_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.success_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.success_text_2
locale.huanso.failure_label = gameforge.locale.huanso.failure_label
locale.huanso.failure_text = locale.NOTICE_COLOR .. gameforge.locale.huanso.failure_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.huanso.failure_text_2

locale.yuhwan = {}
locale.yuhwan.start_level_begin = 40
locale.yuhwan.start_level_end = 43
locale.yuhwan.start_probability = 3
locale.yuhwan.end_level = 45
locale.yuhwan.kill_count = 3
locale.yuhwan.event_item = 30102
locale.yuhwan.find_label = gameforge.locale.yuhwan.find_label
locale.yuhwan.find_text = locale.NOTICE_COLOR .. gameforge.locale.yuhwan.find_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yuhwan.find_text_2
locale.yuhwan.too_late = gameforge.locale.yuhwan.too_late
locale.yuhwan.kill_label = gameforge.locale.yuhwan.kill_label
locale.yuhwan.kill_text = locale.NOTICE_COLOR .. gameforge.locale.yuhwan.kill_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yuhwan.kill_text_2
locale.yuhwan.success_info_label = gameforge.locale.yuhwan.success_info_label
locale.yuhwan.success_info_text = locale.NOTICE_COLOR .. gameforge.locale.yuhwan.success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yuhwan.success_info_text_2
locale.yuhwan.failure_info_label = gameforge.locale.yuhwan.failure_info_label
locale.yuhwan.failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.yuhwan.failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.yuhwan.failure_info_text_2

locale.deokbae = {}
locale.deokbae.start_level_begin = 80
locale.deokbae.start_level_end = 80
locale.deokbae.start_probability = 3
locale.deokbae.end_level = 40
locale.deokbae.kill_count = 3
locale.deokbae.event_item = 30102
locale.deokbae.find_deokbae_label = gameforge.locale.deokbae.find_deokbae_label
locale.deokbae.find_deokbae_text = locale.NOTICE_COLOR .. gameforge.locale.deokbae.find_deokbae_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.deokbae.find_deokbae_text_2
locale.deokbae.too_late = gameforge.locale.deokbae.too_late
locale.deokbae.kill_label = gameforge.locale.deokbae.kill_label
locale.deokbae.kill_text = locale.NOTICE_COLOR .. gameforge.locale.deokbae.kill_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.deokbae.kill_text_2
locale.deokbae.success_info_label = gameforge.locale.deokbae.success_info_label
locale.deokbae.success_info_text = locale.NOTICE_COLOR .. gameforge.locale.deokbae.success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.deokbae.success_info_text_2
locale.deokbae.failure_info_label = gameforge.locale.deokbae.failure_info_label
locale.deokbae.failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.deokbae.failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.deokbae.failure_info_text_2

locale.bookworm = {}
locale.bookworm.start_level_begin = 28
locale.bookworm.start_level_end = 31
locale.bookworm.start_probability = 3
locale.bookworm.end_level = 34
locale.bookworm.kill_count = 3
locale.bookworm.talk = gameforge.locale.bookworm.talk
locale.bookworm.aranyeo_talk = gameforge.locale.bookworm.aranyeo_talk
locale.bookworm.etc_shop_talk_about_bookworm = gameforge.locale.bookworm.etc_shop_talk_about_bookworm
locale.bookworm.talk_about_milgyo_book = gameforge.locale.bookworm.talk_about_milgyo_book
locale.bookworm.do_you_find_the_book = gameforge.locale.bookworm.do_you_find_the_book
locale.bookworm.do_you_mean_this_book = gameforge.locale.bookworm.do_you_mean_this_book
locale.bookworm.START_say = gameforge.locale.bookworm.start_say
locale.bookworm.START_say2 = gameforge.locale.bookworm.start_say2
locale.bookworm.aranyeo_START_say = gameforge.locale.bookworm.aranyeo_start_say
locale.bookworm.aranyeo_START_say2 = gameforge.locale.bookworm.aranyeo_start_say2
locale.bookworm.etc_shop_START_say = gameforge.locale.bookworm.etc_shop_start_say
locale.bookworm.etc_shop_START_say2 = gameforge.locale.bookworm.etc_shop_start_say2
locale.bookworm.START_find_bookworm_label = gameforge.locale.bookworm.start_find_bookworm_label
locale.bookworm.START_find_bookworm_text = locale.NOTICE_COLOR .. gameforge.locale.bookworm.start_find_bookworm_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.bookworm.start_find_bookworm_text_2
locale.bookworm.ORDER_say1 = gameforge.locale.bookworm.order_say1
locale.bookworm.ORDER_say2 = gameforge.locale.bookworm.order_say2
locale.bookworm.ORDER_accept = gameforge.locale.bookworm.order_accept
locale.bookworm.ORDER_refuse = gameforge.locale.bookworm.order_refuse
locale.bookworm.ORDER_accept_answer = gameforge.locale.bookworm.order_accept_answer
locale.bookworm.ORDER_refuse_answer = gameforge.locale.bookworm.order_refuse_answer
locale.bookworm.ORDER_find_book_label = gameforge.locale.bookworm.order_find_book_label
locale.bookworm.ORDER_find_book_text = locale.NOTICE_COLOR .. gameforge.locale.bookworm.order_find_book_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.bookworm.order_find_book_text_2
locale.bookworm.ORDER_too_late = gameforge.locale.bookworm.order_too_late
locale.bookworm.ACCEPT_success_say = locale.NOTICE_COLOR .. gameforge.locale.bookworm.accept_success_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.bookworm.accept_success_say_2
locale.bookworm.ACCEPT_failure_say = locale.NOTICE_COLOR .. gameforge.locale.bookworm.accept_failure_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.bookworm.accept_failure_say_2
locale.bookworm.ACCEPT_ask = gameforge.locale.bookworm.accept_ask
locale.bookworm.ACCEPT_success_info_label = gameforge.locale.bookworm.accept_success_info_label
locale.bookworm.ACCEPT_success_info_text = locale.NOTICE_COLOR .. gameforge.locale.bookworm.accept_success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.bookworm.accept_success_info_text_2
locale.bookworm.ACCEPT_failure_info_label = gameforge.locale.bookworm.accept_failure_info_label
locale.bookworm.ACCEPT_failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.bookworm.accept_failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.bookworm.accept_failure_info_text_2
locale.bookworm.SUCCESS_thanks = gameforge.locale.bookworm.success_thanks
locale.bookworm.COMPLETE_say = gameforge.locale.bookworm.complete_say
locale.bookworm.aranyeo_COMPLETE_say = gameforge.locale.bookworm.aranyeo_complete_say
locale.bookworm.aranyeo_COMPLETE_say2 = gameforge.locale.bookworm.aranyeo_complete_say2

locale.old_pirate = {}
locale.old_pirate.start_level_begin = 48
locale.old_pirate.start_level_end = 51
locale.old_pirate.start_probability = 3
locale.old_pirate.end_level = 54
locale.old_pirate.kill_count = 3
locale.old_pirate.talk = gameforge.locale.old_pirate.talk
locale.old_pirate.baekgo_talk = gameforge.locale.old_pirate.baekgo_talk
locale.old_pirate.etc_shop_talk_about_old_pirate = gameforge.locale.old_pirate.etc_shop_talk_about_old_pirate
locale.old_pirate.talk_about_medicine = gameforge.locale.old_pirate.talk_about_medicine
locale.old_pirate.do_you_get_the_medicine = gameforge.locale.old_pirate.do_you_get_the_medicine
locale.old_pirate.do_you_mean_this_medicine = gameforge.locale.old_pirate.do_you_mean_this_medicine
locale.old_pirate.baekgo_talk_about_old_pirate = gameforge.locale.old_pirate.baekgo_talk_about_old_pirate
locale.old_pirate.baekgo_do_you_get_the_virus = gameforge.locale.old_pirate.baekgo_do_you_get_the_virus
locale.old_pirate.baekgo_iv_got_the_virus = gameforge.locale.old_pirate.baekgo_iv_got_the_virus
locale.old_pirate.START_say = gameforge.locale.old_pirate.start_say
locale.old_pirate.etc_shop_START_say = gameforge.locale.old_pirate.etc_shop_start_say
locale.old_pirate.etc_shop_START_say2 = gameforge.locale.old_pirate.etc_shop_start_say2
locale.old_pirate.START_find_old_pirate_label = gameforge.locale.old_pirate.start_find_old_pirate_label
locale.old_pirate.START_find_old_pirate_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.start_find_old_pirate_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.start_find_old_pirate_text_2
locale.old_pirate.ORDER_say1 = gameforge.locale.old_pirate.order_say1
locale.old_pirate.ORDER_say2 = gameforge.locale.old_pirate.order_say2
locale.old_pirate.ORDER_accept = gameforge.locale.old_pirate.order_accept
locale.old_pirate.ORDER_refuse = gameforge.locale.old_pirate.order_refuse
locale.old_pirate.ORDER_accept_answer = gameforge.locale.old_pirate.order_accept_answer
locale.old_pirate.ORDER_refuse_answer = gameforge.locale.old_pirate.order_refuse_answer
locale.old_pirate.ORDER_find_medicine_label = gameforge.locale.old_pirate.order_find_medicine_label
locale.old_pirate.ORDER_find_medicine_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.order_find_medicine_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.order_find_medicine_text_2
locale.old_pirate.ORDER_too_late = gameforge.locale.old_pirate.order_too_late
locale.old_pirate.ACCEPT_success_say = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.accept_success_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.accept_success_say_2
locale.old_pirate.ACCEPT_failure_say = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.accept_failure_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.accept_failure_say_2
locale.old_pirate.ACCEPT_ask = gameforge.locale.old_pirate.accept_ask
locale.old_pirate.ACCEPT_success_info_label = gameforge.locale.old_pirate.accept_success_info_label
locale.old_pirate.ACCEPT_success_info_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.accept_success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.accept_success_info_text_2
locale.old_pirate.ACCEPT_failure_info_label = gameforge.locale.old_pirate.accept_failure_info_label
locale.old_pirate.ACCEPT_failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.accept_failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.accept_failure_info_text_2
locale.old_pirate.BAEKGO_START_say = gameforge.locale.old_pirate.baekgo_start_say
locale.old_pirate.BAEKGO_START_say2 = gameforge.locale.old_pirate.baekgo_start_say2
locale.old_pirate.BAEKGO_START_say3 = gameforge.locale.old_pirate.baekgo_start_say3
locale.old_pirate.BAEKGO_ORDER_find_virus_label = gameforge.locale.old_pirate.baekgo_order_find_virus_label
locale.old_pirate.BAEKGO_ORDER_find_virus_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.baekgo_order_find_virus_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.baekgo_order_find_virus_text_2
locale.old_pirate.BAEKGO_ORDER_too_late = gameforge.locale.old_pirate.baekgo_order_too_late
locale.old_pirate.BAEKGO_ACCEPT_success_say = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.baekgo_accept_success_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.baekgo_accept_success_say_2
locale.old_pirate.BAEKGO_ACCEPT_failure_say = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.baekgo_accept_failure_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.baekgo_accept_failure_say_2
locale.old_pirate.BAEKGO_ACCEPT_ask = gameforge.locale.old_pirate.baekgo_accept_ask
locale.old_pirate.BAEKGO_ACCEPT_success_info_label = gameforge.locale.old_pirate.baekgo_accept_success_info_label
locale.old_pirate.BAEKGO_ACCEPT_success_info_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.baekgo_accept_success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.baekgo_accept_success_info_text_2
locale.old_pirate.BAEKGO_ACCEPT_failure_info_label = gameforge.locale.old_pirate.baekgo_accept_failure_info_label
locale.old_pirate.BAEKGO_ACCEPT_failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.old_pirate.baekgo_accept_failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.old_pirate.baekgo_accept_failure_info_text_2
locale.old_pirate.BAEKGO_SUCCESS_say = gameforge.locale.old_pirate.baekgo_success_say
locale.old_pirate.SUCCESS_thanks = gameforge.locale.old_pirate.success_thanks
locale.old_pirate.COMPLETE_say = gameforge.locale.old_pirate.complete_say

locale.stamina_food = {}
locale.stamina_food.butcher = {}
locale.stamina_food.food_manager = {}
locale.stamina_food.start_probability = 3
locale.stamina_food.start_level_begin = 90
locale.stamina_food.start_level_end = 90
locale.stamina_food.end_level = 59
locale.stamina_food.kill_count = 3
locale.stamina_food.butcher.talk = gameforge.locale.stamina_food.butcher.talk
locale.stamina_food.food_manager.talk = gameforge.locale.stamina_food.food_manager.talk
locale.stamina_food.food_manager.talk_about_greeenfrog_soup = gameforge.locale.stamina_food.food_manager.talk_about_greeenfrog_soup
locale.stamina_food.i_dont_have_the_greeenfrog_meat_yet = gameforge.locale.stamina_food.i_dont_have_the_greeenfrog_meat_yet
locale.stamina_food.i_have_the_greenfrog_meat = gameforge.locale.stamina_food.i_have_the_greenfrog_meat
locale.stamina_food.butcher.START_say = gameforge.locale.stamina_food.butcher.start_say
locale.stamina_food.butcher.START_say2 = gameforge.locale.stamina_food.butcher.start_say2
locale.stamina_food.food_manager.ORDER_say = gameforge.locale.stamina_food.food_manager.order_say
locale.stamina_food.food_manager.ORDER_say2 = gameforge.locale.stamina_food.food_manager.order_say2
locale.stamina_food.ORDER_accept = gameforge.locale.stamina_food.order_accept
locale.stamina_food.ORDER_refuse = gameforge.locale.stamina_food.order_refuse
locale.stamina_food.ORDER_accept_answer = gameforge.locale.stamina_food.order_accept_answer
locale.stamina_food.ORDER_refuse_answer = gameforge.locale.stamina_food.order_refuse_answer
locale.stamina_food.ORDER_get_greenfrog_general_meat_label = gameforge.locale.stamina_food.order_get_greenfrog_general_meat_label
locale.stamina_food.ORDER_get_greenfrog_general_meat_text = locale.NOTICE_COLOR .. gameforge.locale.stamina_food.order_get_greenfrog_general_meat_text
locale.stamina_food.ORDER_too_late = gameforge.locale.stamina_food.order_too_late
locale.stamina_food.ACCEPT_success_say = locale.NOTICE_COLOR .. gameforge.locale.stamina_food.accept_success_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.stamina_food.accept_success_say_2
locale.stamina_food.ACCEPT_failure_say = locale.NOTICE_COLOR .. gameforge.locale.stamina_food.accept_failure_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.stamina_food.accept_failure_say_2
locale.stamina_food.ACCEPT_ask = gameforge.locale.stamina_food.accept_ask
locale.stamina_food.ACCEPT_success_info_label = gameforge.locale.stamina_food.accept_success_info_label
locale.stamina_food.ACCEPT_success_info_text = locale.NOTICE_COLOR .. gameforge.locale.stamina_food.accept_success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.stamina_food.accept_success_info_text_2
locale.stamina_food.BAEKGO_ACCEPT_failure_info_label = gameforge.locale.stamina_food.baekgo_accept_failure_info_label
locale.stamina_food.BAEKGO_ACCEPT_failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.stamina_food.baekgo_accept_failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.stamina_food.baekgo_accept_failure_info_text_2
locale.stamina_food.SUCCESS_say = gameforge.locale.stamina_food.success_say
locale.stamina_food.COMPLETE_say = gameforge.locale.stamina_food.complete_say

locale.skeleton_gem = {}
locale.skeleton_gem.start_probability = 3
locale.skeleton_gem.start_level_begin = 55
locale.skeleton_gem.start_level_end = 57
locale.skeleton_gem.end_level = 60
locale.skeleton_gem.kill_count = 3
locale.skeleton_gem.talk_about_skeleton_gem = gameforge.locale.skeleton_gem.talk_about_skeleton_gem
locale.skeleton_gem.i_dont_have_the_skeleton_gem_yet = gameforge.locale.skeleton_gem.i_dont_have_the_skeleton_gem_yet
locale.skeleton_gem.i_have_the_skeleton_gem = gameforge.locale.skeleton_gem.i_have_the_skeleton_gem
locale.skeleton_gem.START_say = gameforge.locale.skeleton_gem.start_say
locale.skeleton_gem.START_find_peddler_label = gameforge.locale.skeleton_gem.start_find_peddler_label
locale.skeleton_gem.START_find_peddler_text = locale.NOTICE_COLOR .. gameforge.locale.skeleton_gem.start_find_peddler_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.skeleton_gem.start_find_peddler_text_2
locale.skeleton_gem.ORDER_say = gameforge.locale.skeleton_gem.order_say
locale.skeleton_gem.ORDER_say2 = gameforge.locale.skeleton_gem.order_say2
locale.skeleton_gem.ORDER_accept = gameforge.locale.skeleton_gem.order_accept
locale.skeleton_gem.ORDER_refuse = gameforge.locale.skeleton_gem.order_refuse
locale.skeleton_gem.ORDER_accept_answer = gameforge.locale.skeleton_gem.order_accept_answer
locale.skeleton_gem.ORDER_refuse_answer = gameforge.locale.skeleton_gem.order_refuse_answer
locale.skeleton_gem.ORDER_get_skeleton_gem_ = gameforge.locale.skeleton_gem.order_get_skeleton_gem_
locale.skeleton_gem.ORDER_get_skeleton_gem_text = locale.NOTICE_COLOR .. gameforge.locale.skeleton_gem.order_get_skeleton_gem_text
locale.skeleton_gem.ORDER_too_late = gameforge.locale.skeleton_gem.order_too_late
locale.skeleton_gem.ACCEPT_success_say = locale.NOTICE_COLOR .. gameforge.locale.skeleton_gem.accept_success_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.skeleton_gem.accept_success_say_2
locale.skeleton_gem.ACCEPT_failure_say = locale.NOTICE_COLOR .. gameforge.locale.skeleton_gem.accept_failure_say_1 .. locale.NORMAL_COLOR .. gameforge.locale.skeleton_gem.accept_failure_say_2
locale.skeleton_gem.ACCEPT_ask = gameforge.locale.skeleton_gem.accept_ask
locale.skeleton_gem.ACCEPT_success_info_label = gameforge.locale.skeleton_gem.accept_success_info_label
locale.skeleton_gem.ACCEPT_success_info_text = locale.NOTICE_COLOR .. gameforge.locale.skeleton_gem.accept_success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.skeleton_gem.accept_success_info_text_2
locale.skeleton_gem.ACCEPT_failure_info_label = gameforge.locale.skeleton_gem.accept_failure_info_label
locale.skeleton_gem.ACCEPT_failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.skeleton_gem.accept_failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.skeleton_gem.accept_failure_info_text_2
locale.skeleton_gem.SUCCESS_say = gameforge.locale.skeleton_gem.success_say

locale.ariyeong = {}
locale.ariyeong.start_probability = 3
locale.ariyeong.start_level_begin = 25
locale.ariyeong.start_level_end = 27
locale.ariyeong.end_level = 30
locale.ariyeong.kill_count = 2
locale.ariyeong.event_item = 30101
locale.ariyeong.find_ariyeong_label = gameforge.locale.ariyeong.find_ariyeong_label
locale.ariyeong.find_ariyeong_text = locale.NOTICE_COLOR .. gameforge.locale.ariyeong.find_ariyeong_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.ariyeong.find_ariyeong_text_2
locale.ariyeong.kill_label = gameforge.locale.ariyeong.kill_label
locale.ariyeong.kill_text = locale.NOTICE_COLOR .. gameforge.locale.ariyeong.kill_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.ariyeong.kill_text_2
locale.ariyeong.too_late = gameforge.locale.ariyeong.too_late
locale.ariyeong.success_info_label = gameforge.locale.ariyeong.success_info_label
locale.ariyeong.success_info_text = locale.NOTICE_COLOR .. gameforge.locale.ariyeong.success_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.ariyeong.success_info_text_2
locale.ariyeong.failure_info_label = gameforge.locale.ariyeong.failure_info_label
locale.ariyeong.failure_info_text = locale.NOTICE_COLOR .. gameforge.locale.ariyeong.failure_info_text_1 .. locale.NORMAL_COLOR .. gameforge.locale.ariyeong.failure_info_text_2

locale.levelup = {}
locale.levelup.choice1 = gameforge.locale.levelup.choice1
locale.levelup.choice2 = gameforge.locale.levelup.choice2
locale.levelup.slay_them1 = gameforge.locale.levelup.slay_them1
locale.levelup.slay_them2 = gameforge.locale.levelup.slay_them2
locale.levelup.slay_target_is = gameforge.locale.levelup.slay_target_is 
locale.levelup.slay_target_count = gameforge.locale.levelup.slay_target_count
locale.levelup.slayed_them = gameforge.locale.levelup.slayed_them
locale.levelup.remain_count = gameforge.locale.levelup.remain_count
locale.levelup.cause_of_levelup = gameforge.locale.levelup.cause_of_levelup
locale.levelup.receive_direct = gameforge.locale.levelup.receive_direct
locale.levelup.levelup_quest_fail = gameforge.locale.levelup.levelup_quest_fail
locale.levelup.lets_go = gameforge.locale.levelup.lets_go
locale.levelup.kill_count = gameforge.locale.levelup.kill_count
locale.levelup.prev_quest1 = gameforge.locale.levelup.prev_quest1
locale.levelup.prev_quest2 = gameforge.locale.levelup.prev_quest2
locale.levelup.prev_quest_info = gameforge.locale.levelup.prev_quest_info
locale.levelup.prev_quest_go = gameforge.locale.levelup.prev_quest_go
locale.levelup.random_item = gameforge.locale.levelup.random_item

locale.stash = {}
locale.stash.buy_silver_and_gold_bar = gameforge.locale.stash.buy_silver_and_gold_bar
locale.stash.gold_bar_beginner_info = gameforge.locale.stash.gold_bar_beginner_info
locale.stash.enlarge_first1_cannot = gameforge.locale.stash.enlarge_first1_cannot
locale.stash.greet1 = gameforge.locale.stash.greet1
locale.stash.not_enough_money = gameforge.locale.stash.not_enough_money
locale.stash.pay_ok = gameforge.locale.stash.pay_ok
locale.stash.pay_no = gameforge.locale.stash.pay_no
locale.stash.first_open = gameforge.locale.stash.first_open
locale.stash.dont_open = gameforge.locale.stash.dont_open
locale.stash.greet2 = gameforge.locale.stash.greet2
locale.stash.beginner_info = gameforge.locale.stash.beginner_info
locale.stash.enlarge = gameforge.locale.stash.enlarge
locale.stash.nomatter = gameforge.locale.stash.nomatter
locale.stash.enlarge_first1 = gameforge.locale.stash.enlarge_first1
locale.stash.enlarge_item1 = 50
locale.stash.enlarge_item2 = 5030
locale.stash.enlarge_first2 = gameforge.locale.stash.enlarge_first2
locale.stash.enlarge_first3 = gameforge.locale.stash.enlarge_first3
locale.stash.enlarge_try_ok = gameforge.locale.stash.enlarge_try_ok
locale.stash.enlarge_try_no = gameforge.locale.stash.enlarge_try_no
locale.stash.enlarge_try_ok_answer = gameforge.locale.stash.enlarge_try_ok_answer
locale.stash.enlarge_1_title = gameforge.locale.stash.enlarge_1_title
locale.stash.enlarge_1_counter = gameforge.locale.stash.enlarge_1_counter
locale.stash.enlarge_1_item = gameforge.locale.stash.enlarge_1_item
locale.stash.enlarge_1_timer = gameforge.locale.stash.enlarge_1_timer
locale.stash.enlarge_no_need = gameforge.locale.stash.enlarge_no_need
locale.stash.greet3 = gameforge.locale.stash.greet3
locale.stash.enlarge_2_no_mark = gameforge.locale.stash.enlarge_2_no_mark
locale.stash.enlarge_2_again = gameforge.locale.stash.enlarge_2_again
locale.stash.enlarge_2_title = gameforge.locale.stash.enlarge_2_title
locale.stash.enlarge_2_item = 60002
locale.stash.enlarge_2_mark_item = 60003
locale.stash.enlarge_2_have_mark = c_item_name(locale.stash.enlarge_2_mark_item .. gameforge.locale.stash.enlarge_2_have_mark)
locale.stash.enlarge_2_no_mark2 = gameforge.locale.stash.enlarge_2_no_mark2
locale.stash.enlarge_2_have_mark2 = gameforge.locale.stash.enlarge_2_have_mark2
locale.stash.enlarge_2_have_mark3 = gameforge.locale.stash.enlarge_2_have_mark3
locale.stash.enlarge_2_msg1 = gameforge.locale.stash.enlarge_2_msg1
locale.stash.enlarge_2_msg2 = gameforge.locale.stash.enlarge_2_msg2
locale.stash.greet4 = gameforge.locale.stash.greet4
locale.stash.enlarge_3_msg1 = gameforge.locale.stash.enlarge_3_msg1
locale.stash.enlarge_3_title = gameforge.locale.stash.enlarge_3_title
locale.stash.enlarge_3_counter = gameforge.locale.stash.enlarge_3_counter
locale.stash.enlarge_3_timer = gameforge.locale.stash.enlarge_3_timer
locale.stash.enlarge_4_msg = gameforge.locale.stash.enlarge_4_msg
locale.stash.enlarge_2_recommendation = gameforge.locale.stash.enlarge_2_recommendation
locale.stash.enlarge_2_oldwoman_greet = gameforge.locale.stash.enlarge_2_oldwoman_greet
locale.stash.enlarge_2_oldwoman_reject = gameforge.locale.stash.enlarge_2_oldwoman_reject
locale.stash.enlarge_2_oldwoman_title1 = gameforge.locale.stash.enlarge_2_oldwoman_title1
locale.stash.enlarge_2_oldwoman_title2 = gameforge.locale.stash.enlarge_2_oldwoman_title2
locale.stash.enlarge_2_oldwoman_accept = gameforge.locale.stash.enlarge_2_oldwoman_accept
locale.stash.enlarge_1_info1 = gameforge.locale.stash.enlarge_1_info1
locale.stash.enlarge_1_info2 = gameforge.locale.stash.enlarge_1_info2
locale.stash.enlarge_1_take_bear = gameforge.locale.stash.enlarge_1_take_bear
locale.stash.enlarge_1_success = gameforge.locale.stash.enlarge_1_success
locale.stash.enlarge_1_notyet = gameforge.locale.stash.enlarge_1_notyet
locale.stash.enlarge_1_getone = gameforge.locale.stash.enlarge_1_getone
locale.stash.enlarge_1_complete = gameforge.locale.stash.enlarge_1_complete
locale.stash.enlarge_1_fail_info = gameforge.locale.stash.enlarge_1_fail_info
locale.stash.enlarge_1_fail_msg = gameforge.locale.stash.enlarge_1_fail_msg
locale.stash.enlarge_3_complete = gameforge.locale.stash.enlarge_3_complete
locale.stash.enlarge_3_success = gameforge.locale.stash.enlarge_3_success
locale.stash.enlarge_3_notyet = gameforge.locale.stash.enlarge_3_notyet
locale.stash.open_mall = gameforge.locale.stash.open_mall

locale.guild = {}
locale.guild.guild_master_greet = gameforge.locale.guild.guild_master_greet
locale.guild.disband = gameforge.locale.guild.disband
locale.guild.cancel = gameforge.locale.guild.cancel
locale.guild.yes = gameforge.locale.guild.yes
locale.guild.no = gameforge.locale.guild.no
locale.guild.disband_confirm = gameforge.locale.guild.disband_confirm
locale.guild.disband_msg = gameforge.locale.guild.disband_msg
locale.guild.guild_member_greet = gameforge.locale.guild.guild_member_greet
locale.guild.withdraw = gameforge.locale.guild.withdraw
locale.guild.withdraw_confirm = gameforge.locale.guild.withdraw_confirm
locale.guild.withdraw_msg = gameforge.locale.guild.withdraw_msg
locale.guild.noguild_greet = gameforge.locale.guild.noguild_greet
locale.guild.create = gameforge.locale.guild.create
locale.guild.create_confirm = gameforge.locale.guild.create_confirm
locale.guild.create_not_enough_money = gameforge.locale.guild.create_not_enough_money
locale.guild.not_enough_leadership = gameforge.locale.guild.not_enough_leadership
locale.guild.no_guild_create_item = gameforge.locale.guild.no_guild_create_item
locale.guild.create_confirm_for_china = gameforge.locale.guild.create_confirm_for_china
locale.guild.cannot_create_guild_withdraw_delay = gameforge.locale.guild.cannot_create_guild_withdraw_delay
locale.guild.cannot_create_guild_disband_delay = gameforge.locale.guild.cannot_create_guild_disband_delay
locale.guild.war_entry_ask_head = gameforge.locale.guild.war_entry_ask_head
locale.guild.war_entry_ask_tail = gameforge.locale.guild.war_entry_ask_tail
locale.guild.war_list_button = gameforge.locale.guild.war_list_button
locale.guild.war_list_none = gameforge.locale.guild.war_list_none
locale.guild.war_list_choose = gameforge.locale.guild.war_list_choose
locale.guild.war_join_request = gameforge.locale.guild.war_join_request
locale.guild.war_over = gameforge.locale.guild.war_over
locale.guild.war_bet_list_button = gameforge.locale.guild.war_bet_list_button
locale.guild.war_bet_info = gameforge.locale.guild.war_bet_info
locale.guild.war_bet_no_money = gameforge.locale.guild.war_bet_no_money
locale.guild.war_bet_price = gameforge.locale.guild.war_bet_price
locale.guild.war_bet_done = gameforge.locale.guild.war_bet_done
locale.guild.war_bet_list_none = gameforge.locale.guild.war_bet_list_none
locale.guild.war_bet_list_choose = gameforge.locale.guild.war_bet_list_choose
locale.guild.war_bet_already = gameforge.locale.guild.war_bet_already
locale.guild.war_bet_assume1 = gameforge.locale.guild.war_bet_assume1
locale.guild.war_bet_assume2 = gameforge.locale.guild.war_bet_assume2
locale.guild.war_bet_assume3 = gameforge.locale.guild.war_bet_assume3

locale.oldwoman = {}
locale.oldwoman.no_matter = gameforge.locale.oldwoman.no_matter
locale.oldwoman.skill_reset1 = gameforge.locale.oldwoman.skill_reset1
locale.oldwoman.skill_reset2 = gameforge.locale.oldwoman.skill_reset2
locale.oldwoman.skill_reset_ok = gameforge.locale.oldwoman.skill_reset_ok
locale.oldwoman.skill_reset_no = gameforge.locale.oldwoman.skill_reset_no
locale.oldwoman.not_enough_money = gameforge.locale.oldwoman.not_enough_money
 
locale.skill_group = {}
locale.skill_group.lets_join_skill_group = {
	gameforge.locale.skill_group.lets_join_skill_group_1,
	gameforge.locale.skill_group.lets_join_skill_group_2,
	gameforge.locale.skill_group.lets_join_skill_group_3,
	gameforge.locale.skill_group.lets_join_skill_group_4
}
locale.skill_group.invite = gameforge.locale.skill_group.invite
locale.skill_group.board = gameforge.locale.skill_group.board
locale.skill_group.dialog = {{
	[WARRIOR] = {
		{
			gameforge.locale.skill_group.dialog_warrior_1,
			gameforge.locale.skill_group.dialog_warrior_2,
		},
		{
			gameforge.locale.skill_group.dialog_warrior_3,
			gameforge.locale.skill_group.dialog_warrior_4,
		},
		gameforge.locale.skill_group.dialog_warrior_5,
	},
	[ASSASSIN] = {
		{
			gameforge.locale.skill_group.dialog_assassin_1,
			gameforge.locale.skill_group.dialog_assassin_2,
		},
		{
			gameforge.locale.skill_group.dialog_assassin_3,
			gameforge.locale.skill_group.dialog_assassin_4,
		},
		gameforge.locale.skill_group.dialog_assassin_5,
	},
	[SURA] = {
		{
			gameforge.locale.skill_group.dialog_sura_1,
			gameforge.locale.skill_group.dialog_sura_2,
		},
		{
			gameforge.locale.skill_group.dialog_sura_3,
			gameforge.locale.skill_group.dialog_sura_4,
		},
		gameforge.locale.skill_group.dialog_sura_5,
	},
	[SHAMAN] = {
		{
			gameforge.locale.skill_group.dialog_shaman_1,
			gameforge.locale.skill_group.dialog_shaman_2,
		},
		{
			gameforge.locale.skill_group.dialog_shaman_3,
			gameforge.locale.skill_group.dialog_shaman_4,
		},
		gameforge.locale.skill_group.dialog_shaman_5,
	}
}}

locale.neutral_warp = {}
locale.neutral_warp.greet = gameforge.locale.neutral_warp.greet 
locale.neutral_warp.go = gameforge.locale.neutral_warp.go
locale.neutral_warp.not_go = gameforge.locale.neutral_warp.not_go
locale.neutral_warp.where_to_go = gameforge.locale.neutral_warp.where_to_go
locale.neutral_warp.money_need1 = gameforge.locale.neutral_warp.money_need1
locale.neutral_warp.money_need2 = gameforge.locale.neutral_warp.money_need2
locale.neutral_warp.cannot_go_yet = gameforge.locale.neutral_warp.cannot_go_yet

locale.blacksmith = {}
locale.blacksmith.refine_info = gameforge.locale.blacksmith.refine_info

locale.monster_chat = { 
	[301] = {
		gameforge.locale.monster_chat_301_1,
		gameforge.locale.monster_chat_301_2,
		gameforge.locale.monster_chat_301_3,
		gameforge.locale.monster_chat_301_4,
		gameforge.locale.monster_chat_301_5
	},
	[302] = {
		gameforge.locale.monster_chat_302_1,
		gameforge.locale.monster_chat_302_2,
		gameforge.locale.monster_chat_302_3,
		gameforge.locale.monster_chat_302_4,
		gameforge.locale.monster_chat_302_5,
		gameforge.locale.monster_chat_302_6
	},
	[303] = {
		gameforge.locale.monster_chat_303_1,
		gameforge.locale.monster_chat_303_2,
		gameforge.locale.monster_chat_303_3,
		gameforge.locale.monster_chat_303_4,
		gameforge.locale.monster_chat_303_5,
		gameforge.locale.monster_chat_303_6
	},
	[304] = {
		gameforge.locale.monster_chat_304_1,
		gameforge.locale.monster_chat_304_2,
		gameforge.locale.monster_chat_304_3,
		gameforge.locale.monster_chat_304_4,
		gameforge.locale.monster_chat_304_5,
		gameforge.locale.monster_chat_304_6
	},
	[305] = {
		gameforge.locale.monster_chat_305_1,
		gameforge.locale.monster_chat_305_2,
		gameforge.locale.monster_chat_305_3,
		gameforge.locale.monster_chat_305_4,
		gameforge.locale.monster_chat_305_5,
		gameforge.locale.monster_chat_305_6
	},
	[306] = {
		gameforge.locale.monster_chat_306_1,
		gameforge.locale.monster_chat_306_2,
		gameforge.locale.monster_chat_306_3,
		gameforge.locale.monster_chat_306_4,
		gameforge.locale.monster_chat_306_5,
		gameforge.locale.monster_chat_306_6
	},
	[307] = {
		gameforge.locale.monster_chat_307_1,
		gameforge.locale.monster_chat_307_2,
		gameforge.locale.monster_chat_307_3,
		gameforge.locale.monster_chat_307_4,
		gameforge.locale.monster_chat_307_5,
		gameforge.locale.monster_chat_307_6
	},
	[394] = {
		gameforge.locale.monster_chat_394_1,
		gameforge.locale.monster_chat_394_2,
		gameforge.locale.monster_chat_394_3,
		gameforge.locale.monster_chat_394_4,
		gameforge.locale.monster_chat_394_5,
		gameforge.locale.monster_chat_394_6
	},
	[401] = {
		gameforge.locale.monster_chat_401_1,
		gameforge.locale.monster_chat_401_2,
		gameforge.locale.monster_chat_401_3,
		gameforge.locale.monster_chat_401_4,
		gameforge.locale.monster_chat_401_5,
		gameforge.locale.monster_chat_401_6
	},
	[402] = {
		gameforge.locale.monster_chat_402_1,
		gameforge.locale.monster_chat_402_2,
		gameforge.locale.monster_chat_402_3,
		gameforge.locale.monster_chat_402_4,
		gameforge.locale.monster_chat_402_5,
		gameforge.locale.monster_chat_402_6
	},
	[403] = {
		gameforge.locale.monster_chat_403_1,
		gameforge.locale.monster_chat_403_2,
		gameforge.locale.monster_chat_403_3,
		gameforge.locale.monster_chat_403_4,
		gameforge.locale.monster_chat_403_5,
		gameforge.locale.monster_chat_403_6
	},
	[404] = {
		gameforge.locale.monster_chat_404_1,
		gameforge.locale.monster_chat_404_2,
		gameforge.locale.monster_chat_404_3,
		gameforge.locale.monster_chat_404_4,
		gameforge.locale.monster_chat_404_5,
		gameforge.locale.monster_chat_404_6
	},
	[405] = {
		gameforge.locale.monster_chat_405_1,
		gameforge.locale.monster_chat_405_2,
		gameforge.locale.monster_chat_405_3,
		gameforge.locale.monster_chat_405_4,
		gameforge.locale.monster_chat_405_5,
		gameforge.locale.monster_chat_405_6
	},
	[406] = {
		gameforge.locale.monster_chat_406_1,
		gameforge.locale.monster_chat_406_2,
		gameforge.locale.monster_chat_406_3,
		gameforge.locale.monster_chat_406_4,
		gameforge.locale.monster_chat_406_5,
		gameforge.locale.monster_chat_406_6
	},
	[491] = {
		gameforge.locale.monster_chat_491_1,
		gameforge.locale.monster_chat_491_2,
		gameforge.locale.monster_chat_491_3,
		gameforge.locale.monster_chat_491_4,
		gameforge.locale.monster_chat_491_5,
		gameforge.locale.monster_chat_491_6
	},
	[492] = {
		gameforge.locale.monster_chat_492_1,
		gameforge.locale.monster_chat_492_2,
		gameforge.locale.monster_chat_492_3,
		gameforge.locale.monster_chat_492_4,
		gameforge.locale.monster_chat_492_5,
		gameforge.locale.monster_chat_492_6
	},
	[493] = {
		gameforge.locale.monster_chat_493_1,
		gameforge.locale.monster_chat_493_2,
		gameforge.locale.monster_chat_493_3,
		gameforge.locale.monster_chat_493_4,
		gameforge.locale.monster_chat_493_5,
		gameforge.locale.monster_chat_493_6
	},
	[494] = {
		gameforge.locale.monster_chat_494_1,
		gameforge.locale.monster_chat_494_2,
		gameforge.locale.monster_chat_494_3,
		gameforge.locale.monster_chat_494_4,
		gameforge.locale.monster_chat_494_5,
		gameforge.locale.monster_chat_494_6
	},
	[501] = {
		gameforge.locale.monster_chat_501_1,
		gameforge.locale.monster_chat_501_2,
		gameforge.locale.monster_chat_501_3,
		gameforge.locale.monster_chat_501_4,
		gameforge.locale.monster_chat_501_5,
		gameforge.locale.monster_chat_501_6
	},
	[502] = {
		gameforge.locale.monster_chat_502_1,
		gameforge.locale.monster_chat_502_2,
		gameforge.locale.monster_chat_502_3,
		gameforge.locale.monster_chat_502_4,
		gameforge.locale.monster_chat_502_5,
		gameforge.locale.monster_chat_502_6
	},
	[503] = {
		gameforge.locale.monster_chat_503_1,
		gameforge.locale.monster_chat_503_2,
		gameforge.locale.monster_chat_503_3,
		gameforge.locale.monster_chat_503_4,
		gameforge.locale.monster_chat_503_5,
		gameforge.locale.monster_chat_503_6
	},
	[504] = {
		gameforge.locale.monster_chat_504_1,
		gameforge.locale.monster_chat_504_2,
		gameforge.locale.monster_chat_504_3,
		gameforge.locale.monster_chat_504_4,
		gameforge.locale.monster_chat_504_5,
		gameforge.locale.monster_chat_504_6
	},
	[601] = {
		gameforge.locale.monster_chat_601_1,
		gameforge.locale.monster_chat_601_2,
		gameforge.locale.monster_chat_601_3,
		gameforge.locale.monster_chat_601_4,
		gameforge.locale.monster_chat_601_5,
		gameforge.locale.monster_chat_601_6
	},
	[601] = {
		gameforge.locale.monster_chat_601_7,
		gameforge.locale.monster_chat_602_1,
		gameforge.locale.monster_chat_602_2,
		gameforge.locale.monster_chat_602_3,
		gameforge.locale.monster_chat_602_4,
		gameforge.locale.monster_chat_602_5,
		gameforge.locale.monster_chat_602_6
	},
	[603] = {
		gameforge.locale.monster_chat_603_1,
		gameforge.locale.monster_chat_603_2,
		gameforge.locale.monster_chat_603_3,
		gameforge.locale.monster_chat_603_4,
		gameforge.locale.monster_chat_603_5,
		gameforge.locale.monster_chat_603_6
	},
	[604] = {
		gameforge.locale.monster_chat_604_1,
		gameforge.locale.monster_chat_604_2,
		gameforge.locale.monster_chat_604_3,
		gameforge.locale.monster_chat_604_4,
		gameforge.locale.monster_chat_604_5,
		gameforge.locale.monster_chat_604_6
	},
	[631] = {
		gameforge.locale.monster_chat_631_1,
		gameforge.locale.monster_chat_631_2,
		gameforge.locale.monster_chat_631_3,
		gameforge.locale.monster_chat_631_4,
		gameforge.locale.monster_chat_631_5,
		gameforge.locale.monster_chat_631_6
	},
	[632] = {
		gameforge.locale.monster_chat_632_1,
		gameforge.locale.monster_chat_632_2,
		gameforge.locale.monster_chat_632_3,
		gameforge.locale.monster_chat_632_4,
		gameforge.locale.monster_chat_632_5,
		gameforge.locale.monster_chat_632_6
	},
	[633] = {
		gameforge.locale.monster_chat_633_1,
		gameforge.locale.monster_chat_633_2,
		gameforge.locale.monster_chat_633_3,
		gameforge.locale.monster_chat_633_4,
		gameforge.locale.monster_chat_633_5,
		gameforge.locale.monster_chat_633_6
	},
	[634] = {
		gameforge.locale.monster_chat_634_1,
		gameforge.locale.monster_chat_634_2,
		gameforge.locale.monster_chat_634_3,
		gameforge.locale.monster_chat_634_4,
		gameforge.locale.monster_chat_634_5,
		gameforge.locale.monster_chat_634_6
	},
	[635] = {
		gameforge.locale.monster_chat_635_1,
		gameforge.locale.monster_chat_635_2,
		gameforge.locale.monster_chat_635_3,
		gameforge.locale.monster_chat_635_4,
		gameforge.locale.monster_chat_635_5,
		gameforge.locale.monster_chat_635_6
	},
	[636] = {
		gameforge.locale.monster_chat_636_1,
		gameforge.locale.monster_chat_636_2,
		gameforge.locale.monster_chat_636_3,
		gameforge.locale.monster_chat_636_4,
		gameforge.locale.monster_chat_636_5,
		gameforge.locale.monster_chat_636_6
	},
	[636] = {
		gameforge.locale.monster_chat_636_7,
		gameforge.locale.monster_chat_637_1,
		gameforge.locale.monster_chat_637_2,
		gameforge.locale.monster_chat_637_3,
		gameforge.locale.monster_chat_637_4,
		gameforge.locale.monster_chat_637_5,
		gameforge.locale.monster_chat_637_6
	},
	[691] = {
		gameforge.locale.monster_chat_691_1,
		gameforge.locale.monster_chat_691_2,
		gameforge.locale.monster_chat_691_3,
		gameforge.locale.monster_chat_691_4,
		gameforge.locale.monster_chat_691_5,
		gameforge.locale.monster_chat_691_6
	},
	[701] = {
		gameforge.locale.monster_chat_701_1,
		gameforge.locale.monster_chat_701_2,
		gameforge.locale.monster_chat_701_3,
		gameforge.locale.monster_chat_701_4,
		gameforge.locale.monster_chat_701_5,
		gameforge.locale.monster_chat_701_6
	},
	[702] = {
		gameforge.locale.monster_chat_702_1,
		gameforge.locale.monster_chat_702_2,
		gameforge.locale.monster_chat_702_3,
		gameforge.locale.monster_chat_702_4,
		gameforge.locale.monster_chat_702_5,
		gameforge.locale.monster_chat_702_6
	},
	[703] = {
		gameforge.locale.monster_chat_703_1,
		gameforge.locale.monster_chat_703_2,
		gameforge.locale.monster_chat_703_3,
		gameforge.locale.monster_chat_703_4,
		gameforge.locale.monster_chat_703_5,
		gameforge.locale.monster_chat_703_6
	},
	[704] = {
		gameforge.locale.monster_chat_704_1,
		gameforge.locale.monster_chat_704_2,
		gameforge.locale.monster_chat_704_3,
		gameforge.locale.monster_chat_704_4,
		gameforge.locale.monster_chat_704_5,
		gameforge.locale.monster_chat_704_6
	},
	[705] = {
		gameforge.locale.monster_chat_705_1,
		gameforge.locale.monster_chat_705_2,
		gameforge.locale.monster_chat_705_3,
		gameforge.locale.monster_chat_705_4,
		gameforge.locale.monster_chat_705_5,
		gameforge.locale.monster_chat_705_6
	},
	[706] = {
		gameforge.locale.monster_chat_706_1,
		gameforge.locale.monster_chat_706_2,
		gameforge.locale.monster_chat_706_3,
		gameforge.locale.monster_chat_706_4,
		gameforge.locale.monster_chat_706_5,
		gameforge.locale.monster_chat_706_6
	},
	[707] = {
		gameforge.locale.monster_chat_707_1,
		gameforge.locale.monster_chat_707_2,
		gameforge.locale.monster_chat_707_3,
		gameforge.locale.monster_chat_707_4,
		gameforge.locale.monster_chat_707_5,
		gameforge.locale.monster_chat_707_6
	},
	[791] = {
		gameforge.locale.monster_chat_791_1,
		gameforge.locale.monster_chat_791_2,
		gameforge.locale.monster_chat_791_3,
		gameforge.locale.monster_chat_791_4,
		gameforge.locale.monster_chat_791_5,
		gameforge.locale.monster_chat_791_6
	},
	[901] = {
		gameforge.locale.monster_chat_901_1,
		gameforge.locale.monster_chat_901_2,
		gameforge.locale.monster_chat_901_3,
		gameforge.locale.monster_chat_901_4,
		gameforge.locale.monster_chat_901_5
	},
	[903] = {
		gameforge.locale.monster_chat_903_1,
		gameforge.locale.monster_chat_903_2,
		gameforge.locale.monster_chat_903_3,
		gameforge.locale.monster_chat_903_4,
		gameforge.locale.monster_chat_903_5,
		gameforge.locale.monster_chat_903_6
	},
	[904] = {
		gameforge.locale.monster_chat_904_1,
		gameforge.locale.monster_chat_904_2,
		gameforge.locale.monster_chat_904_3,
		gameforge.locale.monster_chat_904_4,
		gameforge.locale.monster_chat_904_5,
		gameforge.locale.monster_chat_904_6
	},
	[905] = {
		gameforge.locale.monster_chat_905_1,
		gameforge.locale.monster_chat_905_2,
		gameforge.locale.monster_chat_905_3,
		gameforge.locale.monster_chat_905_4,
		gameforge.locale.monster_chat_905_5,
		gameforge.locale.monster_chat_905_6
	},
	[906] = {
		gameforge.locale.monster_chat_906_1,
		gameforge.locale.monster_chat_906_2,
		gameforge.locale.monster_chat_906_3,
		gameforge.locale.monster_chat_906_4,
		gameforge.locale.monster_chat_906_5,
		gameforge.locale.monster_chat_906_6
	},
	[907] = {
		gameforge.locale.monster_chat_907_1,
		gameforge.locale.monster_chat_907_2,
		gameforge.locale.monster_chat_907_3,
		gameforge.locale.monster_chat_907_4,
		gameforge.locale.monster_chat_907_5,
		gameforge.locale.monster_chat_907_6
	},
	[1001] = {
		gameforge.locale.monster_chat_1001_1,
		gameforge.locale.monster_chat_1001_2,
		gameforge.locale.monster_chat_1001_3,
		gameforge.locale.monster_chat_1001_4,
		gameforge.locale.monster_chat_1001_5,
		gameforge.locale.monster_chat_1001_6
	},
	[1002] = {
		gameforge.locale.monster_chat_1002_1,
		gameforge.locale.monster_chat_1002_2,
		gameforge.locale.monster_chat_1002_3,
		gameforge.locale.monster_chat_1002_4,
		gameforge.locale.monster_chat_1002_5
	},
	[1003] = {
		gameforge.locale.monster_chat_1003_1,
		gameforge.locale.monster_chat_1003_2,
		gameforge.locale.monster_chat_1003_3,
		gameforge.locale.monster_chat_1003_4,
		gameforge.locale.monster_chat_1003_5,
		gameforge.locale.monster_chat_1003_6
	},
	[1004] = {
		gameforge.locale.monster_chat_1004_1,
		gameforge.locale.monster_chat_1004_2,
		gameforge.locale.monster_chat_1004_3,
		gameforge.locale.monster_chat_1004_4,
		gameforge.locale.monster_chat_1004_5,
		gameforge.locale.monster_chat_1004_6
	},
	[1035] = {
		gameforge.locale.monster_chat_1035_1,
		gameforge.locale.monster_chat_1035_2,
		gameforge.locale.monster_chat_1035_3,
		gameforge.locale.monster_chat_1035_4,
		gameforge.locale.monster_chat_1035_5,
		gameforge.locale.monster_chat_1035_6
	},
	[1036] = {
		gameforge.locale.monster_chat_1036_1,
		gameforge.locale.monster_chat_1036_2,
		gameforge.locale.monster_chat_1036_3,
		gameforge.locale.monster_chat_1036_4,
		gameforge.locale.monster_chat_1036_5,
		gameforge.locale.monster_chat_1036_6
	},
	[1037] = {
		gameforge.locale.monster_chat_1037_1,
		gameforge.locale.monster_chat_1037_2,
		gameforge.locale.monster_chat_1037_3,
		gameforge.locale.monster_chat_1037_4,
		gameforge.locale.monster_chat_1037_5,
		gameforge.locale.monster_chat_1037_6
	},
	[1038] = {
		gameforge.locale.monster_chat_1038_1,
		gameforge.locale.monster_chat_1038_2,
		gameforge.locale.monster_chat_1038_3,
		gameforge.locale.monster_chat_1038_4,
		gameforge.locale.monster_chat_1038_5,
		gameforge.locale.monster_chat_1038_6
	},
	[1039] = {
		gameforge.locale.monster_chat_1039_1,
		gameforge.locale.monster_chat_1039_2,
		gameforge.locale.monster_chat_1039_3,
		gameforge.locale.monster_chat_1039_4,
		gameforge.locale.monster_chat_1039_5,
		gameforge.locale.monster_chat_1039_6
	},
	[1040] = {
		gameforge.locale.monster_chat_1040_1,
		gameforge.locale.monster_chat_1040_2,
		gameforge.locale.monster_chat_1040_3,
		gameforge.locale.monster_chat_1040_4,
		gameforge.locale.monster_chat_1040_5,
		gameforge.locale.monster_chat_1040_6
	},
	[1041] = {
		gameforge.locale.monster_chat_1041_1,
		gameforge.locale.monster_chat_1041_2,
		gameforge.locale.monster_chat_1041_3,
		gameforge.locale.monster_chat_1041_4,
		gameforge.locale.monster_chat_1041_5,
		gameforge.locale.monster_chat_1041_6
	},
	[1091] = {
		gameforge.locale.monster_chat_1091_1,
		gameforge.locale.monster_chat_1091_2,
		gameforge.locale.monster_chat_1091_3,
		gameforge.locale.monster_chat_1091_4,
		gameforge.locale.monster_chat_1091_5,
		gameforge.locale.monster_chat_1091_6
	},
	[1093] = {
		gameforge.locale.monster_chat_1093_1,
		gameforge.locale.monster_chat_1093_2,
		gameforge.locale.monster_chat_1093_3,
		gameforge.locale.monster_chat_1093_4,
		gameforge.locale.monster_chat_1093_5,
		gameforge.locale.monster_chat_1093_6
	},
	[1105] = {
		gameforge.locale.monster_chat_1105_1,
		gameforge.locale.monster_chat_1105_2,
		gameforge.locale.monster_chat_1105_3,
		gameforge.locale.monster_chat_1105_4,
		gameforge.locale.monster_chat_1105_5,
		gameforge.locale.monster_chat_1105_6
	},
	[1106] = {
		gameforge.locale.monster_chat_1106_1,
		gameforge.locale.monster_chat_1106_2,
		gameforge.locale.monster_chat_1106_3,
		gameforge.locale.monster_chat_1106_4,
		gameforge.locale.monster_chat_1106_5,
		gameforge.locale.monster_chat_1106_6
	},
	[1191] = {
		gameforge.locale.monster_chat_1191_1,
		gameforge.locale.monster_chat_1191_2,
		gameforge.locale.monster_chat_1191_3,
		gameforge.locale.monster_chat_1191_4,
		gameforge.locale.monster_chat_1191_5,
		gameforge.locale.monster_chat_1191_6
	},
	[1301] = {
		gameforge.locale.monster_chat_1301_1,
		gameforge.locale.monster_chat_1301_2,
		gameforge.locale.monster_chat_1301_3,
		gameforge.locale.monster_chat_1301_4,
		gameforge.locale.monster_chat_1301_5,
		gameforge.locale.monster_chat_1301_6
	},
	[1302] = {
		gameforge.locale.monster_chat_1302_1,
		gameforge.locale.monster_chat_1302_2,
		gameforge.locale.monster_chat_1302_3,
		gameforge.locale.monster_chat_1302_4,
		gameforge.locale.monster_chat_1302_5,
		gameforge.locale.monster_chat_1302_6
	},
	[1303] = {
		gameforge.locale.monster_chat_1303_1,
		gameforge.locale.monster_chat_1303_2,
		gameforge.locale.monster_chat_1303_3,
		gameforge.locale.monster_chat_1303_4,
		gameforge.locale.monster_chat_1303_5,
		gameforge.locale.monster_chat_1303_6
	},
	[1305] = {
		gameforge.locale.monster_chat_1305_1,
		gameforge.locale.monster_chat_1305_2,
		gameforge.locale.monster_chat_1305_3,
		gameforge.locale.monster_chat_1305_4,
		gameforge.locale.monster_chat_1305_5,
		gameforge.locale.monster_chat_1305_6
	},
	[1901] = {
		gameforge.locale.monster_chat_1901_1,
		gameforge.locale.monster_chat_1901_2,
		gameforge.locale.monster_chat_1901_3,
		gameforge.locale.monster_chat_1901_4,
		gameforge.locale.monster_chat_1901_5,
		gameforge.locale.monster_chat_1901_6
	},
	[2091] = {
		gameforge.locale.monster_chat_2091_1,
		gameforge.locale.monster_chat_2091_2,
		gameforge.locale.monster_chat_2091_3,
		gameforge.locale.monster_chat_2091_4,
		gameforge.locale.monster_chat_2091_5,
		gameforge.locale.monster_chat_2091_6
	},
	[2092] = {
		gameforge.locale.monster_chat_2092_1,
		gameforge.locale.monster_chat_2092_2,
		gameforge.locale.monster_chat_2092_3,
		gameforge.locale.monster_chat_2092_4,
		gameforge.locale.monster_chat_2092_5,
		gameforge.locale.monster_chat_2092_6
	},
	[2104] = {
		gameforge.locale.monster_chat_2104_1,
		gameforge.locale.monster_chat_2104_2,
		gameforge.locale.monster_chat_2104_3,
		gameforge.locale.monster_chat_2104_4,
		gameforge.locale.monster_chat_2104_5,
		gameforge.locale.monster_chat_2104_6
	},
	[2105] = {
		gameforge.locale.monster_chat_2105_1,
		gameforge.locale.monster_chat_2105_2,
		gameforge.locale.monster_chat_2105_3,
		gameforge.locale.monster_chat_2105_4,
		gameforge.locale.monster_chat_2105_5
	},
	[2106] = {
		gameforge.locale.monster_chat_2106_1,
		gameforge.locale.monster_chat_2106_2,
		gameforge.locale.monster_chat_2106_3,
		gameforge.locale.monster_chat_2106_4,
		gameforge.locale.monster_chat_2106_5,
		gameforge.locale.monster_chat_2106_6
	},
	[2107] = {
		gameforge.locale.monster_chat_2107_1,
		gameforge.locale.monster_chat_2107_2,
		gameforge.locale.monster_chat_2107_3,
		gameforge.locale.monster_chat_2107_4,
		gameforge.locale.monster_chat_2107_5,
		gameforge.locale.monster_chat_2107_6
	},
	[2108] = {
		gameforge.locale.monster_chat_2108_1,
		gameforge.locale.monster_chat_2108_2,
		gameforge.locale.monster_chat_2108_3,
		gameforge.locale.monster_chat_2108_4,
		gameforge.locale.monster_chat_2108_5,
		gameforge.locale.monster_chat_2108_6
	},
	[2191] = {
		gameforge.locale.monster_chat_2191_1,
		gameforge.locale.monster_chat_2191_2,
		gameforge.locale.monster_chat_2191_3,
		gameforge.locale.monster_chat_2191_4,
		gameforge.locale.monster_chat_2191_5,
		gameforge.locale.monster_chat_2191_6
	},
	[2202] = {
		gameforge.locale.monster_chat_2202_1,
		gameforge.locale.monster_chat_2202_2,
		gameforge.locale.monster_chat_2202_3,
		gameforge.locale.monster_chat_2202_4,
		gameforge.locale.monster_chat_2202_5,
		gameforge.locale.monster_chat_2202_6
	},
	[2204] = {
		gameforge.locale.monster_chat_2204_1,
		gameforge.locale.monster_chat_2204_2,
		gameforge.locale.monster_chat_2204_3,
		gameforge.locale.monster_chat_2204_4,
		gameforge.locale.monster_chat_2204_5,
		gameforge.locale.monster_chat_2204_6
	},
	[2205] = {
		gameforge.locale.monster_chat_2205_1,
		gameforge.locale.monster_chat_2205_2,
		gameforge.locale.monster_chat_2205_3,
		gameforge.locale.monster_chat_2205_4,
		gameforge.locale.monster_chat_2205_5,
		gameforge.locale.monster_chat_2205_6
	},
	[2206] = {
		gameforge.locale.monster_chat_2206_1,
		gameforge.locale.monster_chat_2206_2,
		gameforge.locale.monster_chat_2206_3,
		gameforge.locale.monster_chat_2206_4,
		gameforge.locale.monster_chat_2206_5,
		gameforge.locale.monster_chat_2206_6
	},
	[2291] = {
		gameforge.locale.monster_chat_2291_1,
		gameforge.locale.monster_chat_2291_2,
		gameforge.locale.monster_chat_2291_3,
		gameforge.locale.monster_chat_2291_4,
		gameforge.locale.monster_chat_2291_5,
		gameforge.locale.monster_chat_2291_6
	},
	[20001] = {
		gameforge.locale.monster_chat_20001_1,
		gameforge.locale.monster_chat_20001_2,
		gameforge.locale.monster_chat_20001_3,
		gameforge.locale.monster_chat_20001_4,
		gameforge.locale.monster_chat_20001_5
	},
	[20002] = {
		gameforge.locale.monster_chat_20002_1,
		gameforge.locale.monster_chat_20002_2,
		gameforge.locale.monster_chat_20002_3,
		gameforge.locale.monster_chat_20002_4,
		gameforge.locale.monster_chat_20002_5
	},
	[20003] = {
		gameforge.locale.monster_chat_20003_1,
		gameforge.locale.monster_chat_20003_2,
		gameforge.locale.monster_chat_20003_3,
		gameforge.locale.monster_chat_20003_4,
		gameforge.locale.monster_chat_20003_5
	},
	[20004] = {
		gameforge.locale.monster_chat_20004_1,
		gameforge.locale.monster_chat_20004_2,
		gameforge.locale.monster_chat_20004_3,
		gameforge.locale.monster_chat_20004_4,
		gameforge.locale.monster_chat_20004_5
	},
	[20005] = {
		gameforge.locale.monster_chat_20005_1,
		gameforge.locale.monster_chat_20005_2,
		gameforge.locale.monster_chat_20005_3,
		gameforge.locale.monster_chat_20005_4,
		gameforge.locale.monster_chat_20005_5
	},
	[20006] = {
		gameforge.locale.monster_chat_20006_1,
		gameforge.locale.monster_chat_20006_2,
		gameforge.locale.monster_chat_20006_3,
		gameforge.locale.monster_chat_20006_4,
		gameforge.locale.monster_chat_20006_5
	},
	[20007] = {
		gameforge.locale.monster_chat_20007_1,
		gameforge.locale.monster_chat_20007_2,
		gameforge.locale.monster_chat_20007_3,
		gameforge.locale.monster_chat_20007_4,
		gameforge.locale.monster_chat_20007_5
	},
	[20008] = {
		gameforge.locale.monster_chat_20008_1,
		gameforge.locale.monster_chat_20008_2,
		gameforge.locale.monster_chat_20008_3,
		gameforge.locale.monster_chat_20008_4,
		gameforge.locale.monster_chat_20008_5
	},
	[20009] = {
		gameforge.locale.monster_chat_20009_1,
		gameforge.locale.monster_chat_20009_2,
		gameforge.locale.monster_chat_20009_3,
		gameforge.locale.monster_chat_20009_4,
		gameforge.locale.monster_chat_20009_5
	},
	[20010] = {
		gameforge.locale.monster_chat_20010_1,
		gameforge.locale.monster_chat_20010_2,
		gameforge.locale.monster_chat_20010_3,
		gameforge.locale.monster_chat_20010_4,
		gameforge.locale.monster_chat_20010_5
	},
	[20011] = {
		gameforge.locale.monster_chat_20011_1,
		gameforge.locale.monster_chat_20011_2,
		gameforge.locale.monster_chat_20011_3,
		gameforge.locale.monster_chat_20011_4,
		gameforge.locale.monster_chat_20011_5
	},
	[20012] = {
		gameforge.locale.monster_chat_20012_1,
		gameforge.locale.monster_chat_20012_2,
		gameforge.locale.monster_chat_20012_3,
		gameforge.locale.monster_chat_20012_4,
		gameforge.locale.monster_chat_20012_5
	},
	[20013] = {
		gameforge.locale.monster_chat_20013_1,
		gameforge.locale.monster_chat_20013_2,
		gameforge.locale.monster_chat_20013_3,
		gameforge.locale.monster_chat_20013_4,
		gameforge.locale.monster_chat_20013_5
	},
	[20014] = {
		gameforge.locale.monster_chat_20014_1,
		gameforge.locale.monster_chat_20014_2,
		gameforge.locale.monster_chat_20014_3,
		gameforge.locale.monster_chat_20014_4,
		gameforge.locale.monster_chat_20014_5
	},
	[20015] = {
		gameforge.locale.monster_chat_20015_1,
		gameforge.locale.monster_chat_20015_2,
		gameforge.locale.monster_chat_20015_3,
		gameforge.locale.monster_chat_20015_4,
		gameforge.locale.monster_chat_20015_5
	},
	[20016] = {
		gameforge.locale.monster_chat_20016_1,
		gameforge.locale.monster_chat_20016_2,
		gameforge.locale.monster_chat_20016_3,
		gameforge.locale.monster_chat_20016_4,
		gameforge.locale.monster_chat_20016_5
	},
	[20017] = {
		gameforge.locale.monster_chat_20017_1,
		gameforge.locale.monster_chat_20017_2,
		gameforge.locale.monster_chat_20017_3,
		gameforge.locale.monster_chat_20017_4,
		gameforge.locale.monster_chat_20017_5
	},
	[20018] = {
		gameforge.locale.monster_chat_20018_1,
		gameforge.locale.monster_chat_20018_2,
		gameforge.locale.monster_chat_20018_3,
		gameforge.locale.monster_chat_20018_4,
		gameforge.locale.monster_chat_20018_5
	},
	[20019] = {
		gameforge.locale.monster_chat_20019_1,
		gameforge.locale.monster_chat_20019_2,
		gameforge.locale.monster_chat_20019_3,
		gameforge.locale.monster_chat_20019_4,
		gameforge.locale.monster_chat_20019_5
	},
	[20020] = {
		gameforge.locale.monster_chat_20020_1,
		gameforge.locale.monster_chat_20020_2,
		gameforge.locale.monster_chat_20020_3,
		gameforge.locale.monster_chat_20020_4,
		gameforge.locale.monster_chat_20020_5
	},
	[20021] = {
		gameforge.locale.monster_chat_20021_1,
		gameforge.locale.monster_chat_20021_2,
		gameforge.locale.monster_chat_20021_3,
		gameforge.locale.monster_chat_20021_4,
		gameforge.locale.monster_chat_20021_5
	},
	[20022] = {
		gameforge.locale.monster_chat_20022_1,
		gameforge.locale.monster_chat_20022_2,
		gameforge.locale.monster_chat_20022_3,
		gameforge.locale.monster_chat_20022_4,
		gameforge.locale.monster_chat_20022_5
	},
	[20023] = {
		gameforge.locale.monster_chat_20023_1,
		gameforge.locale.monster_chat_20023_2,
		gameforge.locale.monster_chat_20023_3,
		gameforge.locale.monster_chat_20023_4,
		gameforge.locale.monster_chat_20023_5
	},
	[20024] = {
		gameforge.locale.monster_chat_20024_1,
		gameforge.locale.monster_chat_20024_2,
		gameforge.locale.monster_chat_20024_3,
		gameforge.locale.monster_chat_20024_4,
		gameforge.locale.monster_chat_20024_5
	},
	[11000] = {
		gameforge.locale.monster_chat_11000_1,
		gameforge.locale.monster_chat_11000_2,
		gameforge.locale.monster_chat_11000_3,
		gameforge.locale.monster_chat_11000_4,
		gameforge.locale.monster_chat_11000_5,
		gameforge.locale.monster_chat_11000_6,
		gameforge.locale.monster_chat_11000_7,
		gameforge.locale.monster_chat_11000_8,
		gameforge.locale.monster_chat_11000_9,
		gameforge.locale.monster_chat_11000_10,
		gameforge.locale.monster_chat_11000_11,
		gameforge.locale.monster_chat_11000_12,
		gameforge.locale.monster_chat_11000_13,
		gameforge.locale.monster_chat_11000_14,
		gameforge.locale.monster_chat_11000_15,
		gameforge.locale.monster_chat_11000_16,
		gameforge.locale.monster_chat_11000_17,
		gameforge.locale.monster_chat_11000_18,
		gameforge.locale.monster_chat_11000_19,
		gameforge.locale.monster_chat_11000_20,
		gameforge.locale.monster_chat_11000_21,
		gameforge.locale.monster_chat_11000_22,
		gameforge.locale.monster_chat_11000_23,
		gameforge.locale.monster_chat_11000_24,
		gameforge.locale.monster_chat_11000_25,
		gameforge.locale.monster_chat_11000_26,
		gameforge.locale.monster_chat_11000_27,
		gameforge.locale.monster_chat_11000_28,
		gameforge.locale.monster_chat_11000_29,
		gameforge.locale.monster_chat_11000_30,
		gameforge.locale.monster_chat_11000_31
	},
	[11002] = {
		gameforge.locale.monster_chat_11002_1,
		gameforge.locale.monster_chat_11002_2,
		gameforge.locale.monster_chat_11002_3,
		gameforge.locale.monster_chat_11002_4,
		gameforge.locale.monster_chat_11002_5,
		gameforge.locale.monster_chat_11002_6,
		gameforge.locale.monster_chat_11002_7,
		gameforge.locale.monster_chat_11002_8,
		gameforge.locale.monster_chat_11002_9,
		gameforge.locale.monster_chat_11002_10,
		gameforge.locale.monster_chat_11002_11,
		gameforge.locale.monster_chat_11002_12,
		gameforge.locale.monster_chat_11002_13,
		gameforge.locale.monster_chat_11002_14,
		gameforge.locale.monster_chat_11002_15,
		gameforge.locale.monster_chat_11002_16,
		gameforge.locale.monster_chat_11002_17,
		gameforge.locale.monster_chat_11002_18,
		gameforge.locale.monster_chat_11002_19,
		gameforge.locale.monster_chat_11002_20,
		gameforge.locale.monster_chat_11002_21,
		gameforge.locale.monster_chat_11002_22,
		gameforge.locale.monster_chat_11002_23,
		gameforge.locale.monster_chat_11002_24,
		gameforge.locale.monster_chat_11002_25,
		gameforge.locale.monster_chat_11002_26,
		gameforge.locale.monster_chat_11002_27,
		gameforge.locale.monster_chat_11002_28,
		gameforge.locale.monster_chat_11002_29,
		gameforge.locale.monster_chat_11002_30,
		gameforge.locale.monster_chat_11002_31
	},
	[11004] = {
		gameforge.locale.monster_chat_11004_1,
		gameforge.locale.monster_chat_11004_2,
		gameforge.locale.monster_chat_11004_3,
		gameforge.locale.monster_chat_11004_4,
		gameforge.locale.monster_chat_11004_5,
		gameforge.locale.monster_chat_11004_6,
		gameforge.locale.monster_chat_11004_7,
		gameforge.locale.monster_chat_11004_8,
		gameforge.locale.monster_chat_11004_9,
		gameforge.locale.monster_chat_11004_10,
		gameforge.locale.monster_chat_11004_11,
		gameforge.locale.monster_chat_11004_12,
		gameforge.locale.monster_chat_11004_13,
		gameforge.locale.monster_chat_11004_14,
		gameforge.locale.monster_chat_11004_15,
		gameforge.locale.monster_chat_11004_16,
		gameforge.locale.monster_chat_11004_17,
		gameforge.locale.monster_chat_11004_18,
		gameforge.locale.monster_chat_11004_19,
		gameforge.locale.monster_chat_11004_20,
		gameforge.locale.monster_chat_11004_21,
		gameforge.locale.monster_chat_11004_22,
		gameforge.locale.monster_chat_11004_23,
		gameforge.locale.monster_chat_11004_24,
		gameforge.locale.monster_chat_11004_25,
		gameforge.locale.monster_chat_11004_26,
		gameforge.locale.monster_chat_11004_27,
		gameforge.locale.monster_chat_11004_28,
		gameforge.locale.monster_chat_11004_29,
		gameforge.locale.monster_chat_11004_30,
		gameforge.locale.monster_chat_11004_31
	},
}

special.questscroll = {
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_1_text,
		gameforge.locale.questscroll_1_mis,
		gameforge.locale.questscroll_1_hunt,
		25,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_2_text,
		gameforge.locale.questscroll_2_mis,
		gameforge.locale.questscroll_2_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_3_text,
		gameforge.locale.questscroll_3_mis,
		gameforge.locale.questscroll_3_hunt,
		1,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_4_text,
		gameforge.locale.questscroll_4_mis,
		gameforge.locale.questscroll_4_hunt,
		1,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_5_text,
		gameforge.locale.questscroll_5_mis,
		gameforge.locale.questscroll_5_hunt,
		1,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_6_text,
		gameforge.locale.questscroll_6_mis,
		gameforge.locale.questscroll_6_hunt,
		1,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_7_text,
		gameforge.locale.questscroll_7_mis,
		gameforge.locale.questscroll_7_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_8_text,
		gameforge.locale.questscroll_8_mis,
		gameforge.locale.questscroll_8_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_9_text,
		gameforge.locale.questscroll_9_mis,
		gameforge.locale.questscroll_9_hunt,
		25,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_10_text,
		gameforge.locale.questscroll_10_mis,
		gameforge.locale.questscroll_10_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_11_text,
		gameforge.locale.questscroll_11_mis,
		gameforge.locale.questscroll_11_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_12_text,
		gameforge.locale.questscroll_12_mis,
		gameforge.locale.questscroll_12_hunt,
		12,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_13_text,
		gameforge.locale.questscroll_13_mis,
		gameforge.locale.questscroll_13_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_14_text,
		gameforge.locale.questscroll_14_mis,
		gameforge.locale.questscroll_14_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_15_text,
		gameforge.locale.questscroll_15_mis,
		gameforge.locale.questscroll_15_hunt,
		13,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_16_text,
		gameforge.locale.questscroll_16_mis,
		gameforge.locale.questscroll_16_hunt,
		25,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_17_text,
		gameforge.locale.questscroll_17_mis,
		gameforge.locale.questscroll_17_hunt,
		20,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_18_text,
		gameforge.locale.questscroll_18_mis,
		gameforge.locale.questscroll_18_hunt,
		25,
	},
	{ 
		QUEST_SCROLL_TYPE_KILL_MOB, 
		gameforge.locale.questscroll_19_text,
		gameforge.locale.questscroll_19_mis,
		gameforge.locale.questscroll_19_hunt,
		25,
	},	
	{ 
		QUEST_SCROLL_TYPE_KILL_ANOTHER_EMPIRE, 
		gameforge.locale.questscroll_20_text,
		gameforge.locale.questscroll_20_mis,
		gameforge.locale.questscroll_20_hunt,
		4,
	},
}

locale.questscroll = {}
locale.questscroll.exist = gameforge.locale.questscroll.exist
locale.questscroll.finish = gameforge.locale.questscroll.finish
locale.questscroll.finish_button = gameforge.locale.questscroll.finish_button

locale.remove_polymorph_chat = gameforge.locale.remove_polymorph_chat
locale.remove_polymorph_msg = gameforge.locale.remove_polymorph_msg

locale.fortune_words = {
	{
		gameforge.locale.fortune_words_1_title,
		gameforge.locale.fortune_words_1_body
	},
	{
		gameforge.locale.fortune_words_2_title,
		gameforge.locale.fortune_words_2_body
	},
	{
		gameforge.locale.fortune_words_3_title,
		gameforge.locale.fortune_words_3_body
	},
	{
		gameforge.locale.fortune_words_4_title,
		gameforge.locale.fortune_words_4_body
	},
	{
		gameforge.locale.fortune_words_5_title,
		gameforge.locale.fortune_words_5_body
	},
	{
		gameforge.locale.fortune_words_6_title,
		gameforge.locale.fortune_words_6_body
	},
	{
		gameforge.locale.fortune_words_7_title,
		gameforge.locale.fortune_words_7_body
	},
}

locale.christmas_tree = {}
locale.christmas_tree.greet = gameforge.locale.christmas_tree.greet
locale.christmas_tree.sel_put_sock = gameforge.locale.christmas_tree.sel_put_sock
locale.christmas_tree.sel_no_need = gameforge.locale.christmas_tree.sel_no_need
locale.christmas_tree.sel_intro_event = gameforge.locale.christmas_tree.sel_intro_event
locale.christmas_tree.no_more_sock = gameforge.locale.christmas_tree.no_more_sock
locale.christmas_tree.gain_item_msg = gameforge.locale.christmas_tree.gain_item_msg
locale.christmas_tree.no_sock = gameforge.locale.christmas_tree.no_sock
locale.christmas_tree.not_sock_time = gameforge.locale.christmas_tree.not_sock_time
locale.christmas_tree.greet2 = gameforge.locale.christmas_tree.greet2
locale.christmas_tree.intro_msg = gameforge.locale.christmas_tree.intro_msg
locale.christmas_tree.under_level_10 = gameforge.locale.christmas_tree.under_level_10
locale.christmas_tree.christmas_say_title = gameforge.locale.christmas_tree.christmas_say_title

locale.christmas_santa = {}
locale.christmas_santa.intro_msg = gameforge.locale.christmas_santa.intro_msg
locale.christmas_santa.skill_book_buy_cost = 10000
locale.christmas_santa.skill_book_msg = locale.christmas_santa.skill_book_buy_cost .. gameforge.locale.christmas_santa.skill_book_msg
locale.christmas_santa.skill_book_sold_out_msg = gameforge.locale.christmas_santa.skill_book_sold_out_msg
locale.christmas_santa.hairdye_buy_cost = 10000
locale.christmas_santa.hairdye_msg = locale.christmas_santa.hairdye_buy_cost .. gameforge.locale.christmas_santa.hairdye_msg
locale.christmas_santa.hairdye_sold_out_msg = gameforge.locale.christmas_santa.hairdye_sold_out_msg
locale.christmas_santa.not_enough_money = gameforge.locale.christmas_santa.want_skillbook
locale.christmas_santa.want_skillbook = gameforge.locale.christmas_santa.want_skillbook
locale.christmas_santa.want_hairdye = gameforge.locale.christmas_santa.want_hairdye
locale.christmas_santa.no_need = gameforge.locale.christmas_santa.no_need
locale.christmas_santa.bye_msg = gameforge.locale.christmas_santa.bye_msg
locale.christmas_santa.select_buy = gameforge.locale.christmas_santa.select_buy
locale.christmas_santa.select_stop = gameforge.locale.christmas_santa.select_stop

locale.years_greetings = {}
locale.years_greetings.chat_greet = gameforge.locale.years_greetings.chat_greet
locale.years_greetings.greet_msg = gameforge.locale.years_greetings.greet_msg
locale.years_greetings.select_1 = gameforge.locale.years_greetings.select_1
locale.years_greetings.select_2 = gameforge.locale.years_greetings.select_2
locale.years_greetings.select_3 = gameforge.locale.years_greetings.select_3
locale.years_greetings.under_level_15 = gameforge.locale.years_greetings.under_level_15
locale.years_greetings.give_msg = gameforge.locale.years_greetings.give_msg
locale.years_greetings.not_enoguh_inventory = gameforge.locale.years_greetings.not_enoguh_inventory
locale.years_greetings.no_money_this_time = gameforge.locale.years_greetings.no_money_this_time
locale.years_greetings.bless_msg = gameforge.locale.years_greetings.bless_msg
locale.years_greetings.event_info = gameforge.locale.years_greetings.event_info 

locale.make_wonso = {}
locale.make_wonso.want_wonso1 = gameforge.locale.make_wonso.want_wonso1
locale.make_wonso.how_make = gameforge.locale.make_wonso.how_make
locale.make_wonso.how_make_detail = gameforge.locale.make_wonso.how_make_detail
locale.make_wonso.make_msg1 = gameforge.locale.make_wonso.make_msg1
locale.make_wonso.make_msg2 = gameforge.locale.make_wonso.make_msg2
locale.make_wonso.make_msg3 = gameforge.locale.make_wonso.make_msg3

locale.horse_menu = {}
locale.horse_menu.menu = gameforge.locale.horse_menu.menu
locale.horse_menu.show_state = gameforge.locale.horse_menu.show_state
locale.horse_menu.revive = gameforge.locale.horse_menu.revive
locale.horse_menu.feed = gameforge.locale.horse_menu.feed
locale.horse_menu.ride = gameforge.locale.horse_menu.ride
locale.horse_menu.unsummon = gameforge.locale.horse_menu.unsummon
locale.horse_menu.close = gameforge.locale.horse_menu.close

locale.monkey_dungeon = {}
locale.monkey_dungeon.button = gameforge.locale.monkey_dungeon.button
locale.monkey_dungeon.curse_of_monkey_button = gameforge.locale.monkey_dungeon.curse_of_monkey_button
locale.monkey_dungeon.curse_of_monkey = gameforge.locale.monkey_dungeon.curse_of_monkey
locale.monkey_dungeon.cannot_ENTER_yet = gameforge.locale.monkey_dungeon.cannot_ENTER_yet
locale.monkey_dungeon.low_entrance = gameforge.locale.monkey_dungeon.low_entrance
locale.monkey_dungeon.middle_entrance = gameforge.locale.monkey_dungeon.middle_entrance
locale.monkey_dungeon.notice = gameforge.locale.monkey_dungeon.notice
locale.monkey_dungeon.quest_title = gameforge.locale.monkey_dungeon.quest_title
locale.monkey_dungeon.quest_rest_time = gameforge.locale.monkey_dungeon.quest_rest_time
locale.monkey_dungeon.ENTER = gameforge.locale.monkey_dungeon.ENTER
locale.monkey_dungeon.no_ENTER = gameforge.locale.monkey_dungeon.no_ENTER

locale.questscroll5 = {}
locale.questscroll5.mission = {
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Aa",
		gameforge.locale.questscroll5.mission_1_hunt,
		gameforge.locale.questscroll5.mission_1_text,
		gameforge.locale.questscroll5.mission_1_mis,
		"8002",
		3
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Aa",
		gameforge.locale.questscroll5.mission_2_hunt,
		gameforge.locale.questscroll5.mission_2_text,
		gameforge.locale.questscroll5.mission_2_mis,
		"8001",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Aa",
		gameforge.locale.questscroll5.mission_3_hunt,
		gameforge.locale.questscroll5.mission_3_text,
		gameforge.locale.questscroll5.mission_3_mis,
		"107",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Aa",
		gameforge.locale.questscroll5.mission_4_hunt,
		gameforge.locale.questscroll5.mission_4_text,
		gameforge.locale.questscroll5.mission_4_mis,
		"106",
		35
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Aa",
		gameforge.locale.questscroll5.mission_5_hunt,
		gameforge.locale.questscroll5.mission_5_text,
		gameforge.locale.questscroll5.mission_5_mis,
		"108",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Aa",
		gameforge.locale.questscroll5.mission_6_hunt,
		gameforge.locale.questscroll5.mission_6_text,
		gameforge.locale.questscroll5.mission_6_mis,
		"114"	,
		25,
		"110",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ab",
		gameforge.locale.questscroll5.mission_7_hunt,
		gameforge.locale.questscroll5.mission_7_text,
		gameforge.locale.questscroll5.mission_7_mis,
		"8004",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ab",
		gameforge.locale.questscroll5.mission_8_hunt,
		gameforge.locale.questscroll5.mission_8_text,
		gameforge.locale.questscroll5.mission_8_mis,
		"8003",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ab",
		gameforge.locale.questscroll5.mission_9_hunt,
		gameforge.locale.questscroll5.mission_9_text,
		gameforge.locale.questscroll5.mission_9_mis,
		"109",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ab",
		gameforge.locale.questscroll5.mission_10_hunt,
		gameforge.locale.questscroll5.mission_10_text,
		gameforge.locale.questscroll5.mission_10_mis,
		"111",
		25,
		"112",
		25
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ab",
		gameforge.locale.questscroll5.mission_11_hunt,
		gameforge.locale.questscroll5.mission_11_text,
		gameforge.locale.questscroll5.mission_11_mis,
		"104",
		35,
		"105",
		25
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ba",
		gameforge.locale.questscroll5.mission_12_hunt,
		gameforge.locale.questscroll5.mission_12_text,
		gameforge.locale.questscroll5.mission_12_mis,
		"304",
		20
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ba",
		gameforge.locale.questscroll5.mission_13_hunt,
		gameforge.locale.questscroll5.mission_13_text,
		gameforge.locale.questscroll5.mission_13_mis,
		"404",
		25,
		"405",
		25
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ba",
		gameforge.locale.questscroll5.mission_14_hunt,
		gameforge.locale.questscroll5.mission_14_text,
		gameforge.locale.questscroll5.mission_14_mis,
		"631",
		30,
		"632",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ba",
		gameforge.locale.questscroll5.mission_15_hunt,
		gameforge.locale.questscroll5.mission_15_text,
		gameforge.locale.questscroll5.mission_15_mis,
		"501",
		30,
		"502",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Bb",
		gameforge.locale.questscroll5.mission_16_hunt,
		gameforge.locale.questscroll5.mission_16_text,
		gameforge.locale.questscroll5.mission_16_mis,
		"8006",
		3
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Bb",
		gameforge.locale.questscroll5.mission_17_hunt,
		gameforge.locale.questscroll5.mission_17_text,
		gameforge.locale.questscroll5.mission_17_mis,
		"8005",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Bb",
		gameforge.locale.questscroll5.mission_18_hunt,
		gameforge.locale.questscroll5.mission_18_text,
		gameforge.locale.questscroll5.mission_18_mis,
		"393",
		10
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Bb",
		gameforge.locale.questscroll5.mission_19_hunt,
		gameforge.locale.questscroll5.mission_19_text,
		gameforge.locale.questscroll5.mission_19_mis,
		"633",
		20,
		"634",
		15
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Bb",
		gameforge.locale.questscroll5.mission_20_hunt,
		gameforge.locale.questscroll5.mission_20_text,
		gameforge.locale.questscroll5.mission_20_mis,
		"5111",
		30,
		"5112",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Bb",
		gameforge.locale.questscroll5.mission_21_hunt,
		gameforge.locale.questscroll5.mission_21_text,
		gameforge.locale.questscroll5.mission_21_mis,
		"5101",
		30,
		"5102",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Bc",
		gameforge.locale.questscroll5.mission_22_hunt,
		gameforge.locale.questscroll5.mission_22_text,
		gameforge.locale.questscroll5.mission_22_mis,
		"494",
		4
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Bc",
		gameforge.locale.questscroll5.mission_23_hunt,
		gameforge.locale.questscroll5.mission_23_text,
		gameforge.locale.questscroll5.mission_23_mis,
		"8007",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Bc",
		gameforge.locale.questscroll5.mission_24_hunt,
		gameforge.locale.questscroll5.mission_24_text,
		gameforge.locale.questscroll5.mission_24_mis,
		"5113",
		15,
		"5114",
		15
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Bc",
		gameforge.locale.questscroll5.mission_25_hunt,
		gameforge.locale.questscroll5.mission_25_text,
		gameforge.locale.questscroll5.mission_25_mis,
		"5103",
		15,
		"5104",
		15
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ca",
		gameforge.locale.questscroll5.mission_26_hunt,
		gameforge.locale.questscroll5.mission_26_text,
		gameforge.locale.questscroll5.mission_26_mis,
		"8008",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ca",
		gameforge.locale.questscroll5.mission_27_hunt,
		gameforge.locale.questscroll5.mission_27_text,
		gameforge.locale.questscroll5.mission_27_mis,
		"2001",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Ca",
		gameforge.locale.questscroll5.mission_28_hunt,
		gameforge.locale.questscroll5.mission_28_text,
		gameforge.locale.questscroll5.mission_28_mis,
		"2002",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ca",
		gameforge.locale.questscroll5.mission_29_hunt,
		gameforge.locale.questscroll5.mission_29_text,
		gameforge.locale.questscroll5.mission_29_mis,
		"703",
		20,
		"704",
		25
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ca",
		gameforge.locale.questscroll5.mission_30_hunt,
		gameforge.locale.questscroll5.mission_30_text,
		gameforge.locale.questscroll5.mission_30_mis,
		"701",
		30,
		"702",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Ca",
		gameforge.locale.questscroll5.mission_31_hunt,
		gameforge.locale.questscroll5.mission_31_text,
		gameforge.locale.questscroll5.mission_31_mis,
		"706",
		40,
		"707",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Cb",
		gameforge.locale.questscroll5.mission_32_hunt,
		gameforge.locale.questscroll5.mission_32_text,
		gameforge.locale.questscroll5.mission_32_mis,
		"8009",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cb",
		gameforge.locale.questscroll5.mission_33_hunt,
		gameforge.locale.questscroll5.mission_33_text,
		gameforge.locale.questscroll5.mission_33_mis,
		"2031",
		35,
		"2032",
		35
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cb",
		gameforge.locale.questscroll5.mission_34_hunt,
		gameforge.locale.questscroll5.mission_34_text,
		gameforge.locale.questscroll5.mission_34_mis,
		"2031",
		35,
		"2032",
		35
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cb",
		gameforge.locale.questscroll5.mission_35_hunt,
		gameforge.locale.questscroll5.mission_35_text,
		gameforge.locale.questscroll5.mission_35_mis,
		"2033",
		35,
		"2034",
		35
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Cb",
		gameforge.locale.questscroll5.mission_36_hunt,
		gameforge.locale.questscroll5.mission_36_text,
		gameforge.locale.questscroll5.mission_36_mis,
		"2035",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Cc",
		gameforge.locale.questscroll5.mission_37_hunt,
		gameforge.locale.questscroll5.mission_37_text,
		gameforge.locale.questscroll5.mission_37_mis,
		"8010",
		2
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cc",
		gameforge.locale.questscroll5.mission_38_hunt,
		gameforge.locale.questscroll5.mission_38_text,
		gameforge.locale.questscroll5.mission_38_mis,
		"2103",
		30,
		"2104",
		20
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cc",
		gameforge.locale.questscroll5.mission_39_hunt,
		gameforge.locale.questscroll5.mission_39_text,
		gameforge.locale.questscroll5.mission_39_mis,
		"1001",
		30,
		"1002",
		20
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cc",
		gameforge.locale.questscroll5.mission_40_hunt,
		gameforge.locale.questscroll5.mission_40_text,
		gameforge.locale.questscroll5.mission_40_mis,
		"1003",
		30,
		"1004",
		25
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cd",
		gameforge.locale.questscroll5.mission_41_hunt,
		gameforge.locale.questscroll5.mission_41_text,
		gameforge.locale.questscroll5.mission_41_mis,
		"5123",
		15,
		"5124",
		15
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Cd",
		gameforge.locale.questscroll5.mission_42_hunt,
		gameforge.locale.questscroll5.mission_42_text,
		gameforge.locale.questscroll5.mission_42_mis,
		"1107",
		25
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cd",
		gameforge.locale.questscroll5.mission_43_hunt,
		gameforge.locale.questscroll5.mission_43_text,
		gameforge.locale.questscroll5.mission_43_mis,
		"5121",
		30,
		"5122",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Cd",
		gameforge.locale.questscroll5.mission_44_hunt,
		gameforge.locale.questscroll5.mission_44_text,
		gameforge.locale.questscroll5.mission_44_mis,
		"1305",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cd",
		gameforge.locale.questscroll5.mission_45_hunt,
		gameforge.locale.questscroll5.mission_45_text,
		gameforge.locale.questscroll5.mission_45_mis,
		"1105",
		35,
		"1106",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Cd",
		gameforge.locale.questscroll5.mission_46_hunt,
		gameforge.locale.questscroll5.mission_46_text,
		gameforge.locale.questscroll5.mission_46_mis,
		"1301",
		45,
		"1302",
		35
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Cd",
		gameforge.locale.questscroll5.mission_47_hunt,
		gameforge.locale.questscroll5.mission_47_text,
		gameforge.locale.questscroll5.mission_47_mis,
		"1101",
		45
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Da",
		gameforge.locale.questscroll5.mission_48_hunt,
		gameforge.locale.questscroll5.mission_48_text,
		gameforge.locale.questscroll5.mission_48_mis,
		"1063",
		45
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Da",
		gameforge.locale.questscroll5.mission_49_hunt,
		gameforge.locale.questscroll5.mission_49_text,
		gameforge.locale.questscroll5.mission_49_mis,
		"1064",
		60
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Da",
		gameforge.locale.questscroll5.mission_50_hunt,
		gameforge.locale.questscroll5.mission_50_text,
		gameforge.locale.questscroll5.mission_50_mis,
		"2201",
		45
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Db",
		gameforge.locale.questscroll5.mission_51_hunt,
		gameforge.locale.questscroll5.mission_51_text,
		gameforge.locale.questscroll5.mission_51_mis,
		"2202",
		45
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Db",
		gameforge.locale.questscroll5.mission_52_hunt,
		gameforge.locale.questscroll5.mission_52_text,
		gameforge.locale.questscroll5.mission_52_mis,
		"2201",
		60,
		"2203",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Db",
		gameforge.locale.questscroll5.mission_53_hunt,
		gameforge.locale.questscroll5.mission_53_text,
		gameforge.locale.questscroll5.mission_53_mis,
		"2204",
		40
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Db",
		gameforge.locale.questscroll5.mission_54_hunt,
		gameforge.locale.questscroll5.mission_54_text,
		gameforge.locale.questscroll5.mission_54_mis,
		"2205",
		60
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB,
		"Dc",
		gameforge.locale.questscroll5.mission_55_hunt,
		gameforge.locale.questscroll5.mission_55_text,
		gameforge.locale.questscroll5.mission_55_mis,
		"2301",
		30
	},
	{
		QUEST_SCROLL_TYPE_KILL_MOB2,
		"Dc",
		gameforge.locale.questscroll5.mission_56_hunt,
		gameforge.locale.questscroll5.mission_56_text,
		gameforge.locale.questscroll5.mission_56_mis,
		"2302",
		40,
		"2303",
		30
	},
}

locale.questscroll5.reward_exp = {
	["Aa"] = {
		{10000, 30},
		{20000, 40},
		{30000, 25},
		{50000,  3},
		{80000,  2},
	},
	["Ab"] = {
		{10000, 20},
		{20000, 30},
		{30000, 35},
		{50000, 10},
		{80000,  5},
	},
	["Ba"] = {
		{20000, 25},
		{30000, 35},
		{50000, 25},
		{80000, 10},
		{100000,  5},
	},
	["Bb"] = {
		{20000, 10},
		{30000, 25},
		{50000, 35},
		{80000, 20},
		{100000, 10},
	},
	["Bc"] = {
		{20000,  5},
		{30000, 15},
		{50000, 35},
		{80000, 30},
		{100000, 15},
	},
	["Ca"] = {
		{30000, 20},
		{50000, 40},
		{100000, 30},
		{150000,  8},
		{200000,  2},
	},
	["Cb"] = {
		{30000, 15},
		{50000, 30},
		{100000, 35},
		{150000, 10},
		{200000, 10},
	},
	["Cc"] = {
		{30000, 10},
		{50000, 15},
		{100000, 35},
		{150000, 25},
		{200000, 15},
	},
	["Cd"] = {
		{30000,  5},
		{50000, 10},
		{100000, 30},
		{150000, 35},
		{200000, 20},
	},
	["Da"] = {
		{30000, 20},
		{50000, 40},
		{100000, 30},
		{150000,  8},
		{200000,  2},
	},
	["Db"] = {
		{30000, 15},
		{50000, 30},
		{100000, 35},
		{150000, 10},
		{200000, 10},
	},
	["Dc"] = {
		{30000, 10},
		{50000, 15},
		{100000, 35},
		{150000, 25},
		{200000, 15},
	},
}
locale.questscroll5.reward_money = {
	["Aa"] = {
		{5000, 30},
		{10000, 40},
		{20000, 25},
		{50000, 3},
		{100000, 2},
	},
	["Ab"] = {
		{5000, 20},
		{10000, 30},
		{20000, 35},
		{50000, 10},
		{100000, 5},
	},
	["Ba"] = {
		{10000, 30},
		{30000, 40},
		{50000, 25},
		{80000, 3},
		{100000, 2},
	},
	["Bb"] = {
		{10000, 20},
		{30000, 30},
		{50000, 35},
		{80000, 10},
		{100000, 5},
	},
	["Bc"] = {
		{10000, 10},
		{30000, 20},
		{50000, 35},
		{80000, 30},
		{100000, 5},
	},
	["Ca"] = {
		{30000, 20},
		{50000, 40},
		{80000, 30},
		{100000, 8},
		{150000, 2},
	},
	["Cb"] = {
		{30000, 15},
		{50000, 30},
		{80000, 35},
		{100000, 10},
		{150000, 10},
	},
	["Cc"] = {
		{30000, 10},
		{50000, 15},
		{80000, 35},
		{100000, 25},
		{150000, 10},
	},
	["Cd"] = {
		{30000, 5},
		{50000, 10},
		{80000, 30},
		{100000, 35},
		{150000, 20},
	},
	["Da"] = {
		{30000, 20},
		{50000, 40},
		{80000, 30},
		{100000, 8},
		{150000, 2},
	},
	["Db"] = {
		{30000, 15},
		{50000, 30},
		{80000, 35},
		{100000, 10},
		{150000, 10},
	},
	["Dc"] = {
		{30000, 10},
		{50000, 15},
		{80000, 35},
		{100000, 25},
		{150000, 10},
	},
}
locale.questscroll5.reward_item = {
	["Aa"] = {
		{30053, 1},
		{30072, 1},
		{30071, 1},
		{30003, 1},
		{30023, 1},
		{30038, 1},
	},
	["Ab"] = {
		{30037, 1},
		{30010, 1},
		{30004, 1},
		{30028, 1},
		{30069, 1},
		{30027, 1},
		{30070, 1},
	},
	["Ba"] = {
		{30032, 1},
		{30034, 1},
		{30073, 1},
		{30035, 1},
		{30008, 1},
		{30005, 1},
		{30021, 1},
		{30033, 1},
		{30041, 1},
		{30092, 1},
	},
	["Bb"] = {
		{30074, 1},
		{30076, 1},
		{30056, 1},
		{30057, 1},
		{30006, 1},
		{30077, 1},
		{30030, 1},
		{30058, 1},
		{30025, 1},
		{30055, 1},
	},
	["Bc"] = {
		{30046, 1},
		{30045, 1},
		{30018, 1},
		{30007, 1},
		{30052, 1},
		{30081, 1},
		{30031, 1},
		{30017, 1},
		{30011, 1},
	},
	["Ca"] = {
		{30085, 1},
		{30088, 1},
		{30016, 1},
		{30061, 1},
		{30040, 1},
		{30091, 1},
		{30059, 1},
	},
	["Cb"] = {
		{30060, 1},
		{30039, 1},
		{30042, 1},
		{30009, 1},
		{30079, 1},
		{30075, 1},
		{30022, 1},
		{30087, 1},
	},
	["Cc"] = {
		{30015, 1},
		{30019, 1},
		{30014, 1},
		{30048, 1},
		{30078, 1},
		{30083, 1},
		{30086, 1},
		{30090, 1},
		{30089, 1},
	},
	["Cd"] = {
		{30049, 1},
		{30047, 1},
		{30067, 1},
		{30080, 1},
		{30051, 1},
		{30050, 1},
	},
	["Da"] = {
		{30045, 1},
		{30051, 1},
		{30038, 1},
		{30070, 1},
		{30023, 1},
		{30033, 1},
		{30011, 1},
		{30027, 1},
		{30082, 1},
	},
	["Db"] = {
		{30035, 1},
		{30010, 1},
		{30055, 1},
		{30006, 1},
		{30003, 1},
		{30017, 1},
		{30058, 1},
		{30086, 1},
	},
	["Dc"] = {
		{70048, 15},
		{70050, 15},
		{70051, 15},
		{70102, 5},
		{50513, 5},
		{50301, 20},
		{50302, 15},
		{50303, 10},
	},

}

locale.quiz = {
	{
		gameforge.locale.quiz_1_0_question,
		gameforge.locale.quiz_1_1_choice,
		gameforge.locale.quiz_1_2_choice,
		gameforge.locale.quiz_1_3_choice,
		gameforge.locale.quiz_1_4_choice,
		gameforge.locale.quiz_1_5_answer
	},
	{
		gameforge.locale.quiz_2_0_question,
		gameforge.locale.quiz_2_1_choice,
		gameforge.locale.quiz_2_2_choice,
		gameforge.locale.quiz_2_3_choice,
		gameforge.locale.quiz_2_4_choice,
		gameforge.locale.quiz_2_5_answer
	},
	{
		gameforge.locale.quiz_3_0_question,
		gameforge.locale.quiz_3_1_choice,
		gameforge.locale.quiz_3_2_choice,
		gameforge.locale.quiz_3_3_choice,
		gameforge.locale.quiz_3_4_choice,
		gameforge.locale.quiz_3_5_answer
	},
	{
		gameforge.locale.quiz_4_0_question,
		gameforge.locale.quiz_4_1_choice,
		gameforge.locale.quiz_4_2_choice,
		gameforge.locale.quiz_4_3_choice,
		gameforge.locale.quiz_4_4_choice,
		gameforge.locale.quiz_4_5_answer
	},
	{
		gameforge.locale.quiz_5_0_question,
		gameforge.locale.quiz_5_1_choice,
		gameforge.locale.quiz_5_2_choice,
		gameforge.locale.quiz_5_3_choice,
		gameforge.locale.quiz_5_4_choice,
		gameforge.locale.quiz_5_5_answer
	},
	{
		gameforge.locale.quiz_6_0_question,
		gameforge.locale.quiz_6_1_choice,
		gameforge.locale.quiz_6_2_choice,
		gameforge.locale.quiz_6_3_choice,
		gameforge.locale.quiz_6_4_choice,
		gameforge.locale.quiz_6_5_answer
	},
	{
		gameforge.locale.quiz_7_0_question,
		gameforge.locale.quiz_7_1_choice,
		gameforge.locale.quiz_7_2_choice,
		gameforge.locale.quiz_7_3_choice,
		gameforge.locale.quiz_7_4_choice,
		gameforge.locale.quiz_7_5_answer
	},
	{
		gameforge.locale.quiz_8_0_question,
		gameforge.locale.quiz_8_1_choice,
		gameforge.locale.quiz_8_2_choice,
		gameforge.locale.quiz_8_3_choice,
		gameforge.locale.quiz_8_4_choice,
		gameforge.locale.quiz_8_5_answer
	},
	{
		gameforge.locale.quiz_9_0_question,
		gameforge.locale.quiz_9_1_choice,
		gameforge.locale.quiz_9_2_choice,
		gameforge.locale.quiz_9_3_choice,
		gameforge.locale.quiz_9_4_choice,
		gameforge.locale.quiz_9_5_answer
	},
	{
		gameforge.locale.quiz_10_0_question,
		gameforge.locale.quiz_10_1_choice,
		gameforge.locale.quiz_10_2_choice,
		gameforge.locale.quiz_10_3_choice,
		gameforge.locale.quiz_10_4_choice,
		gameforge.locale.quiz_10_5_answer
	},
	{
		gameforge.locale.quiz_11_0_question,
		gameforge.locale.quiz_11_1_choice,
		gameforge.locale.quiz_11_2_choice,
		gameforge.locale.quiz_11_3_choice,
		gameforge.locale.quiz_11_4_choice,
		gameforge.locale.quiz_11_5_answer
	},
	{
		gameforge.locale.quiz_12_0_question,
		gameforge.locale.quiz_12_1_choice,
		gameforge.locale.quiz_12_2_choice,
		gameforge.locale.quiz_12_3_choice,
		gameforge.locale.quiz_12_4_choice,
		gameforge.locale.quiz_12_5_answer
	},
	{
		gameforge.locale.quiz_13_0_question,
		gameforge.locale.quiz_13_1_choice,
		gameforge.locale.quiz_13_2_choice,
		gameforge.locale.quiz_13_3_choice,
		gameforge.locale.quiz_13_4_choice,
		gameforge.locale.quiz_13_5_answer
	},
	{
		gameforge.locale.quiz_14_0_question,
		gameforge.locale.quiz_14_1_choice,
		gameforge.locale.quiz_14_2_choice,
		gameforge.locale.quiz_14_3_choice,
		gameforge.locale.quiz_14_4_choice,
		gameforge.locale.quiz_14_5_answer
	},
	{
		gameforge.locale.quiz_15_0_question,
		gameforge.locale.quiz_15_1_choice,
		gameforge.locale.quiz_15_2_choice,
		gameforge.locale.quiz_15_3_choice,
		gameforge.locale.quiz_15_4_choice,
		gameforge.locale.quiz_15_5_answer
	},
	{
		gameforge.locale.quiz_16_0_question,
		gameforge.locale.quiz_16_1_choice,
		gameforge.locale.quiz_16_2_choice,
		gameforge.locale.quiz_16_3_choice,
		gameforge.locale.quiz_16_4_choice,
		gameforge.locale.quiz_16_5_answer
	},
	{
		gameforge.locale.quiz_17_0_question,
		gameforge.locale.quiz_17_1_choice,
		gameforge.locale.quiz_17_2_choice,
		gameforge.locale.quiz_17_3_choice,
		gameforge.locale.quiz_17_4_choice,
		gameforge.locale.quiz_17_5_answer
	},
	{
		gameforge.locale.quiz_18_0_question,
		gameforge.locale.quiz_18_1_choice,
		gameforge.locale.quiz_18_2_choice,
		gameforge.locale.quiz_18_3_choice,
		gameforge.locale.quiz_18_4_choice,
		gameforge.locale.quiz_18_5_answer
	},
	{
		gameforge.locale.quiz_19_0_question,
		gameforge.locale.quiz_19_1_choice,
		gameforge.locale.quiz_19_2_choice,
		gameforge.locale.quiz_19_3_choice,
		gameforge.locale.quiz_19_4_choice,
		gameforge.locale.quiz_19_5_answer
	},
	{
		gameforge.locale.quiz_20_0_question,
		gameforge.locale.quiz_20_1_choice,
		gameforge.locale.quiz_20_2_choice,
		gameforge.locale.quiz_20_3_choice,
		gameforge.locale.quiz_20_4_choice,
		gameforge.locale.quiz_20_5_answer
	},
	{
		gameforge.locale.quiz_21_0_question,
		gameforge.locale.quiz_21_1_choice,
		gameforge.locale.quiz_21_2_choice,
		gameforge.locale.quiz_21_3_choice,
		gameforge.locale.quiz_21_4_choice,
		gameforge.locale.quiz_21_5_answer
	},
	{
		gameforge.locale.quiz_22_0_question,
		gameforge.locale.quiz_22_1_choice,
		gameforge.locale.quiz_22_2_choice,
		gameforge.locale.quiz_22_3_choice,
		gameforge.locale.quiz_22_4_choice,
		gameforge.locale.quiz_22_5_answer
	},
	{
		gameforge.locale.quiz_23_0_question,
		gameforge.locale.quiz_23_1_choice,
		gameforge.locale.quiz_23_2_choice,
		gameforge.locale.quiz_23_3_choice,
		gameforge.locale.quiz_23_4_choice,
		gameforge.locale.quiz_23_5_answer
	},
	{
		gameforge.locale.quiz_24_0_question,
		gameforge.locale.quiz_24_1_choice,
		gameforge.locale.quiz_24_2_choice,
		gameforge.locale.quiz_24_3_choice,
		gameforge.locale.quiz_24_4_choice,
		gameforge.locale.quiz_24_5_answer
	},
	{
		gameforge.locale.quiz_25_0_question,
		gameforge.locale.quiz_25_1_choice,
		gameforge.locale.quiz_25_2_choice,
		gameforge.locale.quiz_25_3_choice,
		gameforge.locale.quiz_25_4_choice,
		gameforge.locale.quiz_25_5_answer
	},
	{
		gameforge.locale.quiz_26_0_question,
		gameforge.locale.quiz_26_1_choice,
		gameforge.locale.quiz_26_2_choice,
		gameforge.locale.quiz_26_3_choice,
		gameforge.locale.quiz_26_4_choice,
		gameforge.locale.quiz_26_5_answer
	},
	{
		gameforge.locale.quiz_27_0_question,
		gameforge.locale.quiz_27_1_choice,
		gameforge.locale.quiz_27_2_choice,
		gameforge.locale.quiz_27_3_choice,
		gameforge.locale.quiz_27_4_choice,
		gameforge.locale.quiz_27_5_answer
	},
	{
		gameforge.locale.quiz_28_0_question,
		gameforge.locale.quiz_28_1_choice,
		gameforge.locale.quiz_28_2_choice,
		gameforge.locale.quiz_28_3_choice,
		gameforge.locale.quiz_28_4_choice,
		gameforge.locale.quiz_28_5_answer
	},
	{
		gameforge.locale.quiz_29_0_question,
		gameforge.locale.quiz_29_1_choice,
		gameforge.locale.quiz_29_2_choice,
		gameforge.locale.quiz_29_3_choice,
		gameforge.locale.quiz_29_4_choice,
		gameforge.locale.quiz_29_5_answer
	},
	{
		gameforge.locale.quiz_30_0_question,
		gameforge.locale.quiz_30_1_choice,
		gameforge.locale.quiz_30_2_choice,
		gameforge.locale.quiz_30_3_choice,
		gameforge.locale.quiz_30_4_choice,
		gameforge.locale.quiz_30_5_answer
	},
	{
		gameforge.locale.quiz_31_0_question,
		gameforge.locale.quiz_31_1_choice,
		gameforge.locale.quiz_31_2_choice,
		gameforge.locale.quiz_31_3_choice,
		gameforge.locale.quiz_31_4_choice,
		gameforge.locale.quiz_31_5_answer
	},
	{
		gameforge.locale.quiz_32_0_question,
		gameforge.locale.quiz_32_1_choice,
		gameforge.locale.quiz_32_2_choice,
		gameforge.locale.quiz_32_3_choice,
		gameforge.locale.quiz_32_4_choice,
		gameforge.locale.quiz_32_5_answer
	},
	{
		gameforge.locale.quiz_33_0_question,
		gameforge.locale.quiz_33_1_choice,
		gameforge.locale.quiz_33_2_choice,
		gameforge.locale.quiz_33_3_choice,
		gameforge.locale.quiz_33_4_choice,
		gameforge.locale.quiz_33_5_answer
	},
	{
		gameforge.locale.quiz_34_0_question,
		gameforge.locale.quiz_34_1_choice,
		gameforge.locale.quiz_34_2_choice,
		gameforge.locale.quiz_34_3_choice,
		gameforge.locale.quiz_34_4_choice,
		gameforge.locale.quiz_34_5_answer
	},
	{
		gameforge.locale.quiz_35_0_question,
		gameforge.locale.quiz_35_1_choice,
		gameforge.locale.quiz_35_2_choice,
		gameforge.locale.quiz_35_3_choice,
		gameforge.locale.quiz_35_4_choice,
		gameforge.locale.quiz_35_5_answer
	},
	{
		gameforge.locale.quiz_36_0_question,
		gameforge.locale.quiz_36_1_choice,
		gameforge.locale.quiz_36_2_choice,
		gameforge.locale.quiz_36_3_choice,
		gameforge.locale.quiz_36_4_choice,
		gameforge.locale.quiz_36_5_answer
	},
	{
		gameforge.locale.quiz_37_0_question,
		gameforge.locale.quiz_37_1_choice,
		gameforge.locale.quiz_37_2_choice,
		gameforge.locale.quiz_37_3_choice,
		gameforge.locale.quiz_37_4_choice,
		gameforge.locale.quiz_37_5_answer
	},
	{
		gameforge.locale.quiz_38_0_question,
		gameforge.locale.quiz_38_1_choice,
		gameforge.locale.quiz_38_2_choice,
		gameforge.locale.quiz_38_3_choice,
		gameforge.locale.quiz_38_4_choice,
		gameforge.locale.quiz_38_5_answer
	},
	{
		gameforge.locale.quiz_39_0_question,
		gameforge.locale.quiz_39_1_choice,
		gameforge.locale.quiz_39_2_choice,
		gameforge.locale.quiz_39_3_choice,
		gameforge.locale.quiz_39_4_choice,
		gameforge.locale.quiz_39_5_answer
	},
	{
		gameforge.locale.quiz_40_0_question,
		gameforge.locale.quiz_40_1_choice,
		gameforge.locale.quiz_40_2_choice,
		gameforge.locale.quiz_40_3_choice,
		gameforge.locale.quiz_40_4_choice,
		gameforge.locale.quiz_40_5_answer
	},
	{
		gameforge.locale.quiz_41_0_question,
		gameforge.locale.quiz_41_1_choice,
		gameforge.locale.quiz_41_2_choice,
		gameforge.locale.quiz_41_3_choice,
		gameforge.locale.quiz_41_4_choice,
		gameforge.locale.quiz_41_5_answer
	},
	{
		gameforge.locale.quiz_42_0_question,
		gameforge.locale.quiz_42_1_choice,
		gameforge.locale.quiz_42_2_choice,
		gameforge.locale.quiz_42_3_choice,
		gameforge.locale.quiz_42_4_choice,
		gameforge.locale.quiz_42_5_answer
	},
	{
		gameforge.locale.quiz_43_0_question,
		gameforge.locale.quiz_43_1_choice,
		gameforge.locale.quiz_43_2_choice,
		gameforge.locale.quiz_43_3_choice,
		gameforge.locale.quiz_43_4_choice,
		gameforge.locale.quiz_43_5_answer
	},
	{
		gameforge.locale.quiz_44_0_question,
		gameforge.locale.quiz_44_1_choice,
		gameforge.locale.quiz_44_2_choice,
		gameforge.locale.quiz_44_3_choice,
		gameforge.locale.quiz_44_4_choice,
		gameforge.locale.quiz_44_5_answer
	},
	{
		gameforge.locale.quiz_45_0_question,
		gameforge.locale.quiz_45_1_choice,
		gameforge.locale.quiz_45_2_choice,
		gameforge.locale.quiz_45_3_choice,
		gameforge.locale.quiz_45_4_choice,
		gameforge.locale.quiz_45_5_answer
	},
	{
		gameforge.locale.quiz_46_0_question,
		gameforge.locale.quiz_46_1_choice,
		gameforge.locale.quiz_46_2_choice,
		gameforge.locale.quiz_46_3_choice,
		gameforge.locale.quiz_46_4_choice,
		gameforge.locale.quiz_46_5_answer
	},
	{
		gameforge.locale.quiz_47_0_question,
		gameforge.locale.quiz_47_1_choice,
		gameforge.locale.quiz_47_2_choice,
		gameforge.locale.quiz_47_3_choice,
		gameforge.locale.quiz_47_4_choice,
		gameforge.locale.quiz_47_5_answer
	},
	{
		gameforge.locale.quiz_48_0_question,
		gameforge.locale.quiz_48_1_choice,
		gameforge.locale.quiz_48_2_choice,
		gameforge.locale.quiz_48_3_choice,
		gameforge.locale.quiz_48_4_choice,
		gameforge.locale.quiz_48_5_answer
	},
	{
		gameforge.locale.quiz_49_0_question,
		gameforge.locale.quiz_49_1_choice,
		gameforge.locale.quiz_49_2_choice,
		gameforge.locale.quiz_49_3_choice,
		gameforge.locale.quiz_49_4_choice,
		gameforge.locale.quiz_49_5_answer
	},
	{
		gameforge.locale.quiz_50_0_question,
		gameforge.locale.quiz_50_1_choice,
		gameforge.locale.quiz_50_2_choice,
		gameforge.locale.quiz_50_3_choice,
		gameforge.locale.quiz_50_4_choice,
		gameforge.locale.quiz_50_5_answer
	},
	{
		gameforge.locale.quiz_51_0_question,
		gameforge.locale.quiz_51_1_choice,
		gameforge.locale.quiz_51_2_choice,
		gameforge.locale.quiz_51_3_choice,
		gameforge.locale.quiz_51_4_choice,
		gameforge.locale.quiz_51_5_answer
	},
	{
		gameforge.locale.quiz_52_0_question,
		gameforge.locale.quiz_52_1_choice,
		gameforge.locale.quiz_52_2_choice,
		gameforge.locale.quiz_52_3_choice,
		gameforge.locale.quiz_52_4_choice,
		gameforge.locale.quiz_52_5_answer
	},
	{
		gameforge.locale.quiz_53_0_question,
		gameforge.locale.quiz_53_1_choice,
		gameforge.locale.quiz_53_2_choice,
		gameforge.locale.quiz_53_3_choice,
		gameforge.locale.quiz_53_4_choice,
		gameforge.locale.quiz_53_5_answer
	},
	{
		gameforge.locale.quiz_54_0_question,
		gameforge.locale.quiz_54_1_choice,
		gameforge.locale.quiz_54_2_choice,
		gameforge.locale.quiz_54_3_choice,
		gameforge.locale.quiz_54_4_choice,
		gameforge.locale.quiz_54_5_answer
	},
	{
		gameforge.locale.quiz_55_0_question,
		gameforge.locale.quiz_55_1_choice,
		gameforge.locale.quiz_55_2_choice,
		gameforge.locale.quiz_55_3_choice,
		gameforge.locale.quiz_55_4_choice,
		gameforge.locale.quiz_55_5_answer
	},
	{
		gameforge.locale.quiz_56_0_question,
		gameforge.locale.quiz_56_1_choice,
		gameforge.locale.quiz_56_2_choice,
		gameforge.locale.quiz_56_3_choice,
		gameforge.locale.quiz_56_4_choice,
		gameforge.locale.quiz_56_5_answer
	},
	{
		gameforge.locale.quiz_57_0_question,
		gameforge.locale.quiz_57_1_choice,
		gameforge.locale.quiz_57_2_choice,
		gameforge.locale.quiz_57_3_choice,
		gameforge.locale.quiz_57_4_choice,
		gameforge.locale.quiz_57_5_answer
	},
	{
		gameforge.locale.quiz_58_0_question,
		gameforge.locale.quiz_58_1_choice,
		gameforge.locale.quiz_58_2_choice,
		gameforge.locale.quiz_58_3_choice,
		gameforge.locale.quiz_58_4_choice,
		gameforge.locale.quiz_58_5_answer
	},
	{
		gameforge.locale.quiz_59_0_question,
		gameforge.locale.quiz_59_1_choice,
		gameforge.locale.quiz_59_2_choice,
		gameforge.locale.quiz_59_3_choice,
		gameforge.locale.quiz_59_4_choice,
		gameforge.locale.quiz_59_5_answer
	},
	{
		gameforge.locale.quiz_60_0_question,
		gameforge.locale.quiz_60_1_choice,
		gameforge.locale.quiz_60_2_choice,
		gameforge.locale.quiz_60_3_choice,
		gameforge.locale.quiz_60_4_choice,
		gameforge.locale.quiz_60_5_answer
	},
	{
		gameforge.locale.quiz_61_0_question,
		gameforge.locale.quiz_61_1_choice,
		gameforge.locale.quiz_61_2_choice,
		gameforge.locale.quiz_61_3_choice,
		gameforge.locale.quiz_61_4_choice,
		gameforge.locale.quiz_61_5_answer
	},
	{
		gameforge.locale.quiz_62_0_question,
		gameforge.locale.quiz_62_1_choice,
		gameforge.locale.quiz_62_2_choice,
		gameforge.locale.quiz_62_3_choice,
		gameforge.locale.quiz_62_4_choice,
		gameforge.locale.quiz_62_5_answer
	},
	{
		gameforge.locale.quiz_63_0_question,
		gameforge.locale.quiz_63_1_choice,
		gameforge.locale.quiz_63_2_choice,
		gameforge.locale.quiz_63_3_choice,
		gameforge.locale.quiz_63_4_choice,
		gameforge.locale.quiz_63_5_answer
	},
	{
		gameforge.locale.quiz_64_0_question,
		gameforge.locale.quiz_64_1_choice,
		gameforge.locale.quiz_64_2_choice,
		gameforge.locale.quiz_64_3_choice,
		gameforge.locale.quiz_64_4_choice,
		gameforge.locale.quiz_64_5_answer
	},
	{
		gameforge.locale.quiz_65_0_question,
		gameforge.locale.quiz_65_1_choice,
		gameforge.locale.quiz_65_2_choice,
		gameforge.locale.quiz_65_3_choice,
		gameforge.locale.quiz_65_4_choice,
		gameforge.locale.quiz_65_5_answer
	},
	{
		gameforge.locale.quiz_66_0_question,
		gameforge.locale.quiz_66_1_choice,
		gameforge.locale.quiz_66_2_choice,
		gameforge.locale.quiz_66_3_choice,
		gameforge.locale.quiz_66_4_choice,
		gameforge.locale.quiz_66_5_answer
	},
	{
		gameforge.locale.quiz_67_0_question,
		gameforge.locale.quiz_67_1_choice,
		gameforge.locale.quiz_67_2_choice,
		gameforge.locale.quiz_67_3_choice,
		gameforge.locale.quiz_67_4_choice,
		gameforge.locale.quiz_67_5_answer
	},
	{
		gameforge.locale.quiz_68_0_question,
		gameforge.locale.quiz_68_1_choice,
		gameforge.locale.quiz_68_2_choice,
		gameforge.locale.quiz_68_3_choice,
		gameforge.locale.quiz_68_4_choice,
		gameforge.locale.quiz_68_5_answer
	},
	{
		gameforge.locale.quiz_69_0_question,
		gameforge.locale.quiz_69_1_choice,
		gameforge.locale.quiz_69_2_choice,
		gameforge.locale.quiz_69_3_choice,
		gameforge.locale.quiz_69_4_choice,
		gameforge.locale.quiz_69_5_answer
	},
	{
		gameforge.locale.quiz_70_0_question,
		gameforge.locale.quiz_70_1_choice,
		gameforge.locale.quiz_70_2_choice,
		gameforge.locale.quiz_70_3_choice,
		gameforge.locale.quiz_70_4_choice,
		gameforge.locale.quiz_70_5_answer
	},
	{
		gameforge.locale.quiz_71_0_question,
		gameforge.locale.quiz_71_1_choice,
		gameforge.locale.quiz_71_2_choice,
		gameforge.locale.quiz_71_3_choice,
		gameforge.locale.quiz_71_4_choice,
		gameforge.locale.quiz_71_5_answer
	},
}

locale.GM_SKILL_NAME_DICT = {
	[1] = gameforge.locale.gm_skill_name_1,
	[2] = gameforge.locale.gm_skill_name_2,
	[3] = gameforge.locale.gm_skill_name_3,
	[4] = gameforge.locale.gm_skill_name_4,
	[5] = gameforge.locale.gm_skill_name_5,
	[16] = gameforge.locale.gm_skill_name_6,
	[17] = gameforge.locale.gm_skill_name_7,
	[18] = gameforge.locale.gm_skill_name_8,
	[19] = gameforge.locale.gm_skill_name_9,
	[20] = gameforge.locale.gm_skill_name_10,
	[31] = gameforge.locale.gm_skill_name_11,
	[32] = gameforge.locale.gm_skill_name_12,
	[33] = gameforge.locale.gm_skill_name_13,
	[34] = gameforge.locale.gm_skill_name_14,
	[35] = gameforge.locale.gm_skill_name_15,
	[46] = gameforge.locale.gm_skill_name_16,
	[47] = gameforge.locale.gm_skill_name_17,
	[48] = gameforge.locale.gm_skill_name_18,
	[49] = gameforge.locale.gm_skill_name_19,
	[50] = gameforge.locale.gm_skill_name_20,
	[61] = gameforge.locale.gm_skill_name_21,
	[62] = gameforge.locale.gm_skill_name_22,
	[63] = gameforge.locale.gm_skill_name_23,
	[64] = gameforge.locale.gm_skill_name_24,
	[65] = gameforge.locale.gm_skill_name_25,
	[66] = gameforge.locale.gm_skill_name_26,
	[76] = gameforge.locale.gm_skill_name_27,
	[77] = gameforge.locale.gm_skill_name_28,
	[78] = gameforge.locale.gm_skill_name_29,
	[79] = gameforge.locale.gm_skill_name_30,
	[80] = gameforge.locale.gm_skill_name_31,
	[81] = gameforge.locale.gm_skill_name_32,
	[91] = gameforge.locale.gm_skill_name_33,
	[92] = gameforge.locale.gm_skill_name_34,
	[93] = gameforge.locale.gm_skill_name_35,
	[94] = gameforge.locale.gm_skill_name_36,
	[95] = gameforge.locale.gm_skill_name_37,
	[96] = gameforge.locale.gm_skill_name_38,
	[106] = gameforge.locale.gm_skill_name_39,
	[107] = gameforge.locale.gm_skill_name_40,
	[108] = gameforge.locale.gm_skill_name_41,
	[109] = gameforge.locale.gm_skill_name_42,
	[110] = gameforge.locale.gm_skill_name_43,
	[111] = gameforge.locale.gm_skill_name_44,
}
locale.dungeon = {}
locale.dungeon.time_limit_party = "Das Dungeon-Party sosokwon nicht verstrichene Zeit ist begrenzt, da die Position wurde vom Platz gestellt."
locale.dungeon.time_limit = "Dungeon Position Grenzen wurden ausgeschaltet, weil die Zeit noch nicht abgelaufen ist, geschickt."
locale.dungeon.no_ticket_party = "Rote Karten haben keinen Pass Zertifikat der Partei waren sosokwon."
locale.dungeon.no_ticket = "Verhandelbar Zertifikate wurden nicht abgeschickt."
locale.dungeon.level_limit_party = "Parteien nicht erreichen sosokwon %d Lv geschickt wurde."
locale.dungeon.level_limit = "%d Lv nicht erreicht wurden abgeschickt."
locale.dungeon.leader_can_go = "Der Führer der Partei, aber Sie können gehen."
locale.dungeon.enter_yes = "Gib."
locale.dungeon.enter_no = "Geben Sie nicht."
locale.dungeon.leader_can_enter = "Der Führer der Partei, kann aber für die Zulassung beantragen."
locale.dungeon.party_can_enter = "Das Verlies Eingang ist eine Partei an den Staat."

locale.flame_dungeon = {}
locale.flame_dungeon.say_1 = "Yamacheon yeomwang eine Position zu bestrafen sicher?"
locale.flame_dungeon.say_2 = "Position"
locale.flame_dungeon.say_3 = "schließen"
locale.flame_dungeon.say_4 = "Partei Kapitel 103Lv nicht füllen Sie alle Quests, sind weitere Fortschritte nicht möglich."
locale.flame_dungeon.say_5 = "Das Dungeon-Party sosokwon nicht verstrichene Zeit ist begrenzt, da die Position wurde vom Platz gestellt."
locale.flame_dungeon.say_6 = "Dungeon Position Grenzen wurden ausgeschaltet, weil die Zeit noch nicht abgelaufen ist, geschickt."
locale.flame_dungeon.say_7 = "Diese Stelle kann nicht sosokwon Partei Zertifizierung wurde abgeschickt."
locale.flame_dungeon.say_8 = "Verhandelbar Zertifikate wurden nicht abgeschickt."
locale.flame_dungeon.say_9 = "Sosokwon beiden Parteien nicht erreichen 100lv geschickt wurde."
locale.flame_dungeon.say_10 = "Noch nicht erreicht wurde, beenden 100lv."
locale.flame_dungeon.say_11 = "Jeokryong Schloss"
locale.flame_dungeon.say_12 = "Wollen Sie zum Schloss wirklich jeokryong gehen?"
locale.flame_dungeon.say_13 = "Gib"
locale.flame_dungeon.say_14 = "Geben Sie nicht."
locale.flame_dungeon.say_15 = "Der Führer der Partei, kann aber für die Zulassung beantragen."
locale.flame_dungeon.say_16 = "Das Verlies Eingang ist eine Partei an den Staat."
locale.flame_dungeon.say_17 = "Sowohl die Verweilzeit abgelaufen ist, wird der Eintrag jeokryong gehen aus der Stadt."
locale.flame_dungeon.say_18 = "Retentionszeit jeokryong sex position %d Minuten."
locale.flame_dungeon.say_19 = "Alle Sex-Stellungen jeokryong Verweilzeit abgelaufen."
locale.flame_dungeon.say_20 = "Jeokryong wird aus der Stadt etwa 10 Sekunden bewegt."
locale.flame_dungeon.say_21 = "Jeokryong Geschlecht Verhalten."
locale.flame_dungeon.say_22 = "Der Führer der Partei, aber Sie können gehen."
locale.flame_dungeon.say_23 = "Jeokryong daegeon ausgeschaltet ist."
locale.flame_dungeon.say_24 = "Jeokryong sex position wird 1 Stunde, vollständig oder in Entwicklung während jeokryong Stern wurde 30 Minuten gesendet aufrechterhalten werden, wenn die Position ist begrenzt."
locale.flame_dungeon.say_25 = "Jeokryong Stunde Sex-Stellungen zu halten überlassen."
locale.flame_dungeon.say_26 = "Daegeon jeokryong ausgeschaltet ist, einen Platz zum alles Böse zu besiegen."
locale.flame_dungeon.say_27 = "Alles Böse wird nicht besiegt noch."
locale.flame_dungeon.say_28 = "Tötet die restlichen: "
locale.flame_dungeon.say_29 = "Daegeon jeokryong ausgeschaltet ist, um einen Platz für alle entsiegeln finden."
locale.flame_dungeon.say_30 = "Erforderlich, um die bösen Zähne erhalten lassen, wenn die Behandlung ist möglich."
locale.flame_dungeon.say_31 = "Jean Ritzel Dichtung kann nur off ausgeschaltet werden."
locale.flame_dungeon.say_32 = "Daegeon jeokryong freigegeben wird, um herauszufinden, wo die geheimnisvolle Jean Verliehen Incinerator."
locale.flame_dungeon.say_33 = "Jin lebt Incinerator Bars, jeokryong sex Dichtung ist fest."
locale.flame_dungeon.say_34 = "Verliehen Jean Incinerator finden Bars."
locale.flame_dungeon.say_35 = "Daegeon jeokryong freigegeben worden, um herauszufinden, wo man entsiegeln."
locale.flame_dungeon.say_36 = "Sealing Stone Church an der Wende jeokryong Böse erhalten wird."
locale.flame_dungeon.say_37 = "Sealing Stein wurde nicht beide entlassen."
locale.flame_dungeon.say_38 = "Jeokryong daegeon Di hearthstone ausgeschaltet ist, einen Ort zu zerstören finden."
locale.flame_dungeon.say_39 = "Ich glaube nicht, Ruhestein Di zerstört werden, einer nach dem anderen, und sie sind böse."
locale.flame_dungeon.say_40 = "Di zu finden und zerstören den Ruhestein."
locale.flame_dungeon.say_41 = "Halten yamacheon Beat"
locale.flame_dungeon.say_42 = "Alle Monster besiegt wurden."
locale.flame_dungeon.say_43 = "Jeokryong Verhältnis im Laufe des nächsten jeokryong daegeon freizugeben."
locale.flame_dungeon.say_44 = "Beide wurden abgeriegelt."
locale.flame_dungeon.say_45 = "Der eigentliche Schlüssel ist nicht"
locale.flame_dungeon.say_46 = "Bullion wurde von Jin Incinerator besiegt."
locale.flame_dungeon.say_47 = "Bitte zerlegen Sie das zweite Verzauberung"
locale.flame_dungeon.say_48 = "Bitte bauen Sie die dritte Chains."
locale.flame_dungeon.say_49 = "Falsche Reihenfolge."
locale.flame_dungeon.say_50 = "Bitte bauen Sie die vierte Chains."
locale.flame_dungeon.say_51 = "Ketten von vier oder fünf ersten Besuch demontieren."
locale.flame_dungeon.say_52 = "Ketten von vier oder fünf ersten Besuch demontieren."
locale.flame_dungeon.say_53 = "Ketten von vier oder fünf ersten Besuch demontieren."
locale.flame_dungeon.say_54 = "Di hearthstone besiegt wurde."
locale.flame_dungeon.say_55 = "Yeomwang yamacheon besiegt wurde."
locale.flame_dungeon.say_56 = "Sind aus der Stadt nach einer Minute jeokryong bewegt."
locale.flame_dungeon.say_57 = "Parteien nicht erreichen Kapitel 104Lv weitere Fortschritte ist nicht möglich."
locale.flame_dungeon.say_58 = "Partei kann nicht weiter vorgehen Behinderung verschwinden [ENTER]"
locale.flame_dungeon.say_59 = "Re-entry Zeit ist mehr als fünf Minuten ist nicht erlaubt. [ENTER]"
locale.flame_dungeon.say_60 = "Partei nicht mit Dungeon hayeotgeona starten."

locale.main_quest_flame_lv99 = {}
locale.main_quest_flame_lv99.say_1 = "다시 시작되는 추적"
locale.main_quest_flame_lv99.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv99.say_3 = "어서 오게나.[ENTER]일전에는 초마단의 술법을 저지하느라 고생이 많았었지.[ENTER]나는 초마단이 분명 이대로 물러나지 않을 것이라 생각하고 있었다네.[ENTER]그래서 내가 동원할 수 있는 모든 정보망을 가동해서, 추적을 계속하고 있었다네.[ENTER]"
locale.main_quest_flame_lv99.say_4 = "네. 이렇게 부르셨으니 무언가 나온 것이군요?[ENTER]"
locale.main_quest_flame_lv99.say_5 = "그렇지.[ENTER]그런데 말 그대로 심증일 뿐 직접 확인을 하지는 못한 상태라네.[ENTER]그래서 자네가 좀 나를 도와서 확인을 해주었으면 한다네.[ENTER]"
locale.main_quest_flame_lv99.say_6 = "최근들어서 도염화지가 심상치 않다는 정보가 자주 들리는 것으로 보아, 부하를 시켜 확인을 해 보려 했다네.[ENTER]무예가 출중한 부하임에도 불구하고 돌아오지 않고 있다네.[ENTER]도염화지의 어지간한 마물도 어쩌지 못하는 녀석인데, 분명 무언가 일이 터진 것 같다네.[ENTER]자네가 도염화지로가서, 무슨일이 생긴 것인지 보고 와주게나.[ENTER]부하녀석을 찾거든 속히 귀환하라는 말도 전해주고 말이야.[ENTER]"
locale.main_quest_flame_lv99.say_7 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv99.say_8 = "부하와 전서구로 서신을 주고 받던 중, 대략적인 위치는 파악이 된 상태니 내가 지도에 표시해준 곳으로 가보면 될 것일세.[ENTER]다녀 오게나.[ENTER]"
locale.main_quest_flame_lv99.say_9 = "약환(20377)이 도염화지 지도에 표시를 해주었습니다.[ENTER]확인할 지점은 (%d,%d) 입니다."
locale.main_quest_flame_lv99.say_10 = "도염화지를 정찰하자!"
locale.main_quest_flame_lv99.say_11 = "약환(20377)이 알려 준 곳을 정찰하도록 하자.[ENTER]"
locale.main_quest_flame_lv99.say_12 = "내가 지도에 표시해준 곳에 분명 무언가 있을 것이네.[ENTER]자세히 돌아보고 온 것이 맞는가?[ENTER]시급을 다투는 일이니 어서 다녀와 주게나.[ENTER]"
locale.main_quest_flame_lv99.say_13 = "약환(20377)이 알려준 지역에 도착하였습니다.[ENTER]약환(20377)에게 돌아가 보고하십시오."
locale.main_quest_flame_lv99.say_14 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv99.say_15 = "오! 돌아 왔군 그래.[ENTER]무언가 좀 나왔는가?[ENTER]"
locale.main_quest_flame_lv99.say_16 = "지도에 표시해주신 곳에 가보았으나, 아쉽게도 부하분의 행방은 찾지 못하였습니다.[ENTER]주변을 살펴본 결과 마물들이 소리가 우렁차게 들리고, 을씨년스러운 동굴을 하나 발견하였습니다.[ENTER]"
locale.main_quest_flame_lv99.say_17 = "그렇군.[ENTER]아무래도 무언가 불안하군 그래.[ENTER]자네가 말한 지역에 대해서 더 조사하고 다시 부르도록 하겠네.[ENTER]"
locale.main_quest_flame_lv99.say_18 = "조사1-1"
locale.main_quest_flame_lv99.say_19 = "ÀÚ³×°¡ ³ª¿¡°Ô ¾Ë·ÁÁØ ±× µ¿±¼¿¡ ´ëÇØ¼­ ¾Ë¾Æ º¸·Á°í ¾È°£ÈûÀ» ½èÁö¸¸, ¾ÆÁ÷ °¥ÇÇ¸¦ ÀâÁö ¸øÇÏ°í ÀÖ´Ù³×.[ENTER]¾Æ¹«·¡µµ ÇÐÀÚ(20380)ÀÇ µµ¿òÀÌ Àý½ÇÈ÷ ÇÊ¿ä ÇÒ °Í °°±º ±×·¡.[ENTER]³»°¡ ÀÚ³×¿¡°Ô ¼­½ÅÀ» ÁÙ ÅÍÀÌ´Ï ÇÐÀÚ(20380)¿¡°Ô ÀüÇØ ÁÖ°Ô³ª.[ENTER]ÇÐÀÚ(20380)µµ ÀÌ ¼­½ÅÀ» º¸¸é ¾Ë¾Æº¸°í µµ¿òÀ» ÁÙ °ÍÀÌ¾ß.[ENTER]"
locale.main_quest_flame_lv99.say_20 = "악룡군도의의 학자(20380)를 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv99.say_21 = "(Áß¾ó Áß¾ó)[ENTER]±ôÂ¦ÀÌ¾ß¡¦ÀÌ »ç¶÷ ¸Å¹ø »ç¶÷À» ³î·¡Å°´Â ±¸¸¸¡¦[ENTER]¿À¸é ¿Ô´Ù°í ±âº°À» ÇØ¾ß ÇÒ °ÍÀÌ ¾Æ´Ñ°¡?[ENTER]±×·¡. ¾îÀÎ ÀÏ·Î ¿Ô´Â°¡?[ENTER]"
locale.main_quest_flame_lv99.say_22 = "약환(20377)님께서 이 서신을 전해 주라 하셨습니다.[ENTER]"
locale.main_quest_flame_lv99.say_23 = "À½¡¦¾àÈ¯(20377)´ÔÀÇ ¼­½ÅÀ» º¸´Ï, ¾î¶² °ÍÀ» ¿ì·ÁÇÏ½Ã´Â Áö ¾Ë °Í °°±º ±×·¡.[ENTER]¹«¾ùÀÌµç È®½ÇÇÑ °ÍÀÌ ÁÁÀ¸´Ï, ³ª¸¦ Á» µµ¿Í ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv99.say_24 = "자네가 다녀온 그 동굴 인근을 조사해 보아야 할 것 같네.[ENTER]주변 부터 조사해 보는 것이 좋을 것이네. 함부로 나섰다가는 일을 그릇치게 되니 말일세.[ENTER]동굴에서 을씨년스러온 소리가 나왔다고 하니, 주변 마물에도 영향이 있지 않을까 싶네.[ENTER]이 종이를 줄터이니, 무구에 감고서 주변 마물을 처치해 주게나.[ENTER]탁기의 영향이 큰 마물을 처치하면 검게 변할 것이네.[ENTER]"
locale.main_quest_flame_lv99.say_25 = "약환(20377)에게 보고하였던 동굴 주변 마물을 처치하자."
locale.main_quest_flame_lv99.say_26 = "동굴 주변의 마물을 처치하자."
locale.main_quest_flame_lv99.say_27 = "약환(20377)에게 보고하였던 동굴 주변의 마물을 처치 하자.[ENTER]"
locale.main_quest_flame_lv99.say_28 = "À½¡¦ºÐ¸í ÁÖº¯¿¡ Å¹±âÀÇ ¿µÇâÀ» ¹Þ¾ÒÀ» ÅÍÀÎµ¥.[ENTER]Àç´ë·Î È®ÀÎÇÏ°í ¿Â °ÍÀÌ ¸Â´Â°¡?[ENTER]´Ù½Ã ÇÑ¹ø ´Ù³à¿Í ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv99.say_29 = "학자(20380)가 준 종이가 검게 변하였다. 학자(20380)에게 전해주자."
locale.main_quest_flame_lv99.say_30 = "¿Ô±º±×·¡.[ENTER]À½¡¦¿ª½Ã Å¹±âÀÇ ¿µÇâÀ» ¹ÞÀº ¸¶¹°µéÀÌ °÷°÷¿¡ ÆÛÁ® ÀÖ´Â °ÍÀÌ±¸¸¸.[ENTER]ÀÌ Á¾ÀÌ¿¡ ½º¸çµç Å¹±â¸¦ Á¶»ç¸¦ ÇØº¸¾Æ¾ß ÇÏ´Ï ÀÏ´Ü µ¹¾Æ°¡¼­ ½¬°í ÀÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv99.say_31 = "조사1-2"
locale.main_quest_flame_lv99.say_32 = "학자(20380)가 당신을 급하게 찾고 있습니다.[ENTER]학자(20380)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv99.say_33 = "어서오게나.[ENTER]자네가 가져온 탁기가 스며든 종이를 조사해 보고 있던 중이 었네.[ENTER]탁기는 일반인이 접속하면 정신이 탁해지기 때문에, 향을 피우고 조사하여야 한다네.[ENTER]그런데 이번에 조사를 하던 중에 향을 비울 약초가 모두 소진되어서 말이야.[ENTER]"
locale.main_quest_flame_lv99.say_34 = "그렇군요. 심상치 않은 동태가 보이니 제가 어서 약초를 구해 오도록 하겠습니다.[ENTER]"
locale.main_quest_flame_lv99.say_35 = "ÇãÇã.[ENTER]ÀÚ³×´Â ´Ã ±×¸® ´«Ä¡°¡ ºü¸£±¸¸¸¡¦[ENTER]¾àÃÊ´Â À½±â¸¦ ´­·¯¾ß ÇÏ´Â Áß¿äÇÑ ¿ªÇÒÀ» ÇÑ´Ù³×.[ENTER]±×·¯´Ùº¸´Ï ¾ç±â°¡ ³ÑÄ¡´Â °÷¿¡ ÀÚ»ýÀ» ÇÑ´Ù³×.[ENTER]µµ¿°È­Áö´Â ¾Ë °ÍÀÌ°í, ±× ³¡ÂÊ¿¡ ÀÖÀ¸´Ï ÀÚ³×°¡ ´Ù³à¿À°Ô³ª.[ENTER]ÀÌ Áöµµ¸¦ º¸°í °¡¸é µÉ °ÍÀÌ¾ß.[ENTER]¹°·Ð ³ªµµ ±â¾ïÀÌ °¡¹°°¡¹°ÇØ¼­ Á¤È®ÇÏÁø ¾Ê´Ù³×.[ENTER]"
locale.main_quest_flame_lv99.say_36 = "네. 알겠습니다.[ENTER]즉시 다녀 오도록 하겠습니다.[ENTER]"
locale.main_quest_flame_lv99.say_37 = "학자(20380)가 지도에 표시해준 곳에서 태양초(31076)를 구해오자"
locale.main_quest_flame_lv99.say_38 = "태양초(31076)를 구하자."
locale.main_quest_flame_lv99.say_39 = "학자(20380)가 준 지도의 위치에서 약초를 구해오자.[ENTER]"
locale.main_quest_flame_lv99.say_40 = "ÀÌºÁ¡¦[ENTER]ºó¼ÕÀÌ¸é ¾îÂ¼ÀÚ´Â °ÍÀÎ°Õ°¡?[ENTER]±× ÅÂ¾çÃÊ(31076)°¡ ¾øÀ¸¸é ÀÌ ÀÌ»ó Á¶»ç°¡ ¾î·Æ´Ü ¸»ÀÏ¼¼.[ENTER]´Ù½Ã ´Ù³à¿Í ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv99.say_41 = "학자(20380)가 말하던 태양초(31076)를 발견하였다. 학자(20380)에게 전해주자."
locale.main_quest_flame_lv99.say_42 = "그래. 바로 이 약초 일세.[ENTER]이제 조사를 마무리 할 수 있게 되었군 그래.[ENTER]조사가 완료되거든 약환(20377)님에게 일러 둘터이니 나중에 가보도록 하게나.[ENTER]"

locale.main_quest_flame_lv100 = {}
locale.main_quest_flame_lv100.say_1 = "탁기의 정체"
locale.main_quest_flame_lv100.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv100.say_3 = "어서 오게나.[ENTER]자네가 학자(20380)를 도와 조사해준 덕분에 원활하게 조사가 마무리 되었다네.[ENTER]일단 조사의 결과를 자네에게 알려 주도록 하겠네.[ENTER]"
locale.main_quest_flame_lv100.say_4 = "학자(20380)의 말에 의하면, 자네와 조사한 그 동굴 인근의 마물은 본질적으로 다른 기운에 의하여[ENTER]변질이 된 것이라 하더구만.[ENTER]그 영향이 지속이 되면 마물들은 점점 강해지고 변이되며, 보다 난폭한 성질을 보이게 된다는군.[ENTER]일단 탁기가 퍼지는 것부터 막아야, 주변에 피해가 덜 갈 것으로 보이는군 그래.[ENTER]"
locale.main_quest_flame_lv100.say_5 = "그렇다면, 탁기가 퍼지는 것을 막기 위해서는 어떻게 하여야 합니까?[ENTER]"
locale.main_quest_flame_lv100.say_6 = "그렇지 않아도, 탁기의 퍼짐을 막기 위해서 연금술사(20001)와 의견을 나누어 보았다네.[ENTER]연금술사(20001)가 그 막을 방도를 마련중이니, 이 이상 퍼지지 않도록 마물을 처치해 주게나.[ENTER]동굴 근처 마물을 가리지 않고 300마리정도 처치하면 좀 나을 듯 하네.[ENTER]"
locale.main_quest_flame_lv100.say_7 = "네. 지금 즉시 출발 하겠습니다.[ENTER]"
locale.main_quest_flame_lv100.say_8 = "고맙네. 좋은 전과를 기대하고 있겠네.[ENTER]"
locale.main_quest_flame_lv100.say_9 = "약환(20377)과 조사하였던 곳의 주변 마물을 처치하자."
locale.main_quest_flame_lv100.say_10 = "도염화지 마물을 처치하자!"
locale.main_quest_flame_lv100.say_11 = "약환(20377)과 조사하였던 곳의 주변 마물을 처치하자.[ENTER]"
locale.main_quest_flame_lv100.say_12 = "정말로 충분히 처치를 하고 온 것인가?[ENTER]부하들 보고로는 아직 충분치 않아 보이네만.[ENTER]어서가 모조리 처치하고 와주게나.[ENTER]"
locale.main_quest_flame_lv100.say_13 = "주변 마물을 충분히 처치하였다.[ENTER]약환(20377)에게 보고하도록 하자."
locale.main_quest_flame_lv100.say_14 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv100.say_15 = "À½¡¦ÀÌÂëÀÌ¸é ÃæºÐÇÏ°Ô ¸¶¹°À» Ã³Ä¡ÇÑ °Í °°±¸¸¸¡¦[ENTER]¿¬±Ý¼ú»ç(20001)¿¡°Ô´Â ³»°¡ ÁÖº¯ ¸¶¹°À» Ã³Ä¡ÇÑ °ÍÀ» ¾Ë¸± ÅÍÀÌ´Ï ½¬°í ÀÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv100.say_16 = "탁기의 저지1-1"
locale.main_quest_flame_lv100.say_17 = "연금술사(20001)가 당신을 급하게 찾고 있습니다.[ENTER]연금술사(20001)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv100.say_18 = "ÀÌ·± Á¦±æ¡¦[ENTER]¾àÈ¯(20377)´Ô²² ±×¸® ½ÅÁßÇÏ°Ô Çàµ¿ÇØ ´Þ¶ó°í ºÎÅ¹À» Çß°Ç¸¸, ÀÏÀÌ Ä¿Áö°í¾ß ¸»¾Ò±º ±×·¡.[ENTER]Å¹±â´Â ¸¶¹°ÀÌ Á×À¸¸é¼­ Æ¯È÷ ´õ ÆÛÀú³ª°¡´Â ¼ºÁúÀÌ ÀÖ´Âµ¥ 300¸¶¸®³ª µÇ´Â ¸¶¹°À»[ENTER]µµ·úÇØ ¹ö·ÈÀ¸´Ï Å«ÀÏÀÌ³×.[ENTER]"
locale.main_quest_flame_lv100.say_19 = "지금 상황은 오히려 악화가 되어 있는 상태이네.[ENTER]게다가 내가 탁기를 저지하려고 만들었던 비약(31084)이 모자라게 되어 버렸다네.[ENTER]이 약을 무구에 바르고 마물을 처치하게 되면, 탁기가 퍼지는 것을 막을 수 있다네.[ENTER]모자라게 되어 버렸으니 자네가 재료를 더 조달해 주어야 할 것이야.[ENTER]"
locale.main_quest_flame_lv100.say_20 = "네. 알겠습니다. 재료가 무엇입니까?[ENTER]"
locale.main_quest_flame_lv100.say_21 = "ÀÚ³×°¡ Àß ±¸ÇÒ ¼ö ÀÖÀ»·±Áö ÀÇ¹®ÀÌ±¸¸¸¡¦[ENTER]¿øÃ¼ ±¸ÇÏ±â°¡ ½±Áö ¾ÊÀº °ÍµéÀÌ¶ó¼­¡¦[ENTER]ÀÏ´Ü ¾Ë·ÁÁÖµµ·Ï ÇÏÁö!![ENTER]ºñ¾à(31084)À» ¸¸µé±â À§ÇØ¼­´Â ÃÊ¼®(90010)10°³, °õÀÇ ¾µ°³(30010) 50°³, °Å¹ÌÀÇ µ¶ÁÖ¸Ó´Ï(30025)10°³, È°¼®(51001) 200°³°¡ ÇÊ¿äÇÏ´Ù³×.[ENTER]¸ðµÎ ÀÚ³×Á¤µµ µÇ´Â ¹«ÀÎÀÌ¶ó¸é ¾îµð¼­ ±¸ÇÒ ¼ö ÀÖ´ÂÁö´Â ¾ËÅÍÀÌ´Ï ¼³¸íÀº »ý·«ÇÏ°Ú³×.[ENTER]"
locale.main_quest_flame_lv100.say_22 = "쉽게 구할 수 있는 재료는 아니군요. 다녀오도록 하겠습니다.[ENTER]"
locale.main_quest_flame_lv100.say_23 = "알겠네. 우리가 이렇게 말을 하는 와중에도 탁기가 퍼저나가고 있으니 서둘러 주게나.[ENTER]"
locale.main_quest_flame_lv100.say_24 = "연금술사(20001)를 도와 비약(31084)을 완성하도록 하자."
locale.main_quest_flame_lv100.say_25 = "남은 재료"
locale.main_quest_flame_lv100.say_26 = "비약(31084)을 만들 제료를 구하자."
locale.main_quest_flame_lv100.say_27 = "연금술사(20001)가 일러준 제료를 구하도록 하자.[ENTER]"
locale.main_quest_flame_lv100.say_28 = "오~모으기 쉽지 않은 재료였을 터인데.[ENTER]용케도 구해 왔군 그래.[ENTER]비약(31084)을 만들고 부를 터이니 가보게나.[ENTER]"
locale.main_quest_flame_lv100.say_29 = "이봐. 내가 숫자도 해아리지 못하는 장님으로 보이는 겐가?[ENTER]어서 모든 제료를 확보해 주게나.[ENTER]지금 이 시간에도 탁기는 창궐하고 있다네.[ENTER]"
locale.main_quest_flame_lv100.say_30 = "비약(31084)을 만들 재료를 모두 확보하였다. 연금술사(20001)에게 전해주도록 하자."
locale.main_quest_flame_lv100.say_31 = "악룡군도의의 연금술사(20001)를 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv100.say_32 = "탁기의 저지1-2"
locale.main_quest_flame_lv100.say_33 = "ÀÌ·±ÀÌ·±¡¦[ENTER]ÀÌÀü¿¡ ¾àÀ» ¸¸µé ´ç½Ã¿¡ ´ëÀåÀåÀÌ(20016)¿¡°Ô ÅøÅø °Å·È´õ´Ï, ÃÊ¼®(90010)À» °¡·ç·Î ¸¸µé¾î ÁÖÁö ¾Ê´Â ±º±×·¡¡¦[ENTER]ÃÊ¼®(90010)À» °¡·ç·Î ¸¸µé¾î ´Ù¸¥ Àç·á¿Í ÇÔ²² Á¦Á¶ÇÏ¿© ¾àÀ» ¿Ï¼ºÇÏ´Âµ¥ ¸»ÀÌ¾ß¡¦[ENTER]ÀÚÁ¸½É»ó ³»°¡ °¡±ä ±×·¸°í, ÀÚ³×°¡ ÀÌ°É µé°í°¡¼­ ÃÊ¼®(90010)À» °¡·ç·Î ¸¸µé¾î¼­ °¡Á®¿Í ÁÖ°Ú³ª?[ENTER]"
locale.main_quest_flame_lv100.say_34 = "¾î¶»°Ô ÇÏ¼Ì±â¿¡¡¦[ENTER]ÀÏ´Ü ¾Ë°Ú½À´Ï´Ù. ÃÊ¼®(90010)°ú ±× ¹Ù±¸´Ï¸¦ ÁÖ½ÃÁö¿ä.[ENTER]"
locale.main_quest_flame_lv100.say_35 = "고맙네. 기다리고 있겠네.[ENTER]"
locale.main_quest_flame_lv100.say_36 = "대장장이(20016)에게 다녀오자."
locale.main_quest_flame_lv100.say_37 = "대장장이(20016)에게 초석(90010)을 가루로 만들어 오자.[ENTER]"
locale.main_quest_flame_lv100.say_38 = "초석(90010)가루: 1개[ENTER]"
locale.main_quest_flame_lv100.say_39 = "어이구 대협니 어인 일이십니까?[ENTER]질 좋은 무구라도 사러 오신 겝니까?[ENTER]"
locale.main_quest_flame_lv100.say_40 = "아~다름이 아니라 연금술사(20001)님께 이야기는 들었습니다.[ENTER]시국이 시국인 만큼 도움을 청하러 왔습니다.[ENTER]연금술사(20001)님께서 직접 찾아가 사과하지 못함을 미안하게 생각하며, 이 음식 바구니를 보내셨습니다.[ENTER]"
locale.main_quest_flame_lv100.say_41 = "Èì¡¦¸¶À½ °°¾Æ¼± ÇØÁÖ°í ½ÍÁö ¾ÊÁö¸¸¡¦[ENTER]´ëÇù´Ô²²¼­ Á÷Á¢ ÀÌ·¸°Ô Ã£¾ÆÁÖ½Ã°í, »ç°úÀÇ Ç¥Çöµµ ÇÏ¿´À¸´Ï ÃÊ¼®(90010)À» °¥¾Æµå¸®¸®´Ù.[ENTER]Àá½Ã¸¸ ±â´Ù¸®½Ê½Ã¿À.[ENTER]"
locale.main_quest_flame_lv100.say_42 = "네. 감사합니다.[ENTER]"
locale.main_quest_flame_lv100.say_43 = "ÀÚ³×. ¾ç¼Õ¿¡ ÃÊ¼®(90010)°ú ¹Ù±¸´Ï°¡ ±×´ë·ÎÀÌÁö ¾ÊÀº°¡¡¦[ENTER]¾î¼­ ´Ù³à¿Í ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv100.say_44 = "초석(90010)을 모두 갈았다. 연금술사(20001)에게 전해주자.[ENTER]"
locale.main_quest_flame_lv100.say_45 = "¿À~´ëÀåÀåÀÌ(20016)°¡ »ç°ú¸¦ ¹Þ¾ÆÁØ ¸ð¾çÀÌ±º ±×·¡.[ENTER]³ªµµ ÀÌÁ¦ ¼øÇÏ°Ô »ì¾Æ¾ßÁö¿ø¡¦[ENTER]°¡·ç·Î ¸¸µç ÃÊ¼®(90010)À» ³ª¿¡°Ô ÁÖ°í ½¬°í ÀÖ°Ô³ª.[ENTER]¾àÀÌ ¿Ï¼ºÀÌ µÇ¸é ºÎ¸£µµ·Ï ÇÏ°Ú³×.[ENTER]"

locale.main_quest_flame_lv101 = {}
locale.main_quest_flame_lv101.say_1 = "동굴의 정체"
locale.main_quest_flame_lv101.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv101.say_3 = "어서 오게나.[ENTER]연금술사(20001)에게는 자네의 노고에 대해서 전해 들었다네.[ENTER]내가 괜한 일을 벌여서 자네가 고생을 한 것 같더군 그래.[ENTER]미안하게 생각한다네.[ENTER]"
locale.main_quest_flame_lv101.say_4 = "미안한 것은 미안한 것이고, 시국이 더 급박하게 되었으니 용건만 간단히 하도록 하겠네.[ENTER]연금술사(20001)에게 비약(31084)은 내가 건내어 받았다네.[ENTER]그 약을 무구에 바르면 처치한 마물의 탁기가 퍼지는 것을 저지하여 준다네.[ENTER]"
locale.main_quest_flame_lv101.say_5 = "약에 대한 설명은 이 정도로 하고, 자네가 해주어야 할 일을 알려 주겠네.[ENTER]이제 무구도 준비가 되었고, 우리가 발견한 그 동굴을 보다 자세히 알아 볼 필요가 있다네.[ENTER]탁기가 퍼지는 것도 그렇고, 분명 돌입하여 마물들을 처치하다보면, 무언가 단서가 나올 듯 하네.[ENTER]자네가 직접 돌입을 감행하여, 확인해 주게나.[ENTER]"
locale.main_quest_flame_lv101.say_6 = "¾Æ¡¦Çà¿© µ¿±¼¿¡¼­ ºÎÇÏ°¡ ¹ß°ßÀÌ µÇ°Åµç ²À »ì¾Æ¼­ ±ÍÈ¯ÇÏ¶ó°í ÀüÇØ ÁÖ°Ô³ª.[ENTER]¶°µ¹ÀÌ »óÀÎ¿¡ ÀÇÇÏ¸é, ³» ºÎÇÏ°¡ µ¹ÀÔÀ» °¨Çà ÇÑ °Í °°¾Æ¼­ ¸»ÀÌ¾ß.[ENTER]"
locale.main_quest_flame_lv101.say_7 = "네. 지금 즉시 출발 하겠습니다.[ENTER]"
locale.main_quest_flame_lv101.say_8 = "고맙네. 좋은 전과를 기대하고 있겠네.[ENTER]"
locale.main_quest_flame_lv101.say_9 = "동굴에서 마물을 처치하여 단서와 약환의 부하(20393) 생사를 확인하자."
locale.main_quest_flame_lv101.say_10 = "도염화지 마물을 처치하자!"
locale.main_quest_flame_lv101.say_11 = "약환(20377)과 조사하였던 곳의 주변 마물을 처치하자.[ENTER]"
locale.main_quest_flame_lv101.say_12 = "¡ØÇØ´ç ´øÀüÀº °³ÀÎÀÔÀåÀÌ ºÒ°¡ÇÕ´Ï´Ù.[ENTER]   ÆÄÆ¼¸¦ ¸Î¾î ÀÔÀåÇÏ½Ã±â ¹Ù¶ø´Ï´Ù.[ENTER]¡ØÁö±ÞµÈ ÅëÇàÁõÀº Àç¹ß±Þ µÇÁö ¾Ê½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv101.say_13 = "이봐. 내가 한가해서 자네에게 부탁을 했겠나?[ENTER]분명 내부하의 행방과 단서를 찾을 수 있을 것이야.[ENTER]어서 다녀와 주게나.[ENTER]"
locale.main_quest_flame_lv101.say_14 = "부하는 죽어있다. 허리춤에서 남겨진 편지를 약환(20377)에게 전해주자.[ENTER]"
locale.main_quest_flame_lv101.say_15 = "약환(20377)이 말한 부하(20393)의 행방을 확인 하였다."
locale.main_quest_flame_lv101.say_16 = "부하는 행방은 어떠한가?[ENTER]"
locale.main_quest_flame_lv101.say_17 = "¡¦¾Æ½±°Ôµµ ºÎÇÏºÐÀÇ ¼ûÀ» °ÅµÐ µÚ¿´½À´Ï´Ù.[ENTER]Çã¸®Ãã¿¡¼­ ÆíÁö¸¦ ¹ß°ßÇÏ¿´À¸´Ï º¸½ÃÁö¿ä.[ENTER]"
locale.main_quest_flame_lv101.say_18 = "편지는 나중에 보도록 하겠네.[ENTER]단서는 못구한 것 같으니 어서가 단서까지 확보하여 돌아와 주게나.[ENTER]"
locale.main_quest_flame_lv101.say_19 = "약환(20377)이 말한 단서를 확인 하였다."
locale.main_quest_flame_lv101.say_20 = "À½¡¦¿ª½Ã ´Ü¼­°¡ ³ª¿À´Â ±º ±×·¡.[ENTER]ºÎÇÏÀÇ Çà¹æÀº ¾ÆÁ÷ÀÎ°¡ º¸±¸¸¸.[ENTER]³» ºÎÇÏ´Â ºÐ¸í ±× µ¿±¼¿¡ ÀÖÀ» °ÍÀÌ¾ß. ¾î¼­°¡ È®ÀÎÀ» ´Ù½Ã ÇØ ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv101.say_21 = "약환(20377)이 말한 단서와 부하(20393)의 행방을 모두 확인 하였다.[ENTER]약환(20377)에게 보고하도록 하자."
locale.main_quest_flame_lv101.say_22 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv101.say_23 = "오~다녀왔군 그래.[ENTER]단서는 가져온 듯하고 반대편 손에 들려 있는 것은 무엇인가?[ENTER]"
locale.main_quest_flame_lv101.say_24 = "¾Æ¡¦ÀÌ°ÍÀº ºÎÇÏºÐÀÌ ³²±â½Å À¯Ç°À¸·Î º¸ÀÔ´Ï´Ù.[ENTER]¹ß°ßÇÏ¿´À» ¶§¿¡´Â ÀÌ¹Ì ¸íÀ» ´Þ¸®ÇÏ¼Ì°í, Ç°¿¡¼­ ÀÌ°ÍÀ» ¹ß°ß ÇÏ¿´½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv101.say_25 = "±×·¸±º¡¦ÂüÀ¸·Î ¾Æ³¢´ø ºÎÇÏ¿´´Âµ¥ ¸»ÀÏ¼¼ ³»¿ëÀ» È®ÀÎÇÏ°í, ÇâÀÌ³ª ¿Ã¸®·¯ °¡ÁÖ¾î¾ß °Ú³×¡¦[ENTER]ÀÏ´Ü ¼­½ÅÀ» °°ÀÌ ÇÑ¹ø º¸µµ·Ï ÇÏ¼¼³ª.[ENTER]"
locale.main_quest_flame_lv101.say_26 = "약환 부하(20393)"
locale.main_quest_flame_lv101.say_27 = "ÀÌ ¼­½ÅÀ» ½ÀµæÇÑ ÀÚ´Â ¾Ç·æ±ºµµ ¸í½ÃÁø¿¡ ÀÖ´Â ¾àÈ¯(20377)´Ô²² ºÎµð ÀüÇØÁÖ±æ ¹Ù¶ó³×¡¦(Áß·«)[ENTER]¾àÈ¯(20377)´Ô µµ¿°È­Áö¿¡¼­ ¸¶¹°À» Á¶»çÇÏ´ø Áß, ¼ö»óÇÑ µ¿±¼À» ¹ß°ßÇÏ¿© µ¹ÀÔÇÏ¿´À¸³ª.[ENTER]ÀÌ·¸°Ô ±ÍÈ¯ÇÏÁö ¸øÇÏ°í, ÆíÁö¸¦ ³²±é´Ï´Ù.(Áß·«)[ENTER]ÀÌ µ¿±¼Àº µ¿±¼ ¹ÛÀÇ ¸¶¹°º¸´Ù ±â¿îÀÌ °­ÇÏ°í, È­±ÞÇÏ¸ç ¹«¾ð°¡ ÁöÅ°´Â µí º¸¿´½À´Ï´Ù.[ENTER]µ¿±¼ ¾ÈÀ¸·Î °¡¸é °¥ ¼ö·Ï ¿ïºÎÂ¢´Â ¼Ò¸®°¡ °­ÇÏ¿´½À´Ï´Ù.[ENTER]ºÐ¸í ¹«¾ð°¡ ÀÖÀ» °ÍÀÌ´Ï, ÀÌ ¼­½ÅÀ» º¸½Ã°Åµç Àû´çÇÑ ÀÚ¸¦ ÅëÇÏ¿© Á¶»çÇÏ½Ã±æ º÷´Ï´Ù.[ENTER]¸íÀ» ¿Ï¼öÇÏÁö ¸øÇÔÀ» »çÁË µå¸³´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv101.say_28 = "충직한 부하였는데, 다시 또 눈물이 나는 군 그래.[ENTER]이 동굴이 분명 위험한 것은 자명한 것이니, 앞으로 더 면밀하게 조사를 해 보아야 겠군 그래.[ENTER]이 명패는 자네도 일전에 보았다 시피 초마단의 명패일세.[ENTER]학자(20380)에게 건내줄 터이니 학자(20380)가 찾거든 가보도록 하게나.[ENTER]"
locale.main_quest_flame_lv101.say_29 = "초마단의 명패 해독"
locale.main_quest_flame_lv101.say_30 = "학자(20380)가 당신을 급하게 찾고 있습니다.[ENTER]학자(20380)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv101.say_31 = "어서오게나.[ENTER]기다리고 있었다네.[ENTER]약환(20377)님이 보내주신 명패를 보고 다시금 놀라지 않을 수가 없었다네.[ENTER]약환(20377)님께서 이르기를 이 명패를 해독하면 동굴에서 벌어지는 일을 알 수 있을 것 같다 하셨다네.[ENTER]그래서 말이니 자네가 나를 도와 이 명패를 해독하는 것을 도와주게나.[ENTER]"
locale.main_quest_flame_lv101.say_32 = "네. 여부가 있겠습니까![ENTER]"
locale.main_quest_flame_lv101.say_33 = "ÁÁÀº ÀÚ¼¼·Î±º ±×·¡.[ENTER]°Ç³» ¹ÞÀº ÀÌÈÄ·Î ¸íÆÐ¸¦ ¸é¹ÐÇÏ°Ô »ìÆìº¸°í Á¶»ç¸¦ ÇØº» °á°ú ÀÌ ¸íÆÐ¿¡´Â °í´ë ¾ð¾î·Î ¾²¿© ÀÖ´Â °ÍÀ» È®ÀÎ ÇÏ¿´´Ù³×.[ENTER]ÀÌ ¾ð¾î¸¦ Áï½Ã ÇØµ¶ÇÏ¿© ¾Ë·ÁÁÖ°í ½ÍÁö¸¸, ÀüÅõ Áß¿¡ ÈÑ¼ÕÀÌ µÇ¼­ÀÎÁö ¾Ë¾Æ º¼ ¼ö°¡ ¾ø±¸¸¸ ±×·¡.[ENTER]ÀÌ ¸íÆÐ¸¦ º¹¿øÇÏ±â À§ÇØ¼­´Â ºÒ²Éº¸´Ù ¶ß°Ì°Ô Å¸¿À¸£´Â ÇÇ°¡ ÇÊ¿äÇÏ´Ù³×.[ENTER]½±°Ô ¸»ÇÏ¸é È­¿°¿ÕÀÇ ÇÇ(31080)°¡ ÇÊ¿äÇÑ °ÍÀÎµ¥.[ENTER]³»°¡ ÀÌ ÇÔÀ» ÁÙÅÍÀÌ´Ï È­¿°¿Õ(2206)À» Ã³Ä¡ÇÏ°í ¿©±â¿¡ È­¿°¿ÕÀÇ ÇÇ(31080)¸¦ ¹Þ¾Æ¿Í ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv101.say_34 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv101.say_35 = "±¸ÇÏ°Åµç Áï½Ã µ¹¾Æ¿À°Ô³ª. ±× ÇÔµµ Àå½Ã°£ È­¿°¿ÕÀÇ ÇÇ(31080)¸¦ °ßµðÁø ¸øÇÒ °ÍÀÌ¾ß.[ENTER]"
locale.main_quest_flame_lv101.say_36 = "도염화지에 있는 화염왕(2206)을 처치하고 피를 함에 받아오자."
locale.main_quest_flame_lv101.say_37 = "¸íÆÐ¸¦ ÇØµ¶ÇÏ°í ½ÍÁö ¾ÊÀº °Õ°¡?[ENTER]¾î¼­°¡¼­ È­¿°¿ÕÀÇ ÇÇ(31080)¸¦ °¡Á®¿À¶õ ¸»ÀÏ¼¼.[ENTER]"
locale.main_quest_flame_lv101.say_38 = "화염왕(2206)에게서 피를 확보하였다. 학자(20380)에게 돌아가자."
locale.main_quest_flame_lv101.say_39 = "악룡군도의의 학자(20380)를 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv101.say_40 = "À½¡¦È­¿°¿ÕÀÇ ÇÇ(31080)°¡ ¸Â±º ±×·¡.[ENTER]º¹¿øÇÏ´Âµ¥ ½Ã°£ÀÌ ´Ù¼Ò °É¸®´Ï ³»ÀÏ ´Ù½Ã ¿À°Ô³ª.[ENTER]³ªµµ »¡¸® ÇØÁÖ°í ½ÍÁö¸¸ ¾îÂ¿ ¼ö ¾ø´Ù³×.[ENTER]"
locale.main_quest_flame_lv101.say_41 = "잔여시간 : %d시간 %d분"
locale.main_quest_flame_lv101.say_42 = "아직 시간이 되지 않았지 않는가?[ENTER]나중에 다시 오게나.[ENTER]"
locale.main_quest_flame_lv101.say_43 = "명패의 진실"
locale.main_quest_flame_lv101.say_44 = "왔군 그래.[ENTER]명패가 해독이 되었네.[ENTER]해독을 하면서 내가 먼저 읽어 보았는데, 이거 원 무지막지한 내용이 들어 있더군 그래.[ENTER]내가 내용을 알려 주도록 하지.[ENTER]"
locale.main_quest_flame_lv101.say_45 = "명패는 초마단의 것이 분명하네.[ENTER]자네가 처치 중에 명패를 구하게 된 마물이 어떤 마물인지는 모르겠지만,[ENTER]초마단이 6룡이 봉인이 된 곳에 강력한 마물을 보내어, 봉인을 해제 하려 한다는 것이네.[ENTER]그 뿐만 아니라, 용련에 대하여도 언급이 되어 있다네.[ENTER]자세한 내용은 이 서신에 적어두었으니 약환(20377)님께 전해 주게나.[ENTER]"
locale.main_quest_flame_lv101.say_46 = "고맙네. 자세한 것은 약환(20377)님이 알려주실 것이야.[ENTER]"
locale.main_quest_flame_lv101.say_47 = "학자(20380)에게 받은 서신을 약환(20377)에게 전해주자."
locale.main_quest_flame_lv101.say_48 = "약환(20377)에게 서신을 전하자!"
locale.main_quest_flame_lv101.say_49 = "약환(20377)에게 학자(20380)가 준 서신을 전하자.[ENTER]"
locale.main_quest_flame_lv101.say_50 = "어서오게. 학자(20380)에게는 전해 들었다네.[ENTER]서신을 주게나.[ENTER]"
locale.main_quest_flame_lv101.say_51 =  "À½¡¦ÀÌ°Å »ý°¢º¸´Ù ½É°¢ÇÏ±º ±×·¡.[ENTER]6·æÀÌ ºÀÀÎµÈ °÷¿¡ ÃÊ¸¶´ÜÀÌ ¸¶¹°À» º¸³»¾î ºÀÀÎÀ» ÇØÁ¦ÇÏ·Á ÇÑ´Ù´Â °ÍÀº µé¾úÀ» °ÍÀÌ°í,[ENTER]¿ë·Ã¿¡ ´ëÇØ¼­ ¾Ë·ÁÁÖ°Ú³×.[ENTER]¿ë·ÃÀº ÇÐÀÚ(20380)ÀÇ ¸»À» µû¸£¸é, 6·æÀ» °¨½ÃÇÏ°í ÀÌ¿ëÇÏ´Â ÀÌÇØ°ü°è°¡ ÀÖ´Â ¼¼·ÂÀ» ¸»ÇÑ´Ù³×.[ENTER]ÃÊ¸¶´Üµµ ÃÊ¸¶´ÜÀÌÁö¸¸ ¶§¿¡ µû¶ó¼­´Â ¸øÁö ¾Ê°Ô À§Çè ÇÒ ¼ö ÀÖ´Â ¼¼·ÂÀÌÁö.[ENTER]"
locale.main_quest_flame_lv101.say_52 = "도염화지의 화염왕(2206)을 처치하고 피를 받아오자.[ENTER]"
locale.main_quest_flame_lv101.say_53 = "용련은 위해가 가지 않는 경우라면 별 문제가 되지 않을 수도 있지만, 가장 우리가 초점을 두어야 할 것은[ENTER]초마단이 6룡에게 마물을 보냈다는 것이네.[ENTER]게다가 도염화지인 것과 6룡의 봉인 등을 미루어 볼 시에 그 동굴은 적룡성(zone1)일 것으로 보이네.[ENTER]아무래도 적룡성(zone1) 안쪽에 돌입하여 과감하게 알아볼 필요가 있을 것 같네.[ENTER]방도를 마련하고 다시 기별을 넣을 터이니 가서 쉬고 있게나.[ENTER]"

locale.main_quest_flame_lv102 = {}
locale.main_quest_flame_lv102.say_1 = "돌입"
locale.main_quest_flame_lv102.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv102.say_3 = "일전에 이야기해 준 적룡성(zone1)의 돌입 때문에 이렇게 부르게 되었다네.[ENTER]무공이 고강한 나의 부하가 무참히 죽었다는 점, 그곳이 6룡중 하나인 적룡이 봉인 된 것으로 추측되는 점 등[ENTER]돌입에는 신경써야 할 것이 한 두가지가 아니네.[ENTER]"
locale.main_quest_flame_lv102.say_4 = "자네가 나를 도와 적룡성(zone1) 인근 마물을 처치하여 탁기가 퍼짐을 막아준 덕분에 내 부하들을 시켜 주변을 봉쇄하였다네.[ENTER]적어도 외부로 피해가 번지는 것은 막을 수 있을 것이네.[ENTER]이제 자네가 해줄 일은 적룡성(zone1)에 돌입하여 어떠한 마물들이 있는지, 적룡이 봉인 된 것이 맞는지 확인을 해주었으면 한다네.[ENTER]"
locale.main_quest_flame_lv102.say_5 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv102.say_6 = "고맙네.[ENTER]한번 싸워보고 알 수 없는 노릇이니, 안에 있는 마물들을 처치하고 그 증거를 나에게 30개씩 가져다 주게나.[ENTER]그럼 부탁 하겠네.[ENTER]"
locale.main_quest_flame_lv102.say_7 = "적룡성(zone1)의 마물을 처치하고 증거를 가져오자."
locale.main_quest_flame_lv102.say_8 = "적룡성(zone1)에 돌입하자!"
locale.main_quest_flame_lv102.say_9 = "약환(20377)의 지령에 따라서 적룡성(zone1)에 돌입하여, 척후를 행하자.[ENTER]"
locale.main_quest_flame_lv102.say_10 = "%s의 발톱: %d개 %s의 손톱: %d개"
locale.main_quest_flame_lv102.say_11 = "%s의 갑옷 파편: %d개 %s의 무기 파편: %d개"
locale.main_quest_flame_lv102.say_12 = "À½¡¦¾î´ÀÁ¤µµ Ã³Ä¡ÇØ º¸¾Æ¾ß ¾î´ÀÁ¤µµ ¸¶¹°ÀÇ °­ÇÔÀ» ÆÄ¾ÇÇÏ°í Àü·«À» ¼ö¸³ÇÒ ¼ö ÀÖ´Ù³×.[ENTER]¾î¼­ µ¹ÀÔÇÏ¿© Áõ°Å¸¦ ¸ðµÎ °¡Á®¿Í ÁÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv102.say_13 = "적룡성(zone1)의 마물을 처치한 증거를 모두 확보하였다.[ENTER]약환(20377)에게 보고하도록 하자."
locale.main_quest_flame_lv102.say_14 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv102.say_15 = "오~왔군 그래.[ENTER]보아하니 증거는 모두 가져 온 것 같군 그래.[ENTER]자네가 직접 체감한 느낌은 어떠한가?[ENTER]"
locale.main_quest_flame_lv102.say_16 = "도염화지에 있는 마물들도 보였으나, 적룡성(zone1) 밖의 마물과 달리 다소 강하였습니다.[ENTER]그 밖에도 처음 보는 강력한 마물들이 출몰하여 여차하면 저도 생환하지 못할 뻔 했습니다.[ENTER]돌입을 해본 결과 적룡의 봉인해제를 도우며 적룡성(zone1)을 지키고 있는 염왕 야마천(6091)이라는 마물을 확인 할 수 있었습니다.[ENTER]"
locale.main_quest_flame_lv102.say_17 = "그렇군. 탁기의 영향만이 문제가 아니었군.[ENTER]초마단 녀석들 이번에 단단히 각오하고 은밀하게 움직였구만.[ENTER]다시 부르도록 할 터이니 가보게나.[ENTER]"
locale.main_quest_flame_lv102.say_18 = "염왕 야마천(6091)?!"
locale.main_quest_flame_lv102.say_19 = "학자(20380)가 당신을 급하게 찾고 있습니다.[ENTER]학자(20380)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv102.say_20 = "¾àÈ¯(20377)´Ô¿¡°Ô ÀüÇØ µé¾ú³×.[ENTER]Àû·æ¼º(zone1)¿¡¼­ ¿°¿Õ ¾ß¸¶Ãµ(6091)À» ¾Ë°Ô µÇ¾ú´Ù°í µé¾ú³×.[ENTER]ÇÏÇÊÀÌ¸é ¿Ö ±× ³à¼®ÀÎÁö ¿ø¡¦[ENTER]"
locale.main_quest_flame_lv102.say_21 = "¹«½¼ ¹®Á¦¶óµµ¡¦[ENTER]"
locale.main_quest_flame_lv102.say_22 = "무슨 일이라니 이 사람아!![ENTER]염왕 야마천(6091)은 빙제 한마의(6191)와 더불어서 강력하고 악독한 마물중에 하나라네.[ENTER]이 추적을 계속 함에 있어서 가장 큰 걸림 돌이 될 수 있는 녀석이라네.[ENTER]"
locale.main_quest_flame_lv102.say_23 = "일단은 나도 고 문헌과 풍문을 통해서 알게된 것이기 때문에, 보다 정확한 정보를 알아야만 약환(20377)님께 도움을 드릴 수 있을 것으로 보이네.[ENTER]자네가 나를 도와서 야마천(6091)에 대한 정보를 확보해 주게나.[ENTER]"
locale.main_quest_flame_lv102.say_24 = "어떤 것을 도와드리면 됩니까?[ENTER]"
locale.main_quest_flame_lv102.say_25 = "염왕 야마천(6091)이 탁기를 퍼뜨리는 주범인지 아닌지는 확실하지 않은 상태에서[ENTER]야마천(6091)을 바로 상대하는 것은 위험할 것으로 생각하네.[ENTER]일단은 적룡성(zone1) 안의 마물을 말살해 주게나.[ENTER]탁기가 스며든 마물은 분명 탁한 기운을 응집하여 몸안에 지니고 있을 것이네.[ENTER]탁기를 정화시킬 방도를 마련하고 야마천(6091)을 상대해도 늦지 않을 터이니 탁기 응집체(31081)를 한 10개쯤 구해다 주게나.[ENTER]"
locale.main_quest_flame_lv102.say_26 = "알겠네. 그럼 기다리고 있겠네.[ENTER]"
locale.main_quest_flame_lv102.say_27 = "적룡성(zone1)의 마물을 처치하고 탁기 응집체(31081)를 가져오자."
locale.main_quest_flame_lv102.say_28 = "탁기 응집체(31081)를 구하자!"
locale.main_quest_flame_lv102.say_29 = "적룡성(zone1)에 돌입하여, 탁기 응집체(31081)를 구하자.[ENTER]"
locale.main_quest_flame_lv102.say_30 = "¡ØÁö±ÞµÈ ÅëÇàÁõÀº Àç¹ß±Þ µÇÁö ¾Ê½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv102.say_31 = "ÀÌºÁ ¾ÆÁ÷µµ ¸ðµÎ ±¸ÇÏÁö ¸øÇÏ¿´³ª?[ENTER]ÀÚ³×¶ó¸é ´ÉÈ÷ ÀÌ ÀÏÀ» ÇØ³¾ ¼ö ÀÖÀ» °ÍÀÌ¶ó ÆÇ´ÜÇÏ¿´°Å´Ã¡¦[ENTER]´Ù½Ã ´Ù³à¿Í ÁÖ°Ô³ª![ENTER]"
locale.main_quest_flame_lv102.say_32 = "적룡성(zone1) 마물을 처치하고 탁기 응집체(31081)를 모두 구하였다. [ENTER]학자(20380)에게 보고하자."
locale.main_quest_flame_lv102.say_33 = "¾îÈÄ~º¸±â¸¸ÇØµµ Å¹±â°¡ ¸ö¿¡ ½º¸ç µé °Í °°±¸¸¸.[ENTER]ÀÚ³×°¡ ¹«°øÀÌ °í°­ÇÏ±ä ÇÑ°¡ º¸±¸¸¸.[ENTER]±ôºýÇÏ°í Å¹±â ÀÀÁýÃ¼(31081)¸¦ ´ãÀ» ÇÔÀ» ÁÖÁö ¾Ê¾Ò´Âµ¥ ¸»ÀÌÁö¡¦[ENTER]ÀÌÁ¦ ³»°¡ Á¤È­ÇÒ ¹æµµ¸¦ Ã£°í ÀÚ³×¸¦ ºÎ¸£µµ·Ï ÇÏ°Ú³×.[ENTER]°¡º¸°Ô³ª.[ENTER]"
locale.main_quest_flame_lv102.say_34 = "(¿ØÁö Á¶½ÉÇØ¾ß ÇÒ °Í ¸¸ °°´Ù¡¦)[ENTER]"
locale.main_quest_flame_lv102.say_35 = "탁기의 정화"
locale.main_quest_flame_lv102.say_36 = "잘왔네.[ENTER]조금전에 탁기를 정화 시킬 방도를 찾았네.[ENTER]탁기를 정화 시킬 수 있어야만, 적룡성(zone1)에 돌입하여도 버틸 수가 있을 것이니 말일세.[ENTER]자네 혼자 모조리 처치한다면야 상관이 없지만, 그건 무리고 약환(20377)님의 무하들과 함께 돌입하여야[ENTER]다소 수월하고 희생을 줄일 수 있을 것이네.[ENTER]"
locale.main_quest_flame_lv102.say_37 = "그 방도가 어떤 것입니까?[ENTER]"
locale.main_quest_flame_lv102.say_38 = "재촉하지 말게나.[ENTER]그렇지 않아도 말해 줄터이니 말일세.[ENTER]얼마전에 나와 탁기를 조사 할때 음양의 조화에 대해서 알게 되었을 것이니 자세한 말은 생략하겠네.[ENTER]불과 물, 음과 양과 같이 상극인 성질이 있는 것!![ENTER]탁한 것은 맑은 것으로 중화 시킬 수 있다네.[ENTER]물론 맑음이 크면 결국 정화가 되지.[ENTER]"
locale.main_quest_flame_lv102.say_39 = "자네 진보환액(50819)이라는 것은 알 것이네. 진보환액(50819)에 활석(51001)을 녹이면 맑은 성질을 띄는 액이 된다네.[ENTER]이것을 복용한 상태에서 적룡성(zone1)의 마물을 상대할때 큰 효용을 발휘하게 될 수 있을 것이야.[ENTER]말은 간단하지만 쉽사리 완성이 되지 않기때문에, 진보환액(50819) 50개와 활석(51001) 100개정도 구해서 악사 유환(20017)에게 가져다 주게나.[ENTER]"
locale.main_quest_flame_lv102.say_40 = "알겠네. 건투를 빌겠네.[ENTER]"
locale.main_quest_flame_lv102.say_41 = "진보환액(50819)과 활석(51001)을 구해 악사 유환(20017)에게 가져다주자."
locale.main_quest_flame_lv102.say_42 = "탁기 정화 제료를 구하자!"
locale.main_quest_flame_lv102.say_43 = "진보환액(50819)과 활석(51001)을 구하자.[ENTER]"
locale.main_quest_flame_lv102.say_44 = "이 친구 일처리가 빠르군 그래.[ENTER]나머지는 나에게 맡기도록 하고, 피로를 좀 풀고 있게나.[ENTER]"
locale.main_quest_flame_lv102.say_45 = "학자(20380)님께 전해 들은 것보다 적다네.[ENTER]아직도 충분히 구해오지 못한 것이로 구만.[ENTER]어서 나머지 재료도 충분히 구해다 주게.[ENTER]"
locale.main_quest_flame_lv102.say_46 = "진보환액(50819)과 활석(51001)을 충분히 구했다. 악사 유환(20017)에게 가져다주자."
locale.main_quest_flame_lv102.say_47 = "악사 유환(20017)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv102.say_48 = "정화의 액 전달"
locale.main_quest_flame_lv102.say_49 = "악사 유환(20017)이 당신을 급하게 찾고 있습니다.[ENTER]유환(20017)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv102.say_50 = "어서오게.[ENTER]자네가 보내준 제료로 정화의 액을 만들었다네.[ENTER]원래대로라면 수량이 더 나와야 하지만, 일부는 제조 중에 실패하여 수량이 좀 줄긴 하였네.[ENTER]그래도 충분한 수량이 나왔으니, 약환(20377)님께 전해주게나.[ENTER]"
locale.main_quest_flame_lv102.say_51 = "그런데 액이라 하였는데 왜 환약과 같이 단단합니까?[ENTER]"
locale.main_quest_flame_lv102.say_52 = "이 친구 그런 사소한 것은 신경 쓰지 말게나.[ENTER]그 만큼 농축하여 그리 된 것이네.[ENTER]"
locale.main_quest_flame_lv102.say_53 = "그럼 부탁 함세.[ENTER]"
locale.main_quest_flame_lv102.say_54 = "악사 유환(20017)에게 받은 정화의 액을 약환(20377)에게 전해주자."
locale.main_quest_flame_lv102.say_55 = "왜 손에 아직 정화의 액이 있는 것인가?[ENTER]어서 약환(20377)님께 정화의 액을 전해주게나.[ENTER]"
locale.main_quest_flame_lv102.say_56 = "오~어서오게나.[ENTER]한참을 기다리고 있었다네.[ENTER]유환(20017)에게도 말은 이미 전해 들었고 말이야.[ENTER]정화의 액은 가지고 왔는가?[ENTER]"
locale.main_quest_flame_lv102.say_57 = "네. 농축하여 거의 환이 되었습니다.[ENTER]적룡성(zone1) 돌입 시에 유용하게 사용이 될 것 같습니다.[ENTER]"
locale.main_quest_flame_lv102.say_58 = "그래. 고생하였네.[ENTER]부하들과 함께 돌입 준비도 하여야하고 정화의 액은 두고 가게나.[ENTER]그리고 하나는 자네가 가지고 가게.[ENTER]아무래도 자네가 큰 힘의 축이 될 것 같아서 말이야.[ENTER]나중에 돌입 강행 시에 부르도록 하겠네.[ENTER]"

locale.main_quest_flame_lv103 = {}
locale.main_quest_flame_lv103.say_1 = "적룡성(zone1)을 향해"
locale.main_quest_flame_lv103.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv103.say_3 = "어서오게.[ENTER]책사들과 논의를 하여, 엄선된 병사를 추려 정화의 액을 녹인 물을 마시게 하였다네.[ENTER]효과가 대단하더군 그래.[ENTER]온몸에 청아한 기운이 돌고 정신이 또렷해지며, 기운이 올라가더구만.[ENTER]그래서 적룡성(zone1)에 파견을 하였다네.[ENTER]"
locale.main_quest_flame_lv103.say_4 = "부하들이 먼저 출발 하였으니 자네가 지금 뒤따라 가면 마주 할 수 있을 것이야.[ENTER]내 부하들과 함께 적룡성(zone1)을 쓸어버리고, 가능하다면 야마천(6091)까지 처치 부탁하네.[ENTER]최 우선적으로 소통을 위하여 보낸 내 부하를 찾길 바라네.[ENTER]자네가 할 일을 일러 줄 것이네.[ENTER]"
locale.main_quest_flame_lv103.say_5 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv103.say_6 = "그럼 부탁 함세.[ENTER]"
locale.main_quest_flame_lv103.say_7 = "적룡성(zone1)에서 약환의 부하(20393)를 찾자."
locale.main_quest_flame_lv103.say_8 = "약환의 부하(20393)를 찾자."
locale.main_quest_flame_lv103.say_9 = "적룡성(zone1)에 돌입한 약환의 부하(20393)를 찾자.[ENTER]"
locale.main_quest_flame_lv103.say_10 = "이봐. 안가고 왜 다시 온게야.[ENTER]어서 적룡성(zone1)으로 출발하게나.[ENTER]"
locale.main_quest_flame_lv103.say_11 = "´ë¡¦Çù´Ô¡¦¿©±é´Ï´Ù¡¦[ENTER]¸¶¹°µéÀ» ¸»»ìÇÏ°í µ¹ÀÔÀ» ÇÏ¿´À¸³ª, °á±¹ ¾ß¸¶Ãµ(6091)ÀÇ ¾ÐµµÀûÀÎ Èû ¾Õ¿¡[ENTER]Àü¸ê ´çÇÏ°í ¸»¾Ò½À´Ï´Ù.[ENTER]È¥ÀÚ¼­ ÇØ°á ÇÒ ¼ö ÀÖ´Â ºÎºÐÀÌ ¾Æ´Ï´Ï ¾àÈ¯(20377)´Ô²² ¸»À» ÀüÇØÁÖ½ÃÁö¿ä.[ENTER]"
locale.main_quest_flame_lv103.say_12 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv103.say_13 = "생각보다 빨리 오는 군 그래.[ENTER]적룡성(zone1)은 해결하고 오는 길인가?[ENTER]"
locale.main_quest_flame_lv103.say_14 = "¾Æ¡¦Àú¡¦±×°Ô¡¦[ENTER]"
locale.main_quest_flame_lv103.say_15 = "무엇이기에 그리 말을 흐리는 겐가?[ENTER]무슨 일이라도 있었나?[ENTER]"
locale.main_quest_flame_lv103.say_16 = "정화의 액을 마신 병사들은 강력하게 변화하여 적룡성(zone1)에 돌입을 하였으나, 염왕 야마천(6091)에게 몰살을 당하였습니다.[ENTER]이 내용도 마지막에 남은 병사가 겨우 말을 전하고 명을 달리 하였습니다.[ENTER]다소 성급하게 돌입을 한 것이 화근인 것 같습니다.[ENTER]"
locale.main_quest_flame_lv103.say_17 = "´ÄÀ¸¸é Á×¾î¾ßÁö¡¦[ENTER]³ªÀÇ ¼º±ÞÇÑ ÆÇ´ÜÀ¸·Î ºÎÇÏµéÀ» »çÁö·Î ³»¸ô°Ô µÈ °ÍÀÌ·Î±¸³ª¡¦[ENTER]Àü¿­À» °¡´Ùµë¾î¾ß °Ú³×.[ENTER]±âº°À» ³ÖÀ» ÅÍÀÌ´Ï ´ë±âÇÏ°í ÀÖ°Ô³ª.[ENTER]"
locale.main_quest_flame_lv103.say_18 = "점진적 격파"
locale.main_quest_flame_lv103.say_19 = "이제 오는겐가?[ENTER]지난번 나의 과오로 인하여, 숱한 생명이 사라져 갔네.[ENTER]초마단의 음모를 저지하여야하는 중요한 시기이기에 여기서 포기 할 수 없었다네.[ENTER]그리하여, 논의 끝에 자네를 주축으로 하는 책략을 준비하였네.[ENTER]"
locale.main_quest_flame_lv103.say_20 = "일단 자네가 무공이 고강한 것이 사실이기에 그에 맞는 책략이니 너무 어려워하지는 말게나.[ENTER]지난번 정화의 액을 사용으로 인한 검증은 충분히 되었네.[ENTER]이번에도 정화의 액을 사용하여 병사를 강화할 것이네.[ENTER]그러나 염왕 야마천(6091)의 처치는 우리쪽에서는 어려울 것으로 보여, 우리 병사들은 외부로[ENTER]창궐하려하는 마물을 제거할 생각이네.[ENTER]"
locale.main_quest_flame_lv103.say_21 = "±× µ¿¾È ÀÚ³×´Â Àû·æ¼º(zone1)¿¡ µ¹ÀÔÇÏ¿©, ¸¶¹°À» 1Ãµ¸¶¸® °¡¸®Áö ¾Ê°í Á¦°ÅÇØ ÁÖ°Ô³ª.[ENTER]¼öÂ÷·Ê µ¹ÀÔÇØ¼­ ±× Á¤µµ ¸¶¹°ÀÇ ¼ö¸¦ Á¦°ÅÇÑ´Ù¸é ¾ß¸¶Ãµ(6091)µµ ±äÀåÇÒ °ÍÀÌ°í, µ¿¿äÇÏ´Â Æ´¿¡[ENTER]±âÈ¸°¡ ºÐ¸í ¿À°Ô µÉ °ÍÀÌ¾ß.[ENTER]ºÎÅ¹ÇÏ°Ú³×.[ENTER]"
locale.main_quest_flame_lv103.say_22 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv103.say_23 = "적룡성(zone1)에 돌입하여 적룡성(zone1)의 마물을 처치하자."
locale.main_quest_flame_lv103.say_24 = "마물을 도륙하자."
locale.main_quest_flame_lv103.say_25 = "적룡성(zone1)에 돌입하여, 마물을 1000마리 격파하자.[ENTER]"
locale.main_quest_flame_lv103.say_26 = "¡ØÁö±ÞµÈ ÅëÇàÁõÀº Àç¹ß±Þ µÇÁö ¾Ê½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv103.say_27 = "아직 모두 격파하지 못한 것 같군 그래.[ENTER]적룡성(zone1)의 마물이 아직도 밖으로 창궐하고 있으니 말이야.[ENTER]요령피울 생각하지 말고, 어서 다녀오게나.[ENTER]"
locale.main_quest_flame_lv103.say_28 = "적룡성(zone1)의 마물을 1000마리 모두 격파하였다.[ENTER]약환(20377)에게 보고하자."
locale.main_quest_flame_lv103.say_29 = "부하들의 말에 따르면 적룡성(zone1)에서 창궐하여 외부로 나오는 마물이 이제 거의 사라졌다고 하는군.[ENTER]제네가 충분히 격파해준 것 같군 그래.[ENTER]고생하였네.[ENTER]다음 계획을 학자(20380)와 논의 중이니 잠시만 기다리고 있게나.[ENTER]"
locale.main_quest_flame_lv103.say_30 = "적룡성(zone1)의 골칫거리"
locale.main_quest_flame_lv103.say_31 = "잘왔네.[ENTER]자네가 마물을 격파해준 덕분에 돌입이 수월해지나 싶었지만, 척후에 의하면 아직 어려운 것 같더군 그래.[ENTER]아무래도 어줍지 않은 마물을 처치해서는 돌입에 영향을 줄 것 같지가 않네.[ENTER]그래서 척후의 정보에 따라 수뇌부로 보이는 녀석들을 우선적으로 처치하여야 할 것 같다네.[ENTER]"
locale.main_quest_flame_lv103.say_32 = "그 녀석들은 화마(6005),염마(6007),화괴(6006),염괴,초열괴(6009) 이 5녀석들이라네.[ENTER]이 녀석들은 적룡성(zone1)에서도 상급 마물이기도 하지만, 염왕 야마천(6091)을 제외한 수뇌부로 판단하였네.[ENTER]뱀의 머리를 자르듯 수뇌부에 막대한 피해를 주면 우리의 계획이 순조롭게 진행이 될 것이네.[ENTER]그래서 말이니, 5녀석들을 각 50마리씩 처치해 주게나.[ENTER]"
locale.main_quest_flame_lv103.say_33 = "적룡성(zone1)에 돌입하여 수뇌부 마물을 제거하자."
locale.main_quest_flame_lv103.say_34 = "적룡성(zone1) 수뇌부를 격파하라."
locale.main_quest_flame_lv103.say_35 = "적룡성(zone1)에 돌입하여, 수뇌부 마물을 처치하자.[ENTER]"
locale.main_quest_flame_lv103.say_36 = "아직 수뇌부를 충분히 격파하지 못한 것 같군 그래.[ENTER]우리의 계획을 위해 꼭 필요한 작전이니 다시 다녀오게나.[ENTER]"
locale.main_quest_flame_lv103.say_37 = "적룡성(zone1)의 수뇌부 마물을 모두 처치하였다.[ENTER]약환(20377)에게 보고하도록 하자."
locale.main_quest_flame_lv103.say_38 = "이제 우리가 적룡성(zone1)에 돌입하여 초마단의 음모를 저지할 수 있겠군 그래.[ENTER]이제 조금만 더 나아가면 되니 조금만 더 힘내 주게나.[ENTER]고생하였네.[ENTER]"
locale.main_quest_flame_lv103.say_39 = "야마천(6091) 토벌1-1"
locale.main_quest_flame_lv103.say_40 = "자네가 수뇌부를 격파해준 덕분에 내 부하들이 순풍을 만난 배처럼 적룡성(zone1)을 점진적으로 격파해 나아가고 있다네.[ENTER]정말 자네의 덕이 크다네.[ENTER]다시금 감사를 표하지.[ENTER]이제 적룡성(zone1)의 격파와 함께 초마단의 음모를 저지할 수 있을 것같구만.[ENTER]자네를 부른 것은 현황을 알려주는 것과 감사를 표하기 위해서 였네.[ENTER]학자(20380)가 책략을 구상해 둔 것이 있으니 지금 학자(20380)에게 가보게나.[ENTER]"
locale.main_quest_flame_lv103.say_41 = "학자(20380)에게 책략을 들으러 가자."
locale.main_quest_flame_lv103.say_42 = "학자(20380)를 찾아가자.[ENTER]"
locale.main_quest_flame_lv103.say_43 = "어서오게. 기다리고 있었다네.[ENTER]약환(20377)님께 자네의 혁혁한 전과를 계속 접하고 있었다네.[ENTER]일단 그건 그거고, 초마단의 음모를 막기 위함이 크니 책략부터 설명해 주겠네.[ENTER]"
locale.main_quest_flame_lv103.say_44 = "약환(20377)님의 도움으로 무예가 뛰어난 병사를 선발하여, 염왕 야마천(6091)에 대하여 조사를 하였네.[ENTER]그 결과 큰 소득을 얻었다네.[ENTER]염왕 야마천(6091)은 마물들이 득실거리는 곳에 숨겨둔 화마석(8057)에서 힘을 얻는 것 같더군.[ENTER]그 화마석(8057)을 제거한다면 야마천(6091)의 처치는 훨씬 수월하게 될 것이야.[ENTER]"
locale.main_quest_flame_lv103.say_45 = "그러면 제가 그 화마석(8057)을 제거하고 오면 되는 겁니까?[ENTER]"
locale.main_quest_flame_lv103.say_46 = "그렇다네.[ENTER]자네도 알다시피 화마석(8057)을 격파하면 마물들이 엄청나게 출몰을 하게되지.[ENTER]그것을 유념하여 조심스러우면서 과감하게 일을 행해 주게나.[ENTER]화마석(8057)이 어느곳에서 찾게 될지는 미지수이니 늘 긴장을 하게나.[ENTER]"
locale.main_quest_flame_lv103.say_47 = "야마천(6091)을 상대하여야 할 때에는 약환(20377)님이 일러주실 것이야.[ENTER]그러니 자네는 지금 적룡성(zone1)으로 출발하여, 화마석(8057)을 1개 격파하고 약환(20377)님께 보고해 주게나.[ENTER]"
locale.main_quest_flame_lv103.say_48 = "적룡성(zone1)에서 화마석(8057)을 격파하자."
locale.main_quest_flame_lv103.say_49 = "화마석(8057)을 격파하자."
locale.main_quest_flame_lv103.say_50 = "적룡성(zone1)에 있는 화마석(8057)을 격파하자.[ENTER]"
locale.main_quest_flame_lv103.say_51 = "남은  파괴수"
locale.main_quest_flame_lv103.say_52 = "내 부하가 야마천(6091)을 상대하기는 어려우나, 숨어서 척후를 함에는 문제가 없다네.[ENTER]아무리 내가 늙었다 하더라도, 나를 속이려 드는 겐가?[ENTER]화마석(8057)을 격파하여야 다음 책략을 수행할 수 있으니 어서 격파하고 돌아오게나.[ENTER]"
locale.main_quest_flame_lv103.say_53 = "적룡성(zone1)에 있는 화마석(8057)을 모두 격파하였다.[ENTER]약환(20377)에게 전하도록 하자."
locale.main_quest_flame_lv103.say_54 = "화마석(8057)을 모두 격파하였다는 보고를 받았네.[ENTER]자네가 올 것이라는 것도 알고 있었고 말이야.[ENTER]고생하였네. 다음 책략은 자네도 눈치가 있으니 알것이네.[ENTER]바로 염왕 야마천(6091)의 격파이네.[ENTER]준비에 시간이 걸리기도하니, 나중에 내가 부르거든 지금 건내어 줄[ENTER]정화의 액도 함께 챙겨 오도록 하게나.[ENTER]"

locale.main_quest_flame_lv104 = {}
locale.main_quest_flame_lv104.say_1 = "야마천(6091) 토벌 1-2"
locale.main_quest_flame_lv104.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv104.say_3 = "오래 기다렸네.[ENTER]자네가 화마석(8057)을 격파하였기 때문에 야마천(6091)의 격파도 이제 가능해 졌다네.[ENTER]이제 그 시기가 온것이야.[ENTER]초마단의 음모를 저지하고 염왕 야마천(6091)의 격파를 위해 아직까지 많은 희생이 있었으니[ENTER]이번에는 기필코 완수해 내어야만 한다네.[ENTER]"
locale.main_quest_flame_lv104.say_4 = "자네 일전에 주었던 정액을 가지고 있을 것이네.[ENTER]부하들에게는 정제하여 주었지만 자네의 것은 온전한 것이기에 효력이 어마어마하지.[ENTER]그것을 복용하고 야마천(6091)을 격파해주게나.[ENTER]아무리 온전한 것이라도 1시간이상 사용하기 어려우니 적당한 시기에 잘 복용하게나.[ENTER]그렇지 않으면 야마천(6091)을 상대할때 큰 애로사항이 꽃필것이네.[ENTER]"
locale.main_quest_flame_lv104.say_5 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv104.say_6 = "적룡성(zone1)에서 염왕 야마천(6091)을 격파하자."
locale.main_quest_flame_lv104.say_7 = "염왕 야마천(6091)을 격파하라."
locale.main_quest_flame_lv104.say_8 = "적룡성(zone1)에 돌입하여 염왕 야마천(6091)을 격파하자.[ENTER]"
locale.main_quest_flame_lv104.say_9 = "¡ØÁö±ÞµÈ ÅëÇàÁõÀº Àç¹ß±Þ µÇÁö ¾Ê½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv104.say_10 = "염왕 야마천(6091)이 쉬운 상대가 아니지.[ENTER]그러나 우리가 초마단의 음모를 저지하고 불필요한 살상을 없에기 위해서는[ENTER]꼭 격파하여야만 한다네.[ENTER]다시금 힘을 내어 다녀와 주게나.[ENTER]"
locale.main_quest_flame_lv104.say_11 = "ÀÌÁ¦ Á¶±Ý¸¸ ´õÇÏ¸é Àû·æÀÇ ºÀÀÎÀ» Ç® ¼ö ÀÖ¾ú´Âµ¥¡¦[ENTER]³ª¸¦ ¹æÇØÇÏ´Ù´Ï¡¦[ENTER]Àû·æµµ ³¯¶Ù°í ½Í¾îÇÏ¿© ³ª¸¦ ±×´ë·Î Á×°ÔÇÏÁö ¾ÊÀ» °ÍÀÌ´Ù.[ENTER]³ª´Â ºÐ¸íÈ÷ ´Ù½Ã µ¹¾Æ ¿Ã °ÍÀÌ´Ù.[ENTER]"
locale.main_quest_flame_lv104.say_12 = "적룡성(zone1)의 염왕 야마천(6091)을 격파하였다.[ENTER]약환(20377)에게 전하도록 하자."
locale.main_quest_flame_lv104.say_13 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv104.say_14 = "대단해. 아주 대단해.[ENTER]자네가 범상치 않은 인물인 것은 알았지만, 이렇게 야마천(6091)까지 격파하였을 줄이야.[ENTER]고생하였네 가보게나.[ENTER]"
locale.main_quest_flame_lv104.say_15 = "약환(20377)님 그런데 염왕 야마천(6091)이 죽으면서, 이런 말을 하였습니다.[ENTER]적룡이 날뛰고 싶어하여 나를 그대로 죽게하지 않을 것이고, 분명 나는 다시 돌아 올 것이다. 라고 했습니다.[ENTER]아무래도 이게 끝이 아닐 것으로 보입니다.[ENTER]"
locale.main_quest_flame_lv104.say_16 = "알겠네.[ENTER]이제 끝일 줄 알았는데 그게 아니었군 그래.[ENTER]내가 더 알아본 후에 부르도록 하겠네.[ENTER]"
locale.main_quest_flame_lv104.say_17 = "음모의 저지1-1"
locale.main_quest_flame_lv104.say_18 = "어서오게.[ENTER]자네가 야마천(6091)을 처치하고 그 녀석이 남긴 말을 따라 다양하게 조사를 해 보았네.[ENTER]척후를 보내어 확인도 하였고 말이야.[ENTER]자네 말처럼 야마천(6091)은 부활하여 있었네.[ENTER]거기다가 적룡성(zone1)에서 뿜어저나오는 탁기와 괴성이 더 심해졌다네.[ENTER]"
locale.main_quest_flame_lv104.say_19 = "아무래도 염왕 야마천(6091)을 다시 제거하면서 금제를 가하여야만 안심을 할 수 있을 것으로 보이네.[ENTER]더 강력해진 녀석을 상대하자면 적지않은 희생이 따를 것으로 보이네.[ENTER]수고스럽겠지만 이부분에 대해서는 학자(20380)에게도 일러 두었으니, 학자(20380)에게 가서 자세한 방안을 들어보도록 하게나.[ENTER]"
locale.main_quest_flame_lv104.say_20 = "학자(20380)에게 가서 책략을 듣도록 하자."
locale.main_quest_flame_lv104.say_21 = "학자(20380)에게 가자."
locale.main_quest_flame_lv104.say_22 = "학자(20380)에게 가서 책략을 들어보자.[ENTER]"
locale.main_quest_flame_lv104.say_23 = "ÇÐÀÚ(20380)¿¡°Ô °¡º¸¶ó´Ï±î ¿Ö ´Ù½Ã¿À´Â °Ô¾ß¡¦[ENTER]¾î¼­ °¡º¸°Ô³ª.[ENTER]"
locale.main_quest_flame_lv104.say_24 = "어서오게. 기다리고 있었다네.[ENTER]얼마전에는 염왕 야마천(6091)을 물리쳤다지?[ENTER]범상치 않은 기운을 품고 있어, 잘 해내리라 생각했지만 대단하네 그려.[ENTER]일단 책략을 설명해 주도록 하겠네.[ENTER]"
locale.main_quest_flame_lv104.say_25 = "야마천(6091)이 적룡의 기운으로 되살아 난 것은 사실이네.[ENTER]그 힘을 차단하기에는 적룡에게 직접적인 금제를 가하여야 효율적일 것이네.[ENTER]그렇기때문에 금제를 가할 방책을 구하였으니 자네가 나를 도와 이행을 해주면 된다네.[ENTER]"
locale.main_quest_flame_lv104.say_26 = "알겠습니다. 그럼 제가 도울일은 어떤 것입니까?[ENTER]"
locale.main_quest_flame_lv104.say_27 = "일전에 음양오행의 이치를 간단히 알려주면서 음과 양에 대해서 알게 되었을 것이네.[ENTER]그런데 어느 성질이든 극을 달성한 이후에서는 그 이상 힘을 발휘하는 것이 아니라 통제를 할 수 없게된다네.[ENTER]그래서 생각한 방안이 양을 양으로 제자하는 방책을 새웠다네.[ENTER]이 방책을 이행하기 위해서는 양의 성질이 강한 일종의 핵을 구하여야 한다네.[ENTER]알아본 결과 양질의 핵을 구하기 위해서는 화마(6005),염마(6007),화괴(6006),염괴,초열괴(6009) 이 마물들을 처치하고 핵을 구해 와야 할 것이네.[ENTER]그 일을 할 사람은 자네 뿐이니 부디 핵5개쯤 구해다 주게.[ENTER]"
locale.main_quest_flame_lv104.say_28 = "금제에 필요한 재료를 구하자."
locale.main_quest_flame_lv104.say_29 = "금제에 필요한 재료를 구해서 학자(20380)에게 전해주자..[ENTER]"
locale.main_quest_flame_lv104.say_30 = "오~이정도면 충분하군.[ENTER]이제 %d개만 더 구해오면 되겠군 그래.[ENTER]좀더 힘내게나.[ENTER]"
locale.main_quest_flame_lv104.say_31 = "이제 핵은 모두 모았군 그래.[ENTER]하지만 이것 말고도 필요한 재료는 더 있다네.[ENTER]일단 이 핵들을 정제한 후에 기별을 넣을 터이니 쉬고 있게나.[ENTER]"
locale.main_quest_flame_lv104.say_32 = "À½¡¦ÀÚ³×°¡ °í»ýÇÏ¿© ±¸ÇØ¿ÔÁö¸¸, ÀÌ ÇÙÀº ³Ê¹« ¿­±â°¡ ³·¾Æ »ç¿ëÇÏ±â ¾î·Á¿ï °Í °°±º±×·¡.[ENTER]´Ù¸¥°É ±¸ÇØ´Ù ÁÖ°Ú³ª?[ENTER]"
locale.main_quest_flame_lv104.say_33 = "음모의 저지1-2"
locale.main_quest_flame_lv104.say_34 = "학자(20380)가 당신을 급하게 찾고 있습니다.[ENTER]학자(20380)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv104.say_35 = "뜨거운 핵(31082)을 구한 이후 정제에 들어가 있는 상태라네.[ENTER]너무 뜨거워서 공력이 없는 나같은 학자(20380)나부랭이는 여간 고생이 아니라네.[ENTER]그 덕에 손에 화상도 입고 말이야.[ENTER]상황이 상황이니 만큼 다음 재료를 알려주겠네.[ENTER]자네가 이번에 가져다 줄 것은 피독액(50814)이라네.[ENTER]"
locale.main_quest_flame_lv104.say_36 = "피독액(50814)은 정제한 핵을 위에 올리고 증기로 녹여 내야하는데 쓰인다네.[ENTER]그게 완성이 되고 나면, 금제를 시작 할 수 있을 것이네.[ENTER]방법은 나중에 약환(20377)님께 듣도록 하고, 피독액(50814)은 좀 많이 필요하니 100개정도 구해다 주게나.[ENTER]그럼 기다리고 있겠네.[ENTER]"
locale.main_quest_flame_lv104.say_37 = "피독액(50814)을 구해 학자(20380)에게 전해주자."
locale.main_quest_flame_lv104.say_38 = "피독액(50814)을 구해 학자(20380)에게 전해주자.[ENTER]"
locale.main_quest_flame_lv104.say_39 = "가져왔군 그래.[ENTER]이제 증기로 녹여내면 거의 완성이 된다네.[ENTER]고생하였네.[ENTER]"
locale.main_quest_flame_lv104.say_40 = "거의 완성이라뇨?[ENTER]아직 남은 것입니까?[ENTER]"
locale.main_quest_flame_lv104.say_41 = "마지막이 남긴 하였네.[ENTER]일단 녹여내는데 신중을 기해야하니 나중에 다시 부르도록 하겠네.[ENTER]"
locale.main_quest_flame_lv104.say_42 = "ÀÚ³× ¼³¸¶ ¼ýÀÚ¸¦ ÇØ¾Æ¸®Áö ¸øÇÏ´Â °ÍÀÎ°¡?[ENTER]¾Æ¹«¸® ¹«ÀÎÀÌ¶óÁö¸¸ 100±îÁö´Â ÇØ¾Æ·Á¾ßÁö¡¦ÂìÂì[ENTER]¾î¼­°¡¼­ ³ª¸ÓÁö ÇÇµ¶¾×(50814)±îÁö ±¸ÇØ ´Ù½Ã¿À°Ô³ª.[ENTER]"
locale.main_quest_flame_lv104.say_43 = "피독액(50814)을 모두 구하였다. 학자(20380)에게 전해주자."
locale.main_quest_flame_lv104.say_44 = "악룡군도의의 학자(20380)를 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv104.say_45 = "음모의 저지1-3"
locale.main_quest_flame_lv104.say_46 = "자네가 나를 도와준 덕분에 준비가 착착 되어가고 있네.[ENTER]이제 막바지로 달리고 있으니 조금만 더 힘내주게나.[ENTER]마지막 단계이네.[ENTER]"
locale.main_quest_flame_lv104.say_47 = "이제 마무리는 이 정제액을 온전하게 담아줄 용기를 만드는 것이네.[ENTER]지금 물질은 양의 기운이 넘쳐흘러 몹시 상태가 불안정한 상태라네.[ENTER]그 만큼 효과는 보증이지만 말이야.[ENTER]각설하고 자네 일전에 초마단을 추적하면서 범익족을 상대한 적이 있을 것이네.[ENTER]범익족 중에서도 범익 족장(3291)에게서 가죽을 구해다 주게나.[ENTER]겹겹이 동여매어야 하니, 5장만 구해다 주게나.[ENTER]"
locale.main_quest_flame_lv104.say_48 = "질 좋은 가죽을 구해 학자(20380)에게 전해주자."
locale.main_quest_flame_lv104.say_49 = "범익족의 가죽(31024)을 구해 학자(20380)에게 전해주자.[ENTER]"
locale.main_quest_flame_lv104.say_50 = "이제 마무리인데 분발해서 모두 구해다 주게.[ENTER]초마단도 적룡도 모두 저지해야만하네.[ENTER]그럼 기다리고 있겠네.[ENTER]"
locale.main_quest_flame_lv104.say_51 = "오~이제 모든 재료가 구해졌구만.[ENTER]잠시만 기다리게.[ENTER]곧 처리하여 줄터이니 나중에 약환(20377)님이 부르시거든 가지고 가게나.[ENTER]"
locale.main_quest_flame_lv104.say_52 = "가죽을 모두 구하였다. 학자(20380)에게 전해주자."

locale.main_quest_flame_lv105 = {}
locale.main_quest_flame_lv105.say_1 = "격전의 시작"
locale.main_quest_flame_lv105.say_2 = "약환(20377)이 당신을 급하게 찾고 있습니다.[ENTER]약환(20377)에게 자세한 이야기를 들어보도록 하십시오.[ENTER]"
locale.main_quest_flame_lv105.say_3 = "오렌만이네.[ENTER]학자(20380)를 도와 금제에 필요한 물질을 만들었다고 들었네.[ENTER]금제 주머니(31083)를 가져 왔는가?[ENTER]"
locale.main_quest_flame_lv105.say_4 = "여기 있습니다. 총 3개입니다.[ENTER]"
locale.main_quest_flame_lv105.say_5 = "금제 주머니(31083)의 사용법은 나중에 알려주겠네.[ENTER]일단 자네가 해주어야 할 일을 알려주도록 하지.[ENTER]금제 주머니(31083)가 완성이 되었지만, 자네도 알다시피 적룡의 영향으로 염왕 야마천(6091)까지 모두 되살아난 상태이네.[ENTER]무척이나 애로사항이 꽃피는 상황이지.[ENTER]이번에는 내 직할의 멸마군의 5할을 작전에 투입할 것이네.[ENTER]"
locale.main_quest_flame_lv105.say_6 = "적룡성(zone1)에 있는 마물을 내 병사로 어느정도 정리 할 수가 있다네.[ENTER]그러나 수뇌부 마물부터 염왕 야마천(6091)까지는 아직 내 병사들이 감당하기에는 너무나 [ENTER]큰 장벽과도 같지.[ENTER]일단 수뇌부 마물인 화마(6005),염마(6007),화괴(6006),염괴,초열괴(6009)를 10마리씩 처리하고 돌아와 주게나.[ENTER]"
locale.main_quest_flame_lv105.say_7 = "네. 알겠습니다.[ENTER]"
locale.main_quest_flame_lv105.say_8 = "적룡성(zone1)에 돌입하여 수뇌부 마물을 제거하자."
locale.main_quest_flame_lv105.say_9 = "수뇌부를 격파하자."
locale.main_quest_flame_lv105.say_10 = "적룡성(zone1)에 돌입하여, 수뇌부 마물을 처치하자.[ENTER]"
locale.main_quest_flame_lv105.say_11 = "¡ØÁö±ÞµÈ ÅëÇàÁõÀº Àç¹ß±Þ µÇÁö ¾Ê½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv105.say_12 = "아직 수뇌부를 충분히 격파하지 못한 것 같군 그래.[ENTER]수뇌부를 격파하지 못하면 적룡의 금제고 초마단이고 모두 어찌할 방도가 없어진다네.[ENTER]어서 다녀오게.[ENTER]"
locale.main_quest_flame_lv105.say_13 = "적룡성(zone1)의 수뇌부 마물을 모두 처치하였다.[ENTER]약환(20377)에게 보고하도록 하자."
locale.main_quest_flame_lv105.say_14 = "악룡군도의의 약환(20377)을 찾아 말을 걸도록 하자.[ENTER]"
locale.main_quest_flame_lv105.say_15 = "이제 우리가 적룡성(zone1)에 돌입하여 초마단의 음모를 저지할 수 있겠군 그래.[ENTER]이제 적룡을 금제하는 것만 남았군 그래.[ENTER]고생하였네.[ENTER]"
locale.main_quest_flame_lv105.say_16 = "은원"
locale.main_quest_flame_lv105.say_17 = "ÀÚ³×°¡ ¼ö³úºÎ¸¦ °ÝÆÄÇÏ°í ÀÏÀü¿¡ ¿°¿Õ ¾ß¸¶Ãµ(6091)À» Ã³Ä¡Çß¾ú´øÁö¶ó, ºÎÈ°ÇÑ ¾ß¸¶Ãµ(6091)ÀÌ »ó´çÈ÷ ºÐÇÑ°¡ º¸´õ±º.[ENTER]±«¼ºÀÌ Àû·æ¼º(zone1)¿¡ Â¼··Â¼·· ÇÏ´Ù´Â º¸°í°¡ µé¾î ¿Ô³×.[ENTER]¹¹ ¾ß¸¶Ãµ(6091)Àº ÀÚ³×¿¡°Ô ¼÷ÀûÀÌ µÈ ¼ÀÀÌ´Ï ±×·²¸¸µµÇÏÁö.[ENTER]±×·¸´Ù°í Å©°Ô ´Þ¸®ÁöÁö´Â ¾Ê°ÚÁö¸¸ ¸»ÀÌ¾ß.[ENTER]"
locale.main_quest_flame_lv105.say_18 = "이번에 자네가 해줄 일은 눈치가 있으면 알겠지만, 바로 염왕 야마천(6091)의 처치이네.[ENTER]전에도 처치한 이력이 있으나, 적룡에 의해서 다시 부활하였고 자네에게 앙심을 품고 있으니[ENTER]주의하게나.[ENTER]아무리 쉬운일 해내었던 일도 방심하면 그릇치게 되니 명심하고 다녀오게.[ENTER]"
locale.main_quest_flame_lv105.say_19 = "적룡성(zone1)에서 염왕 야마천(6091)을 격파하자."
locale.main_quest_flame_lv105.say_20 = "염왕 야마천(6091)을 처단하자."
locale.main_quest_flame_lv105.say_21 = "적룡성(zone1)에 돌입하여, 염왕 야마천(6091)을 처치하자.[ENTER]"
locale.main_quest_flame_lv105.say_22 = "염왕 야마천(6091)이 무서웠던 겐가?[ENTER]어서가서 야마천(6091)을 처치하고 와주게나.[ENTER]자네라면 능히 해낼 수 있을 것이야.[ENTER]"
locale.main_quest_flame_lv105.say_23 = "적룡성(zone1)의 염왕 야마천(6091)을 격파하였다.[ENTER]약환(20377)에게 전하도록 하자."
locale.main_quest_flame_lv105.say_24 = "역시!!역시 대단해.[ENTER]고생하였네.[ENTER]부하로부터 보고를 좀전에 받았다네.[ENTER]이제 적룡에게 직접적인 금제를 가하기만하면 어느정도 마무리가 될 것 같구만.[ENTER]병사들를 정비하고 다시 부르도록 하겠네.[ENTER]"
locale.main_quest_flame_lv105.say_25 = "적룡의 금제"
locale.main_quest_flame_lv105.say_26 = "드디어 때가 되었네.[ENTER]적룡에게 금제를 가하고 초마단의 음모를 저지시킬 시간이말이야.[ENTER]이제 금제 주머니(31083)의 사용법과 해야할 행동을 지시하도록 하겠네.[ENTER]"
locale.main_quest_flame_lv105.say_27 = "전에 나에게 전해준 금제주머니(31083)는 내 부하들을 통하여 2개의 지점에 둘러서 진을 설치해 두었다네.[ENTER]금제 주머니(31083)에 들어 있는 것으로 진을 설치할 수 있고, 사용에 따라 격한 기폭제가 되기도하지.[ENTER]자네가 할 일은 바로 기폭이 일어날 수 있도록 하는 것이네.[ENTER]염왕 야마천(6091)을 처치한 곳에서 적룡이 봉인된 곳이 보일 것이네.[ENTER]거기에 이 금제 주머니(31083)를 강하게 던지게.[ENTER]주머니속이 불안정하기 때문에 기폭이 일어날 것이고 내 부하들이 설치한 진식으로 인하여[ENTER]금제가 발동 할 것이네.[ENTER]"
locale.main_quest_flame_lv105.say_28 = "적룡성(zone1)에 돌입하여 적룡에게 금제를 가하자."
locale.main_quest_flame_lv105.say_29 = "남은 금제수량"
locale.main_quest_flame_lv105.say_30 = "적룡에게 금제를 가하자."
locale.main_quest_flame_lv105.say_31 = "적룡성(zone1)에 돌입하여, 적룡봉인석에 금제주머니(31083)를 사용하자.[ENTER]"
locale.main_quest_flame_lv105.say_32 = "적룡 봉인석(20390): 1개"
locale.main_quest_flame_lv105.say_33 = "적룡 화극"
locale.main_quest_flame_lv105.say_34 = "Å©¸£¸£¡¦°Ü..¿ì °Ü¿ì ÀÌÁ¤µµÀÎ°¡!! Å©ÇÏÇÏÇÏ[ENTER]¿¹±âÄ¡ ¸øÇÏ¿© ÈûÀ» ¸¹ÀÌ ÀÒ¾úÁö¸¸, ÀÌ Á¤µµ·Î´Â ³ª¿¡°Ô ±ÝÁ¦¸¦ °¡ÇÒ ¼ö ¾ø´Ù.[ENTER]³ªµµ ¿°¿Õ ¾ß¸¶Ãµ(6091)µµ ºÎÈ°ÇÏ°í ºÀÀÎÀ» Ç®°Ô µÉ °ÍÀÌ´Ù.[ENTER]Å©ÇÏÇÏÇÏÇÏ[ENTER]"
locale.main_quest_flame_lv105.say_35 = "적룡에게 금제를 가하려 시도하였으나, 실패하였다.[ENTER]약환(20377)에게 전해주자."
locale.main_quest_flame_lv105.say_36 = "어서오게나. 대충 전해 듣기는 하였으나, 소상히 나에게 보고해 주게.[ENTER]"
locale.main_quest_flame_lv105.say_37 = "¿¹.[ENTER]Áø½ÄÀÇ ÀÛµ¿°ú ±âÆøÀº Á¤È®ÇÏ°Ô ÀÏ¾î ³µ½À´Ï´Ù.[ENTER]¹°·Ð ±ÝÁ¦°¡ ½Ãµµ°¡ µÇ¾ú°í ¸»ÀÔ´Ï´Ù.[ENTER]±×·¯³ª Àû·æÀÇ °Å´ëÇÑ Èû¿¡ ±ÝÁ¦´Â ½ÇÆÐÇÏ¿´½À´Ï´Ù.[ENTER]Àû·æÀº ¿°¿Õ ¾ß¸¶Ãµ(6091)µµ ºÎÈ°ÇÒ °ÍÀÌ°í ÀÚ½Åµµ ºÀÀÎÀ» Ç®°ÔµÉ °ÍÀÌ¶ó Çß½À´Ï´Ù.[ENTER]Çã³ª, ÀÌ¹ø ±ÝÁ¦ ½Ãµµ°¡ ¿¹»óÇÏÁö ¸øÇÏ¿´´ÂÁö Å« ÇÇÇØ¸¦ ÀÔÈ÷±â´Â ÇÏ¿´½À´Ï´Ù.[ENTER]"
locale.main_quest_flame_lv105.say_38 = "그렇군.[ENTER]고생하였네. 자네 덕분에 여기까지 잘 해낸것이니까 말이야.[ENTER]보고를 받고부터 책사와 학자(20380) 모두를 불러 이런 경우를 대비하려 했다네.[ENTER]지금은 내 부하들을 시켜 지속적으로 감시하면서 직접적인 금제를 가할 방도를 찾아야 하는 실정이라네.[ENTER]적룡에게 금제를 가하고 완벽하게 봉인할 방안을 찾거든 자네를 다시 부르도록 하겠네.[ENTER]고생하였네.[ENTER]"

locale.snow_dungeon = {}
locale.snow_dungeon.say_1 = "Black Dragon Start Intellekt"
locale.snow_dungeon.say_2 = "Pass this one abgelaufen ist. [ENTER] Black Dragon Intelligenz Position für 1 Stunde gehalten, und wurde in der Mitte des weißen Drachen intellektuellen Fortschritt gesendet oder abgeschlossen, wenn die Position auf 30 Minuten begrenzt ist."
locale.snow_dungeon.say_3 = "nicht passieren das Verlies wird raus."
locale.snow_dungeon.say_4 = "10 Sekunden später mit dem Chef Zimmer gehen"
locale.snow_dungeon.say_5 = "White Dragon von Intelligenz"
locale.snow_dungeon.say_6 = "Wollen Sie wirklich in Black Dragon Intelligenz?"
locale.snow_dungeon.say_7 = "10 Sekunden bis in den zweiten Stock zu bewegen"
locale.snow_dungeon.say_8 = "10 Sekunden bis in den vierstöckigen gehen"
locale.snow_dungeon.say_9 = "10 Sekunden bis in den fünfstöckigen gehen"
locale.snow_dungeon.say_10 = "Metin Steine ??gefunden"
locale.snow_dungeon.say_11 = "10 Sekunden bis in den dritten Stock zu bewegen"
locale.snow_dungeon.say_12 = "ist nicht der eigentliche Schlüssel"
locale.snow_dungeon.say_13 = "Bitte zerlegen Sie das zweite Verzauberung"
locale.snow_dungeon.say_14 = "Bitte zerlegen Sie das dritte Verzauberung"
locale.snow_dungeon.say_15 = "falsche Reihenfolge"
locale.snow_dungeon.say_16 = "Bitte bauen Sie die vierte Chains"
locale.snow_dungeon.say_17 = "Bitte bauen Sie die fünfte Chains"
locale.snow_dungeon.say_18 = "10 Sekunden bis in das sechsstöckige gehen"
locale.snow_dungeon.say_19 = "10 Sekunden bis in die sieben Stockwerke zu gehen"
locale.snow_dungeon.say_20 = "false verschwindet"
locale.snow_dungeon.say_21 = "10 Sekunden bis in den acht-Geschichte gehen"
locale.snow_dungeon.say_22 = "10 Sekunden bis in den neun-stöckigen gehen"
locale.snow_dungeon.say_23 = "Boss besiegt"
locale.snow_dungeon.say_24 = "1 Minuten nach dem Black Dragon Intelligenz bewegt sich aus."
locale.snow_dungeon.say_25 = "Position die ganze Zeit verstrichen ist, wird die Wartung der White Dragon Verstand bewegt werden."
locale.snow_dungeon.say_26 = "Black Dragon Intelligenz Verweilzeit Eintrag% d Minuten."
locale.snow_dungeon.say_27 = "Black Dragon Intelligenz Positionen wurden halten die Zeit abgelaufen ist."
locale.snow_dungeon.say_28 = "10 Sekunden erlischt die Black Dragon Intelligenz."
locale.snow_dungeon.say_29 = "re-entry Zeit ist mehr als fünf Minuten ist nicht erlaubt. [ENTER]"
locale.snow_dungeon.say_30 = "eine Partei kann nicht weiter vorgehen Behinderung verschwinden [ENTER]"
locale.snow_dungeon.say_31 = "Parteien nicht erreichen Kapitel 104Lv weitere Fortschritte ist unmöglich."
locale.snow_dungeon.say_32 = "Party Kapitel 103Lv nicht füllen Sie alle Quests, sind weitere Fortschritte unmöglich."
locale.snow_dungeon.say_33 = "Hanma Entscheidung der Position du das wollen?"
locale.snow_dungeon.say_34 = "position"
locale.snow_dungeon.say_35 = "Schließen"
locale.snow_dungeon.say_36 = "Boss Room Entry"
locale.snow_dungeon.say_37 = "um alles Böse zu besiegen."
locale.snow_dungeon.say_38 = "den Schlüssel, mit dem Eis von dem Bösen."
locale.snow_dungeon.say_39 = "all das Böse zu besiegen die jincho naenggoe vergeben."
locale.snow_dungeon.say_40 = "all das Siegel zu brechen."
locale.snow_dungeon.say_41 = "Abbruch notwendigen Elemente für das Böse kann bezogen werden."
locale.snow_dungeon.say_42 = "Bing Manastein durch den Sieg über alle Monster zu zerstören."
locale.snow_dungeon.say_43 = "jincho Verliehen naenggoe."
locale.snow_dungeon.say_44 = "Böse aus der Nutzung jigeon White Dragon erhalten."
locale.snow_dungeon.say_45 = "White Dragon Wheel, um zu zerstören."
locale.snow_dungeon.say_46 = "Hanma Verliehen für."
locale.snow_dungeon.say_47 = "versäumt, das Verlies mit Partei hayeotgeona starten"

locale.event_flame_dungeon_open = {}
locale.event_flame_dungeon_open.say_1 = "도염화지의 난"
locale.event_flame_dungeon_open.say_2 = "아.. 이야기를 들었는가..[ENTER]도염화지에 큰 유적이 발견됐다고 하는구만...[ENTER]나도 자네처럼 용사였으면....[ENTER]그 유적을 직접보고 싶지만 나의 역량으로 부족한듯 하이..."
locale.event_flame_dungeon_open.say_3 = "흠.... 잠시 후에 다시 오면...[ENTER]내 도염화지 유적지의 좌표가 적힌 이동서를 하나 주겠네...[ENTER]그것으로 다녀와서 나에게 말해주지 않겠는가?![ENTER]정말 궁금하구만.... 허허" 
locale.event_flame_dungeon_open.say_4 = "이동서를 주세요~!"
locale.event_flame_dungeon_open.say_5 = "오호~!![ENTER]용기가 있는 멋진 사람이로구먼...[ENTER]덮고 척박하며, 마물이 넘쳐나는 도염화지를 이렇게 가겠다니[ENTER]어지간한 강심장이 아니면 불가능하지..[ENTER]어디보자...."
locale.event_flame_dungeon_open.say_6 = "자~!!![ENTER]여기 있네~!! 도염화지 유적지의 좌표가 기록된 이동서일세[ENTER]그리고 덤으로 통행권을 줄터니 잘 살펴보고 나중에 다녀온 이야기나 해주게나~![ENTER]그럼 몸조심 하게나~!!"
locale.event_flame_dungeon_open.say_7 = "어허~!!![ENTER]이사람이~!! 아무리 나라도 이동서를 만드는데는 한계가 있네...[ENTER]하루에 한번만 줄 수 있으니 내일 오게나..."
