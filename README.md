## Xenomods [![xenomods CI](https://github.com/BlockBuilder57/xenomods/actions/workflows/ci.yml/badge.svg)](https://github.com/BlockBuilder57/xenmods/actions/workflows/ci.yml)

Some mod code by [modeco80](https://github.com/modeco80) and [BlockBuilder57](https://github.com/BlockBuilder57) for the Xenoblade games on Switch.

Initially based off [Ether Flow](https://github.com/3096/ether) by 3096/dualie. As such, this includes a custom fork of [Skyline](https://github.com/skyline-dev/skyline/), which has been modified to have an architecture similar to [exlaunch](https://github.com/shadowninja108/exlaunch).


## Installation instructions (LayeredFS)

In /atmosphere/contents/<TITLE_ID>/ make a exefs directory. In there, place the latest `main` exefs version with symbols.

Latest compatible exefs versions:

| Game                         | Title ID                                       | Last Working Version   | Codename |
|------------------------------|:-----------------------------------------------|------------------------|:---------|
| Xenoblade 2                  | 0100E95004038000<br/>(0100F3400332C000 for JP) | 2.0.0 (or Torna 1.0.0) | bf2      |
| Xenoblade 2: Torna           | 0100C9F009F7A000                               | 1.0 .0 (or XB2 2.0.0)  | ira      |
| Xenoblade Definitive Edition | 0100FF500E34A000                               | All versions           | bfsw     |
| Xenoblade 3                  | 010074F013262000                               | **None**               | bf3      |

Once you've done that, simply copy the contents of the zip to the root of your SD card. (The `atmosphere` folders should merge.)

## Usage

Documentation for using the mod can be found in the [docs](docs) directory.

## Build Instructions

- Install devkitPro with Switch development features.
- Clone this repository with `--recursive`.
- Run the following commands:

```bash
$ cmake -Bbuild --preset release-[codename]
$ cmake --build build
# ...
# profit?
```

Once built, copy xenomods.nso (renamed to `subsdk1`) and main.npdm from build/ to the LayeredFS exefs directory.

For those with sys-ftpd installed, adding `-DXENOMODS_SWITCH_IP=<ip>` to the first CMake command will automatically copy the files to your console when builds finish.

## Credits

All the contributors and maintainers of Skyline

3096 for the original repository, and for graciously allowing their event freecam to be published. And for their dbgutil library as well!