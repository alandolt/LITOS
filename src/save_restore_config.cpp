/**
 * @file save_restore_config.cpp
 * @author Alex Landolt 
 * @brief C file for save_restore_config class
 * @version 0.3
 * @date 2020-05-26
 */
#include "save_restore_config.h"
#include "version.h"
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
    if (SPIFFS.exists(config_file))
    {
        File file = SPIFFS.open(config_file);
        if (file && !file.isDirectory())
        {
            StaticJsonDocument<2000> doc;
            ReadBufferingStream bufferedFile(file, 64); /// read config file into buffer to be able to decode it faster
            deserializeJson(doc, bufferedFile);
            /// in this part the decoded variables from the config file are copied into RAM
            _config.port = doc["webserver"]["port"] | 80;
            _config.adv_set = doc["webserver"]["adv_set"] | false;
            strlcpy(_config.hostname, doc["webserver"]["hostname"] | "LITOS", sizeof(_config.hostname));
            _config.connection_mode = con_mode(doc["webserver"]["mode"] | 1);

            Serial.println(_config.connection_mode);
            Serial.println(_config.AP_ssid);

            strlcpy(_config.ssid, doc["WPA_mode"]["ssid"], sizeof(_config.ssid));
            strlcpy(_config.wlan_password, doc["WPA_mode"]["wlan_password"], sizeof(_config.wlan_password));

            _config.AP_password_protected = doc["AP_mode"]["AP_prot"] | false;
            strlcpy(_config.AP_ssid, doc["AP_mode"]["AP_ssid"], sizeof(_config.AP_ssid));
            strlcpy(_config.AP_password, doc["AP_mode"]["AP_password"], sizeof(_config.AP_password));

            strlcpy(_config.EAP_identity, doc["EAP_mode"]["EAP_identity"], sizeof(_config.EAP_identity));
            strlcpy(_config.EAP_password, doc["EAP_mode"]["EAP_password"], sizeof(_config.EAP_password));

            _config.two_wellplates = doc["wellplate_settings"]["two_wellplates"] | true;
            _config.last_wellplate_A = doc["wellplate_settings"]["last_wellplate_A"] | 1;
            _config.last_wellplate_B = doc["wellplate_settings"]["last_wellplate_B"] | 1;
            strlcpy(_config.last_config_file_A, doc["wellplate_settings"]["last_config_file_A"] | "/demo.csv", sizeof(_config.last_config_file_A));
            strlcpy(_config.last_config_file_B, doc["wellplate_settings"]["last_config_file_B"] | "/demo.csv", sizeof(_config.last_config_file_B));

            _config.global_corr_activated = doc["mat_cor"]["act"] | false;
            _config.matriz_correction_x = doc["mat_cor"]["x"] | 0;

            _config.mark_col.r = doc["mark_col"]["r"] | 255;
            _config.mark_col.g = doc["mark_col"]["g"] | 0;
            _config.mark_col.b = doc["mark_col"]["b"] | 0;

            file.close();

            int file_length = strlen(_config.last_config_file_A);

            for (uint8_t i = 6; i <= file_length; i++)
            {
                char c = _config.last_config_file_A[i];
                _config.last_config_filename_A[i - 6] = c;
            }
            _config.last_config_filename_A[16] = '\0';
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
            _config.last_config_filename_B[16] = '\0';
        }
        else
        {
            goto no_config_file;
        }
    }
    else
    {
    no_config_file:
        _config.port = 80;
        _config.adv_set = false;
        config.set_hostname("LITOS");
        _config.connection_mode = AP_mode;

        _config.global_corr_activated = false;
        _config.matriz_correction_x = 0;
        _config.matriz_correction_y = 0;
    }

    /**
     * Code used to create a char array in which the filename (path with extension) of all illumination patterns stored in SPIFFS  
     * is copyed to a huge char array in order to avoid interference betweeen SPIFFS and webserver (especially websocket) during
     * the run of a programm (probably not enough heap present or other quircky bug of SPIFFS)
     */
    _config.file_list[0] = '\0';
    File root_folder = SPIFFS.open("/conf");
    File file = root_folder.openNextFile();
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
    else
    {
        StaticJsonDocument<2000> doc; /// I use static JSON to have more place in heap (which is used by SMARTMATRIX and Webserver)
        JsonObject webserver = doc.createNestedObject("webserver");
        JsonObject wlan_connect = doc.createNestedObject("WPA_mode");
        JsonObject AP_mode = doc.createNestedObject("AP_mode");
        JsonObject EAP_mode = doc.createNestedObject("EAP_mode");
        JsonObject wellplate_settings = doc.createNestedObject("wellplate_settings");
        JsonObject matriz_global_correction = doc.createNestedObject("mat_cor");
        JsonObject mark_col = doc.createNestedObject("mark_col");

        webserver["port"] = _config.port;
        webserver["hostname"] = _config.hostname;
        webserver["mode"] = int(_config.connection_mode);
        webserver["adv_set"] = _config.adv_set;

        wlan_connect["ssid"] = _config.ssid;
        wlan_connect["wlan_password"] = _config.wlan_password;

        AP_mode["AP_prot"] = _config.AP_password_protected;
        AP_mode["AP_ssid"] = _config.AP_ssid;
        AP_mode["AP_password"] = _config.AP_password;

        EAP_mode["EAP_identity"] = _config.EAP_identity;
        EAP_mode["EAP_password"] = _config.EAP_password;

        wellplate_settings["two_wellplates"] = _config.two_wellplates;
        wellplate_settings["last_config_file_A"] = _config.last_config_file_A;
        wellplate_settings["last_wellplate_A"] = _config.last_wellplate_A;
        wellplate_settings["last_config_file_B"] = _config.last_config_file_B;
        wellplate_settings["last_wellplate_B"] = _config.last_wellplate_B;

        matriz_global_correction["act"] = _config.global_corr_activated;
        matriz_global_correction["x"] = _config.matriz_correction_x;
        matriz_global_correction["y"] = _config.matriz_correction_y;

        mark_col["r"] = _config.mark_col.r;
        mark_col["g"] = _config.mark_col.g;
        mark_col["b"] = _config.mark_col.b;

        WriteBufferingStream bufferedFile(file, 64);
        serializeJson(doc, bufferedFile);
        bufferedFile.flush();
    }
    file.close();
}

