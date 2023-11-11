## Config

With optional TOML config files, Bdat values can be overridden with ease.

The game-specific override file can be created/found at `sd:/config/xenomods/{game}/bdatOverride.toml`. This currently does not work for Xenoblade 3.

---

### Examples

```toml
# Simple single-row replacements

[menu_ms.1610] # replaces loading text
name = "Loading (modded)"

[CHR_Dr.1] # puts Nia's model on Rex
Model = "pc/pc000201"
Model2 = "pc/pc000201"
Motion = "pc/pc000201"
Com_Eff = "pc/pc000201"
```

```toml
# Multiple row replacements
# (TOML does not always deal with subsequent table headers properly,
# so this is recommended if changing multiple rows in a single sheet)

[CHR_Bl]
# Create a new blade
# "_fb" defines a fallback id, in this case it's Pneuma
# All columns not defined here will be taken from the fallback
1114 = { _fb = 1003, Name = 12, DebugName = "mod", OrbNum = 3 }
# Make Mythra better
1002 = { Model = "pc/pc710601" }


# Adds the Chapter 1 titlescreen to the Event Theater
[menu_ev_theater_ms]
594 = { style = 163, name = "Chapter 1 Titlescreen" }

[MNU_Stream_thumb_ev]
416 = { filename = "mnu001_titlebg" }

[MNU_EventTheater]
373 = { _fb = 309, title = 594, event_id = 10504, thumbnail = 416, category = 1, chapter = 1, flag_index = 1 }
```