#ifndef LocalESPStorage_h
#define LocalESPStorage_h

#include "Arduino.h"
#include "SPIFFS.h"
#include "FS.h"

class LocalESPStorage
{
public:
    LocalESPStorage();
    void begin(boolean format);
    void getFileList(const char *dirname, String files[]);
    int getFileCount(const char *dirname);
    void writeFile(const char *path, const char *message);
    void deleteFile(const char *path);
    void renameFile(const char *path1, const char *path2);
    void appendFile(const char *path, const char *message);
    String readFile(const char *path);

private:
};

#endif
