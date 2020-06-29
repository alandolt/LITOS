/**
 * @file save_restore_config.cpp
 * @author Alex Landolt 
 * @brief C file for save_restore_config class
 * @version 0.3
 * @date 2020-05-26
 */
#include "save_restore_config.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <StreamUtils.h> /// used for buffering config json file during loading from SPIFFS

save_restore_config config("/set/config.json");

/**
 * @brief Construct a new save restore config::save restore config object
 * 
 * @param _config_file config file that is used for loading
 */
save_restore_config::save_restore_config(const char *_config_file)
{
    strcpy(config_file, _config_file);
}

void save_restore_config::load_configuration() /// called in setup to load configuration
{
    File file = SPIFFS.open(config_file);
    StaticJsonDocument<2000> doc;
    ReadBufferingStream bufferedFile(file, 64); /// read config file into buffer to be able to decode it faster
    deserializeJson(doc, bufferedFile);
    /// in this part the decoded variables from the config file are copied into RAM
    _config.port = doc["webserver"]["port"] | 80;
    strlcpy(_config.hostname, doc["webserver"]["hostname"] | "LIGHTOS", sizeof(_config.hostname));

    strlcpy(_config.ssid, doc["wlan_connect"]["ssid"], sizeof(_config.ssid));
    strlcpy(_config.wlan_password, doc["wlan_connect"]["wlan_password"], sizeof(_config.wlan_password));

    _config.is_AP = doc["AP_mode"]["is_AP"] | false;
    _config.AP_password_protected = doc["AP_mode"]["AP_password_protected"] | false;
    strlcpy(_config.AP_ssid, doc["AP_mode"]["AP_ssid"], sizeof(_config.AP_ssid));
    strlcpy(_config.AP_password, doc["AP_mode"]["AP_password"], sizeof(_config.AP_password));

    _config.is_EAP = doc["EAP_mode"]["is_EAP"] | false;
    strlcpy(_config.EAP_identity, doc["EAP_mode"]["EAP_identity"], sizeof(_config.EAP_identity));
    strlcpy(_config.EAP_password, doc["EAP_mode"]["EAP_password"], sizeof(_config.EAP_password));

    _config.two_wellplates = doc["wellplate_settings"]["two_wellplates"] | true;
    _config.last_wellplate_A = doc["wellplate_settings"]["last_wellplate_A"] | 1;
    _config.last_wellplate_B = doc["wellplate_settings"]["last_wellplate_B"] | 1;
    strlcpy(_config.last_config_file_A, doc["wellplate_settings"]["last_config_file_A"] | "/demo.csv", sizeof(_config.last_config_file_A));
    strlcpy(_config.last_config_file_B, doc["wellplate_settings"]["last_config_file_B"] | "/demo.csv", sizeof(_config.last_config_file_B));

    file.close();

    int file_length = strlen(_config.last_config_file_A);

    for (uint8_t i = 6; i <= file_length; i++)
    {
        char c = _config.last_config_file_A[i];
        _config.last_config_filename_A[i - 6] = c;
    }
    _config.last_config_filename_A[file_length - 10] = '\0';
    /**
     * in the config file only the filenames with the full path (SPIFFS does not have folder, path is encoded in filename) are stored
     * In some application (e.g. display name of the illumination pattern) we need however the real filename without path. 
     * So here we remove the path and the file extensio by copying the relevant bytes of the char array containing the filename 
     * with the path and extension to a second to obtain only the filename. 
     * 
     */

    file_length = strlen(_config.last_config_file_B);

    for (uint8_t i = 6; i <= file_length; i++)
    {
        char c = _config.last_config_file_B[i];
        _config.last_config_filename_B[i - 6] = c;
    }
    _config.last_config_filename_B[file_length - 10] = '\0';

    // calc_file_count_spiffs(); /// deprecated and not used anymore in v 0.3

    /**
     * Code used to create a char array in which the filename (path with extension) of all illumination patterns stored in SPIFFS  
     * is copyed to a huge char array in order to avoid interference betweeen SPIFFS and webserver (especially websocket) during
     * the run of a programm (probably not enough heap present or other quircky bug of SPIFFS)
     */
    _config.file_list[0] = '\0';
    File root_folder = SPIFFS.open("/conf");
    file = root_folder.openNextFile();
    while (file)
    {
        strcat(_config.file_list, file.name());
        strcat(_config.file_list, ",");
        file.close();
        file = root_folder.openNextFile();
    }
    file.close();
    root_folder.close();
    _config.file_list[strlen(_config.file_list)] = '\0';
    Serial.println(_config.file_list);
}

