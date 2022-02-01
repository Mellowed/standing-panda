#include "LocalESPStorage.h"

LocalESPStorage::LocalESPStorage()
{
}

void LocalESPStorage::begin(boolean format)
{

    if (!SPIFFS.begin(format))
    {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    unsigned int totalBytes = SPIFFS.totalBytes();
    unsigned int usedBytes = SPIFFS.usedBytes();

    Serial.printf("File system info.\nTotal Space: %d bytes\n Total Space Used: %d bytes \n ", totalBytes, usedBytes);
}

int LocalESPStorage::getFileCount(const char *dirname)
{
    Serial.printf("Counting files in directory: %s\r\n", dirname);
    File root = SPIFFS.open(dirname);
    File file = root.openNextFile();
    int fileIndex = 0;
    while (file)
    {
        fileIndex++;
        file = root.openNextFile();
    }

    return fileIndex;
}

void LocalESPStorage::getFileList(const char *dirname, String files[])
{
    Serial.printf("Listing directory: %s\r\n", dirname);
    File root = SPIFFS.open(dirname);
    File file = root.openNextFile();
    int fileIndex = 0;
    while (file)
    {
        Serial.println(file.name());
        files[fileIndex] = file.name();
        fileIndex++;
        file = root.openNextFile();
    }
}

void LocalESPStorage::writeFile(const char *path, const char *message)
{
    Serial.printf("Writing file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_WRITE);
    if (!file)
    {
        Serial.println("- failed to open file for writing");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- file written");
    }
    else
    {
        Serial.println("- write failed");
    }
    file.close();
}

String LocalESPStorage::readFile(const char *path)
{
    Serial.printf("Reading file: %s\r\n", path);

    String result = "";

    File file = SPIFFS.open(path);
    if (!file || file.isDirectory())
    {
        Serial.println("- failed to open file for reading");
        return result;
    }

    Serial.println("- read from file:");
    while (file.available())
    {
        result = file.readString();
        Serial.write(file.read());
    }
    file.close();

    return result;
}

void LocalESPStorage::deleteFile(const char *path)
{
    Serial.printf("Deleting file: %s\r\n", path);
    if (SPIFFS.remove(path))
    {
        Serial.println("- file deleted");
    }
    else
    {
        Serial.println("- delete failed");
    }
}

void LocalESPStorage::renameFile(const char *path1, const char *path2)
{
    Serial.printf("Renaming file %s to %s\r\n", path1, path2);
    if (SPIFFS.rename(path1, path2))
    {
        Serial.println("- file renamed");
    }
    else
    {
        Serial.println("- rename failed");
    }
}

void LocalESPStorage::appendFile(const char *path, const char *message)
{
    Serial.printf("Appending to file: %s\r\n", path);

    File file = SPIFFS.open(path, FILE_APPEND);
    if (!file)
    {
        Serial.println("- failed to open file for appending");
        return;
    }
    if (file.print(message))
    {
        Serial.println("- message appended");
    }
    else
    {
        Serial.println("- append failed");
    }
    file.close();
}