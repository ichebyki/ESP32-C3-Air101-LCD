#ifndef BLINK_H
#define BLINK_H

#include "options.h"

#ifndef NO_BLINK
void blink_setup();
void blink_loop();
#else
#define blink_setup() (void)0
#define blink_loop() (void)0
#endif

#endif // BLINK_H
