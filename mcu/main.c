/**
 * File:          main.c
 * Version:       1.0.0.0
 * Start date:    14-12-2013
 * Last update:   16-12-2013
 *
 * Course:        IMP (winter semester, 2013)
 * Project:       FITkit micro controller based VOLTMETER.
 *
 * Author:        David Kaspar (aka Dee'Kej), 3BIT
 * 
 * Faculty:       Faculty of Information Technologies,
 *                Brno University of Technologies,
 *                Czech Republic
 *
 * E-mails:       xkaspa34@stud.fit.vutbr.cz
 *
 * Description:   Main module containing usage of voltage measuring functions &
 *                usage VGA monitor, which displays voltage history as simple
 *                oscilloscope. 
 *
 * More info @:   https://www.fit.vutbr.cz/study/courses/index.php?id=9368
 */


/* ************************************************************************** *
 * ***[ START OF MAIN.C ]**************************************************** *
 * ************************************************************************** */


/* ************************************************************************** *
 ~ ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

#include <fitkitlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <lcd/display.h>

#include "vga_block.h"
#include "voltmeter.h"


/* ************************************************************************** *
 ~ ~~~[ MODULE CONSTANTS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

#define OUTPUT_STR_LEN    9
#define VGA_BUFFER_SIZE   81      /* We need one more space to remember. */
#define VGA_WIDTH         80
#define VGA_WIDTH_MAX     79
#define VGA_HEIGHT        60
#define VGA_TOP           0
#define VGA_BOTTOM        60
#define VOLTAGE_NORM      100


/* ************************************************************************** *
 ~ ~~~[ DATA TYPES DEFINITIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

/**
 * Structure containing pixel value & color of one vertical line.
 */
struct line {
  unsigned char max_val;
  unsigned char color;
};

struct line vga_buffer[VGA_BUFFER_SIZE];    /* Buffer for storing history. */


/* ************************************************************************** *
 ~ ~~~[ FUNCTION DEFINITIONS REQUIRED BY FITKIT ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

/**
 * Displaying help for user in QDevKit.
 */
void print_user_help(void)
{{{
  term_send_str_crlf("FITkit VOLTMETER\r\nAuthor: xkaspa34@stud.fit.vutbr.cz");

  return;
}}}


/**
 * No user-commands processing. 
 */
unsigned char decode_user_cmd(char *cmd_ucase, char *cmd)
{{{
  return (CMD_UNKNOWN);
}}}


/**
 * Initial message for FITkit display.
 */
void fpga_initialized(void)
{{{
  LCD_init();
  LCD_write_string("Voltmeter by:   xkaspa34        ");

  VRAM_Init();

  return;
}}}


/* ************************************************************************** *
 ~ ~~~[ PRIMARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

/**
 * Procedure for redrawing vertical line on VGA based on current and old voltage
 * values.
 */
void redraw_line(struct line new, struct line old, short x_coord)
{{{
  short i;

  if (new.max_val == old.max_val) {
    return;                                       /* Same voltage */
  }
  else if (new.color != old.color) {

    /* Voltage colors are different, redrawing whole line. */
    for (i = (VGA_BOTTOM - 1); i >= (VGA_BOTTOM - new.max_val); i--) {
      VGA_SetPixelXY(x_coord, i, new.color);
    }
    
    /* Erasing the rest of line. */
    for ( ; i > VGA_TOP; i--) {
      VGA_SetPixelXY(x_coord, i, VGA_BLACK);
    }
  }
  else if (new.max_val < old.max_val) {

    /* Voltage is lower, erasing excess pixels. */
    for (i = (VGA_BOTTOM - old.max_val); i < (VGA_BOTTOM - new.max_val); i++) {
      VGA_SetPixelXY(x_coord, i, VGA_BLACK);
    }

    return;
  }
  else {

    /* New maximum value is higher than older one, drawing missing pixels. */
    for (i = (VGA_BOTTOM - new.max_val); i < (VGA_BOTTOM - old.max_val); i++) {
      VGA_SetPixelXY(x_coord, i, new.color);
    }
  }

  /* Setting RED pixel if we have reached the top of the screen. */
  if (new.max_val == VGA_HEIGHT) {
    VGA_SetPixelXY(x_coord, VGA_TOP, VGA_RED);
  }
  else {
    VGA_SetPixelXY(x_coord, VGA_TOP, VGA_BLACK);
  }

  return;
}}}


