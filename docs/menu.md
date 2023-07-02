## Menu

Xenomods has a menu for controlling its different modules.

The Menu can be controlled by P1, but will swap to P2 if the controller is connected.

---

#### Basic controls:
| Action      | Control    | Description                                                              |
|-------------|------------|--------------------------------------------------------------------------|
| Toggle Menu | L+R+ZL+ZR  | Opens the Menu. Will take the place of logger messages.                  |
| Up          | Dpad Up    | Navigates to the previous choice.                                        |
| Down        | Dpad Down  | Navigates to the next choice.                                            |
| Select      | Dpad Right | Selects the currently hovered choice. Will automatically call functions. |
| Back        | Dpad Left  | Returns to the parent section/deselects the current option.              |

#### Option controls:

These are the controls for Options (numbers, booleans, enums).

| Action         | Control   | Description                                                                                                                                 |
|----------------|-----------|---------------------------------------------------------------------------------------------------------------------------------------------|
| Increment      | Dpad Up   | Increments the number or toggles the boolean.                                                                                               |
| Decrement      | Dpad Down | Decrements the number or toggles the boolean.                                                                                               |
| Multiply by 2  | B         | Hold while changing a number to instead multiply or divide the number by 2.                                                                 |
| Change by Tens | A         | Hold while changing a number to increment/decrement by the integer log of the number.<br>(12 would increment by 10, 123 would be 100, etc.) |
| Negate         | Y         | Press to negate the current number.                                                                                                         |
| Set to 0       | X         | Sets the current number to 0.                                                                                                               |


### Modules

Modules control a majority of the functionality of Xenomods. A number of options do not work in every game (especially in 3), so be sure to check to see if the option you are looking appears in the Menu.

#### Debug Stuff
| Option                  | Description                                                                    | Default |
|-------------------------|--------------------------------------------------------------------------------|---------|
| Enable debug rendering  | Toggles various debug displays, both original to the game and custom.          | true    |
| Access closed landmarks | Toggles the accessibility of blocked landmarks (ie, you can visit Indol again) | true    |
| Temp Int                | A temporary number for use with the below functions.                           | 0       |
| Jump to Landmark        | Jumps to the MapJump with the id of the temp int. Look below for a list.       | -       |
| Play Sound Effect       | Plays a common sound effect with the id of the temp int.                       | -       |
| Return To Title         | Returns the player to the title screen.                                        | -       |

