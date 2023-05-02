#ifndef AIR101_LCD_NEW_H
#define AIR101_LCD_NEW_H

#include "options.h"

void tft_setup();

void joystick_loop();
void tft_loop();

void show_freq(uint16_t freq, const char *suffix);

#endif // AIR101_LCD_NEW_H