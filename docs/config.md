## Config

xenomods has an optional TOML config file located on the SD card. By default, the file does not exist, but can be created at `sd:/config/xenomods/config.toml`.

---

### Example

Below is an example, with defaults. Note that all are optional, excluding them will just have it fall back to its default.

```toml
# Port the TCP logger will use.
port = 6969

# List of title screen overrides, by Bdat id
# This list contains all chapters, game clears, and NG+ clears
titleEvents = [10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609]
# A bitflag for event debugging defaults. The list is in EventDebugUtils.cpp
eventDebugBits = 0b1

# Dumps files to /config/xenomods/{game}/dump/
dumpFileReads = false
# Reads overrides from /config/xenomods/{game}/override/
enableFileOverrides = true
# Skips overriding these specific Bdat sheets (due to heavy performance penalties)
bdatSkipOverrides = ["CHR_Ir", "FLD_Condition", "FLD_Quest", "BTL_Enhance", "BTL_Skill_Dr", "CHR_EnArrange"]

# Using a header can override configs for specific games

[bf2]
# Tries to load "ira-xm.ard" from the filesystem and mounts if present
mountTornaContent = true

[ira]
# This list contains the start and post-credits titlescreen
titleEvents = [10709, 10710]
```

A list of event IDs can be found in [EVT_listBf](https://xenoblade.github.io/xb2/bdat/common/EVT_listBf.html).