#include <WiFi.h>
#include <esp_now.h>

typedef struct struct_message {
    bool relayState; // 1 para ligado, 0 para desligado
} struct_message;

struct_message myData;

uint8_t relayMACAddresses[][6] = {
    {0x60, 0x01, 0x94, 0x75, 0x73, 0xEB}, // MAC do primeiro relé
    {0xCC, 0x50, 0xE3, 0xDA, 0xEF, 0xE3}, // MAC do segundo relé
    {0x84, 0xF3, 0xEB, 0xAD, 0x99, 0x1F}, // MAC do terceiro relé
    {0x2C, 0xF4, 0x32, 0x8D, 0x54, 0xCD}  // MAC do quarto relé
};

const int buttonPin = 14; // Pino onde o botão está conectado
int relayCount = 4;       // Número de relés
int currentRelay = 0;     // Índice do relé atual

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    if (myData.relayState) {
        Serial.println("Relé ligado.");
    } else {
        Serial.println("Relé desligado.");
    }
}

void setup() {
    Serial.begin(115200);
    WiFi.mode(WIFI_STA);
    esp_now_init();
    
    // Adiciona os destinatários ESP8266 (relés)
    for (int i = 0; i < relayCount; i++) {
        esp_now_peer_info_t peerInfo;
        memcpy(peerInfo.peer_addr, relayMACAddresses[i], 6);
        peerInfo.channel = 0;
        peerInfo.encrypt = false;
        esp_now_add_peer(&peerInfo);
    }

    esp_now_register_send_cb(OnDataSent);
    
    // Configura o pino do botão
    pinMode(buttonPin, INPUT_PULLUP); // Usa o resistor pull-up interno
}

void loop() {
    static bool buttonPressed = false; // Armazena o estado do botão

    if (digitalRead(buttonPin) == LOW) {
        if (!buttonPressed) {
            buttonPressed = true; // Marca que o botão foi pressionado
            Serial.println("Botão pressionado, iniciando sequencia.");
        }
        
        // Liga o próximo relé
        myData.relayState = true;
        esp_now_send(relayMACAddresses[currentRelay], (uint8_t *)&myData, sizeof(myData));
        delay(500); // Tempo para observar o relé ligado
        
        currentRelay++; // Avança para o próximo relé
        
        // Se já ligou todos, começa a desligar um por um
        if (currentRelay >= relayCount) {
            currentRelay = 0;
            Serial.println("Iniciando ciclo de desligamento.");
            for (int i = 0; i < relayCount; i++) {
                myData.relayState = false;
                esp_now_send(relayMACAddresses[i], (uint8_t *)&myData, sizeof(myData));
                delay(500); // Tempo entre desligamentos
            }
        }
    } else {
        buttonPressed = false; // Reseta o estado do botão quando solto
    }

    delay(50); // Ajuste para evitar múltiplas leituras rápidas
}
