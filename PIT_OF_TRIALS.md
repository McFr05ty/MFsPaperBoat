# Pit of 100 Trials

A 100 floor dungeon, entered from a pipe in Toad Town Tunnels. This file describes how it works now. Source files are in `src/pit/`, with the setting names and shared functions in `include/pit_of_trials.h`.

## A run

- The entrance is a blue pipe in the Blooper room (TIK_02). Beside it are a sign, a save block and a heart block. Going down the pipe starts a run at floor 1. Nothing resumes.
- You can not save inside the Pit. Game over loads your last save, as usual.
- Floors 1-99 are battles, except every tenth floor (10, 20, ... 90), which is a rest room. That makes 90 battle floors, 9 rest floors and the boss on floor 100.
- Each floor has a pipe down to the next one. Rest rooms also have a return pipe back to the tunnels, and everything you earned stays with you.
- The entrance sign shows "Lowest floor reached". It is stored in the config, so all save files share it.

## Battle floors

- The overworld enemy looks like the first enemy of the battle it starts (`pit_enemy.c` has a look for each enemy).
- The battle is chosen from every battle in the game by a difficulty score, so the enemies climb from about level 5 on floor 1 to about level 30 on floor 99, with neighbouring chapters mixed together (`pit_battles.c`, `pit_pick_battle` in `pit.c`).
- From floor 80, one fight in twenty is a single Anti Guy.
- Winning raises the pipe to the next floor.

## Rest floors

- **Chest.** Each rest floor has a fixed reward, paid out once per save file. After that the chest holds coins equal to the floor number.

  | Floor | Reward |
  | --- | --- |
  | 10 | HP Plus |
  | 20 | Happy Heart |
  | 30 | FP Plus |
  | 40 | Strange Sack (key item) |
  | 50 | Defend Plus |
  | 60 | Power Plus |
  | 70 | Damage Dodge |
  | 80 | Happy Flower |
  | 90 | Flower Saver |

  The badges are the spare copies the game never hands out, so they can not clash with ones found in the story.
- **Shop.** Rip Cheato sets up shop on a rest floor with a chance of 33% (Pit menu slider). He sells 3 random items from a pool of healing and battle items. The price is the item's normal price plus 5% for each floor.
- **The Mover** (see below) can be here too.

## The Mover

On any floor from 1 to 90 the Mover can be there, with a default chance of 3% (Pit menu slider, at most 10%). On a battle floor he takes the place of the enemy. He skips 3 floors for 30 coins or 5 floors for 50 coins, plus 5% of that price for each floor after the first. A skip never goes past floor 99.

## The boss

Floor 100 is an Electro Super Blooper: an Electro Blooper as big as a Super Blooper, with 150 HP and attacks that do 4 times as much damage. When it falls, the return pipe rises and a chest appears. The chest holds the **Holofoil Hero** badge (an Ultra Jelly if Mario already has it, a Dried Shroom after a weenie run).

## Challenge options

- **Disable Spin/First/Bump Attack Badges** is on by default, tooltip "Only Weenies Disable This Option". While it is on, those three badges do nothing inside the Pit. Outside the Pit they work as normal.
- If the option is off on any floor of a run (checked every frame), that is a **weenie run**. The boss chest holds a Dried Shroom, and if the run set the lowest floor, the sign adds "That would be impressive if you weren't such a weenie."
- The sign adds "Your Paper Killing Skills are Remarkable!" when the lowest floor was reached on a run where all of these held on every floor: Hard Mode on with at least 2.0x health and 1.5x attack; Action Command Difficulty and Block Window at "Original"; Super Guard off or at 1 frame; the badge option above on; the Mover chance at 3% or lower and the shop chance at 33% or lower.
- Both sign lines belong to the run that set the lowest floor. A later record on a run that does not qualify replaces them. The "Reset entrance sign to 0" button in the menu clears the record and both lines.

## Menu (MF's Enhancements > Pit of Trials)

Always there: Reset entrance sign to 0, Disable Spin/First/Bump Attack Badges, Shop chance, Mover chance.

Debug builds only: Enter Pit (prototype), Start floor, Reset Pit rewards, Sign: show weenie line, Sign: show impressive line. Release builds ignore a start floor left in the config. A run that starts above floor 1 does not count towards the lowest floor.

## How it is built

- The Pit rooms are extra maps added to the Toad Town Tunnels area when the Pit is first entered (`pit.c`). The battle and rest rooms copy the shape of TIK_18 and the boss room copies TIK_02.
- The Rooms use three spare game flags (`GF_Unused_EVT_0A`, `0B`, `0C`) for the pipes and the chest. They are cleared when a room loads and when the Pit is left.
- `pit_rewards.c`: rest-floor chests. The claimed rewards are kept in the save file (`pitRewardsClaimed`).
- `pit_shop.c`, `pit_mover.c`, `pit_text.c`: the NPCs, with their speech and menus built in memory.
- `pit_boss.c`, `pit_boss_actor.c`: floor 100. The extra damage is in `src/port/enhancements/gameplay/PitOfTrials.cpp`.
- `pit_sign.c`: the floor sign and the entrance sign.
- The Strange Sack (`strange_sack.c`) and the Holofoil Hero (`holofoil_hero.c`) are separate small features that the Pit hands out.

## Not yet checked

- The chest reward on every rest floor, and that each pays out only once per save.
- The weenie and Remarkable sign lines, and the Dried Shroom prize.
- The Swooper, Clubba and Koopatrol overworld looks.
- The Pit in a release build. The Super Guard crash only appeared in release, so every new feature needs a release test.
