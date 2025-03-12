#ifndef CHARACTER_LOGIC_H
#define CHARACTER_LOGIC_H

#include "lvgl.h"

extern char selected_character[50];
extern int selected_character_hp;
extern int selected_character_max_hp;

// Load characters into Dropdown2 based on selected profile
void load_characters(const char *profile_name);

// Handle character selection
void on_character_selected(lv_event_t * e);

#endif
