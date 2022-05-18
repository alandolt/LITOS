/**
 * @file button.cpp
 * @author Alex Landolt
 * @brief Class definition file for Button class
 * @version 0.3
 * @date 2020-05-25
 *
 */
#include "button.h"

const byte pin_button_1 = 36;
const byte pin_button_2 = 39;
const byte pin_button_3 = 34;
const byte pin_button_4 = 35;

Button button_1(pin_button_1); /// Initialization of the buttons
Button button_2(pin_button_2);
Button button_3(pin_button_3);
Button button_4(pin_button_4);

Button::Button(byte button_pin)
{
    _delay = 50;
    Button_u(button_pin);
}

Button::Button(byte button_pin, unsigned long delay)
{
    _delay = delay;
    Button_u(button_pin);
}

void Button::Button_u(byte &button_pin)
{
    pinMode(button_pin, INPUT);
    _button = button_pin;
    _state = _lastState = _reading = digitalRead(_button);
    _last = millis();
    _count = 0;
    _1st_press = false;
}
/**
 * @brief Function that is used in every CPU cycle to check if button has been pressed
 *
 * @return true when button was pressed
 * @return false when button was not pressed
 */
bool Button::pressed()
{
    _reading = digitalRead(_button); // get current button state.
    if (_reading != _lastState)
    {                     // detect edge: current vs last state:
        _last = millis(); // store millis if change was detected.
        _wait = true;     // Just to avoid calling millis() unnecessarily.
    }

    if (_wait && (millis() - _last) > _delay)
    {                           // after the delay has passed:
        if (_reading != _state) // check again
        {
            _count++;
            _state = _reading; // if no change ==> button has been pressed
            _wait = false;
            if (_1st_press)
            {
                _1st_press = false;
                return true;
            }
            _1st_press = true;
        }
    }
    _lastState = _reading;
    return false;
}

unsigned int Button::count()
{
    Button::pressed();
    return _count / 2; /// :2, as up/down are marked as two presses
}

void Button::resetCount()
{
    _count = 0;
    return;
}
