/**
 * File:          voltmeter.h
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
 * Description:   Header file containing CONSTANTS & functional prototypes of
 *                voltmeter.c module.
 *
 * More info @:   https://www.fit.vutbr.cz/study/courses/index.php?id=9368
 */


/* ************************************************************************** *
 * ***[ START OF VOLTMETER.H ]*********************************************** *
 * ************************************************************************** */

/* Safety mechanism against multi-including of this header file. */
#ifndef VOLTMETER_H
#define VOLTMETER_H


/* ************************************************************************** *
 ~ ~~~[ GLOBAL CONSTANTS & VARIABLE]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

#define REFER_LOW       0     /* 1.5V reference voltage. */
#define REFER_HIGH      1     /* 2.5V reference voltage. */
#define MAX_VOLTAGE     500   /* Maximum voltage after conversion, in 10x mV. */

extern char Refer_Voltage;    /* Global variable for info of actual reference.*/


/* ************************************************************************** *
 ~ ~~~[ FUNCTIONAL PROTOTYPES ]~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ ~
 * ************************************************************************** */

unsigned long get_voltage(void);


/* ************************************************************************** *
 * ***[ END OF VOLTMETER.H ]************************************************* *
 * ************************************************************************** */

#endif

