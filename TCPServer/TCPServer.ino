#include <SPI.h>
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <Preferences.h>
#include <solo-serial.h>

// ============================================================
// Persistent storage (NVS)
// ============================================================

Preferences prefs;

// ============================================================
// LCD
// ============================================================

TFT_eSPI tft = TFT_eSPI();

// ============================================================
// WiFi
// ============================================================

// SOLO SERIAL:
// These are now supplied through the Solo Serial web console.
String ssid = "";
String password = "";

WiFiServer server(5000);
WiFiClient client;

// WiFi state
bool wifiConnectionRequested = false;
bool wifiConnected = false;
bool serverStarted = false;

// ============================================================
// Solo Serial command definitions
// ============================================================

// SOLO SERIAL:
// Maximum WiFi SSID is normally 32 characters.
// WPA/WPA2 passwords are normally up to 63 characters.

static const SoloSerial_ArgSpec setSSIDArgs[] = {
    SoloSerial_stringArg("ssid", 1, 32)
};

static const SoloSerial_ArgSpec setPasswordArgs[] = {
    SoloSerial_stringArg("password", 1, 63)
};


// ============================================================
// Solo Serial commands
// ============================================================

int setSSIDCommand(int argc, char* argv[])
{
    ssid = SoloSerial_getString(argv, 1);

    Serial.print("SSID set to: ");
    Serial.println(ssid);

    SoloSerial_sendData("SSID updated");

    return SUCCESS;
}


int setPasswordCommand(int argc, char* argv[])
{
    password = SoloSerial_getString(argv, 1);

    Serial.println("WiFi password updated.");

    SoloSerial_sendData("Password updated");

    return SUCCESS;
}


int connectWiFiCommand(int argc, char* argv[])
{
    if (ssid.length() == 0)
    {
        SoloSerial_sendData("SSID is empty");
        return GENERIC_ERROR;
    }

    if (password.length() == 0)
    {
        SoloSerial_sendData("Password is empty");
        return GENERIC_ERROR;
    }

    // Tell loop() to start the connection.
    wifiConnectionRequested = true;
    wifiConnected = false;

    // Disconnect any previous connection.
    WiFi.disconnect();

    SoloSerial_sendData("WiFi connection requested");

    return SUCCESS;
}


int clearWiFiCommand(int argc, char* argv[])
{
    // Wipe the saved credentials from NVS.
    prefs.clear();

    ssid = "";
    password = "";

    Serial.println("Saved WiFi credentials cleared.");

    // Note: an active connection is left alone on purpose.
    // The board simply won't auto-connect after the next reset.

    SoloSerial_sendData("WiFi credentials cleared");

    return SUCCESS;
}


// ============================================================
// LED configuration
// ============================================================

#define BUFFER_SIZE 256
char recvBuffer[BUFFER_SIZE];
String leftover = "";

// Display buffer height
#define MSG_Y_START 60
#define MSG_HEIGHT 60


// ============================================================
// Setup
// ============================================================

void setup()
{


    // --------------------------------------------------------
    // Solo Serial
    // --------------------------------------------------------
    //
    // IMPORTANT:
    // Do NOT use Serial.begin(115200) here.
    //
    // SoloSerial_init() starts Serial at the Solo Serial
    // command-mode baud rate (74880 by default).
    //

    SoloSerial_Config cfg;

    cfg.enableDataMode = false;
    cfg.maxCommands = 16;
    cfg.maxArguments = 5;
    cfg.maxCommandLength = 128;
    cfg.debugEnabled = false;

    SoloSerial_init(cfg);


    // --------------------------------------------------------
    // Load saved WiFi credentials
    // --------------------------------------------------------

    prefs.begin("wifi", false);

    ssid     = prefs.getString("ssid", "");
    password = prefs.getString("pass", "");


    // Register our custom commands.

    SoloSerial_registerCommand(
        "setSSID",
        setSSIDCommand,
        "Set the WiFi network name.",
        setSSIDArgs,
        1
    );

    SoloSerial_registerCommand(
        "setPassword",
        setPasswordCommand,
        "Set the WiFi password.",
        setPasswordArgs,
        1
    );

    SoloSerial_registerCommand(
        "connectWiFi",
        connectWiFiCommand,
        "Connect to WiFi using the configured SSID and password."
    );

    SoloSerial_registerCommand(
        "clearWiFi",
        clearWiFiCommand,
        "Erase the saved SSID and password from flash."
    );


    // Device information shown by Solo Serial.

    SoloSerial_setInfo("name", "ESP32 TCP Server");
    SoloSerial_setInfo("hardware_version", "ESP32");
    SoloSerial_setInfo("firmware_version", "1.0");


    // --------------------------------------------------------
    // TFT
    // --------------------------------------------------------

    tft.init();
    tft.setRotation(1);
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(0, 0);

    tft.println("ESP32 TCP Server");
    tft.println();

    if (ssid.length() > 0 && password.length() > 0)
    {
        // handleWiFi() in loop() picks this up on the first pass.
        wifiConnectionRequested = true;

        tft.println("Saved network:");
        tft.println(ssid);
    }
    else
    {
        tft.println("Waiting for WiFi");
        tft.println("configuration...");
    }
}


