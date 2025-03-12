#ifndef PROFILE_LOGIC_H
#define PROFILE_LOGIC_H

#include "lvgl.h"

// Function to load profiles into dropdown
void load_profiles();

// Function to handle profile selection
void on_profile_selected(lv_event_t * e);

extern char selected_profile[50];  // Stores the selected profile name

#endif