void save_restore_config::get_settings_web(String &buffer)
{
    DynamicJsonDocument doc(1000);
    doc["response"] = "get_settings";
    doc["vers"] = SOFTWARE_VERSION;

    JsonObject webserver = doc.createNestedObject("webserver");
    webserver["port"] = _config.port;
    webserver["hostname"] = _config.hostname;
    webserver["mode"] = int(_config.connection_mode);
    webserver["adv_set"] = _config.adv_set;

    if (_config.connection_mode == AP_mode)
    {
        JsonObject AP = doc.createNestedObject("AP_mode");
        AP["AP_prot"] = _config.AP_password_protected;
        AP["AP_ssid"] = _config.AP_ssid;
        AP["AP_password"] = _config.AP_password;
    }
    else if (_config.connection_mode == WPA_mode)
    {
        JsonObject WPA = doc.createNestedObject("WPA_mode");
        WPA["ssid"] = _config.ssid;
        WPA["wpa_password"] = _config.wlan_password;
    }
    else if (_config.connection_mode == EAP_mode)
    {
        JsonObject EAP = doc.createNestedObject("EAP_mode");
        EAP["EAP_identity"] = _config.EAP_identity;
        EAP["EAP_password"] = _config.EAP_password;
    }

    JsonObject matrix_corr = doc.createNestedObject("mat_cor");
    matrix_corr["act"] = _config.global_corr_activated;
    matrix_corr["x"] = _config.matriz_correction_x;
    matrix_corr["y"] = _config.matriz_correction_y;

    doc["mark_col"] = get_mark_col_web();

    serializeJson(doc, buffer);
    Serial.println(buffer);
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

void save_restore_config::set_port(int _port, bool update_config)
{
    _config.port = _port;
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
        _config.last_config_filename_A[16] = '\0';
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
        _config.last_config_filename_B[16] = '\0';
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
const int save_restore_config::get_port()
{
    return _config.port;
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
    return _config.connection_mode == AP_mode;
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
    return _config.connection_mode == EAP_mode;
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

void save_restore_config::set_global_correction(bool _activated, bool update_config)
{
    _config.global_corr_activated = _activated;
    if (update_config)
    {
        save_configuration();
    }
}

const bool save_restore_config::get_global_correction_activated()
{
    return _config.global_corr_activated;
}

const int save_restore_config::get_global_correction_x()
{
    return _config.matriz_correction_x;
}

void save_restore_config::set_global_correction_x(int x_correction, bool update_config)
{
    _config.matriz_correction_x = x_correction;
    if (update_config)
    {
        save_configuration();
    }
}

const int save_restore_config::get_global_correction_y()
{
    return _config.matriz_correction_y;
}

void save_restore_config::set_global_correction_y(int y_correction, bool update_config)
{
    _config.matriz_correction_y = y_correction;
    if (update_config)
    {
        save_configuration();
    }
}

const con_mode save_restore_config::get_con_mode()
{
    return _config.connection_mode;
}

void save_restore_config::set_con_mode(uint8_t _connection_mode, bool update_config)
{
    _config.connection_mode = con_mode(_connection_mode);
    if (update_config)
    {
        save_configuration();
    }
}

const bool save_restore_config::get_adv_set()
{
    return _config.adv_set;
}

void save_restore_config::set_adv_set(bool _adv_set, bool update_config)
{
    _config.adv_set = _adv_set;
    if (update_config)
    {
        save_configuration();
    }
}

void save_restore_config::set_mark_col(String color_string, bool update_config)
{
    char rgb_code[15];
    char *ptr;

    strcpy(rgb_code, &(color_string.c_str())[4]);
    ptr = strtok(rgb_code, ",");
    _config.mark_col.r = atoi(ptr);
    ptr = strtok(NULL, ",");
    _config.mark_col.g = atoi(ptr);
    ptr = strtok(NULL, ")");
    _config.mark_col.b = atoi(ptr);

    if (update_config)
    {
        save_configuration();
    }
}

save_restore_config::internal_col_ref save_restore_config::get_mark_col()
{
    return _config.mark_col;
}

String save_restore_config::get_mark_col_web()
{
    String buffer("rgb(");
    buffer = buffer + _config.mark_col.r + "," + _config.mark_col.g + "," + _config.mark_col.b + ")";
    return (buffer);
}