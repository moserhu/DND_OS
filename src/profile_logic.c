#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "profile_logic.h"
#include "../ui/ui.h"
#include <cjson/cJSON.h>
#include "character_logic.h"


#define JSON_FILE_PATH "/home/dnd1/Documents/DND_Screen/health_data.json"

char selected_profile[50];  // Store the selected profile name

void load_profiles() {
    FILE *file = fopen(JSON_FILE_PATH, "r");
    if (!file) {
        printf("ERROR: Failed to open health_data.json\n");
        return;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *json_data = (char *)malloc(file_size + 1);
    fread(json_data, 1, file_size, file);
    fclose(file);
    json_data[file_size] = '\0';

    cJSON *json = cJSON_Parse(json_data);
    free(json_data);

    if (!json) {
        printf("ERROR: Failed to parse JSON\n");
        return;
    }

    int profile_count = cJSON_GetArraySize(json);
    char dropdown_options[512] = "";

    if (profile_count > 0) {
        cJSON *first_profile = cJSON_GetArrayItem(json, 0);
        cJSON *name = cJSON_GetObjectItem(first_profile, "name");

        if (name) {
            strcpy(selected_profile, name->valuestring);  // 🚀 Auto-select first profile in logic
            printf("DEBUG: Default selected profile: %s\n", selected_profile);
        }
    }

    for (int i = 0; i < profile_count; i++) {
        cJSON *profile = cJSON_GetArrayItem(json, i);
        cJSON *name = cJSON_GetObjectItem(profile, "name");

        if (name) {
            strcat(dropdown_options, name->valuestring);
            strcat(dropdown_options, "\n");
        }
    }

    lv_dropdown_set_options(ui_Dropdown1, dropdown_options);
    lv_dropdown_set_selected(ui_Dropdown1, 0);  // 🚀 UI default selection
    cJSON_Delete(json);

    // 🚀 Manually trigger logic as if user selected first profile
    on_profile_selected(NULL);
}



void on_profile_selected(lv_event_t * e) {
    int selected_index = lv_dropdown_get_selected(ui_Dropdown1);

    FILE *file = fopen(JSON_FILE_PATH, "r");
    if (!file) return;

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *json_data = (char *)malloc(file_size + 1);
    fread(json_data, 1, file_size, file);
    fclose(file);
    json_data[file_size] = '\0';

    cJSON *json = cJSON_Parse(json_data);
    free(json_data);

    if (!json) return;

    cJSON *profile = cJSON_GetArrayItem(json, selected_index);
    if (!profile) {
        cJSON_Delete(json);
        return;
    }

    strcpy(selected_profile, cJSON_GetObjectItem(profile, "name")->valuestring);
    printf("DEBUG: Profile selected: %s\n", selected_profile);
    cJSON_Delete(json);

    // 🚀 Load characters for this profile immediately
    load_characters(selected_profile);
}
