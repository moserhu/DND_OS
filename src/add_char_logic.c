#include "add_char_logic.h"
#include "../ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>  
#include <cjson/cJSON.h>
#include <curl/curl.h>
#include "profile_logic.h"
#include "character_logic.h"
#include "health_tracker.h"

#define DB_API_URL "http://192.168.1.222:8000/profiles/%d/characters"
#define JSON_FILE_PATH "/home/dnd1/Documents/DND_Screen/health_data.json"


// Buffer to store API response
static char response_buffer[1024] = {0};

// Function to fetch profiles from API
void run_fetch_profiles() {
    printf("DEBUG: Fetching profiles from API...\n");
    int result = system("python3 /home/dnd1/Documents/DND_Screen/fetch_profiles.py");

    if (result != 0) {
        printf("ERROR: Failed to fetch profiles!\n");
    } else {
        printf("DEBUG: Profiles fetched successfully.\n");
    }
}

//function to load the last character int he list
void load_last_character(const char *profile_name) {
    printf("DEBUG: Loading last character for profile: %s\n", profile_name);

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

    for (int i = 0; i < profile_count; i++) {
        cJSON *profile = cJSON_GetArrayItem(json, i);
        cJSON *name = cJSON_GetObjectItem(profile, "name");

        if (name && strcmp(name->valuestring, profile_name) == 0) {
            cJSON *characters = cJSON_GetObjectItem(profile, "characters");
            int char_count = cJSON_GetArraySize(characters);

            if (char_count > 0) {
                cJSON *last_character = cJSON_GetArrayItem(characters, char_count - 1);
                cJSON *char_name = cJSON_GetObjectItem(last_character, "name");
                cJSON *char_id = cJSON_GetObjectItem(last_character, "id");

                if (char_name && char_id) {
                    strcpy(selected_character, char_name->valuestring);
                    selected_character_id = char_id->valueint;
                    printf("DEBUG: Last character selected: %s (ID: %d)\n", selected_character, selected_character_id);
                }
            }
            break;
        }
    }

    cJSON_Delete(json);
}


void add_new_character(lv_event_t *e) {
    printf("DEBUG: Starting character creation...\n");

    int profile_id = get_profile_id(selected_profile);
    if (profile_id == -1) {
        printf("ERROR: No profile selected! Cannot add character.\n");
        return;
    }

    // Step 1: Send API Request to Create New Character
    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("ERROR: Failed to initialize cURL\n");
        return;
    }

    char url[256];
    snprintf(url, sizeof(url), DB_API_URL, profile_id);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    char json_payload[256];
    snprintf(json_payload, sizeof(json_payload), 
             "{\"name\": \"New Character\", \"current_hp\": 100, \"max_hp\": 100, \"temp_hp\": 0}");

    response_buffer[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response_buffer);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        printf("ERROR: Failed to send request. cURL error: %s\n", curl_easy_strerror(res));
        return;
    }


    // Step 3: Refresh Profiles & Characters
    run_fetch_profiles();
    sleep(1); // Allow data to refresh

    // Step 2: Parse API Response
    cJSON *json = cJSON_Parse(response_buffer);
    if (!json) {
        printf("ERROR: Failed to parse API response\n");
        return;
    }

    cJSON *char_id_item = cJSON_GetObjectItem(json, "character_id");
    if (!cJSON_IsNumber(char_id_item)) {
        printf("ERROR: Invalid character ID received\n");
        cJSON_Delete(json);
        return;
    }
    
    int new_character_id = char_id_item->valueint;
    printf("DEBUG: Received Character ID from API = %d\n", new_character_id);  // Add this
    selected_character_id = new_character_id;
    
    cJSON_Delete(json);

    
    // Step 4: Manually Update `selected_character` Variables
    strcpy(selected_character, "New Character");
    selected_character_id = new_character_id;
    selected_character_hp = 100;
    selected_character_max_hp = 100;

    // Step 5: Write to `health_data.json`
    write_health_data(selected_character_hp, selected_character_max_hp, 0);

    run_fetch_profiles();
    sleep(1); // Allow data to refresh

    load_last_character(selected_profile);

    // Step 6: Update UI to Show the New Character
    update_health_display();

    printf("DEBUG: Character successfully added and set as active.\n");
}

// Function to attach the event listener
void setup_add_char_events() {
    printf("DEBUG: Attaching Add Character Event...\n");
    lv_obj_add_event_cb(ui_Settings1, add_new_character, LV_EVENT_CLICKED, NULL);
    printf("DEBUG: Add Character Event Attached Successfully!\n");
}
