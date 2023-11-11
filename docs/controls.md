## Controls

Xenomods mostly utilizes a menu to interact with the game, as most buttons are already taken up on the controller. A few specific things like moonjump and event debugging are notable exceptions to this.

---

#### P1 controls:
| Action   | Control | Description                                        |
|----------|---------|----------------------------------------------------|
| Moonjump | R+B     | Hold to make the player infinitely fly in the air. |

#### Camera controls:
The camera can be controlled by P1 while the Menu is open, but ideally should be used by P2 so both analog sticks are always free.

| Action                    | Control       | Description                                                                                                                                    |
|---------------------------|---------------|------------------------------------------------------------------------------------------------------------------------------------------------|
| Freecam Toggle            | LStick+RStick | Enables the free camera. Left stick controls movement, right stick controls the view.                                                          |
| -                         | -             | **!! ALL BELOW ONLY FUNCTION WHILE THE FREECAM IS ACTIVE !!**                                                                                  |
| Freecam - Speed Up        | L+R+Plus      | Increases the speed of the freecam.                                                                                                            |
| Freecam - Speed Down      | L+R+Minus     | Decreases the speed of the freecam.                                                                                                            |
| Freecam - Change FOV      | L+R+LStick    | Changes the field of view of the freecam. Up on the stick to tighten in, down to loosen up.<br/>Click to reset to 80.                          |
| Freecam - Change Roll     | L+R+RStick    | Changes the roll (tilt) of the freecam. The stick controls the amount of added tilt in each direction.<br/>Click to completely reset rotation. |
| Freecam - Teleport Player | L+R+B         | Teleports the player to the freecam's position.                                                                                                |

#### Menu controls:
| Action        | Control   | Description                                                                                       |
|---------------|-----------|---------------------------------------------------------------------------------------------------|
| Open Menu     | L+R+ZL+ZR | Opens the Menu.                                                                                   |
| Select Window | Y         | Selects which window to highlight with the controller. `(Main menu bar)` will select the top bar. |

Once opened, the Menu can be interacted with through the touch screen, or through a connected controller. The remainder of the controls can be seen in the Dear ImGui controls for the Switch:

![Controls shown on JoyCons](https://www.dearimgui.com/controls_sheets/imgui%20controls%20v6%20-%20Switch.png)

Refer to [menu.md](menu.md) for extra information.
