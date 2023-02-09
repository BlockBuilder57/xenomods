## Config

bf2mods has an optional TOML config file located on the SD card. By default, the file does not exist, but can be created at `sd:/config/bf2mods/config.toml`.

---

### Example

Below is an example, with defaults. Note that all are optional, excluding them will just have it fall back to its default.

```toml
# Port the TCP logger will use.
port = 6969

# List of title screen overrides, by BDAT id
# This list contains all chapters, game clears, and NG+ clears
titleEvents = [10001, 10504, 10505, 10506, 10507, 10508, 10509, 10510, 10511, 10512, 10513, 10605, 10607, 10608, 10609]
# A bitflag for event debugging defaults. The list is in EventDebugUtils.cpp
eventDebugBits = 0b1

# Dumps files to /config/bf2mods/{game}/dump/
dumpFileReads = false
# Reads overrides from /config/bf2mods/{game}/override/
enableFileOverrides = true

# Using a header can override configs for specific games
[ira]
# This list contains the start and post-credits titlescreen
titleEvents = [10709, 10710]
```

A list of event IDs can be found in [EVT_listBf](https://xenoblade.github.io/xb2/bdat/common/EVT_listBf.html).