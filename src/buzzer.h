#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>

enum beep_type : uint8_t
{
    INACTIVE,
    ACTIVE,
    SHORT_BEEP,
    LONG_BEEP
};

class Buzzer
{
public:
    Buzzer(uint8_t buzzer_pin);
    void init_buzzer();
    void invoke_beep(beep_type _beep);
    void check_beep(unsigned long int &act_time);

private:
    unsigned int long time_activated;
    unsigned int beep_time;
    uint8_t buzzer_pin;
    beep_type beep;
    const int freq = 5000;
    const int ledChannel = 0;
    const int resolution = 8;
};

extern Buzzer buzzer;

#endif