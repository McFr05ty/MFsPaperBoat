# Pit of 100 Trials (design draft)

Status: design only, nothing is implemented. Items marked (?) are unverified assumptions about the engine.

## Summary

A 100 floor dungeon entered from a pipe in Toad Town Tunnels (TIK_02), near the chest. Floors 1-99 are battles. Floors 10, 20, ... 90 are rest floors. Floor 100 is the only boss, an Electro Super Blooper.

## Decided

- Every run starts at floor 1. Nothing resumes.
- No saving inside the pit. The only save block is outside it.
- Death is a normal loss and nothing from the run is kept. (?) Game over reloads the last save.
- Leaving through a rest floor keeps everything earned so far.
- Difficulty follows the chapters: the 90 battle floors are blended across 8 chapters, about 11 floors each, with neighbouring chapters mixed near the boundaries.
- Rest floors have a way to leave, a chest with a Jelly Ultra (temporary, reward pool undecided), and a chance of a shop NPC or a floor-skip NPC.
- Shop: healing and battle items only, random stock per visit, prices rise with depth.
- Skip NPC: skips 3 floors for 30 coins or 5 floors for 50 coins.
- Floor 100: Electro Super Blooper. It uses the Super Blooper model with the Electro Blooper behaviour and raised stats.
- The pit does not depend on the other custom features. It has its own files, setting and menu section, and works with all of them off.

## Proposed, to confirm

- Price formula: base price x (1 + floor / 25), rounded.
- A skip never jumps past a rest floor or floor 100.
- The other custom features still apply inside the pit when they are on. Hard Mode multiplies the pit's own scaling, Super Guard and the star point floor work as usual. Rare Enemies do not roll, because every pit fight is a scripted battle.
- The pit uses Mario's normal coins and inventory.
- Battles start from a hidden Pit Master NPC in TIK_02 through the normal encounter system, and TIK_02 is the rest room. (?)

## Open questions

- Reward pool for rest floors, and a reward for floor 100.
- Exact pipe position, and how the pipe is created without editing a large upstream file.
- Whether the hybrid boss can reuse the Electro Blooper scripts with the Super Blooper sprite. (?)
- Whether repeated scripted battles from one NPC script work. (?)

## Stages

1. Prototype: one battle from the pipe through the Pit Master NPC, and a return to the map.
2. Floor chain with chapter-blended enemies.
3. Rest floors, chest, and leaving.
4. Shop and skip NPC.
5. Floor 100 boss.
6. Polish, exemptions from other features, release test.

Each stage gets a debug test and a release test before it ships. The Super Guard crash only appeared in release.
