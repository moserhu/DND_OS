#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lvgl.h"
#include "profile_logic.h"
#include "../ui/ui.h"
#include <cjson/cJSON.h>
#include "character_logic.h"
#include <curl/curl.h>
#include "add_char_logic.h"

#define JSON_FILE_PATH "/home/dnd1/Documents/DND_Screen/health_data.json"
#define API_BASE_URL "http://192.168.1.222:8000"

char selected_profile[50];  // Store the selected profile name

void load_profiles() {
    printf("DEBUG: Loading profiles...\n");

    // Fetch profiles from API
    run_fetch_profiles();

    // Read JSON from local file
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

    printf("DEBUG: JSON Data Loaded: %s\n", json_data); // Check if it's valid JSON

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
            strcpy(selected_profile, name->valuestring);  // ✅ Ensure profile is set
            printf("DEBUG: Default selected profile: %s\n", selected_profile);
        }
    } else {
        printf("ERROR: No profiles found in JSON\n");
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
    lv_dropdown_set_selected(ui_Dropdown1, 0);  // ✅ UI default selection
    cJSON_Delete(json);

    // ✅ Manually trigger profile selection
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

// 🚀 Helper function to store API response
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    size_t total_size = size * nmemb;
    char *response = (char *)userdata;

    if (strlen(response) + total_size < 2048 - 1) {  // Prevent overflow
        strncat(response, (char *)ptr, total_size);
    }
    return total_size;
}



// 🚀 Function to get the Profile ID from API
int get_profile_id(const char *profile_name) {
    if (!profile_name) {
        printf("ERROR: Profile name is NULL\n");
        return -1;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("ERROR: Failed to initialize cURL\n");
        return -1;
    }

    char url[256];
    snprintf(url, sizeof(url), "%s/profiles", API_BASE_URL);

    char response[2048] = {0};
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, response);
    
    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        printf("ERROR: Failed to fetch profiles from API. cURL error: %s\n", curl_easy_strerror(res));
        return -1;
    }

    // 🚀 Parse API response JSON
    cJSON *json = cJSON_Parse(response);
    if (!json) {
        printf("DEBUG: Raw API Response for profiles: %s\n", response);
        return -1;
    }

    int profile_id = -1;
    cJSON *profile;
    cJSON_ArrayForEach(profile, json) {
        cJSON *name = cJSON_GetObjectItem(profile, "name");
        cJSON *id = cJSON_GetObjectItem(profile, "id");

        if (name && id && strcmp(name->valuestring, profile_name) == 0) {
            profile_id = id->valueint;
            break;
        }
    }

    cJSON_Delete(json);

    if (profile_id == -1) {
        printf("ERROR: Profile '%s' not found in API\n", profile_name);
    } else {
        printf("DEBUG: Profile ID for '%s' = %d\n", profile_name, profile_id);
    }

    return profile_id;
}
