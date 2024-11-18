#include "BluetoothSerial.h"

// saídas que ligam as entradas na ponte H
int IN1 = 27;
int IN2 = 14;
int IN3 = 12;
int IN4 = 13;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth não está habilitado! Por favor, entre nas configurações e habilite o Bluetooth.
#endif

BluetoothSerial SerialBT;

byte veloc1 = 255; // Velocidade máxima
byte veloc2 = 200; // Velocidade reduzida para curvas

void setup() {
  // declaração dos pinos
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);// 
  pinMode(IN4, OUTPUT);// 
  
  Serial.begin(9600);
  SerialBT.begin("Carrinho Bluetooth"); // nome do dispositivo Bluetooth
  // Serial.println("O dispositivo foi iniciado, agora você pode emparelhá-lo com bluetooth!");
}

void loop() {
  char Z;
  if (SerialBT.available() > 0) {
    Z = SerialBT.read();
    delay(20); // atualiza as informações

    // PRA FRENTE
    if (Z == 'B') {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, HIGH);
    }

    // PRA TRÁS
    if (Z == 'F') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }

    // PRA ESQUERDA (parado)
    if (Z == 'L') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    }

    // PRA FRENTE E ESQUERDA
    if (Z == 'G') {
      digitalWrite(IN1, veloc2); 
      digitalWrite(IN2, veloc2);
      digitalWrite(IN3, HIGH);   
      digitalWrite(IN4, LOW);
    }

    // PRA DIREITA (parado)
    if (Z == 'R') {
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }

    // PRA FRENTE E DIREITA
    if (Z == 'I') {
      digitalWrite(IN1, HIGH);   
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, veloc2); 
      digitalWrite(IN4, veloc2);
    }

    // PRA TRÁS E ESQUERDA
    if (Z == 'H') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, veloc2); 
    }

    // PRA TRÁS E DIREITA
    if (Z == 'J') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, veloc2); 
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    }

    // PARADO
    if (Z == 'S') {
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
  }
}
