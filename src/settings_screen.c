#include "settings_screen.h"
#include "../ui/ui.h"
#include "character_logic.h"
#include "profile_logic.h"
#include <string.h>
#include <stdio.h>

void update_settings_screen_info(void) {
    if (strlen(selected_profile) > 0)
        lv_label_set_text(ui_CPText, selected_profile);
    else
        lv_label_set_text(ui_CPText, "None selected");

    if (strlen(selected_character) > 0)
        lv_label_set_text(ui_CCText, selected_character);
    else
        lv_label_set_text(ui_CCText, "None selected");

    printf("DEBUG: Updated settings screen:\n  Profile: %s\n  Character: %s\n", selected_profile, selected_character);
}