/* ************************************************************************** *
 ~ ~~~[ MAIN FUNCTION ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

int main(void)
{{{
  unsigned char output[OUTPUT_STR_LEN];       /* Buffer for output string. */
  unsigned short x_coord, act, oldest;        /* Auxiliary iterators. */
  unsigned long voltage;                      /* Converted voltage. */
  long i;                                     /* Another iterator. */


  /* Hardware init & watchdog stopping. */
  initialize_hardware();
  WDG_stop();


  /* Delay to show init message & VGA monitor to wake up. */
  delay_ms(3000);


  /* Initialization of rotary buffer. */
  vga_buffer[VGA_BUFFER_SIZE - 1].color = VGA_BLACK;
  vga_buffer[VGA_BUFFER_SIZE - 1].max_val = 0;


  /* Start of voltage displaying + initial filling of rotary buffer. */  
  for (act = 0; act < VGA_BUFFER_SIZE; act++) {
    set_led_d5(1);                    /* New cycle start indication. */
    voltage = get_voltage();


    /* Assigning proper color & setting D6 diode if needed. */
    if (Refer_Voltage == REFER_HIGH) {
      vga_buffer[act].color = VGA_ORANGE;
      set_led_d6(1);
    }
    else {
      vga_buffer[act].color = VGA_GREEN;
      set_led_d6(0);
    }
    

    /* Voltage to VGA pixel conversion. */
    vga_buffer[act].max_val = voltage * VGA_HEIGHT / MAX_VOLTAGE;


    /* Printing actual voltage to FITkit terminal. */
    sprintf(output, "%01lu.%.02lu [V]", voltage / VOLTAGE_NORM,
                                        voltage % VOLTAGE_NORM);
    LCD_write_string(output);
    

    /* Refreshing voltage history on VGA monitor. */
    for (x_coord = VGA_WIDTH_MAX, i = act; i >= 0; x_coord--, i--) {
      redraw_line(vga_buffer[i], vga_buffer[(i - 1) < 0 ? (VGA_BUFFER_SIZE - 1) : (i - 1)], x_coord);
    }

    set_led_d5(0);                    /* End of cycle indication. */
    delay_ms(100);
  }

  
  oldest = 0;             /* New starting position for rotary buffer. */

  
  /*
   * Rotary buffer is filled, endless loop can begin.
   */
  while (1) {
    set_led_d5(1);                    /* New cycle start indication. */
    voltage = get_voltage();


    /* Increasing rotary buffer iterators. */
    act = oldest;
    oldest = (oldest + 1) % VGA_BUFFER_SIZE;


    /* Assigning proper color & setting D6 diode if needed. */
    if (Refer_Voltage == REFER_HIGH) {
      vga_buffer[act].color = VGA_ORANGE;
      set_led_d6(1);
    }
    else {
      vga_buffer[act].color = VGA_GREEN;
      set_led_d6(0);
    }


    /* Voltage to VGA pixel conversion. */
    vga_buffer[act].max_val = voltage * VGA_HEIGHT / MAX_VOLTAGE;


    /* Printing actual voltage to FITkit terminal. */
    sprintf(output, "%01lu.%.02lu [V]", voltage / VOLTAGE_NORM,
                                        voltage % VOLTAGE_NORM);
    LCD_write_string(output);


    /* Refreshing the voltage history displayed on VGA. */
    if (act == (VGA_BUFFER_SIZE - 1)) {

      /* Special case of rotary buffer which can be redrawn as whole.. */
      for (x_coord = VGA_WIDTH_MAX, i = act; i > 0; x_coord--, i--) {
        redraw_line(vga_buffer[i], vga_buffer[i - 1], x_coord);
      }
    }
    else {
      /* 'Splitting' and redrawing the 2 parts of rotary buffer. */
      for (x_coord = VGA_WIDTH_MAX, i = act; i >= 0; x_coord--, i--) {
        redraw_line(vga_buffer[i], vga_buffer[(i - 1) < 0 ? (VGA_BUFFER_SIZE - 1) : (i - 1)], x_coord);
      }

      for (i = VGA_BUFFER_SIZE - 1; i > oldest; x_coord--, i--) {
        redraw_line(vga_buffer[i], vga_buffer[i - 1], x_coord);
      }
    }

    set_led_d5(0);                    /* End of cycle indication. */
    delay_ms(100);
  }
}}}


/* ************************************************************************** *
 * ***[ END OF MAIN.C ]****************************************************** *
 * ************************************************************************** */