// ============================================================
// Connect WiFi
// ============================================================

void startWiFiConnection()
{
    wifiConnectionRequested = false;

    Serial.println();
    Serial.println("Starting WiFi connection...");
    Serial.print("SSID: ");
    Serial.println(ssid);

    tft.fillScreen(TFT_BLACK);
    tft.setCursor(0, 0);
    tft.println("ESP32 TCP Server");
    tft.println();
    tft.println("Connecting WiFi");
    tft.println(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    wifiConnected = false;
}


// ============================================================
// Handle WiFi state
// ============================================================

void handleWiFi()
{
    // --------------------------------------------------------
    // A connectWiFi command was received.
    // --------------------------------------------------------

    if (wifiConnectionRequested)
    {
        startWiFiConnection();
    }


    // --------------------------------------------------------
    // Check connection status.
    // --------------------------------------------------------

    if (!wifiConnected &&
        WiFi.status() == WL_CONNECTED)
    {
        wifiConnected = true;

        Serial.println();
        Serial.println("WiFi connected.");

        prefs.putString("ssid", ssid);
        prefs.putString("pass", password);
        SoloSerial_sendEventf("WiFi credentials saved");

        Serial.print("ESP32 local IP: ");
        Serial.println(WiFi.localIP());


        // Start TCP server only once.

        if (!serverStarted)
        {
            server.begin();
            serverStarted = true;

            Serial.println("Server started");
        }


        // ----------------------------------------------------
        // TFT
        // ----------------------------------------------------

        IPAddress ip = WiFi.localIP();

        tft.fillScreen(TFT_BLACK);
        tft.setCursor(0, 0);

        tft.println("ESP32 TCP Server");
        tft.println();
        tft.println("IP:");
        tft.println(ip.toString());

        tft.println();
        tft.println("Port: 5000");
        tft.println("Waiting...");


        // Tell the web console too.

        SoloSerial_sendEventf(
            "wifiConnected ip=%s",
            ip.toString().c_str()
        );
    }
}



// ============================================================
// Main loop
// ============================================================

void loop()
{
    // ========================================================
    // SOLO SERIAL
    // ========================================================
    //
    // This is essential.
    //
    // It checks whether the web console has sent a command.
    //

    SoloSerial_handleCommands();


    // ========================================================
    // WiFi
    // ========================================================

    handleWiFi();


    // ========================================================
    // Don't try TCP until WiFi/server are ready.
    // ========================================================

    if (!serverStarted)
    {
        delay(5);
        return;
    }


    // ========================================================
    // Accept new client
    // ========================================================

    if (!client || !client.connected())
    {
        WiFiClient newClient = server.available();

        if (newClient)
        {
            client = newClient;
            client.setTimeout(50);
            leftover = "";

            Serial.println("Unity connected");

            // Tell the web console a client attached.
            SoloSerial_sendEventf(
                "unityConnected ip=%s",
                client.remoteIP().toString().c_str()
            );

            tft.fillRect(0, 120, 240, 40, TFT_BLACK);
            tft.setCursor(0, 120);
            tft.println("Unity connected");

            // Clear message area
            tft.fillRect(
                0,
                MSG_Y_START,
                240,
                MSG_HEIGHT,
                TFT_BLACK
            );
        }
    }


    // ========================================================
    // Read TCP data
    // ========================================================

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


            // ------------------------------------------------
            // TEMPORARY DIAGNOSTIC - remove once framing is
            // confirmed. Logs every received byte so we can see
            // exactly what Unity sends and whether a terminator
            // ever arrives.
            // ------------------------------------------------

            SoloSerial_sendEventf(
                "rx byte=0x%02X char=%c len=%d",
                (uint8_t)c,
                isPrintable(c) ? c : '.',
                leftover.length()
            );


            if (c == '\n')
            {
                String msg = leftover;

                msg.trim();

                leftover = "";


                if (msg.length() > 0)
                {
                    Serial.println("Unity: " + msg);


                    // Show the marker in the web console.
                    // %s is used so a '%' inside msg is not
                    // treated as a format specifier.
                    SoloSerial_sendEventf(
                        "marker value=%d raw=%s",
                        value,
                        msg.c_str()
                    );


                    tft.fillScreen(TFT_BLACK);
                    tft.setCursor(0, 0);

                    tft.println("From Unity: ");
                    tft.println(msg);


                    client.println("ACK: " + msg);
                }
            }
        }
    }


    delay(5);
}