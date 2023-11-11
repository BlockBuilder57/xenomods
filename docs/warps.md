## Config

Warps are saved locations specific to specific maps. This can be used to warp to a specific location multiple times, useful for practicing speedrunning techniques.

Warps must be manually loaded in game. When creating new warps, they will named according to the map and the total number of warps. However, once warps are saved, this file can be edited, and the name for that warp can be changed.

The game-specific warp file can be created/found at `sd:/config/xenomods/{game}/warps.toml`.

---

### Examples

```toml
[[warps]]
mapId = 0                      # The specific map the warp is for. 0 allows a warp to be used on any map
mapNameReadOnly = 'No Map'     # Changing this value has no effect
name = 'Map Origin'            # The name of the warp
position = [ 0.0, 0.0, 0.0 ]   # Where the player will be position
rotation = [ 0.0, 0.0, 180.0 ] # The rotation of the player at that point
velocity = [ 0.0, 0.0, 0.0 ]   # The velocity of the player there

[[warps]]
mapId = 2
mapNameReadOnly = 'Colony 9'
name = 'Really Awesome Clip'
position = [ 75.30000305175781, 14.100000381469727, 50.79999923706055 ]
rotation = [ -180.0, -89.98021697998047, 0.0 ]
velocity = [ 80.0, 0.0, 0.0 ]
```
