# Custom features

This fork of [PaperBoat](https://github.com/HarbourMasters/PaperBoat) adds optional gameplay features. All of them are under **Enhancements > Gameplay** in the in-game menu.

PaperBoat is made by Harbour Masters and is based on the Paper Mario decompilation and Paper Mario DX. This fork is not affiliated with them. You need your own legally obtained US copy of Paper Mario; no game assets are included here.

## Hard Mode

- Enemies start battles with double health.
- Enemy attacks deal 1.5x damage to Mario and his partners, rounded up, applied after defense and blocking.
- Status effects are not changed.
- A save file loaded while Hard Mode is on receives a Lucky Star key item (if it does not already have one and there is room in the key item list), and action commands are enabled. Nothing is shown on screen. The story scene that normally gives out the Lucky Star is unchanged, and turning Hard Mode off later does not remove the item or action commands.

## Rare Enemies

Settings: **Rare Enemies** (checkbox) and **Rare Enemy Spawn Chance** (slider, 0-100%, default 5%). Each hostile overworld enemy rolls when a map loads. Friendly NPCs never roll.

- Rare enemies shimmer with a rainbow, holographic-card palette effect in the overworld and in battle.
- A fight started by a rare enemy gives its enemies +2 attack and +1 defense. The defense bonus is skipped for weak hits so they are never reduced to zero by it.
- A rare enemy drops one extra item from a pool of Super Shroom, Maple Syrup, Ultra Shroom and Life Shroom.
- Each defeated enemy drops double star points (at least +1) with a floor of 5, never exceeding the 100-point cap per battle.

## Super Guard

Settings: **Super Guard** (checkbox, off by default) and **Super Guard Timing** (slider, 1-5 frames, default 1).

- Press B within the timing window before an enemy attack hits to take no damage. A B press in the frames just before the window counts as mashing and cancels it.
- Direct attacks also reflect 1 damage back onto the attacker as a shock hit, with the usual shock animation and sound. Attacks flagged as ranged are only negated.
- Bob-ombs survive the reflect unless Mario is electrified, in which case contact still defeats them as in the original game.
- Mario only. Status effects that an attack inflicts are not specially handled and have not been tested.
- The reflect interrupts the attacker, so enemy scripts that were not written for it can behave oddly. Not every enemy has been tested.

## Tuning

Rare enemy constants (spawn default, shimmer strength and speed, rare bonuses, star point rules) are in `include/rare_enemy.h`. Super Guard defaults and the reflect damage are in `include/super_guard.h`.

## Testing status

- Checked in play: Hard Mode health and damage; rare enemy spawning, the shimmer in the overworld and in battle, the rare attack and defense bonuses, and the spawn chance slider; Super Guard and its toggle; the Hard Mode Lucky Star gift enabling action commands.
- Not yet checked: the 5-point star floor; Hard Mode combined with rare enemies; Hard Mode in boss fights (some boss scripts react to specific HP values); the Super Guard timing slider; the Bob-omb exception for the Super Guard reflect.

## Building

See `docs/BUILDING.md`. Building with a recent Visual Studio needed the extra Clang flag `-Wno-incompatible-pointer-types`, which is already added in `CMakeLists.txt`. New source files are found by a CMake pattern that only refreshes when you configure again, so re-run `cmake --preset windows-debug` (or the release preset) after pulling this fork.
