#include "PaperboatMenu.h"
#include "higher_level_cap.h"
#include "hard_mode.h"
#include "super_guard.h"
#include "rare_enemy.h"
#include "pit_of_trials.h"

namespace PaperboatGui {

extern std::shared_ptr<PaperboatMenu> mPaperboatMenu;

using namespace UIWidgets;

// Hard Mode multipliers, stored as a choice number (0 = 1.0x ... 4 = 3.0x).
static const std::unordered_map<int32_t, const char*> hardModeMultiplierOptions = {
    { 0, "1.0x" },
    { 1, "1.5x" },
    { 2, "2.0x" },
    { 3, "2.5x" },
    { 4, "3.0x" },
};

void PaperboatMenu::AddMenuMFEnhancements() {
    // Add MF's Enhancements menu (right after Enhancements)
    AddMenuEntry("MF's Enhancements", CVAR_SETTING("Menu.MFEnhancementsSidebarSection"));

    // MF's Enhancements > Hard Mode
    WidgetPath path = { "MF's Enhancements", "Hard Mode", SECTION_COLUMN_1 };
    AddSidebarEntry("MF's Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Hard Mode", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_HARD_MODE)
        .Options(
            CheckboxOptions().Tooltip(
                "Enemies have more health and deal more attack damage (the options below set how much; defaults are 2.0x and 1.5x, rounded up). "
                "Status effects are not changed. Toggle before entering a battle."
            )
        );

    AddWidget(path, "Hard Mode Health", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_HARD_MODE_HEALTH)
        .PreFunc([](WidgetInfo& info) {
            // An unset or out-of-range value would make the combo box throw, so reset it to the default first.
            int32_t choice = CVarGetInteger(CVAR_HARD_MODE_HEALTH, -1);
            if (choice < 0 || choice > 4) {
                CVarSetInteger(CVAR_HARD_MODE_HEALTH, HARD_MODE_DEFAULT_HEALTH_CHOICE);
            }
        })
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Enemy health multiplier while Hard Mode is on (rounded up). Default 2.0x. "
                    "Applies to battles that start afterward."
                )
                .ComboMap(hardModeMultiplierOptions)
        );

    AddWidget(path, "Hard Mode Attack", WIDGET_CVAR_COMBOBOX)
        .CVar(CVAR_HARD_MODE_ATTACK)
        .PreFunc([](WidgetInfo& info) {
            int32_t choice = CVarGetInteger(CVAR_HARD_MODE_ATTACK, -1);
            if (choice < 0 || choice > 4) {
                CVarSetInteger(CVAR_HARD_MODE_ATTACK, HARD_MODE_DEFAULT_ATTACK_CHOICE);
            }
        })
        .Options(
            ComboboxOptions()
                .Tooltip(
                    "Enemy attack damage multiplier while Hard Mode is on (rounded up). Default 1.5x. "
                    "Applied after defense and blocking. Status effects are not changed."
                )
                .ComboMap(hardModeMultiplierOptions)
        );

    // MF's Enhancements > Level Cap
    path = { "MF's Enhancements", "Level Cap", SECTION_COLUMN_1 };
    AddSidebarEntry("MF's Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Higher Level Cap", WIDGET_CVAR_CHECKBOX)
        .CVar(CVAR_HIGHER_LEVEL_CAP)
        .Options(
            CheckboxOptions().Tooltip(
                "Raises the most HP, FP and BP Mario can reach to 99 each. Stats above the normal limits are kept if you turn this off, "
                "but cannot be raised further. It also raises the level cap to the point where all three are 99, and guarantees 1 to 4 star points per enemy until then."
            )
        );

    // MF's Enhancements > Super Guard
    path = { "MF's Enhancements", "Super Guard", SECTION_COLUMN_1 };
    AddSidebarEntry("MF's Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

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

    // MF's Enhancements > Rare Enemies
    path = { "MF's Enhancements", "Rare Enemies", SECTION_COLUMN_1 };
    AddSidebarEntry("MF's Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

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

    // MF's Enhancements > Pit of Trials (prototype)
    path = { "MF's Enhancements", "Pit of Trials", SECTION_COLUMN_1 };
    AddSidebarEntry("MF's Enhancements", path.sidebarName, 1);
    path.column = SECTION_COLUMN_1;

    AddWidget(path, "Enter Pit (prototype)", WIDGET_BUTTON)
        .Options(
            ButtonOptions().Tooltip(
                "Prototype: jumps to a test room. There is no way back yet except loading a save, so use a copy of a save."
            )
        )
        .Callback([](WidgetInfo& info) { pit_request_enter(); });

        AddWidget(path, "Start floor (debug)", WIDGET_CVAR_SLIDER_INT)
            .CVar(CVAR_PIT_START_FLOOR)
            .Options(
                IntSliderOptions()
                    .Tooltip("Debug: the floor Enter Pit starts on. Floors 80 and up can show a single Anti Guy.")
                    .Min(1)
                    .Max(100)
                    .DefaultValue(1)
                    .Format("Start floor: %d")
            );

    AddWidget(path, "Shop chance", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_PIT_SHOP_CHANCE)
        .Options(
            IntSliderOptions()
                .Tooltip("Chance that a rest floor has Rip Cheato's shop. Rolled each time a floor loads.")
                .Min(0)
                .Max(100)
                .DefaultValue(PIT_SHOP_DEFAULT_CHANCE)
                .Format("Shop chance: %d%%")
        );

    AddWidget(path, "Mover chance", WIDGET_CVAR_SLIDER_INT)
        .CVar(CVAR_PIT_MOVER_CHANCE)
        .Options(
            IntSliderOptions()
                .Tooltip(
                    "Chance that the Mover is on a floor (floors 1 to 90, battle floors included, where he replaces the enemy). "
                    "Rolled each time a floor loads."
                )
                .Min(0)
                .Max(PIT_MOVER_MAX_CHANCE)
                .DefaultValue(PIT_MOVER_DEFAULT_CHANCE)
                .Format("Mover chance: %d%%")
        );
}

} // namespace PaperboatGui
