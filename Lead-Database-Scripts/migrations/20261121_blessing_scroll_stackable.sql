-- Blessing Scroll (25040) is a consumable and must stack like the other refine/enchant items
UPDATE player.item_proto SET flag = flag | 4 WHERE vnum = 25040;
