## Controls

Xenomods mostly utilizes a menu to interact with the game, as most buttons are already taken up on the controller. A few specific things like moonjump and event debugging are notable exceptions to this.

---

#### P1 controls:
| Action                | Control      | Description                                                                                                  |
|-----------------------|--------------|--------------------------------------------------------------------------------------------------------------|
| Moonjump              | R+B          | Hold to make the player infinitely fly in the air.                                                           |
| Events - Toggle Debug | R+Dpad Up    | Toggles debug rendering from the currently selected event manager.<br>**Only applicable in 2/Torna events.** |
| Events - Prev Debug   | R+Dpad Left  | Selects the previous event manager.<br>**Only applicable in 2/Torna events.**                                |
| Events - Next Debug   | R+Dpad Right | Selects the next event manager.<br>**Only applicable in 2/Torna events.**                                    |

#### Camera controls:

Camera controls only exist on P2, so both analog sticks are always free.

| Action                    | Control               | Description                                                                                                                                    |
|---------------------------|-----------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| Freecam Toggle            | LStick+RStick, **P2** | Enables the free camera. Left stick controls movement, right stick controls the view.                                                          |
| -                         | -                     | **!! ALL BELOW ONLY FUNCTION WHILE THE FREECAM IS ACTIVE !!**                                                                                  |
| Freecam - Speed Up        | L+R+Plus, **P2**      | Increases the speed of the freecam.                                                                                                            |
| Freecam - Speed Down      | L+R+Minus, **P2**     | Decreases the speed of the freecam.                                                                                                            |
| Freecam - Change FOV      | L+R+LStick, **P2**    | Changes the field of view of the freecam. Up on the stick to tighten in, down to loosen up.<br/>Click to reset to 80.                          |
| Freecam - Change Roll     | L+R+RStick, **P2**    | Changes the roll (tilt) of the freecam. The stick controls the amount of added tilt in each direction.<br/>Click to completely reset rotation. |
| Freecam - Teleport Player | L+R+B, **P2**         | Teleports the player to the freecam's position.                                                                                                |

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

#### File Detours
| Option                | Description                                                                 | Default |
|-----------------------|-----------------------------------------------------------------------------|---------|
| Dump file reads       | Config option. Dumps files that use compressed blocks to the SD card.       | false   |
| Enable file overrides | Config option. Overrides compressed block reads using files on the SD card. | true    |

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
| Option                | Description                                                                                                                                                 | Default |
|-----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|---------|
| Toggle map            | Toggles rendering of the map geometry. Props are included, but any interactable objects or cutscene props remain.                                           | -       |
| Toggle fog            | Toggles rendering of the fog. For some reason, this also includes the sky dome.                                                                             | -       |
| Toggle bloom          | Toggles the bloom effect.                                                                                                                                   | -       |
| Toggle tonemapping    | Toggles tonemapping. This simulates what eyes do when entering dark areas. As a result, turning this off makes many scenes dark (sometimes to cool effect!) | -       |
| Toggle depth of field | Toggles the depth of field/blur effect.                                                                                                                     | -       |
| Toggle motion blur    | Toggles motion blur. Not incredibly distracting in this engine, but is often seen in cutscenes.                                                             | -       |
| Toggle color filter   | Toggles color filters/color correction.                                                                                                                     | -       |

| Option                          | Description                                                                           | Default |
|---------------------------------|---------------------------------------------------------------------------------------|---------|
| Straighten font                 | Straightens out the font used in 2 and Torna. Nice meme.                              | false   |
| Skip UI rendering               | Skips rendering the UI. (Try this on loading screens!)                                | false   |
| Skip particle+overlay rendering | Skips rendering particle effects as well as any screen overlay effects, like visions. | false   |
| Skip cloud (sea) rendering      | Skips rendering the cloud effect, used by skyboxes in 2 and the cloud sea.            | false   |
| Skip sky dome rendering         | Skips rendering the "sky dome," the plain sky gradient in the background.             | false   |

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

#### Battle Cheats
| Option                   | Description                                                     | Default |
|--------------------------|-----------------------------------------------------------------|---------|
| Player damage multiplier | Config option. Multiplies the damage done by player characters. | 1.0     |
| Enemy damage multiplier  | Config option. Multiplies the damage done by enemies.           | 1.0     |

#### Party Editor (DE)
| Option      | Description                                            | Default |
|-------------|--------------------------------------------------------|---------|
| PC 1-7      | Sets which player character occupies which party slot. | -       |
| Apply Party | Applies the party choices set above.                   | -       |
| Get Party   | Gets the current party and fills the list above.       | -       |


### State

| Option                       | Description                                                  | Default |
|------------------------------|--------------------------------------------------------------|---------|
| Draw menu background         | Toggles drawing of the Menu's backround.                     | true    |
| Reload config/BDAT overrides | Reloads the config file and BDAT overrides from the SD card. | -       |
