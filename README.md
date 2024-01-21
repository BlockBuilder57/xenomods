## Xenomods [![xenomods CI](https://github.com/BlockBuilder57/xenomods/actions/workflows/ci.yml/badge.svg)](https://github.com/BlockBuilder57/xenomods/actions/workflows/ci.yml)

Some mod code by [modeco80](https://github.com/modeco80) and [BlockBuilder57](https://github.com/BlockBuilder57) for the Xenoblade games on Switch.

Initially based off [Ether Flow](https://github.com/3096/ether) by Frank ([3096](https://github.com/3096)). As such, this includes a custom fork of [Skyline](https://github.com/skyline-dev/skyline/), which has since been modified to have an architecture similar to [exlaunch](https://github.com/shadowninja108/exlaunch).


## Installation instructions (LayeredFS)

In the `/atmosphere/contents/<title id>/` folder, make an exefs directory. Create the title id folder if it doesn't already exist. Consult the table below for the proper name. Then, download the latest release or build artifact, and unzip into the root of your SD card. The `atmosphere` folders should merge.

Compatible game versions:

| Game                         | Title ID                                           | Working Versions              | Codename |
|------------------------------|:---------------------------------------------------|-------------------------------|:---------|
| Xenoblade 2                  | `0100E95004038000`<br/>(`0100F3400332C000` for JP) | 1.0.0-2.0.0 (or Torna 1.0.0*) | `bf2`    |
| Xenoblade 2: Torna           | `0100C9F009F7A000`                                 | 1.0.0 (or XB2 2.0.0)          | `ira`    |
| Xenoblade Definitive Edition | `0100FF500E34A000`                                 | All versions                  | `bfsw`   |
| Xenoblade 3                  | `010074F013262000`                                 | 2.0.0-2.2.0**                 | `bf3`    |

*: Torna 1.0.0 does not load the Torna DLC in Xenoblade 2, or potentially any other add-on content.<br>
**: Xenoblade 3 has limited features. Only a select few patches work, including the freecam.

If your game is too up-to-date, there may be some success found in copying the executable from an older version and placing it as `/atmosphere/contents/<title id>/exefs/main`. For instance, this allows you to take your 2.0.0 dump of Xenoblade 2 and use it on 2.1.0. Just be aware that this will re-introduce bugs and break some features, notably Massive Melee Mythra and Korean language support.

### A note on Skyline plugins
Xenomods supports Skyline plugins, acting as its own version of Skyline. However, this means that the `main.npdm` and `subsdk9` files from other mods will overwrite Xenomods. To get plugins to work alongside Xenomods, always use the ones provided in releases. Skyline mods should still function properly when the NSO is placed in `/atmosphere/contents/<title id>/romfs/skyline/plugins/`. 

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

Once built, copy xenomods.nso (renamed to `subsdk9`) and main.npdm from build/ to the LayeredFS exefs directory.

For those with sys-ftpd installed, adding `-DXENOMODS_SWITCH_IP=<ip>` to the first CMake command will automatically copy the files to your console when builds finish. (Provided it is configured for anonymous log-in.)

## Special Thanks and Attributions

- All the contributors and maintainers of Skyline, especially:
  - [Shadów](https://github.com/shadowninja108) for the newer exlaunch framework, and its patching design that we've since adopted.
  - Frank ([3096](https://github.com/3096)) for the original Ether Flow repository, and for graciously allowing the old event-specific freecam code to be published. And for their really handy skyline-stuff library as well!
- [RoccoDev](https://github.com/RoccoDev) for imgui-xeno and for working out debug drawing in DE and 3. Check out their Xenoblade mods too!
