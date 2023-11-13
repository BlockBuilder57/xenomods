## Menu

Xenomods has a menu for controlling its different modules.

The Menu can be controlled by P1, but control will switch to P2 if the controller is connected. The Menu may have to be re-opened when this happens.

---

### Modules

Modules control a majority of the functionality of Xenomods. A number of options do not work in every game (especially in 3), so ensure the option you are looking for appears in-game.

#### Debug Stuff
| Option                   | Description                                                                     | Default |
|--------------------------|---------------------------------------------------------------------------------|---------|
| Enable debug rendering   | Toggles various debug displays, both original to the game and custom.           | true    |
| Debug unlock all         | Unlocks all landmarks and max level arts.                                       | -       |
| Access closed landmarks  | Toggles the accessibility of blocked landmarks (ie, you can visit Indol again). | false   |
| Temp Int                 | A temporary number for use with the below functions.                            | 0       |
| Jump to Landmark         | Jumps to the MapJump with the id of the temp int. Look below for a list.        | -       |
| Play common sound effect | Plays a common sound effect with the id of the temp int.                        | -       |
| Return to Title          | Returns the player to the title screen.                                         | -       |

Landmark ids can be found [here](https://xenobladedata.github.io/xb2/bdat/common/SYS_MapJumpList.html) for 2/Torna and [here](https://xenobladedata.github.io/xb1de/bdat/bdat_common/landmarklist.html) for DE.

#### Camera Tools
| Option                        | Description                                                                                            | Default |
|-------------------------------|--------------------------------------------------------------------------------------------------------|---------|
| Freecam                       | Toggles the freecam being on or not.                                                                   | false   |
| Freecam speed (m/s)           | Controls the movement speed of the freecam.                                                            | 8.0     |
| Camera pos X/Y/Z              | Displays the position of the camera. Modifying the value will enable the freecam and set its position. | -       |
| Camera rot X/Y/Z              | Displays the rotation of the camera. Modifying the value will enable the freecam and set its rotation. | -       |
| Camera FOV                    | Displays the field of view of the camera. Modifying the value will enable the freecam and set its FOV. | -       |
| Teleport party lead to camera | Sets the position of the party lead to the position of the camera.                                     | -       |

#### Rendering Controls
| Option                   | Description                                                                               | Default |
|--------------------------|-------------------------------------------------------------------------------------------|---------|
| Straighten font          | Straightens out the font used in 2 and Torna. Nice meme.                                  | false   |
| Shadow strength          | Sets the strength of the realtime shadows. Can go above 1 in DE.                          | 1.0     |
| Clear Texture Cache      | Clears the texture streaming cache, which gets all textures again.                        | -       |
| Freeze texture streaming | Freezes texture streaming. New models will be black until at least one texture is loaded. | false   |

###### Toggles...
| Option                          | Description                                                                                                                                                  | Default |
|---------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| Skip UI rendering               | Skips rendering the UI. (Try this on loading screens!)                                                                                                       | false   |
| Skip particle+overlay rendering | Skips rendering particle effects as well as any screen overlay effects, like visions.                                                                        | false   |
| Skip cloud (sea) rendering      | Skips rendering the cloud effect, used by skyboxes in 2 and the cloud sea.                                                                                   | false   |
| Skip sky dome rendering         | Skips rendering the "sky dome," the plain sky gradient in the background.                                                                                    | false   |
| Hide map                        | Hides rendering of the map geometry. Props are included, but any interactable objects or cutscene props remain.                                              | false   |
| Disable fog                     | Disables rendering of the fog. For some reason, this also includes the sky dome.                                                                             | false   |
| Disable bloom                   | Disables the bloom effect.                                                                                                                                   | false   |
| Disable tonemapping             | Disables tonemapping. This simulates what eyes do when entering dark areas. As a result, turning this off makes many scenes dark (sometimes to cool effect!) | false   |
| Force disable depth of field    | Forcibly disables the depth of field/blur effect.                                                                                                            | false   |
| Force disable motion blur       | Forcibly disables motion blur. Not incredibly distracting in this engine, but is often seen in cutscenes.                                                    | false   |
| Force disable color filter      | Forcibly disables color filters/color correction.                                                                                                            | false   |
| Enable AA                       | Enables anti-ailasing.                                                                                                                                       | false   |
| Enable TMAA                     | Enables temporal anti-ailasing.                                                                                                                              | false   |

###### GBuffer debug...
| Option           | Description                                                               | Default |
|------------------|---------------------------------------------------------------------------|---------|
| GBuffer debug    | Toggles enabling the rendering options below.                             | false   |
| Reset parameters | Resets base color, metalness, roughness, emission, and ambient occlusion. | -       |

#### Battle Cheats
| Option                               | Description                                                     | Default |
|--------------------------------------|-----------------------------------------------------------------|---------|
| Player damage multiplier             | Config option. Multiplies the damage done by player characters. | 1.0     |
| Enemy damage multiplier              | Config option. Multiplies the damage done by enemies.           | 1.0     |
| Disable vision BGM change            | Disables the battle music changing when a vision occurs.        | 1.0     |
| Disable low tension/death BGM change | Disables the battle music changing on low tension/PC death.     | 1.0     |
| Sections                             | Chooses the number of sections for the party gauge.             | 3.0     |
| Set Party Gauge                      | Sets the party gauge to the number of sections.                 | -       |

#### Party Editor

###### Party Order... (DE)
| Option     | Description                                            | Default |
|------------|--------------------------------------------------------|---------|
| Member 1-7 | Sets which player character occupies which party slot. | -       |
| Load       | Gets the current party and fills the member list.      | -       |
| Apply      | Applies the party choices in the member list.          | -       |

###### PC Status... (DE)
| Option         | Description                                                                                                                  | Default   |
|----------------|------------------------------------------------------------------------------------------------------------------------------|-----------|
| Member         | Selects which party member to edit.                                                                                          | -         |
| HP             | Controls the current HP of the selected member. Unless done in battle, this will still be affected by health regen.          | -         |
| Level          | Controls the level of the selected party member. Note that without actually leveling up, other stats will not be affected.   | -         |
| Total EXP      | Controls the amount of experience for the current level.                                                                     | -         |
| Bonus EXP      | Controls the amount of bonus experience, for leveling up in Expert Mode.                                                     | -         |
| Affinity Coins | Controls the maximum amount of affinity coins available to the character.                                                    | -         |
| **Base Stats** | ---------------------------------------------------------------------------------------------------------------------------- | --------- |
| Base HP        | Controls the base HP stat. (HP is calculated from this)                                                                      | -         |
| Base Strength  | Controls the base stat for strength.                                                                                         | -         |
| Base Agility   | Controls the base stat for agility.                                                                                          | -         |
| Base Ether     | Controls the base stat for ether.                                                                                            | -         |

###### Experience... (DE)
| Option       | Description                                                         | Default |
|--------------|---------------------------------------------------------------------|---------|
| Member       | Selects which party member to grant experience to.                  | -       |
| Clear Values | Clears the values below.                                            | -       |
| AP           | Controls the amount of Art Points to be granted.                    | 0       |
| SP           | Controls the amount of Skill Points to be granted.                  | 0       |
| EXP          | Controls the amount of experience to be granted.                    | 0       |
| Show on UI   | Sets whether UI alerts (Level Up!, party exclaiming) should be done | false   |
| Add          | Adds the given amounts of EXP, AP, and SP.                          | -       |

###### Party... (2)
| Option                     | Description                                      | Default |
|----------------------------|--------------------------------------------------|---------|
| Apply Party                | Applies the party choices set above.             | -       |
| Get Party                  | Gets the current party and fills the list above. | -       |
| Index                      | Selects which index of the party to edit.        | -       |
| Driver                     | Sets which Driver is at the current index.       | -       |
| Active Blade Index         | Controls which Blade is currently active.        | -       |
| Blade 0-2 (or 0-5 for Rex) | Sets which Blade is at this index.               | -       |

###### Teams... (Torna)
| Option       | Description                                     | Default |
|--------------|-------------------------------------------------|---------|
| Apply Teams  | Applies the team choices set above.             | -       |
| Get Teams    | Gets the teams and fills the list above.        | -       |
| Index        | Selects the index of the team to edit.          | -       |
| Vanguard     | Sets the driver ID of the vanguard.             | -       |
| Rear Guard 1 | Sets the driver ID of the current rear guard.   | -       |
| Rear Guard 2 | Sets the driver ID of the auxiliary rear guard. | -       |

#### Player Movement
| Option                    | Description                                                                                      | Default |
|---------------------------|--------------------------------------------------------------------------------------------------|---------|
| Show Warps Window         | Opens the Warps window. Warps can be loaded and saved, as well as tweaked from in-game.          | -       |
| Disable fall damage       | Toggles taking fall damage. Incredibly useful so you don't fall to your death with the moonjump. | false   |
| Movement speed multiplier | Multiplies the player's movement speed by the given amount.                                      | 1.0     |

###### Warps...
| Option               | Description                                                                         | Default |
|----------------------|-------------------------------------------------------------------------------------|---------|
| Load Warps From File | Loads all of the saved warp's in the game's warps.toml.                             | -       |
| Save Warps To File   | Saves all current warps to the game's warps.toml.                                   | -       |
| Show all warps       | Shows all warps in the file, including those not usable on the current map.         | false   |
| Show warps on map    | Displays warps that are valid for the current map as text on the field.             | false   |
| +                    | Creates a new warp at the party lead's location.                                    | -       |
| ---                  | ---                                                                                 | -       |
| Position             | Sets the position of the currently selected warp.                                   | -       |
| Rotation             | Sets the rotation of the currently selected warp.                                   | -       |
| Velocity             | Sets the velocity of the currently selected warp.                                   | -       |
| Go To Warp           | Teleports the party lead to the currently selected warp.                            | -       |
| Overwrite Warp       | Overwrites the currently selected warp with the party lead's location.              | -       |
| Delete Warp          | Deletes the currently selected warp. Can also be done with the X button on the tab. | -       |

#### BDAT Randomizer
| Option        | Description                                                                                          | Default  |
|---------------|------------------------------------------------------------------------------------------------------|----------|
| Scramble type | Toggles scrambling the text in BDAT tables. Cycles between Disabled, Scramble Index, and Sheet Name. | Disabled |

#### Event Debug
| Option             | Description                                                                 | Default |
|--------------------|-----------------------------------------------------------------------------|---------|
| event::*           | Enables a certain part of 2's event debug. In need of proper documentation. | false   |

#### Resource Manager
| Option | Description                             | Default |
|--------|-----------------------------------------|---------|
| Money  | Sets the amount of money/gold you have. | 0       |

###### Item Giver
| Option    | Description                           | Default |
|-----------|---------------------------------------|---------|
| ID        | Chooses the ID of the item to give.   | 0       |
| Count     | Chooses how many of the item to give. | 0       |
| Give Item | Gives {Count} amount of Item #{ID}    | -       |

### State
| Option                       | Description                                                    | Default |
|------------------------------|----------------------------------------------------------------|---------|
| Menu Theme                   | Chooses the theme of the Menu. Can also be set through config. | Auto    |
| Menu Font                    | Chooses the font that is used in the Menu.                     | -       |
| Reload config/BDAT overrides | Reloads the config file and BDAT overrides from the SD card.   | -       |
