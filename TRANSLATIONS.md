# Translations

All texts a player sees live in the client, one folder per language:

```
Lead-Client/pack/locale/<code>/     texts of one language (de, en, ro, ae, ...)
Lead-Client/pack/locale/common/     data and pictures shared by all languages
```

Every file is UTF-8, one entry per line, columns separated by a TAB.

| File | Content | Line format |
|---|---|---|
| `locale_game.txt` | texts used by the client scripts | `KEY<TAB>text` |
| `locale_interface.txt` | texts used by window layouts | `KEY<TAB>text` |
| `locale_string.txt` | messages sent by the server | `number<TAB>text` |
| `locale_quest.txt` | quest texts, NPC talk, quiz questions | `number<TAB>text` |
| `item_names.txt`, `mob_names.txt` | item and monster names | `vnum<TAB>name` |
| `itemdesc.txt`, `skilldesc.txt` | item and skill descriptions | as in the existing lines |

After any change, repack the pack and restart the client:

```
cd Lead-Client/pack
..\..\Lead-Tools\PackMakerLite\PackMakerLite.exe --nolog --parallel -p locale
```

## Change a text

Edit the line in the language folder and repack. Keep placeholders such as `%s` and `%d` and markers such as `[ENTER]` as they are, in the same order.

## Add an interface text

1. Add `MY_KEY<TAB>text` to `locale_game.txt` (used as `localeInfo.MY_KEY` in scripts) or to `locale_interface.txt` (used as `uiScriptLocale.MY_KEY` in layouts), in every language folder.
2. Use the key in the script.

A key that is missing in a language shows its own name and is written to `syserr.txt`; the client keeps running.

## Add a server message

1. Pick a free number from 5000 upwards.
2. Add `number<TAB>text` to `locale_string.txt` in every language folder.
3. Send it from the server as a tag: `ch->ChatPacket(CHAT_TYPE_INFO, "[LS;5400;%s;%d]", name, count);`. The values after the number fill the placeholders of the text in order.
4. For item, monster and skill names pass `LC_ITEM(vnum)`, `LC_MOB(vnum)` or `LC_SKILL(vnum)` instead of a name, so the client shows them in its own language.

## Add a quest text

1. Add `number<TAB>text` to `locale_quest.txt` in every language folder.
2. Give the number a name in `Lead-Serverfiles/share/common/translate.lua`: `gameforge.my_quest.my_key = "[LC;number]"`, or `"[LC;number;%s]"` when the text has placeholders. The file holds no text, only these names, so quests stay readable.
3. Use the name in the quest: `say(gameforge.my_quest.my_key)`.

## Add a language

1. Copy `Lead-Client/pack/locale/en` to `Lead-Client/pack/locale/<code>` and translate the files.
2. Add the language to `LANGUAGE_NAME_LIST` in `Lead-Client/pack/root/localeinfo.py`: `("xx", "Name")`, or `("xx", "Name", True)` for a right-to-left language.
3. Add a flag picture `Lead-Client/pack/locale/common/ui/flag/<code>.tga` (18x12 pixels).
4. Optional: add the language to `DetectSystemLanguage` in `Lead-Client-Source/UserInterface/Locale.cpp`, so new installs pick it from the Windows language.
5. Repack `locale` and `root`.

The language is chosen in the system options or on the login screen, changes without a restart, and is stored as `LANGUAGE <code>` in `metin2.cfg`.
