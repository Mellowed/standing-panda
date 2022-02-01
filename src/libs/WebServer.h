#ifndef WebServer_h
#define WebServer_h

#include "Arduino.h"
#include "ESPAsyncWebServer.h"
#include "./LocalESPStorage.h"
#include <ArduinoJson.h>
#include <functional>

// Not sure why std::function typedefs are indicating as errors. It does seem to work...
typedef std::function<void(String data)> WsDataCallback;

class WebServer
{
public:
    WebServer(const char *ssid, const char *password, LocalESPStorage *localESPStorage, int port = 80, String hostname = "Habitat Desk");
    void begin();
    void serveWebApp(const char *dirname);
    void initWebSocket();
    void notifyClients(String message);
    void setWsDataCallback(WsDataCallback wd);
    void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
    String _ip;
    String _hostname;
    int _connectionCount;

private:
    WsDataCallback _wsDataCallback;
    LocalESPStorage *_localESPStorage;
    const char *_password;
    const char *_ssid;
    int _port;
};

#endif
