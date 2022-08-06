## bf2mods

Some mod code by [modeco80](https://github.com/modeco80) and [BlockBuilder57](https://github.com/BlockBuilder57) for all Xenoblade games on Switch. Based off [Ether Flow](https://github.com/3096/ether) by 3096/dualie.

As such, this includes a custom fork of a [custom fork](https://github.com/3096/skyline/tree/xde) of [Skyline](https://github.com/skyline-dev/skyline/), made by many people much smarter people than us.


## Installation instructions (LayeredFS)

In /atmosphere/contents/<TITLE_ID>/ make a exefs directory. In there, place the latest `main` exefs version with symbols.

Latest compatible exefs versions:

| Game                         | Title ID                                       | Last Working Version   | Codename |
|------------------------------|:-----------------------------------------------|------------------------|:---------|
| Xenoblade 2                  | 0100E95004038000<br/>(0100F3400332C000 for JP) | 2.0.0 (or Torna 1.0.0) | bf2      |
| Xenoblade 2: Torna           | 0100C9F009F7A000                               | 1.0.0 (or XB2 2.0.0)   | ira      |
| Xenoblade Definitive Edition | 0100FF500E34A000                               | All versions           | bfsw     |

Once you've done that, simply copy the main.npdm and subsdk1 files to the exefs directory.

## Usage

Documentation for using the mod can be found in the [docs](docs) directory.

## Build Instructions

- Install devKitPro with Switch development features.
- Clone this repository with `--recursive`.
- Run the following commands:

```bash
$ cmake -Bbuild -DCMAKE_BUILD_TYPE=Release --toolchain cmake/NintendoSwitch.cmake 
$ cd build
$ make -j4
# ...
# profit?
# to build for other games, for example DE, append -DBF2MODS_CODENAME=bfsw to the cmake command
```

Once built, copy bf2mods.nso (as `subsdk1`) and main.npdm from build/src to the LayeredFS exefs directory.

For those with sys-ftpd installed, adding `-DBF2MODS_SWITCH_IP=<ip>` to the CMake command will automatically copy the files to your console when builds finish.

## Credits

All the contributors and maintainers of Skyline

3096 for the initial ether repository, and for graciously allowing their event freecam to be published. And for helping out with some other code as well!