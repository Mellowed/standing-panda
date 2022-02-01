#ifndef ConfigurationMemory_h
#define ConfigurationMemory_h

#include "Arduino.h"
#include <Preferences.h>

class ConfigurationMemory
{
public:
    ConfigurationMemory();
    void setNamespace(String name); // limited to 15 characters
    void setString(String key, String value);
    void setInt(String key, int value);
    int getInt(String key, int defaultValue = 70);
    String getString(String key);

    const char *_nameSpace;
    Preferences _preferences;

private:
};

#endif
