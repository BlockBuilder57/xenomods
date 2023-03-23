## Controls

Xenomods mostly utilizes a second controller to interact with the game, as most buttons are already taken up on the first controller. Moonjump and event debugging are notable exceptions to this however, they are used by player 1.

Check out the list below to see what does what.

---

### Gameplay:
| Action              | Control               | Description                                                                                                             |
|---------------------|-----------------------|-------------------------------------------------------------------------------------------------------------------------|
| Moonjump            | R+B, **P1**           | Hold to make the player infinitely fly in the air.                                                                      |
| Movement Speed Up   | Dpad Up, **P2**       | Multiplies the player's movement speed by 2.                                                                            |
| Movement Speed Down | Dpad Down, **P2**     | Divides the player's movement speed by 2.                                                                               |
| Fall Damage Toggle  | A, **P2**             | Toggles taking fall damage. Incredibly useful so you don't fall to your death with the moonjump.<br/>**On by default.** |
| Return To Title     | L+R+ZL+ZR, **P2**     | Returns the player to the title screen.                                                                                 |
| Temp Int Increment  | ZL+Dpad Right, **P2** | Increments the temp int.                                                                                                |
| Temp Int Decrement  | ZL+Dpad Left, **P2**  | Decrements the temp int.                                                                                                |
| Jump to MapJump     | ZL+Dpad Down, **P2**  | Jumps to the MapJump with the id of the temp int. Look below for a list.                                                |
| Play Sound Effect   | ZL+Minus, **P2**      | Plays a common sound effect with the id of the temp int.                                                                |
| Straighten Font     | L+Y, **P2**           | Straightens out the font used in 2 and Torna. Nice meme.                                                                |

MapJump/Landmark ids can be found [here](https://xenoblade.github.io/xb2/bdat/common/SYS_MapJumpList.html) for 2/Torna and [here](https://xenoblade.github.io/xb1de/bdat/bdat_common/landmarklist.html) for DE.

### Debug:
| Action                  | Control              | Description                                                                                                                                    |
|-------------------------|----------------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| Toggle Debug Rendering  | B, **P2**            | Toggles various debug displays, both original to the game and custom.<br/>**On by default.**                                                   |
| Toggle UI Rendering     | Y, **P2**            | Toggles UI rendering of the engine. (Try this on loading screens!)                                                                             |
| Events - Prev Debug     | R+Dpad Left, **P1**  | **Only applicable in events.** Selects the previous event manager.                                                                             |
| Events - Next Debug     | R+Dpad Right, **P1** | **Only applicable in events.** Selects the next event manager.                                                                                 |
| Events - Toggle Debug   | R+Dpad Up, **P1**    | **Only applicable in events.** Toggles debug rendering from the currently selected event manager.                                              |
| Swap BDAT Scramble Type | X, **P2**            | Toggles scrambling the results of text fetches in BDAT tables. Cycles between Disabled, Scramble, and Sheet Name.<br/>**Disabled by default.** |

### Camera:
| Action                            | Control                | Description                                                                                                                                                 |
|-----------------------------------|------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Freecam Toggle                    | LStick+RStick, **P2**  | Enables the free camera. Left stick controls movement, right stick controls the view.                                                                       |
| Freecam - Speed Up                | L+R+Plus, **P2**       | Increases the speed of the freecam.                                                                                                                         |
| Freecam - Speed Down              | L+R+Minus, **P2**      | Decreases the speed of the freecam.                                                                                                                         |
| Freecam - Change FOV              | L+R+LStick, **P2**     | Changes the field of view of the freecam. Up on the stick to tighten in, down to loosen up.<br/>Double click to reset the FOV.                              |
| Freecam - Change Roll             | L+R+RStick, **P2**     | Changes the roll (tilt) of the freecam. The stick controls the amount of added tilt in each direction.<br/>Double click to completely reset rotation.       |
| Freecam - Teleport Player         | L+R+B, **P2**          | Teleports the player to the freecam's position. **Only works for 2/Torna at the moment.**                                                                   |
| Rendering - Toggle Map            | L+R+Dpad Up, **P2**    | Toggles rendering of the map geometry. Props are included, but certain ones (like interactable objects) remain.                                             |
| Rendering - Toggle Fog (+ Sky)    | L+R+Dpad Left, **P2**  | Toggles rendering of the fog effect. By extension, this also includes the nice blue sky.                                                                    |
| Rendering - Toggle Bloom          | L+R+Dpad Right, **P2** | Toggles the bloom effect.                                                                                                                                   |
| Rendering - Toggle Tonemap        | L+R+Dpad Down, **P2**  | Toggles tonemapping. This simulates what eyes do when entering dark areas. As a result, turning this off makes many scenes dark (sometimes to good effect.) |
| Rendering - Toggle Depth of Field | L+R+Y, **P2**          | Toggles the depth of field (blur) effect.                                                                                                                   |
| Rendering - Toggle Motion Blur    | L+R+X, **P2**          | Toggles motion blur. Not too common in this engine, but distinct enough.                                                                                    |
| Rendering - Toggle Color Filter   | L+R+A, **P2**          | Disables color filters/color correction.                                                                                                                    |
