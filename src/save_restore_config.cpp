#include "save_restore_config.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

save_restore_config config("/set/config.json");

save_restore_config::save_restore_config(const char *_config_file)
{
    strcpy(config_file, _config_file);
}

void save_restore_config::load_configuration()
{
    File file = SPIFFS.open(config_file);
    StaticJsonDocument<500> doc;
    ReadBufferingStream bufferedFile(file, 64);
    deserializeJson(doc, bufferedFile);
    _config.port = doc["port"] | 80;
    _config.access_point = doc["access_point"] | true;
    _config.last_wellplate_A = doc["last_wellplate_A"] | 1;
    _config.last_wellplate_B = doc["last_wellplate_B"] | 1;
    strlcpy(_config.hostname, doc["hostname"] | "LIGHTOS", sizeof(_config.hostname));
    strlcpy(_config.ssid, doc["ssid"], sizeof(_config.ssid));
    strlcpy(_config.wlan_password, doc["wlan_password"], sizeof(_config.wlan_password));
    strlcpy(_config.last_config_file_A, doc["last_config_file_A"] | "/demo.csv", sizeof(_config.last_config_file_A));
    strlcpy(_config.last_config_file_B, doc["last_config_file_B"] | "/demo.csv", sizeof(_config.last_config_file_B));
    file.close();

    int file_length = strlen(_config.last_config_file_A);

    for (uint8_t i = 6; i <= file_length; i++)
    {
        char c = _config.last_config_file_A[i];
        _config.last_config_filename_A[i - 6] = c;
    }
    _config.last_config_filename_A[file_length - 10] = '\0';

    file_length = strlen(_config.last_config_file_B);

    for (uint8_t i = 6; i <= file_length; i++)
    {
        char c = _config.last_config_file_B[i];
        _config.last_config_filename_B[i - 6] = c;
    }
    _config.last_config_filename_B[file_length - 10] = '\0';
}

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
    StaticJsonDocument<500> doc;

    doc["access_point"] = _config.access_point;
    doc["ssid"] = _config.ssid;
    doc["wlan_password"] = _config.wlan_password;
    doc["hostname"] = _config.hostname;
    doc["port"] = _config.port;
    doc["last_config_file_A"] = _config.last_config_file_A;
    doc["last_wellplate_A"] = _config.last_wellplate_A;
    doc["last_config_file_B"] = _config.last_config_file_B;
    doc["last_wellplate_B"] = _config.last_wellplate_B;

    if (serializeJson(doc, file) == 0)
    {
        Serial.println(F("Failed to write to file"));
    }

    file.close();
}

void save_restore_config::set_access_point(const char *_access_point, bool update_config)
{
    _config.access_point = _access_point;
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
void save_restore_config::set_port(int _port, bool update_config)
{
    _config.port = _port;
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
    }
    else if (identifier == 'B')
    {
        strcpy(_config.last_config_file_B, _last_config_file);
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

const char *save_restore_config::get_ssid()
{
    return _config.ssid;
}

const bool save_restore_config::get_acess_point()
{
    return _config.access_point;
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