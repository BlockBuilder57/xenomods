## bf2mods

Some mod code by [modeco80](https://github.com/modeco80) and [BlockBuilder57](https://github.com/BlockBuilder57) (with special guest [3096](https://github.com/3096)) for Xenoblade 2 / Torna.

Includes a custom fork of a [custom fork](https://github.com/3096/skyline/tree/xde) of [Skyline](https://github.com/skyline-dev/skyline/), made by much smarter people than us.

## Build Instructions

- Install devKitPro with Switch development features.
- Clone this repository with --recursive.
- (Temporary until CMake port works) Move the files in `skyline/attic-build` out into `skyline` folder.
- Run `make`.
- ...
- Profit! (patch-bf2 directory in skyline/ will contain things needed to be installed)

## Installation instructions (LayeredFS)

In /atmosphere/contents/<BF2_TITLE_ID>/ make a exefs directory. In there, place the 2.0.0 ExeFS.

Once you've done that, simply place all the files from the skyline/patch-bf2 directory into the exefs directory (overwriting any conflicts).. and tada!

## Credits

All the Skyline peeps (sorry we couldn't do anything nicer, but at least document your plugin code lol)

3096 for the initial ether repository (and for helping as well)