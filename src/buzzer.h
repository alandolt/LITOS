/**
 * @file buzzer.h
 * @author Alex Landolt 
 * @brief Header file for piezo Buzzer class. 
 * @version 0.3
 * @date 2020-05-25
 * 
 * As it the piezo buzzer must be activated asynchronous, I have written here a handler that triggers it by short commands from 
 * other parts of the code (invoke_beep). The function check beep is called in void loop of main and verify if a beep from 
 * the buzzer is needed.  
 * 
 */
#ifndef BUZZER_H
#define BUZZER_H

#include <Arduino.h>
/**
 * @brief enum that stores beeping Types
 * Active is when the buzzer is currently active, short beep will do a short beep for the next cpu cycle. 
 * 
 */
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
    void init_buzzer();                           /// initialize buzzer in void setup
    void invoke_beep(beep_type _beep);            /// can be called from other parts of LITOS code to start a beep
    void check_beep(unsigned long int &act_time); /// is executed in void loop of main to check if there is a beep that must be executed

private:
    unsigned int long time_activated;
    unsigned int beep_time;
    uint8_t buzzer_pin;
    beep_type beep;
    const int freq = 5000;    /// buzzer is controlled over PWM channel of ESP32, so we have to give a frequence at which the buzzer will beep
    const int ledChannel = 0; /// ESP32 has multiple PWM channels, we use channel 0
    const int resolution = 8; /// it is possible to have a better PWM resolution, however, for the button we do not need a high resolution
                              ///  => choose of low resolution to save processing power.
};

extern Buzzer buzzer; /// declaration of new buzzer object from buzzer class.

#endif