## Controls

bf2mods mostly utilizes a second controller to interact with the game, as most buttons are already taken up on the first controller. Moonjump is a notable exception to this however, as it can be used by player 1.

Check out the list below to see what does what.

---

| Action                  | Control               | Description                                                                                                                                                 |
|-------------------------|-----------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Moonjump                | R+B, **P1**           | Hold to make the player infinitely fly in the air.                                                                                                          |
| Fall Damage Toggle      | A, **P2**             | Toggles taking fall damage. Incredibly useful so you don't fall to your death with the moonjump.<br/>**On by default.**                                     |
| Movement Speed Up       | Dpad Up, **P2**       | Multiplies the player's movement speed by 2.                                                                                                                |
| Movement Speed Down     | Dpad Down, **P2**     | Divides the player's movement speed by 2.                                                                                                                   |
| Swap BDAT Scramble Type | X, **P2**             | Toggles scrambling the results of text fetches in BDAT tables. Cycles between Disabled, Scramble, and Sheet Name.<br/>**Disabled by default.**              |
| Toggle UI Rendering     | Y, **P2**             | Toggles UI rendering of the engine. (Try this on loading screens!)                                                                                          |
| Toggle Debug Rendering  | B, **P2**             | Toggles various debug displays, both original to the game and custom.<br/>**On by default.**                                                                |
| Freecam Toggle          | LStick+RStick, **P2** | Enables the free camera. Left stick controls movement, right stick controls the view.                                                                       |
| Freecam - Speed Up      | ZL, **P2**            | Increases the speed of the freecam.                                                                                                                         |
| Freecam - Speed Down    | L, **P2**             | Decreases the speed of the freecam.                                                                                                                         |
| Freecam - Change FOV    | LStick, **P2**        | Changes the field of view of the freecam. Up on the left stick to tighten in, down to loosen up.<br/>Double click to set the FOV to 80.                     |
| Freecam - Change Roll   | RStick, **P2**        | Changes the roll (tilt) of the freecam. The right stick controls the amount of added tilt in each direction.<br/>Double click to completely reset rotation. |
| Events - Prev Debug     | R+Dpad Left, **P1**   | **Only applicable in events.** Selects the previous event manager.                                                                                          |
| Events - Next Debug     | R+Dpad Right, **P1**  | **Only applicable in events.** Selects the next event manager.                                                                                              |
| Events - Toggle Debug   | R+Dpad Up, **P1**     | **Only applicable in events.** Toggles debug rendering from the currently selected event manager.                                                           |
| Return To Title         | L+R+ZL+ZR, **P2**     | Returns the player to the title screen.                                                                                                                     |
| Temp Int Increment      | ZL+Dpad Right, **P2** | Increments the temp int.                                                                                                                                    |
| Temp Int Decrement      | ZL+Dpad Left, **P2**  | Decrements the temp int.                                                                                                                                    |
| Jump to MapJump         | ZL+Dpad Down, **P2**  | Jumps to the MapJump with the id of the temp int. Look below for a list.                                                                                    |
| Play Sound Effect       | ZL+Minus, **P2**      | Plays a common sound effect with the id of the temp int.                                                                                                    |
| Clear TCP Log           | Plus, **P2**          | Clears the TCP log. Any clients connected will have their terminals cleared of output.                                                                      |
| Test Logger             | Plus+R, **P2**        | A simple logging test for the logger console.                                                                                                               |
| Straighten Font         | L+Y, **P2**           | Straightens out the font used in 2 and Torna. Nice meme.                                                                                                    |

MapJump/Landmark ids can be found [here](https://xenoblade.github.io/xb2/bdat/common/SYS_MapJumpList.html) for 2/Torna and [here](https://xenoblade.github.io/xb1de/bdat/bdat_common/landmarklist.html) for DE.