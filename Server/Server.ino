#include <ESP8266WiFi.h>

#define SSID "ESP32Server"
#define PASSWORD "87654321"
#define SERVER_PORT 5000

#define RELAY_PIN D2 // Usando o pino D2 (GPIO4)

WiFiServer server(SERVER_PORT);

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // Relé desligado inicialmente

  WiFi.mode(WIFI_AP);
  WiFi.softAP(SSID, PASSWORD);
  server.begin();
  Serial.println("Servidor iniciado e pronto para receber conexões.");
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("Cliente conectado.");
    while (client.connected()) {
      if (client.available()) {
        char command = client.read();
        Serial.print("Comando recebido: ");
        Serial.println(command);

        if (command == 'T') { // Recebe comando para alternar o relé
          static bool relayState = false;
          relayState = !relayState;
          digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
          Serial.print("Estado do relé alterado para: ");
          Serial.println(relayState ? "ON" : "OFF");
        }
      }
    }
    client.stop();
    Serial.println("Cliente desconectado.");
  }
}
