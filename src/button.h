#ifndef BUTTON_H
#define BUTTON_H
#include <Arduino.h>

class Button
{
private:
    byte _button, _state, _lastState, _reading;
    unsigned int _count;
    unsigned long _delay, _last;
    boolean _wait;
    bool _1st_press;

public:
    Button(byte button_pin);
    Button(byte button_pin, unsigned long delay);
    void Button_u(byte &button_pin);

    bool pressed();       // returns the debounced button state: LOW or HIGH.
    unsigned int count(); // Returns the number of times the button was pressed.
    void resetCount();    // Resets the button count number.
};

extern Button button_1;
extern Button button_2;
extern Button button_3;
extern Button button_4;

#endif