#include "delete_char_logic.h"
#include "../ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "profile_logic.h"
#include "character_logic.h"
#include "health_tracker.h"
#include "add_char_logic.h" 
#include "settings_screen.h"

#define DELETE_API_URL "http://192.168.1.116:9092/profiles/%d/characters/%d"

static char response_buffer[1024] = {0};

// Delete character from API
void delete_selected_character(lv_event_t *e) {
    printf("DEBUG: Attempting to delete selected character...\n");

    int profile_id = get_profile_id(selected_profile);
    if (profile_id == -1 || selected_character_id == -1) {
        printf("ERROR: Invalid profile or character selected!\n");
        return;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("ERROR: Failed to initialize cURL\n");
        return;
    }

    char url[256];
    snprintf(url, sizeof(url), DELETE_API_URL, profile_id, selected_character_id);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        printf("ERROR: Failed to send DELETE request. cURL error: %s\n", curl_easy_strerror(res));
        return;
    }

    printf("DEBUG: Character deleted. API response: %s\n", response_buffer);

    // Clear character selection
    selected_character[0] = '\0';
    selected_character_id = -1;
    selected_character_hp = 0;
    selected_character_max_hp = 0;

    // Refresh local cache and reload UI state
    run_fetch_profiles();
    sleep(1);
    load_characters(selected_profile);
    load_last_character(selected_profile);
    update_health_display();

    //  Hide the confirm delete panel for next time
    lv_obj_add_flag(ui_ConfirmDeleteContainer, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(ui_AllSettingsContainer, LV_OBJ_FLAG_HIDDEN);

    printf("DEBUG: Character deletion complete and UI updated.\n");

    //  Update settings screen
    update_settings_screen_info();
}

// Attach delete event
void setup_delete_char_events() {
    printf("DEBUG: Attaching Delete Character Event...\n");
    lv_obj_add_event_cb(ui_Button13, delete_selected_character, LV_EVENT_CLICKED, NULL);
    printf("DEBUG: Delete Character Event Attached Successfully!\n");
}
