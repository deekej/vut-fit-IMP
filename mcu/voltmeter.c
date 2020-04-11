/**
 * File:          voltmeter.c
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
 * Description:   Module containing functions for measuring voltage from ADC of
 *                FITkit. The measured voltage is in tenths of mV, ranging from
 *                0 to 5 Volts.
 *
 * More info @:   https://www.fit.vutbr.cz/study/courses/index.php?id=9368
 */


/* ************************************************************************** *
 * ***[ START OF VOLTMETER.C ]*********************************************** *
 * ************************************************************************** */


/* ************************************************************************** *
 ~ ~~~[ HEADER FILES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

#include <msp430x16x.h>
#include <stdlib.h>

#include "voltmeter.h"


/* ************************************************************************** *
 ~ ~~~[ MODULE CONSTANTS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

#define ADC_HALF_RANGE      2048
#define ADC_MAX_VALUE       4095
#define SWITCH_VALUE        2457
#define REFER_VOLTAGE       250

/* ************************************************************************** *
 ~ ~~~[ GLOBAL VARIABLES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

char Refer_Voltage;


/* ************************************************************************** *
 ~ ~~~[ LOCAL FUNCTIONAL PROTOTYPES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

static unsigned long measure_voltage(void);


/* ************************************************************************** *
 ~ ~~~[ PUBLIC FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

/** 
 * Function which returns measured voltage from ADC in milivolts.
 */
unsigned long get_voltage(void)
{{{
  unsigned long raw_voltage;

  Refer_Voltage = REFER_HIGH;
  raw_voltage = measure_voltage();

  if (raw_voltage < SWITCH_VALUE) {
    Refer_Voltage = REFER_LOW;
    raw_voltage = measure_voltage();
  }
  else if (raw_voltage == ADC_MAX_VALUE) {
    return MAX_VOLTAGE;
  }

  return (raw_voltage * REFER_VOLTAGE) / 2048;
}}}


/* ************************************************************************** *
 ~ ~~~[ LOCAL AUXILIARY FUNCTIONS ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

/**
 * Function to measure raw value from ADC, which it returns. Measuring accuracy
 * depends on global variable Refer_Voltage.
 */
static unsigned long measure_voltage(void)
{{{
  unsigned long ret_val;


  /* Setting reference voltage to high if required. */
  if (Refer_Voltage == REFER_HIGH) {
    ADC12CTL0 = REF2_5V;
  }
  else {
    ADC12CTL0 = ~REF2_5V;
  }

  P6SEL = 0x3;                    /* This allows more stable measuring. */

  /* 
   * Setting CONTROL register #0:
   * ADC turned on, reference voltage LOW, multiple sample & conversion.
   */
  ADC12CTL0 |= ADC12ON | SHT0_15 | MSH | REFON;

  /*
   * Setting CONTROL register #1:
   * Source clock MCLK, clock dividing by 8, conversion start address 0140h,
   * conversion mode SEQUENCE-OF-CHANNELS, sample-and-hold from internal timer.
   */
  ADC12CTL1 = ADC12SSEL_2 | ADC12DIV_7 | CSTARTADD_0 | CONSEQ_1 | SHP;


  /*
   * Getting values from ADC and storing them to 8 ADC registers. Selected input
   * is A0 - INCH_2. Reference voltages: VR+ = V_REF+ & V_R-= AV_ss.
   */
  ADC12MCTL0 = SREF_1 | INCH_2;
  ADC12MCTL1 = SREF_1 | INCH_2;
  ADC12MCTL2 = SREF_1 | INCH_2;
  ADC12MCTL3 = SREF_1 | INCH_2;
  ADC12MCTL4 = SREF_1 | INCH_2;
  ADC12MCTL5 = SREF_1 | INCH_2;
  ADC12MCTL6 = SREF_1 | INCH_2;
  ADC12MCTL7 = SREF_1 | INCH_2 | EOS; /* End of conversion sequence. */


  ADC12CTL0 |= ENC;                   /* Enabling conversion. */
  ADC12CTL0 |= ADC12SC;               /* Sampling & converting. */

  while (ADC12CTL0 & ADC12BUSY);      /* Waiting for conversion to complete. */

  ADC12CTL0 &= ~(ENC + ADC12ON);      /* Disabling conversion & ADC. */


  /* Arithmetic mean of 8 recorded values. */
  ret_val = ADC12MEM0 + ADC12MEM1 + ADC12MEM2 + ADC12MEM3;
  ret_val += ADC12MEM4 + ADC12MEM5 + ADC12MEM6 + ADC12MEM7;

  return (ret_val >> 3);
}}}


/* ************************************************************************** *
 * ***[ END OF VOLTMETER.H ]************************************************* *
 * ************************************************************************** */

