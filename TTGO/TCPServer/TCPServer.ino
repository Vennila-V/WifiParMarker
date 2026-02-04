#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>

// LCD
TFT_eSPI tft = TFT_eSPI();

// WiFi
const char* ssid = "iotroam"; 
const char* password = "__"; //type the correct password

WiFiServer server(5000);
WiFiClient client;

#define BUFFER_SIZE 256
char recvBuffer[BUFFER_SIZE];
String leftover = "";

// Display buffer height
#define MSG_Y_START 60
#define MSG_HEIGHT 60

void setup() 
{
    Serial.begin(115200);

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("ESP32 TCP Server");
    tft.println("Connecting WiFi");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        tft.print(".");
    }

    server.begin();

    IPAddress ip = WiFi.localIP();

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.println("ESP32 TCP Server\n");
    tft.println("IP:");
    tft.println(ip.toString());
    tft.println("\nPort: 5000");
    tft.println("Waiting...");

    Serial.print("ESP32 local IP: ");
    Serial.println(WiFi.localIP());
    Serial.println("Server started");
    //Serial.println(ip);
}

void loop()
{
    // Accept new client
    if (!client || !client.connected())
    {
        WiFiClient newClient = server.available();
        if (newClient)
        {
            client = newClient;
            client.setTimeout(50);
            leftover = "";

            Serial.println("Unity connected");
            tft.fillRect(0, 120, 240, 40, TFT_BLACK);
            tft.setCursor(0, 120);
            tft.println("Unity connected");

            // Clear message area
            tft.fillRect(0, MSG_Y_START, 240, MSG_HEIGHT, TFT_BLACK);
        }
    }

    // Read data
    if (client && client.connected() && client.available())
{
    Serial.print("Client connected. Remote IP: ");
    Serial.println(client.remoteIP());
    Serial.print("Available bytes: ");
    Serial.println(client.available());

    while (client.available())
    {
        char c = client.read();
        leftover += c;

        if (c == '\n') // full message received
        {
            String msg = leftover;
            msg.trim();
            leftover = "";

            if (msg.length() > 0)
            {
                Serial.println("Unity: " + msg);

                tft.fillScreen (TFT_BLACK);
                tft.setCursor(0, 0);
                tft.println("From Unity:");
                tft.println(msg);

                client.println("ACK: " + msg); // reply
            }
        }
    }
}

    delay(5);
}
