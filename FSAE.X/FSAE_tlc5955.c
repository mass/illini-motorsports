/**
 * FSAE Library TLC5955
 *
 * Processor: PIC32MZ2048EFM100
 * Compiler:  Microchip XC32
 * Author:    Andrew Mass
 * Created:   2016-2017
 */
#include "FSAE_tlc5955.h"

uint8_t pending_register[NUM_BYTES] = {0};
uint16_t bit_ctr = 0;
uint64_t current_colors[16] = {0};

//TODO: Why is this wrong
uint8_t led_mapping[16] = {4, 15, 6, 2, 5, 1, 10, 8, 14, 12, 7, 13, 9, 11, 0, 3};

/**
 * Initializes the TLC5955 module.
 */
void init_tlc5955(void) {
  // GSCLK PWM Signal
  PWM_TLC5955_TRIS = OUTPUT;
  PWM_TLC5955_LAT = 1;
  
  // Latch - SHFLAT
  LAT_TLC5955_TRIS = OUTPUT;
  LAT_TLC5955_LAT = 0;

  _tlc5955_init_spi();

  _tlc5955_write_control();
  _tlc5955_write_gs(0);
}

/**
 * Initializes the SPI2 communication bus for use with the TLC5955 chip.
 */
void _tlc5955_init_spi(void) {
  unlock_config();

  // Initialize SDO2 PPS pin
  CFGCONbits.IOLOCK = 0;
  TRISGbits.TRISG7 = OUTPUT;
  RPG7Rbits.RPG7R = 0b0110; // SDO2
  CFGCONbits.IOLOCK = 1;

  // Initialize SCK2
  TRISGbits.TRISG6 = OUTPUT; // SCK2

  // Disable interrupts
  IEC4bits.SPI2EIE = 0;
  IEC4bits.SPI2RXIE = 0;
  IEC4bits.SPI2TXIE = 0;

  // Disable SPI2 module
  SPI2CONbits.ON = 0;

  // Clear receive buffer
  uint32_t readVal = SPI2BUF;

  // Use standard buffer mode
  SPI2CONbits.ENHBUF = 0;

  /**
   * F_SCK = F_PBCLK2 / (2 * (SPI2BRG + 1))
   * F_SCK = 100Mhz / (2 * (4 + 1))
   * F_SCK = 10Mhz
   */

  // Set the baud rate (see above equation)
  SPI2BRG = 4;

  SPI2STATbits.SPIROV = 0;

  SPI2CONbits.MCLKSEL = 0; // Master Clock Enable bit (PBCLK2 is used by the Baud Rate Generator)
  SPI2CONbits.SIDL = 0;    // Stop in Idle Mode bit (Continue operation in Idle mode)
  SPI2CONbits.MODE32 = 0;  // 32/16-Bit Communication Select bits (8-bit)
  SPI2CONbits.MODE16 = 0;  // 32/16-Bit Communication Select bits (8-bit)
  SPI2CONbits.DISSDI = 0;
  SPI2CONbits.DISSDO = 0;
  SPI2CONbits.MSTEN = 1;   // Master Mode Enable bit (Master mode)
  SPI2CONbits.CKE = 1;     // SPI Clock Edge Select (Serial output data changes on transition from active clock state to idle clock state)
  SPI2CONbits.SMP = 0;     // SPI Data Input Sample Phase (Input data sampled at middle of output time)
  SPI2CONbits.CKP = 0;     // Clock Polarity Select (Idle state for clock is a low level)

  // Enable SPI2 module
  SPI2CONbits.ON = 1;

  lock_config();
}

/**
 * Writes default values to the TLC5955 control registers.
 */
void _tlc5955_write_control(void) {
  uint8_t i, j;

  bit_ctr = 0;
  for (i = 0; i < NUM_BYTES; i++) {
    pending_register[i] = 0;
  }

  _tlc5955_reg_append(7, 0x0); // Shift in 7 bits to account for 776 bit SPI shifts

  _tlc5955_reg_append(1, 0b1); // MSB indicates control register write
  _tlc5955_reg_append(8, 0x96); // More indication for control register write

  // Write DC registers
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 3; j++) {
      _tlc5955_reg_append(7, 0b1111111);
    }
  }

  // Write MC registers
  _tlc5955_reg_append(3, 0b100); // Red
  _tlc5955_reg_append(3, 0b100); // Green
  _tlc5955_reg_append(3, 0b100); // Blue

  // Write BC registers
  _tlc5955_reg_append(7, 0b1111111); // Red
  _tlc5955_reg_append(7, 0b1111111); // Green
  _tlc5955_reg_append(7, 0b1111111); // Blue

  // Write FC registers
  _tlc5955_reg_append(1, 0b1); // DSPRPT
  _tlc5955_reg_append(1, 0b0); // TMGRST
  _tlc5955_reg_append(1, 0b0); // RFRESH
  _tlc5955_reg_append(1, 0b1); // ESPWM
  _tlc5955_reg_append(1, 0b0); // LSDVLT

  _tlc5955_send_register();
}

/**
 * Writes default values to the TLC5955 GS (grayscale) registers.
 */
