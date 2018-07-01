/**
 * ECU Header
 *
 * Processor:   PIC32MZ2048EFM100
 * Compiler:    Microchip XC32
 * Author:      Andrew Mass
 * Created:     2016-2017
 */
#pragma once

#include <sys/types.h>
#include "../FSAE.X/CAN.h"
#include "../FSAE.X/errno.h"
#include "../FSAE.X/FSAE_adc.h"
#include "../FSAE.X/FSAE_can.h"
#include "../FSAE.X/FSAE_config.h"
#include "../FSAE.X/FSAE_spi.h"

// Thresholds
#define RPM_ON_THRESHOLD 200.0 // rpm

// Timing constants (ms)
#define TEMP_SAMP_INTV     333
#define ADJ_SAMP_INTV      50
#define DIAG_SEND          1000

// Misc state definitions
#define ENG_ON (eng_rpm > RPM_ON_THRESHOLD)
#define CRANK_PERIODS 44

// Pin definitions

#define VR1_PORT  PORTCbits.RC2
#define VR1_TRIS  TRISCbits.TRISC2
#define VR1_ANSEL ANSELCbits.ANSC2
#define VR2_PORT  PORTCbits.RC3
#define VR2_TRIS  TRISCbits.TRISC3
#define VR2_ANSEL ANSELCbits.ANSC3

#define UDEG_SIG_LAT LATBbits.LATB6
#define UDEG_SIG_TRIS TRISBbits.TRISB6

#define INJ1_LAT LATBbits.LATB8
#define INJ2_LAT LATBbits.LATB9
#define INJ3_LAT LATBbits.LATB10
#define INJ4_LAT LATBbits.LATB11
#define INJ1_TRIS TRISBbits.TRISB8
#define INJ2_TRIS TRISBbits.TRISB9
#define INJ3_TRIS TRISBbits.TRISB10
#define INJ4_TRIS TRISBbits.TRISB11

#define IGN1_LAT LATBbits.LATB12
#define IGN2_LAT LATBbits.LATB13
#define IGN3_LAT LATBbits.LATB14
#define IGN4_LAT LATBbits.LATB15
#define IGN1_TRIS TRISBbits.TRISB12
#define IGN2_TRIS TRISBbits.TRISB13
#define IGN3_TRIS TRISBbits.TRISB14
#define IGN4_TRIS TRISBbits.TRISB15

#define ADC_ADJ1_TRIS    TRISDbits.TRISD14
#define ADC_ADJ1_ANSEL   ANSELDbits.ANSD14
#define ADC_ADJ1_CSS     ADCCSS2bits.CSS32
#define ADC_ADJ1_CHN     32
#define ADC_ADJ2_TRIS    TRISDbits.TRISD15
#define ADC_ADJ2_ANSEL   ANSELDbits.ANSD15
#define ADC_ADJ2_CSS     ADCCSS2bits.CSS33
#define ADC_ADJ2_CHN     33

#define INJ1_EN_MASK 0b00000000000000000000000000000001
#define INJ1_DS_MASK 0b00000000000000000000000000000010
#define INJ2_EN_MASK 0b00000000000000000000000000000100
#define INJ2_DS_MASK 0b00000000000000000000000000001000
#define INJ3_EN_MASK 0b00000000000000000000000000010000
#define INJ3_DS_MASK 0b00000000000000000000000000100000
#define INJ4_EN_MASK 0b00000000000000000000000001000000
#define INJ4_DS_MASK 0b00000000000000000000000010000000
#define IGN1_EN_MASK 0b00000000000000000000000100000000
#define IGN1_DS_MASK 0b00000000000000000000001000000000
#define IGN2_EN_MASK 0b00000000000000000000010000000000
#define IGN2_DS_MASK 0b00000000000000000000100000000000
#define IGN3_EN_MASK 0b00000000000000000001000000000000
#define IGN3_DS_MASK 0b00000000000000000010000000000000
#define IGN4_EN_MASK 0b00000000000000000100000000000000
#define IGN4_DS_MASK 0b00000000000000001000000000000000

#define ADD_DEG(deg, num) (deg) += (num); if ((deg) > 7200) { (deg) -= 7200; }

/**
 * Function definitions
 */

void main(void);

// Logic functions
void process_CAN_msg(CAN_message msg);
void kill_engine(uint16_t errno);
inline void check_event_mask();

// ADC sample functions
void sample_temp(void);
void sample_adj();

// CAN message sending functions
void send_diag_can(void);

// Utility functions
void init_adc_ecu(void);
void init_ic1();