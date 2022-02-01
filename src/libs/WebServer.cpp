#include "WebServer.h"
#include <functional>

AsyncWebServer _server(80);
AsyncWebSocket ws("/ws");

WebServer::WebServer(const char *ssid, const char *password, LocalESPStorage *localESPStorage, int Port, String hostname)
{
    _ssid = ssid;
    _password = password;
    _port = Port;

    _localESPStorage = localESPStorage;
    _hostname = hostname;
}

void WebServer::begin()
{

    WiFi.hostname(_hostname.c_str());
    WiFi.begin(_ssid, _password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi..");
    }

    _ip = WiFi.localIP().toString();

    Serial.println("Connected: " + _ip);
}

void WebServer::serveWebApp(const char *dirname)
{

    int fileCount = _localESPStorage->getFileCount("/");
    String files[fileCount];
    _localESPStorage->getFileList("/", files);

    _server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
               { request->send(SPIFFS, "/index.html", "text/html"); });

    _server.serveStatic("/", SPIFFS, "/");

    _server.begin();
}

// Websockets

void WebServer::initWebSocket()
{
    ws.onEvent([&](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) -> void
               {
    switch (type)
        {
        case WS_EVT_CONNECT:
         _connectionCount++;
            Serial.printf("WebSocket client #%u connected from %s\nConnections: #%d \n", client->id(), client->remoteIP().toString().c_str(), _connectionCount);
           
            break;

        case WS_EVT_DISCONNECT:
        _connectionCount--;
            Serial.printf("WebSocket client #%u disconnected\nConnections: #%d \n", client->id(), _connectionCount );
            
            break;

        case WS_EVT_DATA:
            handleWebSocketMessage(arg, data, len);
            break;

        case WS_EVT_PONG:
            Serial.printf("WebSocket client #%u pong\n", client->id());
            break;

        case WS_EVT_ERROR:
            Serial.printf("WebSocket client #%u EVT error\n", client->id());
            break;

        } });
    _server.addHandler(&ws);
}

// Send a message to all the listening clients
void WebServer::notifyClients(String message)
{
    ws.textAll(message);
}

// Set the callback function used when processing data request
// TODO; figure out why "identifier "WsDataCallback" is undefined"
void WebServer::setWsDataCallback(WsDataCallback wd)
{
    _wsDataCallback = wd;
}

// Handle the incoming messages from the client
void WebServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0; // // restore byte as _handleEvent may have added a null terminator i.e., data[len] = 0;
        if (_wsDataCallback)
        {
            _wsDataCallback(String((char *)data));
        }
        else
        {
            Serial.println("Data Received but No data handler assigned");
            Serial.println(String((char *)data));
        }
    }
}