/**
 * @brief function of class save_restore_config to save config from RAM into JSON configuration file
 * 
 */
void save_restore_config::save_configuration()
{
    Serial.println("save invoked");
    SPIFFS.remove(config_file);

    File file = SPIFFS.open(config_file, FILE_WRITE);

    if (!file)
    {
        Serial.println(F("Failed to create file"));
        return;
    }
    StaticJsonDocument<2000> doc; /// I use static JSON to have more place in heap (which is used by SMARTMATRIX and Webserver)
    JsonObject webserver = doc.createNestedObject("webserver");
    JsonObject wlan_connect = doc.createNestedObject("wlan_connect");
    JsonObject AP_mode = doc.createNestedObject("AP_mode");
    JsonObject EAP_mode = doc.createNestedObject("EAP_mode");
    JsonObject wellplate_settings = doc.createNestedObject("wellplate_settings");

    webserver["port"] = _config.port;
    webserver["hostname"] = _config.hostname;

    wlan_connect["ssid"] = _config.ssid;
    wlan_connect["wlan_password"] = _config.wlan_password;

    AP_mode["is_AP"] = _config.is_AP;
    AP_mode["AP_password_protected"] = _config.AP_password_protected;
    AP_mode["AP_ssid"] = _config.AP_ssid;
    AP_mode["AP_password"] = _config.AP_password;

    EAP_mode["is_EAP"] = _config.is_EAP;
    EAP_mode["EAP_identity"] = _config.EAP_identity;
    EAP_mode["EAP_password"] = _config.EAP_password;

    wellplate_settings["two_wellplates"] = _config.two_wellplates;
    wellplate_settings["last_config_file_A"] = _config.last_config_file_A;
    wellplate_settings["last_wellplate_A"] = _config.last_wellplate_A;
    wellplate_settings["last_config_file_B"] = _config.last_config_file_B;
    wellplate_settings["last_wellplate_B"] = _config.last_wellplate_B;

    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }

    file.close();
}

void save_restore_config::calc_file_count_spiffs()
{
    int file_count = 0;
    File root = SPIFFS.open("/conf");
    File file = root.openNextFile();
    while (file)
    {
        file_count++;
        file = root.openNextFile();
    }

    _config.file_count_spiffs = file_count;
}

int save_restore_config::get_file_count_spiffs()
{
    return _config.file_count_spiffs;
}

