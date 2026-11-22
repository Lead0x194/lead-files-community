UPDATE player.item_proto SET name = locale_name WHERE locale_name <> '';
ALTER TABLE player.item_proto DROP COLUMN locale_name;

UPDATE IGNORE player.mob_proto SET name = CONVERT(locale_name USING utf8mb4) WHERE locale_name <> '';
ALTER TABLE player.mob_proto DROP COLUMN locale_name;

DELETE FROM common.locale WHERE mKey = 'DB_NAME_COLUMN';
