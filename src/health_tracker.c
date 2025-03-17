#include "health_tracker.h"
#include "../ui/ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <curl/curl.h> 
#include "character_logic.h"  // 🚀 Needed to access `selected_character`
#include "profile_logic.h"


#define HEALTH_DATA_FILE "/home/dnd1/Documents/DND_Screen/health_data.json"
#define GUEST_DATA_FILE "/home/dnd1/Documents/DND_Screen/guest_data.json"

int is_guest_mode = 0;

// Function to initialize health tracker (Ensures JSON exists)
void init_health_tracker() {
    printf("DEBUG: Initializing Health Tracker\n");

    // 🚀 Ensure guest data resets at startup
    FILE *guest_file = fopen(GUEST_DATA_FILE, "w");
    if (guest_file) {
        fprintf(guest_file, "{\"name\":\"Guest\", \"current_hp\":0, \"max_hp\":0, \"temp_hp\":0}");
        fclose(guest_file);
        printf("DEBUG: guest_data.json reset successfully.\n");
    } else {
        printf("ERROR: Failed to reset guest_data.json!\n");
    }

    // 🚀 Ensure main health_data.json exists
    FILE *file = fopen(HEALTH_DATA_FILE, "r");
    if (!file) {
        printf("DEBUG: Health data file not found, creating new one...\n");
        write_health_data(100, 100, 0); // Default values
    } else {
        fclose(file);
    }
    
    update_health_display();
}

void send_health_update(const char *new_name, int current, int max, int temp) {
    if (is_guest_mode) return;

    int profile_id = get_profile_id(selected_profile);
    int character_id = selected_character_id;  // ✅ Use character ID instead of name

    if (profile_id == -1 || character_id == -1) {
        printf("ERROR: Missing profile or character ID, skipping API update.\n");
        return;
    }

    CURL *curl = curl_easy_init();
    if (!curl) {
        printf("ERROR: Failed to initialize cURL\n");
        return;
    }

    char url[256];
    snprintf(url, sizeof(url), "http://192.168.1.222:8000/profiles/%d/characters/%d", profile_id, character_id);

    char json_data[512];
    snprintf(json_data, sizeof(json_data),
             "{\"name\": \"%s\", \"current_hp\": %d, \"max_hp\": %d, \"temp_hp\": %d}",
             new_name, current, max, temp);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data);

    CURLcode res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        printf("ERROR: Failed to send update. cURL error: %s\n", curl_easy_strerror(res));
    } else {
        printf("DEBUG: Name & Health update sent to API\n");
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
}


