#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "character_logic.h"
#include "../ui/ui.h"
#include <cjson/cJSON.h>
#include "profile_logic.h"
#include "health_tracker.h"

#define JSON_FILE_PATH "/home/dnd1/Documents/DND_Screen/health_data.json"

char selected_character[50];
int selected_character_hp = 0;
int selected_character_max_hp = 0;

void load_characters(const char *profile_name) {
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
    char dropdown_options[512] = ""; // Clear dropdown options

    for (int i = 0; i < profile_count; i++) {
        cJSON *profile = cJSON_GetArrayItem(json, i);
        cJSON *name = cJSON_GetObjectItem(profile, "name");

        if (name && strcmp(name->valuestring, profile_name) == 0) {
            cJSON *characters = cJSON_GetObjectItem(profile, "characters");
            int char_count = cJSON_GetArraySize(characters);

            if (char_count > 0) {
                cJSON *first_character = cJSON_GetArrayItem(characters, 0);
                cJSON *char_name = cJSON_GetObjectItem(first_character, "name");

                if (char_name) {
                    strcpy(selected_character, char_name->valuestring);  // 🚀 Auto-select first character in logic
                    printf("DEBUG: Default selected character: %s\n", selected_character);
                }
            }

            for (int j = 0; j < char_count; j++) {
                cJSON *character = cJSON_GetArrayItem(characters, j);
                cJSON *char_name = cJSON_GetObjectItem(character, "name");

                if (char_name) {
                    strcat(dropdown_options, char_name->valuestring);
                    strcat(dropdown_options, "\n");
                }
            }
            break;
        }
    }

    lv_dropdown_set_options(ui_Dropdown2, dropdown_options);
    lv_dropdown_set_selected(ui_Dropdown2, 0);  // 🚀 UI default selection

    cJSON_Delete(json);

    // 🚀 Manually trigger logic as if user selected first character
    on_character_selected(NULL);
}




void on_character_selected(lv_event_t * e) {
    int selected_index = lv_dropdown_get_selected(ui_Dropdown2);

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

    int profile_count = cJSON_GetArraySize(json);

    for (int i = 0; i < profile_count; i++) {
        cJSON *profile = cJSON_GetArrayItem(json, i);
        cJSON *name = cJSON_GetObjectItem(profile, "name");

        if (name && strcmp(name->valuestring, selected_profile) == 0) {
            cJSON *characters = cJSON_GetObjectItem(profile, "characters");
            cJSON *character = cJSON_GetArrayItem(characters, selected_index);

            if (character) {
                strcpy(selected_character, cJSON_GetObjectItem(character, "name")->valuestring);
                selected_character_hp = cJSON_GetObjectItem(character, "current_hp")->valueint;
                selected_character_max_hp = cJSON_GetObjectItem(character, "max_hp")->valueint;

                printf("DEBUG: Character selected: %s\n", selected_character);

                // 🚀 Update health UI with selected character's values
                write_health_data(selected_character_hp, selected_character_max_hp, 0);
                update_health_display();
            }
            break;
        }
    }

    cJSON_Delete(json);
}
