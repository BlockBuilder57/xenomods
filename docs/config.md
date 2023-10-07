## Config

Xenomods has an optional TOML config file located on the SD card. The file is not packaged with releases, but can be created at `sd:/config/xenomods/config.toml`.

---

### Example

Below is an example, with defaults. Note that all are optional, excluding them will just have it fall back to its default.

```toml
# Port the TCP logger will use.
port = 6969
# Logging level of the console. (Debug = 0, Info, Warning, Error, Fatal = 4)
loggingLevel = 1
# Theme for the Menu. (Auto = 0, Titans, Alrest, Aionios, Dear ImGui Dark, Dear ImGui Light, Dear ImGui Classic = 6)
menuTheme = 0

# Dumps files to /config/xenomods/{game}/dump/
dumpFileReads = false
# Reads overrides from /config/xenomods/{game}/override/
enableFileOverrides = true
# Skips overriding these specific Bdat sheets (due to heavy performance penalties)
bdatSkipOverrides = ["CHR_Ir", "FLD_Condition", "FLD_Quest", "BTL_Enhance", "BTL_Skill_Dr", "CHR_EnArrange"]

# Multiplies final attack damage (2/Torna/DE only)
damagePlayerMult = 1.0
damageEnemyMult = 1.0

# Disables music changes during battles, eg Visions, low tension,  (2/Torna/DE only)
disableBattleBGMChanges = false

# Using a header can set configs for specific games
# Headers can also overwrite "globally" defined settings

[bf2]
# List of title screen overrides, by Bdat id
# This list contains all chapters, game clears, and NG+ clears
titleEvents = [10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609]
# A bitflag for event debugging defaults. The list is in EventDebugUtils.cpp
eventDebugBits = 0b1

# Tries to load "ira-xm.ard" from the filesystem and mounts if present
mountTornaContent = true

[ira]
# This list contains the start and post-credits titlescreen
titleEvents = [10709, 10710]

[bfsw]
# Load Future Connected by default if it was the last saved game
loadFcLatest = false
```

A list of event IDs can be found in [EVT_listBf](https://xenoblade.github.io/xb2/bdat/common/EVT_listBf.html).