Landmark ids can be found [here](https://xenoblade.github.io/xb2/bdat/common/SYS_MapJumpList.html) for 2/Torna and [here](https://xenoblade.github.io/xb1de/bdat/bdat_common/landmarklist.html) for DE.

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

###### Toggles...
| Option                          | Description                                                                                                                                                 | Default |
|---------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| Skip UI rendering               | Skips rendering the UI. (Try this on loading screens!)                                                                                                      | false   |
| Skip particle+overlay rendering | Skips rendering particle effects as well as any screen overlay effects, like visions.                                                                       | false   |
| Skip cloud (sea) rendering      | Skips rendering the cloud effect, used by skyboxes in 2 and the cloud sea.                                                                                  | false   |
| Skip sky dome rendering         | Skips rendering the "sky dome," the plain sky gradient in the background.                                                                                   | false   |
| Toggle map                      | Toggles rendering of the map geometry. Props are included, but any interactable objects or cutscene props remain.                                           | -       |
| Toggle fog                      | Toggles rendering of the fog. For some reason, this also includes the sky dome.                                                                             | -       |
| Toggle bloom                    | Toggles the bloom effect.                                                                                                                                   | -       |
| Toggle tonemapping              | Toggles tonemapping. This simulates what eyes do when entering dark areas. As a result, turning this off makes many scenes dark (sometimes to cool effect!) | -       |
| Toggle depth of field           | Toggles the depth of field/blur effect.                                                                                                                     | -       |
| Toggle motion blur              | Toggles motion blur. Not incredibly distracting in this engine, but is often seen in cutscenes.                                                             | -       |
| Toggle color filter             | Toggles color filters/color correction.                                                                                                                     | -       |

| Option          | Description                                                      | Default |
|-----------------|------------------------------------------------------------------|---------|
| Straighten font | Straightens out the font used in 2 and Torna. Nice meme.         | false   |
| Shadow strength | Sets the strength of the realtime shadows. Can go above 1 in DE. | 1.0     |

#### Battle Cheats
| Option                   | Description                                                     | Default |
|--------------------------|-----------------------------------------------------------------|---------|
| Player damage multiplier | Config option. Multiplies the damage done by player characters. | 1.0     |
| Enemy damage multiplier  | Config option. Multiplies the damage done by enemies.           | 1.0     |

#### Party Editor

###### Party Order... (DE)
| Option      | Description                                            | Default |
|-------------|--------------------------------------------------------|---------|
| PC 1-7      | Sets which player character occupies which party slot. | -       |
| Apply Party | Applies the party choices set above.                   | -       |
| Get Party   | Gets the current party and fills the list above.       | -       |

###### PC Status... (DE)
| Option         | Description                                                                                                                | Default |
|----------------|----------------------------------------------------------------------------------------------------------------------------|---------|
| Party Member   | Selects which party member to edit.                                                                                        | -       |
| Level          | Controls the level of the selected party member. Note that without actually leveling up, other stats will not be affected. | -       |
| Experience     | Controls the amount of experience had at the current level.                                                                | -       |
| Affinity Coins | Controls the maximum amount of affinity coins available to the character.                                                  | -       |
| Base HP        | Controls the base HP stat. (HP is calculated from this)                                                                    | -       |
| Base Strength  | Controls the base stat for strength.                                                                                       | -       |
| Base Agility   | Controls the base stat for agility.                                                                                        | -       |
| Base Ether     | Controls the base stat for ether.                                                                                          | -       |

###### Experience... (DE)
| Option         | Description                                                         | Default |
|----------------|---------------------------------------------------------------------|---------|
| Party Member   | Selects which party member to grant experience to.                  | -       |
| EXP            | Controls the amount of experience to be granted.                    | 0       |
| AP             | Controls the amount of Art Points to be granted.                    | 0       |
| SP             | Controls the amount of Skill Points to be granted.                  | 0       |
| Show UI alerts | Sets whether UI alerts (Level Up!, party exclaiming) should be done | false   |

###### Party... (2)
| Option             | Description                                      | Default |
|--------------------|--------------------------------------------------|---------|
| Index              | Selects which index of the party to edit.        | -       |
| Driver             | Sets which Driver is at the current index.       | -       |
| Active Blade Index | Controls which Blade is currently active.        | -       |
| Blade 0            | Sets which Blade is at this index.               | -       |
| Blade 1            | Sets which Blade is at this index.               | -       |
| Blade 2            | Sets which Blade is at this index.               | -       |
| Apply Party        | Applies the party choices set above.             | -       |
| Get Party          | Gets the current party and fills the list above. | -       |

###### Teams... (Torna)
| Option       | Description                                   | Default |
|--------------|-----------------------------------------------|---------|
| Index        | Selects the index of the team to edit.        | -       |
| Vanguard     | Sets the driver ID of the vanguard.           | -       |
| Rear Guard 1 | Sets the driver ID of the current rear guard. | -       |
| Rear Guard 2 | Sets the driver ID of the aux rear guard.     | -       |
| Apply Teams  | Applies the team choices set above.           | -       |
| Get Teams    | Gets the teams and fills the list above.      | -       |

#### Player Movement

###### Warps...
| Option               | Description                                                            | Default |
|----------------------|------------------------------------------------------------------------|---------|
| Load Warps from file | Loads warps from a file and selects the last open warp.                | -       |
| Save Warps to file   | Saves all current warps to a file, saving which warp was last open.    | -       |
| Current warp index   | Chooses which warp from the current set to use.                        | N/A     |
| New Warp             | Creates a new warp at the party lead's location.                       | -       |
| Overwrite Warp       | Overwrites the currently selected warp with the party lead's location. | -       |
| Go To Warp           | Teleports the party lead to the currently selected warp.               | -       |

| Option                    | Description                                                                                      | Default |
|---------------------------|--------------------------------------------------------------------------------------------------|---------|
| Disable fall damage       | Toggles taking fall damage. Incredibly useful so you don't fall to your death with the moonjump. | true    |
| Movement speed multiplier | Multiplies the player's movement speed by the given amount.                                      | 1.0     |

#### BDAT Randomizer
| Option        | Description                                                                                          | Default  |
|---------------|------------------------------------------------------------------------------------------------------|----------|
| Scramble type | Toggles scrambling the text in BDAT tables. Cycles between Disabled, Scramble Index, and Sheet Name. | Disabled |


### State

| Option                       | Description                                                  | Default |
|------------------------------|--------------------------------------------------------------|---------|
| Draw menu background         | Toggles drawing of the Menu's backround.                     | true    |
| Reload config/BDAT overrides | Reloads the config file and BDAT overrides from the SD card. | -       |
