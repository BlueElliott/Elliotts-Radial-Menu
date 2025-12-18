# Elliott's Radial Menu

A modified version of [**GW2Radial by Friendly0Fire**](https://github.com/Friendly0Fire/GW2Radial), extended with a **Template Wheel** feature for Build and Equipment template switching.

> **Important**: This is a modified fork of the original GW2Radial. All credit for the original addon and its features goes to [Friendly0Fire](https://github.com/Friendly0Fire). I've simply added the Template Wheel functionality for switching Build and Equipment templates.

## What's New: Template Wheel

**Template Wheel** - A new radial menu for switching Build and Equipment template combinations:
* 9 configurable template slots with helmet icons
* Combat queuing with visual timer (works like mount queuing)
* Action chains that switch Build → Equipment templates in sequence
* Customizable visibility controls (Default/Combat/Underwater/On Water/WvW)
* Reorderable slots

## Original GW2Radial Features

All features from [Friendly0Fire's GW2Radial](https://github.com/Friendly0Fire/GW2Radial) are included:
* Mount radial menu with smart automount
* Novelty radial menu
* Marker radial menu
* Custom radial menus
* Input queuing for combat situations
* Conditional menus

## Installation

### Download Release (Easiest)
1. Download the latest release from [Releases](https://github.com/BlueElliott/Elliotts-Radial-Menu/releases)
2. Extract `gw2addon_gw2radial.dll` from the zip
3. Place the DLL in: `<Guild Wars 2 Install>/addons/gw2radial/gw2addon_gw2radial.dll`
   - Example: `C:\Program Files\Guild Wars 2\addons\gw2radial\gw2addon_gw2radial.dll`
4. Run Guild Wars 2

**Note**: You need the [GW2 Addon Loader](https://github.com/gw2-addon-loader/loader-core) installed first

## Usage

### General Usage (Original Features)
- Press `Shift+Alt+M` in-game to open settings
- Configure keybinds for each wheel (Mount, Novelty, Marker, Template)
- Set in-game keybinds (F11 → Control Options) that match your addon keybinds
- Hold the keybind to show the radial menu, move mouse, release to select

### Template Wheel Specific
1. **Configure in-game keybinds** (F11 → Control Options):
   - Set Build Template keybinds (e.g., Ctrl+1, Ctrl+2, etc.)
   - Set Equipment Template keybinds (e.g., Alt+1, Alt+2, etc.)

2. **Configure Template Wheel** (Shift+Alt+M → Build + Equipment Templates):
   - For each slot, enter your Build and Equipment keybinds
   - Set visibility controls (when the slot should show/be usable)
   - Set a keybind to open the Template Wheel

3. **Use the wheel**:
   - Press your Template Wheel keybind
   - Select a template slot
   - Both Build and Equipment templates switch automatically
   - If in combat, the switch queues until combat ends

## Credits

**Original Creator**: [Friendly0Fire](https://github.com/Friendly0Fire) - Creator of [GW2Radial](https://github.com/Friendly0Fire/GW2Radial)

**Template Wheel Addition**: BlueElliott with Claude Code

**Original GW2Radial Credits**:
- @QuitarHero and @TanukiSoup for extensive testing
- Ghost for mount art
- Skiff and fishing icons by John Mausson
- [freepik](https://www.freepik.com/) for novelty art
- deltaconnected and Bhagawan for addon framework guidance

## Support & Issues

### For Template Wheel Issues
Open an issue on this repository: [Elliott's Radial Menu Issues](https://github.com/BlueElliott/Elliotts-Radial-Menu/issues)

### For Original GW2Radial Features
Please refer to the [original GW2Radial repository](https://github.com/Friendly0Fire/GW2Radial) and their [Discord](https://discord.gg/zqeHCEg)

### Want to Support the Original Creator?
If you enjoy this addon, please consider supporting [Friendly0Fire](https://github.com/Friendly0Fire):
- Ko-fi: [ko-fi.com/friendlyfire](https://ko-fi.com/friendlyfire)
- In-game mail: `FriendlyFire.6275`

## Troubleshooting

### The addon won't load or crashes

1. Look in your Guild Wars 2 installation folder for `gw2radial.log`
2. If it exists, check for error messages
3. If it doesn't exist, look for `gw2al_log.txt` (addon loader log)
4. Make sure you have the [addon loader](https://github.com/gw2-addon-loader/loader-core) and [d3d wrapper](https://github.com/gw2-addon-loader/d3d9_wrapper) installed

For more help, [open an issue](https://github.com/BlueElliott/Elliotts-Radial-Menu/issues)

### Template Wheel specific issues

For bugs or questions about the Template Wheel feature, please [open an issue](https://github.com/BlueElliott/Elliotts-Radial-Menu/issues) on this repository.

## License

This project is MIT licensed, maintaining compatibility with the original GW2Radial license.

## Disclaimer

This addon is not affiliated with or endorsed by ArenaNet or Guild Wars 2. Use at your own risk.

This is a community-created modification of GW2Radial. While the original GW2Radial has been acknowledged by ArenaNet, this modified version is an independent project.
