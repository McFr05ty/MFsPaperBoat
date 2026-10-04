#include "PaperboatMenu.h"
#include "hard_mode.h"
#include "super_guard.h"
#include "rare_enemy.h"

namespace PaperboatGui {

extern std::shared_ptr<PaperboatMenu> mPaperboatMenu;

using namespace UIWidgets;

static const std::unordered_map<int32_t, const char*> blockWindowOptions = {
    { 0, "Original (3 frames)" },
    { 1, "Forgiving (7 frames)" },
    { 2, "Very Forgiving (10 frames)" },
};

static const std::unordered_map<int32_t, const char*> actionCommandDifficultyOptions = {
    { 0, "Original" },
    { 1, "Forgiving (-1 level)" },
    { 2, "Very Forgiving (-2 levels)" },
    { 3, "Extremely Forgiving (-3 levels)" },
};

void PaperboatMenu::AddMenuEnhancements() {
    // Add Enhancements Menu
    AddMenuEntry("Enhancements", CVAR_SETTING("Menu.EnhancementsSidebarSection"));

    // Enhancements > Cheats
    WidgetPath path = { "Enhancements", "Cheats", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Infinite Health", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("InfiniteHealth"))
        .Options(CheckboxOptions().Tooltip("Mario's HP won't decrease during battle."));

    AddWidget(path, "Infinite Flower Points", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("InfiniteFlowerPoints"))
        .Options(CheckboxOptions().Tooltip("Mario's FP won't decrease during battle."));

    AddWidget(path, "No Badge Cost", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("NoBPCost"))
        .Options(CheckboxOptions().Tooltip("Equip any badge regardless of BP cost."));

    AddWidget(path, "Max Star Power", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("MaxStarPower"))
        .Options(CheckboxOptions().Tooltip("Star Power stays full and won't decrease."));

    AddWidget(path, "Max Power Bounce Chance", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("MaxPowerBounceChance"))
        .Options(
            CheckboxOptions().Tooltip(
                "Power Bounce's and Goombario's Multibonk's chance to continue stays full, so a chain only ends "
                "when you miss the timing or reach the bounce limit."
            )
        );

    AddWidget(path, "2x Star Points and Coins", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_CHEAT("DoubleRewards"))
        .Options(CheckboxOptions().Tooltip("Defeated enemies give twice the Star Points and coins."));

    // Enhancements > Gameplay
    path = { "Enhancements", "Gameplay", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", path.sidebarName, 2);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "DX: Prevent Loading Zone Storage", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("PreventLoadingZoneStorage"))
        .Options(
            CheckboxOptions().Tooltip(
                "Locks out player input the moment a loading zone is triggered, which patches "
                "out the Loading Zone Storage glitch. Off by default to match the original "
                "game. Note that most loading zones also trigger while you are airborne above "
                "them, so enabling this can freeze Mario in midair until he lands."
            )
        );

    AddWidget(path, "Sprint Button", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("SprintButton"))
        .Options(CheckboxOptions().Tooltip("Hold R to move at double speed in the overworld."));

    AddWidget(path, "Action Command Difficulty", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("ActionCommandDifficulty"))
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Lowers the difficulty of attack action commands, which widens their input "
                    "windows. Stacks with the Dodge Master badge."
                )
                .ComboMap(actionCommandDifficultyOptions)
        );

    AddWidget(path, "Block Window", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_ENHANCEMENT("BlockWindowMode"))
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Sets the window for timed defensive blocks. The default is 3 frames. Anything "
                    "wider overrides the Dodge Master badge."
                )
                .ComboMap(blockWindowOptions)
        );
    AddWidget(path, "Hard Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_HARD_MODE)
        .Options(
            CheckboxOptions().Tooltip(
                "Enemies have double health and deal 1.5x attack damage (rounded up). "
                "Status effects are not changed. Toggle before entering a battle."
            )
        );

    AddWidget(path, "Super Guard", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_SUPER_GUARD)
        .Options(
            CheckboxOptions().Tooltip(
                "Press B on the exact frame an enemy attack hits to negate all of its damage. "
                "Direct attacks also reflect 1 damage back onto the attacker. Mario only."
            )
        );

    AddWidget(path, "Super Guard Timing", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_SUPER_GUARD_WINDOW)
        .Options(
            IntSliderOptions()
                .Tooltip(
                    "How many frames a B press can land before an enemy attack hits and still count as a Super Guard. "
                    "1 is frame perfect. Requires Super Guard to be enabled."
                )
                .Min(1)
                .Max(5)
                .DefaultValue(SUPER_GUARD_WINDOW)
                .Format("%d frames")
        );
		
    AddWidget(path, "Rare Enemies", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_RARE_ENEMIES)
        .Options(
            CheckboxOptions().Tooltip(
                "Some overworld enemies appear as rare versions. Takes effect the next time a map loads."
            )
        );

    AddWidget(path, "Rare Enemy Spawn Chance", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_RARE_CHANCE)
        .Options(
            IntSliderOptions()
                .Tooltip(
                    "Chance that each hostile overworld enemy spawns as a rare version. Requires Rare Enemies to be enabled. "
                    "Takes effect the next time a map loads."
                )
                .Min(0)
                .Max(100)
                .DefaultValue(RARE_ENEMY_CHANCE_PERCENT)
                .Format("%d%%")
        );

    // Enhancements > Graphics
    path = { "Enhancements", "Graphics", SECTION_COLUMN_1 };
    AddSidebarEntry("Enhancements", "Graphics", 1);

    AddWidget(path, "Full Height View", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("Graphics.FullHeightView"))
        .Options(
            CheckboxOptions().Tooltip("Removes the letterboxing bars on the top and bottom of the screen in gameplay.")
        );

    AddWidget(path, "Rounded Projector Reel", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_ENHANCEMENT("Graphics.RoundedReel"))
        .Options(
            CheckboxOptions().Tooltip("Flips the battle projector reel to appear rounded for widescreen resolutions.")
        );
}

} // namespace PaperboatGui