void save_restore_config::set_is_AP(bool is_AP, bool update_config)
{
    _config.is_AP = is_AP;
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_ssid(const char *_ssid, bool update_config)
{
    strcpy(_config.ssid, _ssid);
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_wlan_password(const char *_wlan_password, bool update_config)
{
    strcpy(_config.wlan_password, _wlan_password);
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_hostname(const char *_hostname, bool update_config)
{
    strcpy(_config.hostname, _hostname);
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_is_EAP(bool is_EAP, bool update_config)
{
    _config.is_EAP = is_EAP;
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_EAP_identity(const char *EAP_identity, bool update_config)
{
    strcpy(_config.EAP_identity, EAP_identity);
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_EAP_password(const char *EAP_password, bool update_config)
{
    strcpy(_config.EAP_password, EAP_password);
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_AP_password(const char *AP_password, bool update_config)
{
    strcpy(_config.AP_password, AP_password);
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_AP_password_protected(const bool password_protected, bool update_config)
{
    _config.AP_password_protected = password_protected;
    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_AP_ssid(const char *AP_ssid, bool update_config)
{
    strcpy(_config.AP_ssid, AP_ssid);
    if (update_config)
    {
        save_configuration();
    }
}

void save_restore_config::set_last_config_file(const char *_last_config_file, const char identifier, bool update_config)
{
    if (identifier == 'A')
    {
        strcpy(_config.last_config_file_A, _last_config_file);
        int file_length = strlen(_config.last_config_file_A);
        for (uint8_t i = 6; i <= file_length; i++)
        {
            char c = _config.last_config_file_A[i];
            _config.last_config_filename_A[i - 6] = c;
        }
        _config.last_config_filename_A[file_length - 10] = '\0';
    }
    else if (identifier == 'B')
    {
        strcpy(_config.last_config_file_B, _last_config_file);
        int file_length = strlen(_config.last_config_file_B);
        for (uint8_t i = 6; i <= file_length; i++)
        {
            char c = _config.last_config_file_B[i];
            _config.last_config_filename_B[i - 6] = c;
        }
        _config.last_config_filename_B[file_length - 10] = '\0';
    }

    if (update_config)
    {
        save_configuration();
    }
}
void save_restore_config::set_last_wellplate(int last_wellplate, const char identifier, bool update_config)
{
    if (identifier == 'A')
    {
        _config.last_wellplate_A = last_wellplate;
    }
    else if (identifier == 'B')
    {
        _config.last_wellplate_B = last_wellplate;
    }
    if (update_config)
    {
        save_configuration();
    }
}

void save_restore_config::set_ip(const char *ip)
{
    strcpy(_config.ip, ip);
}

const char *save_restore_config::get_ssid()
{
    return _config.ssid;
}
const char *save_restore_config::get_wlan_password()
{
    return _config.wlan_password;
}

const char *save_restore_config::get_ip()
{
    return _config.ip;
}

const bool save_restore_config::get_is_AP()
{
    return _config.is_AP;
}
const bool save_restore_config::get_AP_password_protected()
{
    return _config.AP_password_protected;
}
const char *save_restore_config::get_AP_password()
{
    return _config.AP_password;
}
const char *save_restore_config::get_AP_ssid()
{
    return _config.AP_ssid;
}
const char *save_restore_config::get_hostname()
{
    return _config.hostname;
}
const bool save_restore_config::get_is_EAP()
{
    return _config.is_EAP;
}
const char *save_restore_config::get_EAP_identity()
{
    return _config.EAP_identity;
}
const char *save_restore_config::get_EAP_password()
{
    return _config.EAP_password;
}

const type_wellplate save_restore_config::get_last_wellplate(const char identifier)
{
    if (identifier == 'A')
    {
        return type_wellplate(_config.last_wellplate_A);
    }
    else if (identifier == 'B')
    {
        return type_wellplate(_config.last_wellplate_B);
    }
    return type_wellplate(_config.last_wellplate_A);
}
const char *save_restore_config::get_last_config_file(const char identifier)
{
    if (identifier == 'A')
    {
        return _config.last_config_file_A;
    }
    else if (identifier == 'B')
    {
        return _config.last_config_file_B;
    }
    return _config.last_config_file_A;
}

const char *save_restore_config::get_last_config_filename(const char identifier)
{
    if (identifier == 'A')
    {
        return _config.last_config_filename_A;
    }
    else if (identifier == 'B')
    {
        return _config.last_config_filename_B;
    }
    return _config.last_config_filename_A;
}

char *save_restore_config::get_file_list()
{
    return _config.file_list;
}

void save_restore_config::set_file_list(const char *new_file_list)
{
    strcpy(_config.file_list, new_file_list);
}

const bool save_restore_config::get_two_wellplates()
{
    return _config.two_wellplates;
}

void save_restore_config::set_two_wellplates(bool two_wellplates, bool update_config)
{
    _config.two_wellplates = two_wellplates;
    if (update_config)
    {
        save_configuration();
    }
}