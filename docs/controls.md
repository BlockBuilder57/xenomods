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

| Action                            | Control                | Description                                                                                                                                                 |
|-----------------------------------|------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Freecam Toggle                    | LStick+RStick, **P2**  | Enables the free camera. Left stick controls movement, right stick controls the view.                                                                       |
| -                                 | -                      | **!! ALL BELOW ONLY FUNCTION WHILE THE FREECAM IS ACTIVE !!**                                                                                               |
| Freecam - Speed Up                | L+R+Plus, **P2**       | Increases the speed of the freecam.                                                                                                                         |
| Freecam - Speed Down              | L+R+Minus, **P2**      | Decreases the speed of the freecam.                                                                                                                         |
| Freecam - Change FOV              | L+R+LStick, **P2**     | Changes the field of view of the freecam. Up on the stick to tighten in, down to loosen up.<br/>Click to reset to 80.                                       |
| Freecam - Change Roll             | L+R+RStick, **P2**     | Changes the roll (tilt) of the freecam. The stick controls the amount of added tilt in each direction.<br/>Click to completely reset rotation.              |
| Freecam - Teleport Player         | L+R+B, **P2**          | Teleports the player to the freecam's position.                                                                                                             |
| Rendering - Toggle Map            | L+R+Dpad Up, **P2**    | Toggles rendering of the map geometry. Props are included, but certain ones (like interactable objects) remain.                                             |
| Rendering - Toggle Fog (+ Sky)    | L+R+Dpad Left, **P2**  | Toggles rendering of the fog effect. By extension, this also includes the nice blue sky.                                                                    |
| Rendering - Toggle Bloom          | L+R+Dpad Right, **P2** | Toggles the bloom effect.                                                                                                                                   |
| Rendering - Toggle Tonemap        | L+R+Dpad Down, **P2**  | Toggles tonemapping. This simulates what eyes do when entering dark areas. As a result, turning this off makes many scenes dark (sometimes to good effect!) |
| Rendering - Toggle Depth of Field | L+R+Y, **P2**          | Toggles the depth of field/blur effect.                                                                                                                     |
| Rendering - Toggle Motion Blur    | L+R+X, **P2**          | Toggles motion blur. Not too common in this engine, but distinct enough.                                                                                    |
| Rendering - Toggle Color Filter   | L+R+A, **P2**          | Disables color filters/color correction.                                                                                                                    |

## Menu

Xenomods has a menu for controlling its different modules.

The menu can be opened by P1, but will always be P2 if the controller is connected.

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

These are the controls for Options (numbers, booleans, functions).

| Action         | Control   | Description                                                                                                                                 |
|----------------|-----------|---------------------------------------------------------------------------------------------------------------------------------------------|
| Increment      | Dpad Up   | Increments the number or toggles the boolean.                                                                                               |
| Decrement      | Dpad Down | Decrements the number or toggles the boolean.                                                                                               |
| Multiply by 2  | B         | Hold while changing a number to instead multiply or divide the number by 2.                                                                 |
| Change by Tens | A         | Hold while changing a number to increment/decrement by the integer log of the number.<br>(12 would increment by 10, 123 would be 100, etc.) |
| Negate         | Y         | Press to negate the current number.                                                                                                         |
| Set to 0       | X         | Sets the current number to 0.                                                                                                               |


### Modules

#### Debug Stuff
| Option                  | Description                                                                                                             |
|-------------------------|-------------------------------------------------------------------------------------------------------------------------|
| Enable debug rendering  | Toggles various debug displays, both original to the game and custom.<br/>**On by default.**                            |
| Access closed landmarks | Toggles the accessibility of blocked landmarks (ie, you can visit Indol again)<br/>**Only for 2/Torna. On by default.** |
| Temp Int                | A temporary number for use with the below functions.                                                                    |
| Jump to Landmark        | Jumps to the MapJump with the id of the temp int. Look below for a list.                                                |
| Play Sound Effect       | Plays a common sound effect with the id of the temp int.                                                                |
| Return To Title         | Returns the player to the title screen.                                                                                 |

Landmark ids can be found [here](https://xenoblade.github.io/xb2/bdat/common/SYS_MapJumpList.html) for 2/Torna and [here](https://xenoblade.github.io/xb1de/bdat/bdat_common/landmarklist.html) for DE.

#### File Detours
| Option                | Description                                                                 |
|-----------------------|-----------------------------------------------------------------------------|
| Dump file reads       | Config option. Dumps files that use compressed blocks to the SD card.       |
| Enable file overrides | Config option. Overrides compressed block reads using files on the SD card. |

#### Camera Tools
| Option                        | Description                                                                                            |
|-------------------------------|--------------------------------------------------------------------------------------------------------|
| Freecam                       | Toggles the freecam being on or not.                                                                   |
| Freecam speed (m/s)           | Controls the movement speed of the freecam.                                                            |
| Camera pos X/Y/Z              | Displays the position of the camera. Modifying the value will enable the freecam and set its position. |
| Camera rot X/Y/Z              | Displays the rotation of the camera. Modifying the value will enable the freecam and set its rotation. |
| Camera FOV                    | Displays the field of view of the camera. Modifying the value will enable the freecam and set its FOV. |
| Teleport party lead to camera | Sets the position of the party lead to the position of the camera.<br>**2/Torna/DE only.**             |

#### Menu Viewer
| Option              | Description                                                        |
|---------------------|--------------------------------------------------------------------|
| Enable UI rendering | Toggles UI rendering of the engine. (Try this on loading screens!) |
| Straighten font     | Straightens out the font used in 2 and Torna. Nice meme.           |

#### Player Movement
| Option                    | Description                                                                                                             |
|---------------------------|-------------------------------------------------------------------------------------------------------------------------|
| Disable fall damage       | Toggles taking fall damage. Incredibly useful so you don't fall to your death with the moonjump.<br/>**On by default.** |
| Movement speed multiplier | Multiplies the player's movement speed by the given amount.                                                             |
| Save Warp                 | Saves the location of the party lead.                                                                                   |
| Load Warp                 | Loads the saved location of the party lead.                                                                             |
| Warp pos X/Y/Z            | Controls the position of the saved warp.                                                                                |

#### BDAT Randomizer
| Option        | Description                                                                                          |
|---------------|------------------------------------------------------------------------------------------------------|
| Scramble type | Toggles scrambling the text in BDAT tables. Cycles between Disabled, Scramble Index, and Sheet Name. |

#### Battle Cheats
| Option                   | Description                                                     |
|--------------------------|-----------------------------------------------------------------|
| Player damage multiplier | Config option. Multiplies the damage done by player characters. |
| Enemy damage multiplier  | Config option. Multiplies the damage done by enemies.           |


### State

| Option                       | Description                                                  |
|------------------------------|--------------------------------------------------------------|
| Draw menu background         | Toggles drawing of the Menu's backround.                     |
| Reload config/BDAT overrides | Reloads the config file and BDAT overrides from the SD card. |
