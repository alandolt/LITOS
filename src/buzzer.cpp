#include "buzzer.h"
const uint8_t connected_buzzer_pin = 2;
Buzzer buzzer(2);

Buzzer::Buzzer(uint8_t _buzzer_pin)
{
    buzzer_pin = _buzzer_pin;
}

void Buzzer::init_buzzer()
{
    ledcSetup(ledChannel, freq, resolution);
    ledcAttachPin(buzzer_pin, ledChannel);
}

void Buzzer::invoke_beep(beep_type _beep)
{
    beep = _beep;
    switch (beep)
    {
    case SHORT_BEEP:
        beep_time = 750;

    default:
        beep_time = 500;
    }
}

void Buzzer::check_beep(unsigned long int &act_time)
{
    switch (beep)
    {
    case INACTIVE:
        break;
    case ACTIVE:
        if ((act_time - time_activated) >= beep_time)
        {
            ledcWriteTone(ledChannel, LOW);
            beep = INACTIVE;
        }
        break;

    case SHORT_BEEP:
        time_activated = act_time;
        ledcWriteTone(ledChannel, 2500);

        beep = ACTIVE;
    default:
        break;
    }
}