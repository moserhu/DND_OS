#ifndef HEALTH_TRACKER_H
#define HEALTH_TRACKER_H

//initialize function
void init_health_tracker();  

//setup all events for interacting with health screen
void setup_health_screen_events();

//update display on health screen
void update_health_display();

//set the max heatlh
void set_max_health(int value);

//modify heatlh
void modify_health(int amount);

//set the temporary health
void set_temp_health(int value);

// JSON Read/Write Functions
void read_health_data(int *current, int *max, int *temp);
void write_health_data(int current, int max, int temp);

#endif 
