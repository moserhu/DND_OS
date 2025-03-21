// This file was generated by SquareLine Studio
// SquareLine Studio version: SquareLine Studio 1.5.0
// LVGL version: 9.1.0
// Project name: DND_Screen

#include "../ui.h"

void ui_CharChoiceScreen_screen_init(void)
{
ui_CharChoiceScreen = lv_obj_create(NULL);
lv_obj_remove_flag( ui_CharChoiceScreen, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_image_src( ui_CharChoiceScreen, &ui_img_wood_paneling_png, LV_PART_MAIN | LV_STATE_DEFAULT );

ui_Container10 = lv_obj_create(ui_CharChoiceScreen);
lv_obj_remove_style_all(ui_Container10);
lv_obj_set_width( ui_Container10, 600);
lv_obj_set_height( ui_Container10, 440);
lv_obj_set_align( ui_Container10, LV_ALIGN_CENTER );
lv_obj_remove_flag( ui_Container10, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_radius(ui_Container10, 50, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_bg_color(ui_Container10, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Container10, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Dropdown2 = lv_dropdown_create(ui_Container10);
lv_dropdown_set_options( ui_Dropdown2, "Option 1\nOption 2\nOption 3" );
lv_obj_set_width( ui_Dropdown2, 400);
lv_obj_set_height( ui_Dropdown2, 50);
lv_obj_set_x( ui_Dropdown2, 0 );
lv_obj_set_y( ui_Dropdown2, 20 );
lv_obj_set_align( ui_Dropdown2, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Dropdown2, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_set_style_border_color(ui_Dropdown2, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_border_opa(ui_Dropdown2, 255, LV_PART_MAIN| LV_STATE_DEFAULT);



ui_ChooseCharacter = lv_label_create(ui_Container10);
lv_obj_set_width( ui_ChooseCharacter, lv_pct(60));
lv_obj_set_height( ui_ChooseCharacter, lv_pct(25));
lv_obj_set_x( ui_ChooseCharacter, 0 );
lv_obj_set_y( ui_ChooseCharacter, -120 );
lv_obj_set_align( ui_ChooseCharacter, LV_ALIGN_CENTER );
lv_label_set_text(ui_ChooseCharacter,"Choose Your Character");
lv_obj_set_style_text_align(ui_ChooseCharacter, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_text_font(ui_ChooseCharacter, &lv_font_montserrat_48, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_BackChar = lv_button_create(ui_Container10);
lv_obj_set_width( ui_BackChar, 180);
lv_obj_set_height( ui_BackChar, 50);
lv_obj_set_x( ui_BackChar, -160 );
lv_obj_set_y( ui_BackChar, 120 );
lv_obj_set_align( ui_BackChar, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_BackChar, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_remove_flag( ui_BackChar, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_BackChar, lv_color_hex(0xC72929), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_BackChar, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Label9 = lv_label_create(ui_BackChar);
lv_obj_set_width( ui_Label9, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Label9, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_Label9, LV_ALIGN_CENTER );
lv_label_set_text(ui_Label9,"Back");
lv_obj_set_style_text_font(ui_Label9, &lv_font_montserrat_20, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Button6 = lv_button_create(ui_Container10);
lv_obj_set_width( ui_Button6, 180);
lv_obj_set_height( ui_Button6, 50);
lv_obj_set_x( ui_Button6, 160 );
lv_obj_set_y( ui_Button6, 121 );
lv_obj_set_align( ui_Button6, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Button6, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_remove_flag( ui_Button6, LV_OBJ_FLAG_SCROLLABLE );    /// Flags

ui_AcceptChar = lv_label_create(ui_Button6);
lv_obj_set_width( ui_AcceptChar, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_AcceptChar, LV_SIZE_CONTENT);   /// 1
lv_obj_set_align( ui_AcceptChar, LV_ALIGN_CENTER );
lv_label_set_text(ui_AcceptChar,"Confirm");
lv_obj_set_style_text_font(ui_AcceptChar, &lv_font_montserrat_20, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Settings1 = lv_button_create(ui_Container10);
lv_obj_set_width( ui_Settings1, 50);
lv_obj_set_height( ui_Settings1, 50);
lv_obj_set_x( ui_Settings1, 0 );
lv_obj_set_y( ui_Settings1, 120 );
lv_obj_set_align( ui_Settings1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Settings1, LV_OBJ_FLAG_SCROLL_ON_FOCUS );   /// Flags
lv_obj_remove_flag( ui_Settings1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_obj_set_style_bg_color(ui_Settings1, lv_color_hex(0x4AAE4A), LV_PART_MAIN | LV_STATE_DEFAULT );
lv_obj_set_style_bg_opa(ui_Settings1, 255, LV_PART_MAIN| LV_STATE_DEFAULT);

ui_Gear1 = lv_image_create(ui_Settings1);
lv_image_set_src(ui_Gear1, &ui_img_plus_png);
lv_obj_set_width( ui_Gear1, LV_SIZE_CONTENT);  /// 1
lv_obj_set_height( ui_Gear1, LV_SIZE_CONTENT);   /// 1
lv_obj_set_x( ui_Gear1, 220 );
lv_obj_set_y( ui_Gear1, 221 );
lv_obj_set_align( ui_Gear1, LV_ALIGN_CENTER );
lv_obj_add_flag( ui_Gear1, LV_OBJ_FLAG_ADV_HITTEST );   /// Flags
lv_obj_remove_flag( ui_Gear1, LV_OBJ_FLAG_SCROLLABLE );    /// Flags
lv_image_set_scale(ui_Gear1,230);
lv_obj_set_style_image_recolor(ui_Gear1, lv_color_hex(0xF2E8E8), LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_image_recolor_opa(ui_Gear1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_min_width(ui_Gear1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_max_width(ui_Gear1, 480, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_min_height(ui_Gear1, 0, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_max_height(ui_Gear1, 480, LV_PART_MAIN| LV_STATE_DEFAULT);
lv_obj_set_style_transform_scale(ui_Gear1, 20, LV_PART_MAIN| LV_STATE_DEFAULT);

lv_obj_add_event_cb(ui_BackChar, ui_event_BackChar, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_Button6, ui_event_Button6, LV_EVENT_ALL, NULL);
lv_obj_add_event_cb(ui_Settings1, ui_event_Settings1, LV_EVENT_ALL, NULL);
uic_Image1 = ui_Gear1;

}
