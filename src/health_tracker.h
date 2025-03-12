#ifndef HEALTH_TRACKER_H
#define HEALTH_TRACKER_H

void init_health_tracker();  

void setup_health_screen_events();
void update_health_display();

void set_max_health(int value);
void modify_health(int amount);
void set_temp_health(int value);
void update_health_from_ui();

// JSON Read/Write Functions
void read_health_data(int *current, int *max, int *temp);
void write_health_data(int current, int max, int temp);

#endif 
