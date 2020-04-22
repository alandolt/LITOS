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
        bool access_point;
        char ssid[33];
        char wlan_password[64];
        char hostname[64];
        int port;
        char last_config_file_A[35];
        char last_config_filename_A[35];
        byte last_wellplate_A;

        char last_config_file_B[35];
        char last_config_filename_B[35];
        byte last_wellplate_B;
    } struct_config;

    struct_config _config;

public:
    save_restore_config(const char *_config_file);
    void load_configuration();
    void save_configuration();

    void set_access_point(const char *_access_point, bool update_config);
    void set_ssid(const char *_ssid, bool update_config);
    void set_wlan_password(const char *_wlan_password, bool update_config);
    void set_hostname(const char *_hostname, bool update_config);
    void set_port(int _port, bool update_config);
    void set_last_config_file(const char *_last_config_file, const char identifier, bool update_config = false);
    void set_last_wellplate(int last_wellplate, const char identifier, bool update_config = false);

    const bool get_acess_point();
    const char *get_ssid();
    const type_wellplate get_last_wellplate(const char identifier);
    const char *get_last_config_file(const char identifier);
    const char *get_last_config_filename(const char identifier);
};

extern save_restore_config config;

#endif