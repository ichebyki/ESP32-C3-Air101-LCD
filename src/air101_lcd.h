#ifndef AIR101_LCD_NEW_H
#define AIR101_LCD_NEW_H

#include "options.h"

void tft_setup();

void joystick_loop();
void tft_loop();

bool show_freq(uint16_t freq, const char *suffix);
void show_radio_name(const char *name);

#endif // AIR101_LCD_NEW_H