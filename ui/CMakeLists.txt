SET(SOURCES components/ui_comp_numpad.c
    components/ui_comp.c
    screens/ui_WelcomeScreen.c
    screens/ui_ProfileChoiceScreen.c
    screens/ui_CharChoiceScreen.c
    screens/ui_Health_Screen.c
    screens/ui_SettingScreen.c
    ui.c
    components/ui_comp_hook.c
    ui_helpers.c
    images/ui_img_wood_paneling_png.c
    images/ui_img_plus_png.c
    images/ui_img_278415715.c
    images/ui_img_arrow_png.c)

add_library(ui ${SOURCES})