// Function to read health data based on selected character
void read_health_data(int *current, int *max, int *temp) {
    const char *file_path = is_guest_mode ? GUEST_DATA_FILE : HEALTH_DATA_FILE;

    FILE *file = fopen(file_path, "r");
    if (!file) {
        printf("ERROR: Cannot open %s!\n", file_path);
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

    if (is_guest_mode) {
        // 🚀 Read simple guest structure
        *current = cJSON_GetObjectItem(json, "current_hp")->valueint;
        *max = cJSON_GetObjectItem(json, "max_hp")->valueint;
        *temp = cJSON_GetObjectItem(json, "temp_hp")->valueint;
    } else {
        // 🚀 Read from structured health_data.json
        cJSON *profile = NULL;
        cJSON *characters = NULL;
        int profile_count = cJSON_GetArraySize(json);

        for (int i = 0; i < profile_count; i++) {
            profile = cJSON_GetArrayItem(json, i);
            cJSON *name = cJSON_GetObjectItem(profile, "name");

            if (name && strcmp(name->valuestring, selected_profile) == 0) {
                characters = cJSON_GetObjectItem(profile, "characters");
                break;
            }
        }

        if (!characters) {
            printf("ERROR: Profile '%s' not found!\n", selected_profile);
            cJSON_Delete(json);
            return;
        }

        cJSON *character = NULL;
        int char_count = cJSON_GetArraySize(characters);

        for (int j = 0; j < char_count; j++) {
            character = cJSON_GetArrayItem(characters, j);
            cJSON *char_name = cJSON_GetObjectItem(character, "name");

            if (char_name && strcmp(char_name->valuestring, selected_character) == 0) {
                *current = cJSON_GetObjectItem(character, "current_hp")->valueint;
                *max = cJSON_GetObjectItem(character, "max_hp")->valueint;
                *temp = cJSON_GetObjectItem(character, "temp_hp")->valueint;
                break;
            }
        }
    }

    cJSON_Delete(json);
}

void write_health_data(int current, int max, int temp) {
    const char *file_path = is_guest_mode ? GUEST_DATA_FILE : HEALTH_DATA_FILE;

    if (is_guest_mode) {
        // 🚀 Guest mode: Write simple JSON structure
        cJSON *guest_json = cJSON_CreateObject();
        cJSON_AddStringToObject(guest_json, "name", "Guest");
        cJSON_AddNumberToObject(guest_json, "current_hp", current);
        cJSON_AddNumberToObject(guest_json, "max_hp", max);
        cJSON_AddNumberToObject(guest_json, "temp_hp", temp);

        char *guest_json_string = cJSON_Print(guest_json);
        FILE *guest_file = fopen(file_path, "w");

        if (guest_file) {
            fputs(guest_json_string, guest_file);
            fclose(guest_file);
        } else {
            printf("ERROR: Failed to write guest data!\n");
        }

        free(guest_json_string);
        cJSON_Delete(guest_json);
        return;
    }

    // 🚀 Normal mode: Modify health_data.json
    FILE *file = fopen(HEALTH_DATA_FILE, "r");
    if (!file) {
        printf("ERROR: Cannot open health data file for writing!\n");
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

    // 🚀 Find the selected profile
    cJSON *profile = NULL;
    cJSON *characters = NULL;
    int profile_count = cJSON_GetArraySize(json);

    for (int i = 0; i < profile_count; i++) {
        profile = cJSON_GetArrayItem(json, i);
        cJSON *name = cJSON_GetObjectItem(profile, "name");

        if (name && strcmp(name->valuestring, selected_profile) == 0) {
            characters = cJSON_GetObjectItem(profile, "characters");
            break;
        }
    }

    if (!characters) {
        printf("ERROR: Profile '%s' not found!\n", selected_profile);
        cJSON_Delete(json);
        return;
    }

    // 🚀 Find the selected character
    cJSON *character = NULL;
    int char_count = cJSON_GetArraySize(characters);

    for (int j = 0; j < char_count; j++) {
        character = cJSON_GetArrayItem(characters, j);
        cJSON *char_name = cJSON_GetObjectItem(character, "name");

        if (char_name && strcmp(char_name->valuestring, selected_character) == 0) {
            cJSON_ReplaceItemInObject(character, "current_hp", cJSON_CreateNumber(current));
            cJSON_ReplaceItemInObject(character, "max_hp", cJSON_CreateNumber(max));
            cJSON_ReplaceItemInObject(character, "temp_hp", cJSON_CreateNumber(temp));
            break;
        }
    }

    if (!character) {
        printf("ERROR: Character '%s' not found!\n", selected_character);
    }

    // Save changes locally
    char *updated_json_string = cJSON_Print(json);
    file = fopen(HEALTH_DATA_FILE, "w");
    if (file) {
        fputs(updated_json_string, file);
        fclose(file);
    } else {
        printf("ERROR: Failed to write updated health data!\n");
    }

    free(updated_json_string);
    cJSON_Delete(json);

    // 🚀 Background API update
    send_health_update(selected_character, current, max, temp);
}


// Function to update the UI health display
void update_health_display() {
    int current_health, max_health, temp_health;
    char character_name[50];

    if (is_guest_mode) {
        // 🚀 Read guest name from guest_data.json
        FILE *guest_file = fopen(GUEST_DATA_FILE, "r");
        if (guest_file) {
            char buffer[256];
            fread(buffer, 1, sizeof(buffer), guest_file);
            fclose(guest_file);

            cJSON *guest_json = cJSON_Parse(buffer);
            if (guest_json) {
                strcpy(character_name, cJSON_GetObjectItem(guest_json, "name")->valuestring);
                cJSON_Delete(guest_json);
            } else {
                strcpy(character_name, "Guest");
            }
        } else {
            strcpy(character_name, "Guest");
        }
    } else {
        strcpy(character_name, selected_character);
    }

    read_health_data(&current_health, &max_health, &temp_health);

    char current_hp_text[10], max_hp_text[10], temp_hp_text[10];
    snprintf(current_hp_text, sizeof(current_hp_text), "%d", current_health);
    snprintf(max_hp_text, sizeof(max_hp_text), "%d", max_health);
    snprintf(temp_hp_text, sizeof(temp_hp_text), "%d", temp_health);

    lv_textarea_set_text(ui_CHInput, current_hp_text);
    lv_textarea_set_text(ui_CHInputDisplay, current_hp_text);
    lv_textarea_set_text(ui_MHInput, max_hp_text);
    lv_textarea_set_text(ui_MHInputDisplay, max_hp_text);
    lv_textarea_set_text(ui_TextArea1, temp_hp_text);
    lv_textarea_set_text(ui_TempInputDisplay, temp_hp_text);

    // 🚀 Update Name Panel & NameBoard
    lv_textarea_set_text(ui_CharName, character_name);
    lv_textarea_set_text(ui_NameBoardDisplay, character_name);
}


void set_character_name(const char *new_name) {
    if (is_guest_mode) {
        printf("WARNING: Guest name cannot be changed!\n");
        return;
    }

    const char *file_path = is_guest_mode ? GUEST_DATA_FILE : HEALTH_DATA_FILE;
    FILE *file = fopen(file_path, "r");
    if (!file) {
        printf("ERROR: Cannot open %s for reading!\n", file_path);
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

    if (is_guest_mode) {
        cJSON_ReplaceItemInObject(json, "name", cJSON_CreateString(new_name));
    } else {
        // ✅ Find profile by ID
        cJSON *profile = NULL;
        cJSON *characters = NULL;
        int profile_count = cJSON_GetArraySize(json);

        for (int i = 0; i < profile_count; i++) {
            profile = cJSON_GetArrayItem(json, i);
            cJSON *profile_id_item = cJSON_GetObjectItem(profile, "id");

            if (profile_id_item && profile_id_item->valueint == get_profile_id(selected_profile)) {
                characters = cJSON_GetObjectItem(profile, "characters");
                break;
            }
        }

        if (!characters) {
            printf("ERROR: Profile '%s' not found!\n", selected_profile);
            cJSON_Delete(json);
            return;
        }

        // ✅ Find character by ID
        cJSON *character = NULL;
        int char_count = cJSON_GetArraySize(characters);

        for (int j = 0; j < char_count; j++) {
            character = cJSON_GetArrayItem(characters, j);
            cJSON *char_id = cJSON_GetObjectItem(character, "id");

            if (char_id && char_id->valueint == selected_character_id) {
                cJSON_ReplaceItemInObject(character, "name", cJSON_CreateString(new_name));
                break;
            }
        }
    }

    // ✅ Save updated JSON to file
    char *updated_json = cJSON_Print(json);
    file = fopen(file_path, "w");
    if (file) {
        fputs(updated_json, file);
        fclose(file);
    } else {
        printf("ERROR: Failed to write health data!\n");
    }

    free(updated_json);
    cJSON_Delete(json);

    // ✅ Update UI and send API update
    strcpy(selected_character, new_name);
    lv_textarea_set_text(ui_CharName, new_name);
    lv_textarea_set_text(ui_NameBoardDisplay, new_name);

    // ✅ Send updated name to the API
    int current_health, max_health, temp_health;
    read_health_data(&current_health, &max_health, &temp_health);
    send_health_update(new_name, current_health, max_health, temp_health);
}




// Function to set max health
void set_max_health(int value) {
    if (value <= 0) return;

    int current_health, max_health, temp_health;
    read_health_data(&current_health, &max_health, &temp_health);

    printf("DEBUG: Old Max Health: %d, New Max Health: %d\n", max_health, value);
    
    max_health = value;
    if (current_health > max_health) current_health = max_health;

    write_health_data(current_health, max_health, temp_health);
    update_health_display();
}

// Function to modify health (handles healing and damage)
void modify_health(int amount) {
    int current_health, max_health, temp_health;
    read_health_data(&current_health, &max_health, &temp_health);

    if (amount > 0) { 
        // Healing
        current_health += amount;
        if (current_health > max_health) current_health = max_health;
    } else { 
        // Damage (temp HP absorbs first)
        int damage = -amount;
        if (temp_health > 0) {
            if (damage >= temp_health) {
                damage -= temp_health;
                temp_health = 0;
            } else {
                temp_health -= damage;
                damage = 0;
            }
        }
        current_health -= damage;
        if (current_health < 0) current_health = 0;
    }

    write_health_data(current_health, max_health, temp_health);
    update_health_display();
}

// Function to update temp health
void set_temp_health(int value) {
    int current_health, max_health, temp_health;
    read_health_data(&current_health, &max_health, &temp_health);

    temp_health = value < 0 ? 0 : value;

    write_health_data(current_health, max_health, temp_health);
    update_health_display();
}


void enable_guest_mode(lv_event_t *e) {
    is_guest_mode = 1;
    printf("DEBUG: Guest Mode Enabled. Loading guest data...\n");

    // 🚀 Reset guest data at startup
    FILE *guest_file = fopen(GUEST_DATA_FILE, "w");
    if (guest_file) {
        fprintf(guest_file, "{\"name\":\"Guest\", \"current_hp\":0, \"max_hp\":0, \"temp_hp\":0}");
        fclose(guest_file);
    }

    update_health_display();
}


//______________CALLBACK SECTION_____________________________________

// Event callback for Heal Button
void heal_button_event_cb(lv_event_t *e) {
    printf("DEBUG: Heal Button Clicked\n");
    fflush(stdout);

    int heal_amount = atoi(lv_textarea_get_text(ui_InputArea));
    modify_health(heal_amount);
}

// Event callback for Damage Button
void damage_button_event_cb(lv_event_t *e) {
    printf("DEBUG: Damage Button Clicked\n");
    fflush(stdout);

    int damage_amount = atoi(lv_textarea_get_text(ui_InputArea));
    modify_health(-damage_amount);
}

// Callback for max health update (Triggered when exiting the sub-screen)
void max_health_input_cb(lv_event_t *e) {
    const char *text = lv_textarea_get_text(ui_MHInputDisplay);
    int new_max_health = atoi(text);

    if (new_max_health > 0) {
        set_max_health(new_max_health);
    }

    update_health_display();
}

// Callback for current health update
void current_health_input_cb(lv_event_t *e) {
    int current_health, max_health, temp_health;
    read_health_data(&current_health, &max_health, &temp_health);

    int new_current_health = atoi(lv_textarea_get_text(ui_CHInputDisplay));
    modify_health(new_current_health - current_health);

    update_health_display();
}

// Callback for temp health update
void temp_health_input_cb(lv_event_t *e) {
    int new_temp_health = atoi(lv_textarea_get_text(ui_TempInputDisplay));
    set_temp_health(new_temp_health);

    update_health_display();
}

// Callback for character name update
void character_name_input_cb(lv_event_t *e) {
    int current_health, max_health, temp_health;
    read_health_data(&current_health, &max_health, &temp_health);

    if (is_guest_mode) {
        printf("WARNING: Guest name cannot be changed!\n");
        return;
    }

    const char *new_name = lv_textarea_get_text(ui_NameBoardDisplay);

    if (strlen(new_name) > 0) {
        set_character_name(new_name);
        write_health_data(current_health, max_health, temp_health);
        update_health_display(); 
    }
}


// Callback for Back Button
void back_button_event_cb(lv_event_t *e) {
    printf("DEBUG: Back Button Pressed\n");
    int profile_id = get_profile_id(selected_profile);

    if (is_guest_mode) {
        printf("DEBUG: Exiting Guest Mode...\n");
        is_guest_mode = 0; // 🚀 Disable guest mode
        
        // Reset selected profile and character
        strcpy(selected_profile, "");
        strcpy(selected_character, "");

        // 🚀 Reload Profiles & Characters (reset dropdowns)
        load_profiles();
        
        // 🚀 Auto-select first profile & character
        lv_dropdown_set_selected(ui_Dropdown1, 0);
        on_profile_selected(NULL);  // Simulate profile selection

        lv_dropdown_set_selected(ui_Dropdown2, 0);
        on_character_selected(NULL);  // Simulate character selection

        // Navigate back to the Welcome Screen
        _ui_screen_change(&ui_WelcomeScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, &ui_WelcomeScreen_screen_init);
    } else {

        load_characters(selected_profile);
        
        lv_dropdown_set_selected(ui_Dropdown2, 0);
        on_character_selected(NULL);  // Simulate character selection
        // 🚀 Normal mode: Return to Character Selection
        _ui_screen_change(&ui_CharChoiceScreen, LV_SCR_LOAD_ANIM_MOVE_RIGHT, 200, 0, &ui_CharChoiceScreen_screen_init);
    }
}



//________________________________________

void setup_health_screen_events() {
    printf("DEBUG: Attaching Events...\n");

    // 🚀 Attach Guest Button Event
    lv_obj_add_event_cb(ui_GuestChoice, enable_guest_mode, LV_EVENT_CLICKED, NULL);

    // 🚀 Attach Back Button Event
    lv_obj_add_event_cb(ui_Back, back_button_event_cb, LV_EVENT_CLICKED, NULL);

    // Attach heal and damage button events
    lv_obj_add_event_cb(ui_HealButton, heal_button_event_cb, LV_EVENT_CLICKED, NULL);
    lv_obj_add_event_cb(ui_DamageButton, damage_button_event_cb, LV_EVENT_CLICKED, NULL);

    // Attach numpad exit events to properly save values
    lv_obj_add_event_cb(ui_MHInputDisplay, max_health_input_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(ui_CHInputDisplay, current_health_input_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(ui_TempInputDisplay, temp_health_input_cb, LV_EVENT_READY, NULL);

    // Close sub-screens when pressing ENTER on the numpad
    lv_obj_add_event_cb(ui_MHPad, max_health_input_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(ui_CHPad, current_health_input_cb, LV_EVENT_READY, NULL);
    lv_obj_add_event_cb(ui_TempPad, temp_health_input_cb, LV_EVENT_READY, NULL);

    // 🚀 Attach callback for name input field
    lv_obj_add_event_cb(ui_NameBoardDisplay, character_name_input_cb, LV_EVENT_READY, NULL);

    printf("DEBUG: Event Callbacks Attached Successfully!\n");
}