## Config

With optional TOML config files, Bdat values can be overridden with ease.

The game-specific override file can be created/found at `sd:/config/bf2mods/{game}/bdatOverride.toml`.

---

### Example

```toml
[menu_ms.1610] # replaces loading text
name = "Loading (modded)"

[CHR_Dr.1] # puts Nia's model on Rex
Model = "pc/pc000201"
Model2 = "pc/pc000201"
Motion = "pc/pc000201"
Com_Eff = "pc/pc000201"
```