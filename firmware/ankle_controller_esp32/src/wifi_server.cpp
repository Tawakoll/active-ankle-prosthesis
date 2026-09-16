/*
 * wifi_server.cpp - minimal HTTP start/stop control.
 *
 * Serves two links that drive operation_pin high or low, so the ankle could be
 * started and stopped from a phone during testing instead of over the serial
 * cable. Experimental; not started by default.
 *
 * Credentials are placeholders. Set them for your own network before use.
 */

#include "config.h"

void wifi_communication(void *) {
  VOID SETUP() {
    pinMode(operation_pin, OUTPUT);
    DELAY(10);

    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      DELAY(500);
      Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    server.begin();
  }

  VOID LOOP() {
    WiFiClient client = server.available();

    if (client) {
      Serial.println("New Client.");
      String currentLine = "";

      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          Serial.write(c);

          if (c == '\n') {
            if (currentLine.length() == 0) { /* blank line ends the request */
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              client.print("Click <a href=\"/H\">here</a> to start walking.<br>");
              client.print("Click <a href=\"/L\">here</a> to stop  walking.<br>");
              client.println();
              break;
            } else {
              currentLine = "";
            }
          } else if (c != '\r') {
            currentLine += c;
          }

          if (currentLine.endsWith("GET /H")) digitalWrite(operation_pin, HIGH);
          if (currentLine.endsWith("GET /L")) digitalWrite(operation_pin, LOW);
        }
      }

      client.stop();
      Serial.println("Client Disconnected.");
    }
  }
}
