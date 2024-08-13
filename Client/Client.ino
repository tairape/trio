#include <SoftwareSerial.h>

#define SSID "ESP32Server"
#define PASSWORD "87654321"
#define SERVER_PORT 5000
#define SERVER_IP "192.168.4.1"  // IP padrão do AP do ESP8266

#define BUTTON_PIN 8

SoftwareSerial espSerial(10, 11);

void setup() {
  Serial.begin(9600);
  espSerial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  connectToWiFi();
}

void loop() {
  if (digitalRead(BUTTON_PIN) == LOW) { // Botão pressionado
    Serial.println("Botão pressionado! Enviando comando.");
    sendCommandToServer('T'); // Envia comando para alternar o relé
    delay(500); // Evita múltiplas leituras devido ao bouncing do botão
  }
}

void connectToWiFi() {
  espSerial.println("AT+CWMODE=1");  // Configura o modo do ESP-01 como cliente
  delay(1000);
  espSerial.println("AT+CWJAP=\"" SSID "\",\"" PASSWORD "\"");  // Conecta ao AP
  delay(5000);

  Serial.println("Conectado ao WiFi!");
}

void sendCommandToServer(char command) {
  espSerial.println("AT+CIPSTART=\"TCP\",\"" SERVER_IP "\"," + String(SERVER_PORT));  // Conecta ao servidor
  delay(1000);
  espSerial.println("AT+CIPSEND=" + String(1));  // Prepara o envio
  delay(1000);
  espSerial.print(command);  // Envia o comando
  Serial.print("Comando enviado: ");
  Serial.println(command);
  delay(500);
  espSerial.println("AT+CIPCLOSE");  // Fecha a conexão
}
