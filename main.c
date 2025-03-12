/**
 * @file main
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define _DEFAULT_SOURCE /* needed for usleep() */
#include <stdlib.h>
#include <unistd.h>
#include "lvgl/lvgl.h"
#include "ui/ui.h"
#include "src/health_tracker.h"
#include "src/profile_logic.h"
#include "src/character_logic.h"
#include <stdio.h>
#include <cjson/cJSON.h>


/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_display_t * hal_init(int32_t w, int32_t h);

/**********************
 *      FUNCTIONS
 **********************/

// Function to fetch profiles at startup
void run_fetch_profiles() {
    printf("DEBUG: Fetching profiles from API...\n");
    int result = system("python3 /home/dnd1/Documents/DND_Screen/fetch_profiles.py"); 

    if (result != 0) {
        printf("ERROR: Failed to fetch profiles!\n");
    } else {
        printf("DEBUG: Profiles fetched successfully.\n");
    }
}

void reset_guest_data() {
    printf("DEBUG: Resetting guest_data.json to default values...\n");

    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "name", "Guest");
    cJSON_AddNumberToObject(json, "current_hp", 0);
    cJSON_AddNumberToObject(json, "max_hp", 0);
    cJSON_AddNumberToObject(json, "temp_hp", 0);

    char *json_string = cJSON_Print(json);
    FILE *file = fopen("/home/dnd1/Documents/DND_Screen/guest_data.json", "w");

    if (file) {
        fputs(json_string, file);
        fclose(file);
        printf("DEBUG: guest_data.json reset successfully.\n");
    } else {
        printf("ERROR: Failed to write guest_data.json!\n");
    }

    free(json_string);
    cJSON_Delete(json);
}


int main(int argc, char **argv) {
    printf("DEBUG: Application Started Successfully!\n");

    /* Initialize LVGL */
    lv_init();

    /* Initialize display and input devices */
    hal_init(800, 480);

    /* Fetch profiles from API */
    run_fetch_profiles();

    /* Initialize UI */
    ui_init();

    /* Load profiles into Dropdown1 */
    load_profiles();

    /* Attach event to handle profile selection */
    lv_obj_add_event_cb(ui_Dropdown1, on_profile_selected, LV_EVENT_VALUE_CHANGED, NULL);

    /* Attach event to handle character selection */
    lv_obj_add_event_cb(ui_Dropdown2, on_character_selected, LV_EVENT_VALUE_CHANGED, NULL);

    /* Load health values into UI */
    init_health_tracker();

    /* Attach UI events for health tracker */
    setup_health_screen_events();

    printf("DEBUG: Starting LVGL loop...\n");

    while (1) {
        lv_timer_handler();
        usleep(5 * 1000);
    }

    lv_deinit();
    return 0;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Initialize the Hardware Abstraction Layer (HAL) for the LVGL graphics
 * library
 */
static lv_display_t * hal_init(int32_t w, int32_t h) {
    lv_group_set_default(lv_group_create());

    lv_display_t * disp = lv_sdl_window_create(w, h);

    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_set_group(mouse, lv_group_get_default());
    lv_indev_set_display(mouse, disp);
    lv_display_set_default(disp);

    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();
    lv_indev_set_display(mousewheel, disp);

    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_set_display(keyboard, disp);
    lv_indev_set_group(keyboard, lv_group_get_default());

    return disp;
}
