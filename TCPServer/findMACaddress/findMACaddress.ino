#include <WiFi.h>

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println();
  Serial.print("WiFi MAC Address: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
}