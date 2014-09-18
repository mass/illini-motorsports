/*
 * @file: ShiftLights.h
 *
 * @processor: PIC18F46K80
 * @compiler: Microchip C18
 * @author: Andrew Mass
 * @author: George Schwieters
 */
#ifndef MAIN_H
#define MAIN_H

#include "GenericTypeDefs.h"

#define INPUT  1
#define OUTPUT 0

#define BLINK_TIME 150
#define ET_ID      0x201L
#define OT_ID      0x200L
#define OP_ID      0x200L
#define RPM_ID     0x200L

#define OP_BYTE  4
#define OT_BYTE  6
#define ET_BYTE  0
#define RPM_BYTE 0

/*****************
 * Address Bytes *
 *****************/

#define TERM_LAT	LATCbits.LATC6

#define RED0   PORTAbits.RA0
#define GREEN0 PORTAbits.RA0
#define BLUE0  PORTAbits.RA0

#define RED1   PORTAbits.RA0
#define GREEN1 PORTAbits.RA0
#define BLUE1  PORTAbits.RA0

#define RED2   PORTAbits.RA0
#define GREEN2 PORTAbits.RA0
#define BLUE2  PORTAbits.RA0

#define RED3   PORTAbits.RA0
#define GREEN3 PORTAbits.RA0
#define BLUE3  PORTAbits.RA0

#define RED4   PORTAbits.RA0
#define GREEN4 PORTAbits.RA0
#define BLUE4  PORTAbits.RA0

/**************
 * Data Bytes *
 **************/

// Colors
#define NONE       0b000
#define RED        0b001
#define GREEN      0b010
#define BLUE       0b100
#define RED_GREEN  RED | GREEN
#define RED_BLUE   RED | BLUE
#define GREEN_BLUE GREEN | BLUE
#define RGB        RED | GREEN | BLUE

// Rev range
#define REV_RANGE_1     7500
#define REV_RANGE_2     8500
#define REV_RANGE_3     9500
#define REV_RANGE_4     10500
#define REV_RANGE_5     11000
#define REV_RANGE_6     11500
#define REV_RANGE_7     12000
#define REV_RANGE_8     12250
#define REV_RANGE_LIMIT 12500

// Custom color choices
#define REV_COLOR       GREEN_BLUE
#define REV_LIMIT_COLOR GREEN

/******************
 * Method Headers *
 ******************/

void high_isr(void);
void setLedToColor(unsigned char led, unsigned char color);
void set_all(unsigned char color);
void set_lights(unsigned char max);
void init_unused_pins();

#endif
