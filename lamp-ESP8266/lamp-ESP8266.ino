#include <ESP8266WiFi.h>
#include <espnow.h>

typedef struct struct_message {
    bool relayState; // 1 para ligado, 0 para desligado
} struct_message;

struct_message incomingData;

uint8_t relayOn[] = {0xA0, 0x01, 0x01, 0xA2}; // Ajuste para o comando correto de ligar o relé
uint8_t relayOff[] = {0xA0, 0x01, 0x00, 0xA1}; // Ajuste para o comando correto de desligar o relé

void OnDataRecv(uint8_t *mac_addr, uint8_t *incomingData, uint8_t len) {
    if (len == sizeof(struct_message)) {
        struct_message *data = (struct_message *)incomingData;
        if (data->relayState) {
            Serial.println("Relé ligado.");
            // Envia comando para ligar o relé
            Serial.write(relayOn, sizeof(relayOn));
        } else {
            Serial.println("Relé desligado.");
            // Envia comando para desligar o relé
            Serial.write(relayOff, sizeof(relayOff));
        }
    } else {
        Serial.println("Dados recebidos com tamanho incorreto.");
    }
}

void setup() {
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    esp_now_init();
    
    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    // Código principal
}
