#include <WiFi.h>
#include <esp_now.h>

// Estrutura para enviar dados
typedef struct struct_message {
    bool relayState; // Estado do relé: 1 para ligado, 0 para desligado
} struct_message;

struct_message myData;

// Endereços MAC dos módulos relé
uint8_t relayMACAddress1[] = {0x60, 0x01, 0x94, 0x75, 0x73, 0xEB};
uint8_t relayMACAddress2[] = {0xCC, 0x50, 0xE3, 0xDA, 0xEF, 0xE3};
uint8_t relayMACAddress3[] = {0x84, 0xF3, 0xEB, 0xAD, 0x99, 0x1F};
uint8_t relayMACAddress4[] = {0x2C, 0xF4, 0x32, 0x8D, 0x54, 0xCD};

const int buttonPin = 14;  // Pino do botão que controla o loop
const int stopButtonPin = 19; // Pino do botão de parada
const int potPin = 35;     // Pino do potenciômetro

bool stopLoop = false; // Variável para controlar a parada do loop

// Função chamada quando os dados são enviados
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
    
    // Configurar os pinos
    pinMode(buttonPin, INPUT_PULLUP);     // Botão principal
    pinMode(stopButtonPin, INPUT_PULLUP); // Botão de parada
    pinMode(potPin, INPUT);               // Potenciômetro

    // Adicionar os destinatários (relés)
    esp_now_peer_info_t peerInfo;
    
    memcpy(peerInfo.peer_addr, relayMACAddress1, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    esp_now_add_peer(&peerInfo);

    memcpy(peerInfo.peer_addr, relayMACAddress2, 6);
    esp_now_add_peer(&peerInfo);

    memcpy(peerInfo.peer_addr, relayMACAddress3, 6);
    esp_now_add_peer(&peerInfo);

    memcpy(peerInfo.peer_addr, relayMACAddress4, 6);
    esp_now_add_peer(&peerInfo);

    esp_now_register_send_cb(OnDataSent);
}

void loop() {
    // Verifica o estado do botão de parada
    if (digitalRead(stopButtonPin) == LOW) {
        stopLoop = true; // Ativa a flag para interromper o loop
    }
    
    // Se o botão principal estiver pressionado e o loop não tiver sido interrompido
    if (digitalRead(buttonPin) == LOW && !stopLoop) {
        int potValue = analogRead(potPin); // Lê o valor do potenciômetro
        int delayTime = map(potValue, 0, 4095, 50, 1000); // Ajusta o delay

        // Ligar e desligar os relés em sequência
        controlRelay(relayMACAddress1, true);
        delayWithInterrupt(delayTime);

        controlRelay(relayMACAddress2, true);
        delayWithInterrupt(delayTime);

        controlRelay(relayMACAddress3, true);
        delayWithInterrupt(delayTime);

        controlRelay(relayMACAddress4, true);
        delayWithInterrupt(delayTime);

        // Desligar os relés em sequência
        controlRelay(relayMACAddress1, false);
        delayWithInterrupt(delayTime);

        controlRelay(relayMACAddress2, false);
        delayWithInterrupt(delayTime);

        controlRelay(relayMACAddress3, false);
        delayWithInterrupt(delayTime);

        controlRelay(relayMACAddress4, false);
        delayWithInterrupt(delayTime);
    } else if (stopLoop) {
        // Se o botão de parada for pressionado, desliga todos os relés
        controlRelay(relayMACAddress1, false);
        controlRelay(relayMACAddress2, false);
        controlRelay(relayMACAddress3, false);
        controlRelay(relayMACAddress4, false);

        stopLoop = false; // Reseta a flag de parada
        Serial.println("Loop interrompido e todos os relés desligados.");
    }
}

// Função para controlar o estado dos relés
void controlRelay(uint8_t *relayMACAddress, bool state) {
    myData.relayState = state;
    esp_now_send(relayMACAddress, (uint8_t *)&myData, sizeof(myData));
}

// Função para realizar o delay com verificação de interrupção
void delayWithInterrupt(int delayTime) {
    unsigned long startTime = millis();
    while (millis() - startTime < delayTime) {
        // Verifica se o botão de parada foi pressionado
        if (digitalRead(stopButtonPin) == LOW) {
            stopLoop = true;
            break;
        }
    }
}
