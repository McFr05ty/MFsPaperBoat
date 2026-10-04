# Custom features

This fork of [PaperBoat](https://github.com/HarbourMasters/PaperBoat) adds three optional gameplay features. All of them are under **Enhancements > Gameplay** in the in-game menu.

PaperBoat is made by Harbour Masters and is based on the Paper Mario decompilation and Paper Mario DX. This fork is not affiliated with them. You need your own legally obtained US copy of Paper Mario; no game assets are included here.

## Hard Mode

- Enemies start battles with double health.
- Enemy attacks deal 1.5x damage to Mario and his partners, rounded up, applied after defense and blocking.
- Status effects are not changed.

## Rare Enemies

Settings: **Rare Enemies** (checkbox) and **Rare Enemy Spawn Chance** (slider, 0-100%, default 5%). Each hostile overworld enemy rolls when a map loads. Friendly NPCs never roll.

- Rare enemies shimmer with a rainbow, holographic-card palette effect in the overworld and in battle.
- A fight started by a rare enemy gives its enemies +2 attack and +1 defense. The defense bonus is skipped for weak hits so they are never reduced to zero by it.
- A rare enemy drops one extra item from a pool of Super Shroom, Maple Syrup, Ultra Shroom and Life Shroom.
- Each defeated enemy drops double star points (at least +1) with a floor of 5, never exceeding the 100-point cap per battle.

## Super Guard

Setting: **Super Guard** (checkbox, off by default).

- Press B on the exact frame an enemy attack hits (a one-frame window) to take no damage.
- Direct attacks also reflect 1 damage back onto the attacker as a shock hit. Ranged attacks are only negated.
- A B press in the frames just before the hit counts as mashing and cancels it.
- Mario only. The reflect plays the standard shock sound.

## Tuning

Constants (spawn default, shimmer strength and speed, rare bonuses, star point rules) are in `include/rare_enemy.h`. Super Guard timing and reflect damage are in `include/super_guard.h`.

## Testing status

- Checked in play: Hard Mode health and damage, rare enemy spawning, the shimmer in the overworld and in battle, the rare attack and defense bonuses, the spawn chance slider, and Super Guard with its toggle.
- Not yet checked: the 5-point star floor, Hard Mode combined with rare enemies, and Hard Mode in boss fights (some boss scripts react to specific HP values).

## Building

See `docs/BUILDING.md`. Building with a recent Visual Studio needed the extra Clang flag `-Wno-incompatible-pointer-types`, which is already added in `CMakeLists.txt`.