void _tlc5955_write_gs(uint8_t color_idx) {
  uint8_t i, j;

  bit_ctr = 0;
  for (i = 0; i < NUM_BYTES; i++) {
    pending_register[i] = 0;
  }

  _tlc5955_reg_append(7, 0x0); // Shift in 7 bits to account for 776 bit SPI shifts

  _tlc5955_reg_append(1, 0b0); // MSB indicates GS write

  for (i = 0; i < 16; i++) {
    for (j = 0; j < 3; j++) {
      if (j == color_idx) {
        _tlc5955_reg_append(8, 0xFF); // GS HW
        _tlc5955_reg_append(8, 0xFF); // GW LW
				current_colors[i] |= 0xFFFF << (8*j); // update current colors
      } else {
        _tlc5955_reg_append(8, 0x00); // GS HW
        _tlc5955_reg_append(8, 0x00); // GW LW
				current_colors[i] &= 0x0000 << (8*j); // update current colors
      }
    }
  }

  _tlc5955_send_register();
}

/**
 * Writes a color to a set of lights specified by onMap
 */
void _tlc5955_write_same_color(uint64_t color, uint16_t onMap) {
  uint8_t i, j;

  bit_ctr = 0;
  for (i = 0; i < NUM_BYTES; i++) {
    pending_register[i] = 0;
  }

  _tlc5955_reg_append(7, 0x0); // Shift in 7 bits to account for 776 bit SPI shifts

  _tlc5955_reg_append(1, 0b0); // MSB indicates GS write

  for (i = 0; i < 16; i++) {
		if(onMap&(1 << led_mapping[i])){
			current_colors[i] = color;
			for (j = 0; j < 3; j++) {
    	    _tlc5955_reg_append(8, 0xFF & (color >> (8*j+4))); // GS HW
    	    _tlc5955_reg_append(8, 0xFF & (color >> (8*j))); // GW LW
    	}
		} else {
			current_colors[i] = 0;
			// 16 bits of color for each if off
			_tlc5955_reg_append(8, 0x00); // zero bit
			_tlc5955_reg_append(8, 0x00); // zero bit
			_tlc5955_reg_append(8, 0x00); // zero bit
			_tlc5955_reg_append(8, 0x00); // zero bit
			_tlc5955_reg_append(8, 0x00); // zero bit
			_tlc5955_reg_append(8, 0x00); // zero bit
		}
  }

  _tlc5955_send_register();
}

/**
 * Writes a color to a set of lights specified by onMap
 */
void _tlc5955_write_colors(uint64_t * colors) {
  uint8_t i, j;

  bit_ctr = 0;
  for (i = 0; i < NUM_BYTES; i++) {
    pending_register[i] = 0;
  }

  _tlc5955_reg_append(7, 0x0); // Shift in 7 bits to account for 776 bit SPI shifts

  _tlc5955_reg_append(1, 0b0); // MSB indicates GS write

  for (i = 0; i < 16; i++) {
      // i = ? => colors[0];
      // i = 7 -> colors[1];
      //colors[1] = color;
      // colors[0] = color;]
      uint8_t idx = i + 1;
      if (idx == 16) { idx = 0; }
      uint64_t color = colors[led_mapping[i]];
      current_colors[i] = color;
      for (j = 0; j < 3; j++) {
	  _tlc5955_reg_append(8, 0xFF & ((uint8_t) (color >> (8*j+4)))); // GS HW
      _tlc5955_reg_append(8, 0xFF & ((uint8_t) (color >> (8*j)))); // GW LW
      }
  }

  _tlc5955_send_register();
}

/**
 * Appends <num_bits> of <data> to the pending_register array in preparation
 * for sending a register to the chip.
 *
 * TODO: Also make this less bad
 */
void _tlc5955_reg_append(uint8_t num_bits, uint8_t data) {
  uint8_t i;
  for (i = 0; i < num_bits; i++) {
    uint8_t bit = data & (1 << (num_bits-1-i)); // selects bits in reverse order
    uint8_t idx = (bit_ctr + i) >> 3; // faster divide by 8
    uint8_t pos = (bit_ctr + i) % 8;

    if (bit) {
      pending_register[idx] |= (1 << pos);
    } else {
      pending_register[idx] &= ~(1 << pos);
    }
  }

  bit_ctr += num_bits;
}

/**
 * Sends the pending_register array over the SPI bus as a continuous steam of
 * <NUM_BYTES> messages.
 */
void _tlc5955_send_register(void) {

  /*
   * Need to send data from MSbit to LSbit. Then, after correct number of
   * pulses, the control bit (bit 769) will have made it to the end of the
   * register.
   *
   * Then latch at the end.
   *
   * Also account for the fact that we're shifting more than 769 bits
   */
  uint8_t i;
  uint8_t resp;

  for (i = 0; i < NUM_BYTES; i++) {
    SPI2BUF = pending_register[i];
    while(!SPI2STATbits.SPIRBF);
    resp = SPI2BUF;
  }

  // Send latch pulse
  LAT_TLC5955_LAT = 1;
  for (i = 0; i < 6; i++);
  LAT_TLC5955_LAT = 0;
}
