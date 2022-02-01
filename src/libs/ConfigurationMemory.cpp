#include "ConfigurationMemory.h"

ConfigurationMemory::ConfigurationMemory()
{
    Preferences _preferences();
}

void ConfigurationMemory::setNamespace(String name)
{
    _nameSpace = name.c_str();
}

void ConfigurationMemory::setInt(String key, int value)
{
    _preferences.begin(_nameSpace);
    _preferences.putInt(key.c_str(), value);
    _preferences.end();
}

void ConfigurationMemory::setString(String key, String value)
{
    _preferences.begin(_nameSpace);
    _preferences.putString(key.c_str(), value);
    _preferences.end();
}

int ConfigurationMemory::getInt(String key, int defaultValue)
{
    _preferences.begin(_nameSpace);
    int result = _preferences.getInt(key.c_str(), defaultValue);
    _preferences.end();
    return result;
}

String ConfigurationMemory::getString(String key)
{
    _preferences.begin(_nameSpace);
    String result = _preferences.getString(key.c_str());
    _preferences.end();
    return result;
}
