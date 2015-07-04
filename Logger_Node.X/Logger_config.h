#ifndef LOGGER_CONFIG_H
#define LOGGER_CONFIG_H

#include <xc.h>
#include <sys/kmem.h>

#define OUTPUT 0
#define INPUT 1

void init_general(void);
void init_unused_pins(void);
void init_peripheral_modules(void);
void init_oscillator(void);
void init_can(void);
void init_timer1(void);

#endif /* LOGGER_CONFIG_H */
