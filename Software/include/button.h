/**
 * @file button.h
 * @author Alex Landolt
 * @brief Header file for button class. Handles pushbutton with Software debounce through checking them multiple times before triggering an action.
 * @version 0.3
 * @date 2020-05-25
 */

#ifndef BUTTON_H
#define BUTTON_H

#define PIN_BUTTON_1 36
#define PIN_BUTTON_2 39
#define PIN_BUTTON_3 34
#define PIN_BUTTON_4 35

#ifdef LITOS_REV_2
#undef PIN_BUTTON_1
#define PIN_BUTTON_1 35
#undef PIN_BUTTON_2
#define PIN_BUTTON_2 34
#undef PIN_BUTTON_3
#define PIN_BUTTON_3 39
#undef PIN_BUTTON_4
#define PIN_BUTTON_4 36
#endif

#include <Arduino.h>

/**
 * @brief Class to register and handle push button press with Software debounce
 *
 */
class Button
{
private:
    byte _button, _state, _lastState, _reading;
    unsigned int _count;
    unsigned long _delay, _last;
    boolean _wait;
    bool _1st_press;
    void Button_u(byte &button_pin); /// underlaying function used for initialization

public:
    Button(byte button_pin);                      /// constructor to create Button object on specific pin of ESP32
    Button(byte button_pin, unsigned long delay); /// Button object with custom debounce delay

    bool pressed();       /// returns the debounced button state: LOW or HIGH.
    unsigned int count(); /// Returns the number of times the button was pressed.
    void resetCount();    /// Resets the button count number.
};

extern Button button_1; /// Declaration of Buttons 1-4 of LITOS
extern Button button_2;
extern Button button_3;
extern Button button_4;

#endif