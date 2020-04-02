#include "button.h"

const byte pin_button_1 = 35;
const byte pin_button_2 = 34;
const byte pin_button_3 = 39;
const byte pin_button_4 = 36;

Button button_1(pin_button_1);
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

bool Button::pressed()
{
    _reading = digitalRead(_button); // get current button state.
    if (_reading != _lastState)
    {                     // detect edge: current vs last state:
        _last = millis(); // store millis if change was detected.
        _wait = true;     // Just to avoid calling millis() unnecessarily.
    }

    if (_wait && (millis() - _last) > _delay)
    { // after the delay has passed:
        if (_reading != _state)
        {                      // if the change wasn't stored yet:
            _count++;          // Stores each change.
            _state = _reading; // store the button state change.
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
    return _count / 2;
}

void Button::resetCount()
{
    _count = 0;
    return;
}
