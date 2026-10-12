# Custom features

This fork of [PaperBoat](https://github.com/HarbourMasters/PaperBoat) adds optional gameplay features. All of them are under **MF's Enhancements** in the in-game menu.

Based on upstream PaperBoat 1.0.3 (Mulberry Delta). Upstream release notes: https://github.com/HarbourMasters/PaperBoat/releases/tag/1.0.3

PaperBoat is made by Harbour Masters and is based on the Paper Mario decompilation and Paper Mario DX. This fork is not affiliated with them. You need your own legally obtained US copy of Paper Mario; no game assets are included here.

## Hard Mode

- Enemy health is multiplied when a battle starts (default 2.0x). Choose 1.0x, 1.5x, 2.0x, 2.5x or 3.0x with the **Hard Mode Health** option, rounded up. Changing it affects battles that start afterward.
- Enemy attack damage to Mario and his partners is multiplied (default 1.5x). Choose 1.0x, 1.5x, 2.0x, 2.5x or 3.0x with the **Hard Mode Attack** option, rounded up and applied after defense and blocking.
- Status effects are not changed.
- A save file loaded while Hard Mode is on receives a Lucky Star key item (if it does not already have one and there is room in the key item list), and action commands are enabled. Nothing is shown on screen. The story scene that normally gives out the Lucky Star is unchanged, and turning Hard Mode off later does not remove the item or action commands.

## Rare Enemies

Settings: **Rare Enemies** (checkbox) and **Rare Enemy Spawn Chance** (slider, 0-100%, default 5%). Each hostile overworld enemy rolls when a map loads. Friendly NPCs never roll.

- Rare enemies are drawn with a soft cycling rainbow tint over their normal sprite, in the overworld and in battle, so texture packs still show. Translucent sprites and sprites with status effects are not tinted. This replaces an earlier palette recolor that made rare enemies show the original art instead of the pack.
- A fight started by a rare enemy gives its enemies +2 attack and +1 defense. The defense bonus is skipped for weak hits so they are never reduced to zero by it.
- A rare enemy drops one extra item from a pool of Super Shroom, Maple Syrup, Ultra Shroom and Life Shroom.
- Each defeated enemy drops double star points (at least +1) with a floor of 5, never exceeding the 100-point cap per battle.

## Super Guard

Settings: **Super Guard** (checkbox, off by default) and **Super Guard Timing** (slider, 1-5 frames, default 1).

- Press B within the timing window before an enemy attack hits to take no damage. A B press in the frames just before the window counts as mashing and cancels it.
- Direct attacks also reflect 1 damage back onto the attacker, shown as a damage number and star burst with a flash. Attacks flagged as ranged are only negated. A reflect never interrupts an attacker that survives it. If it would defeat the attacker, the attacker is defeated through its normal death sequence. No shock effect or sound is used.
- The reflect no longer dispatches a shock event, so Bob-ombs are only defeated by it if it reduces them to 0 HP.
- Mario only. Status effects that an attack inflicts are not specially handled and have not been tested.
- An earlier version of the reflect used a shock hit and could crash the game in some battles because the battle script memory ran out. The current version avoids that, but not every enemy has been tested.

## Higher Level Cap

Setting: **Higher Level Cap** (checkbox, off by default).

- Mario can reach 99 HP, 99 FP and 99 BP instead of 50, 50 and 30. Max HP and FP with an HP Plus or FP Plus badge are capped at 99 as well.
- The level cap moves from level 27 to the point where all three stats are 99, which is about level 70. Each level-up raises one stat (HP and FP by 5, BP by 3, never past 99), so there is always a stat to raise.
- While Mario is below that cap, every defeated enemy gives at least 1 to 4 star points by its level (level 5 or lower: 1, 6-15: 2, 16-20: 3, 21 and up: 4). Enemies that already give more, such as Amazy Dayzees, keep their normal amount.
- Turning the option off keeps any stats already above the normal limits but stops further level-ups past the normal cap.
- In the debug build, the Save Editor limits follow the option.

## Pit of 100 Trials

A 100 floor dungeon. The entrance is a pipe in Toad Town Tunnels (the Blooper room, TIK_02), with a sign, a save block and a heart block beside it. See `PIT_OF_TRIALS.md` for the full details.

- Floors 1-99 are battles, except every tenth floor (10, 20, ... 90), which is a rest room. Enemies are blended across the chapters, from about level 5 on floor 1 to about level 30 on floor 99. Floor 100 is the boss, an Electro Super Blooper.
- Every run starts at floor 1. You can not save inside the Pit, and the entrance sign shows the lowest floor you have reached.
- Each rest room has a chest with a fixed reward that is paid out once per save file (HP Plus, Happy Heart, FP Plus, the Strange Sack, Defend Plus, Power Plus, Damage Dodge, Happy Flower, Flower Saver). After that it holds coins. Rest rooms can also have Rip Cheato's shop (default 33%), and any floor from 1 to 90 can have the Mover (default 3%), who skips floors for coins.
- The boss chest holds the **Holofoil Hero** badge: Mario gets the rare enemy rainbow tint. If you already have it, the chest holds an Ultra Jelly.
- The **Strange Sack** is a key item that raises how many items Mario can carry from 10 to 20. Everything that lists items (the pause menu, the quick menu, battle, shops, pickups) follows it. Saves made before it existed load as before.
- **Disable Spin/First/Bump Attack Badges** (checkbox, on by default, Pit only): inside the Pit those three badges do nothing. If you turn it off on any floor, the run is a weenie run: the boss chest holds a Dried Shroom, and if that run set your lowest floor, the sign says so.
- If your lowest floor was reached with Hard Mode on (at least 2.0x health and 1.5x attack), the action command options at their defaults, Super Guard off or at 1 frame, the badge option above on, and the Mover and shop chances at their defaults or lower, the sign praises your Paper Killing Skills.
- The Pit menu section has sliders for the shop and Mover chances, and a button that sets the entrance sign back to 0. In debug builds it also has an Enter Pit button, a start floor slider, a button that resets the chest rewards, and two checkboxes that force the sign lines.
- The lowest floor reached is stored in the config, so it is shared by all save files. The chest rewards are stored in the save file.

## Tuning

Rare enemy constants (spawn default, shimmer strength and speed, rare bonuses, star point rules) are in `include/rare_enemy.h`. Super Guard defaults and the reflect damage are in `include/super_guard.h`. The Pit's default shop and Mover chances and the highest Mover chance are in `include/pit_of_trials.h`; the Strange Sack's capacity is in `include/inventory.h`.

## Testing status

- Checked in play: Hard Mode health and damage; rare enemy spawning, the shimmer in the overworld and in battle, the rare attack and defense bonuses, and the spawn chance slider; Super Guard and its toggle; the Hard Mode Lucky Star gift enabling action commands; the Super Guard reflect in the release build; the Hard Mode health and attack options; the rare enemy tint, with and without a texture pack; opening the menu and the Blueprint Editor in the release build; Higher Level Cap: leveling past 27; Higher Level Cap: reaching 99 in all three stats; the Higher Level Cap star point floor; turning Higher Level Cap off; the pause, status bar and badge screens at 99 stats, including the 99 BP orb layout; the MF's Enhancements menu.
- Pit of 100 Trials, checked in play: entering and clearing floors; the boss and its Holofoil Hero chest; the Strange Sack raising the item limit to 20 (pickups, the quick menu, shops); the Pit shop.
- Not yet checked: the Pit rest-floor chest rewards on every rest floor and their once-per-save payout; the weenie and Remarkable sign lines and the Dried Shroom prize; the Swooper, Clubba and Koopatrol looks in the Pit; the Pit in a release build; the 5-point star floor; Hard Mode combined with rare enemies; Hard Mode in boss fights (some boss scripts react to specific HP values); the Super Guard timing slider; defeating an attacker with the reflect.

## Building

See `docs/BUILDING.md`. Building with a recent Visual Studio needed the extra Clang flag `-Wno-incompatible-pointer-types`, which is already added in `CMakeLists.txt`. New source files are found by a CMake pattern that only refreshes when you configure again, so re-run `cmake --preset windows-debug` (or the release preset) after pulling this fork.
