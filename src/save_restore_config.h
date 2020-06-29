/**
 * @file save_restore_config.h
 * @author Alex Landolt 
 * @brief Header file for the sotrage and loading of JSON configuration files
 * @version 0.3
 * @date 2020-05-26
 * 
 * Internally all the data of LITOS is stored in JSON configuration files located in /set/config.json. 
 * This code is responsible for loading this file into a structure in RAM to avoid readings from SPIFFS during runtime. 
 * The whole code is condensed in a class, so that it is encapsulated and abstracted from the base code. 
 */
#ifndef SAVE_RESTORE_CONFIG_H
#define SAVE_RESTORE_CONFIG_H

#include <ArduinoJson.h>
#include "struct.h"

class save_restore_config
{
private:
    char config_file[35];
    typedef struct struct_config
    {
        char ssid[35];
        char wlan_password[35];
        char hostname[35];
        int port;

        bool is_AP;
        bool AP_password_protected;
        char AP_ssid[35];
        char AP_password[35];

        bool is_EAP; /// eduroam is an EAP network, so in this case EAP would be true
        char EAP_identity[40];
        char EAP_password[35];

        char ip[16]; /// current IP address

        char last_config_file_A[35];
        char last_config_filename_A[35];
        byte last_wellplate_A;

        char last_config_file_B[35];
        char last_config_filename_B[35];
        byte last_wellplate_B;

        bool two_wellplates;

        int file_count_spiffs; /// deprecated in v0.3
        char file_list[550];   /** to avoid interference with SMARTMATRIX library and the webserver, all the filenames of illumination patterns
                                located in SPIFFS are loaded into a char array at the start of program
                               **/
    } struct_config;

    struct_config _config;

public:
    save_restore_config(const char *_config_file); /// constructor
    void load_configuration();                     /// load configuration file from SPIFFS into structure in RAM
    void save_configuration();                     /// takes structure in RAM and creates configuration file in SPIFFS
    void calc_file_count_spiffs();                 /// function to calculate the amount of illumination patterns present in SPIFFS and store it in RAM

    /// setters to set variable from input to RAM, the bool update config is used when an update of the configuration file located in SPIFFS should be made
    void set_is_AP(bool is_AP, bool update_config);
    void set_ssid(const char *_ssid, bool update_config);
    void set_wlan_password(const char *_wlan_password, bool update_config);
    void set_hostname(const char *_hostname, bool update_config);

    void set_EAP_password(const char *EAP_password, bool update_config);
    void set_EAP_identity(const char *EAP_identity, bool update_config);
    void set_is_EAP(bool is_EAP, bool update_config);

    void set_ip(const char *ip);
    void set_port(int _port, bool update_config);
    void set_last_config_file(const char *_last_config_file, const char identifier, bool update_config = false);
    void set_last_wellplate(int last_wellplate, const char identifier, bool update_config = false);

    void set_AP_ssid(const char *AP_ssid, bool update_config = false);
    void set_AP_password(const char *AP_password, bool update_config = false);
    void set_AP_password_protected(const bool password_protected, bool update_config = false);

    void set_file_list(const char *file_list);

    void set_two_wellplates(bool two_wellplates, bool update_config = false);

    /// getters, to get the variables from RAM
    int get_file_count_spiffs();
    const bool get_two_wellplates();
    const bool get_is_AP();
    const char *get_ssid();
    const char *get_wlan_password();
    const char *get_ip();
    const char *get_hostname();
    const char *get_AP_ssid();
    const char *get_AP_password();
    const char *get_EAP_identity();
    const char *get_EAP_password();
    const bool get_is_EAP();
    const bool get_AP_password_protected();

    const type_wellplate get_last_wellplate(const char identifier); /// identifier is used to chose well plate (A/B)
    const char *get_last_config_file(const char identifier);
    const char *get_last_config_filename(const char identifier);
    char *get_file_list();
};

extern save_restore_config config;

#endif