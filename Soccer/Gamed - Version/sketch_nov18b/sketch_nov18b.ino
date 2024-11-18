#include <Bluepad32.h>

// Configuração dos pinos
const int IN1 = 27;  // Pino PWM para motor esquerdo
const int IN2 = 14;  // Pino PWM para motor esquerdo (reverso)
const int IN3 = 12;  // Pino PWM para motor direito
const int IN4 = 13; // Pino PWM para motor direito (reverso)

// Configuração PWM
const int freq = 5000;           // Frequência PWM em Hz
const int resolution = 8;        // Resolução PWM (8 bits: 0-255)
const int pwmChannelLeftFwd = 0; // Canal PWM para IN1
const int pwmChannelLeftRev = 1; // Canal PWM para IN2
const int pwmChannelRightFwd = 2; // Canal PWM para IN3
const int pwmChannelRightRev = 3; // Canal PWM para IN4

// Velocidades pré-definidas
int veloc1 = 255;  
int veloc2 = 150;     

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    Serial.printf(
        "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
        "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
        ctl->index(),        // Controller Index
        ctl->dpad(),         // D-pad
        ctl->buttons(),      // bitmask of pressed buttons
        ctl->axisX(),        // (-511 - 512) left X Axis
        ctl->axisY(),        // (-511 - 512) left Y axis
        ctl->axisRX(),       // (-511 - 512) right X axis
        ctl->axisRY(),       // (-511 - 512) right Y axis
        ctl->brake(),        // (0 - 1023): brake button
        ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
        ctl->miscButtons(),  // bitmask of pressed "misc" buttons
        ctl->gyroX(),        // Gyro X
        ctl->gyroY(),        // Gyro Y
        ctl->gyroZ(),        // Gyro Z
        ctl->accelX(),       // Accelerometer X
        ctl->accelY(),       // Accelerometer Y
        ctl->accelZ()        // Accelerometer Z
    );
}

void dumpMouse(ControllerPtr ctl) {
    Serial.printf("idx=%d, buttons: 0x%04x, scrollWheel=0x%04x, delta X: %4d, delta Y: %4d\n",
                   ctl->index(),        // Controller Index
                   ctl->buttons(),      // bitmask of pressed buttons
                   ctl->scrollWheel(),  // Scroll Wheel
                   ctl->deltaX(),       // (-511 - 512) left X Axis
                   ctl->deltaY()        // (-511 - 512) left Y axis
    );
}

void dumpKeyboard(ControllerPtr ctl) {
    static const char* key_names[] = {
        "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V",
        "W", "X", "Y", "Z", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0",
      
        "Enter", "Escape", "Backspace", "Tab", "Spacebar", "Underscore", "Equal", "OpenBracket", "CloseBracket",
        "Backslash", "Tilde", "SemiColon", "Quote", "GraveAccent", "Comma", "Dot", "Slash", "CapsLock",
     
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12",
      
        "PrintScreen", "ScrollLock", "Pause", "Insert", "Home", "PageUp", "Delete", "End", "PageDown",
        "RightArrow", "LeftArrow", "DownArrow", "UpArrow",
       
    };
    static const char* modifier_names[] = {
      
        "Left Control", "Left Shift", "Left Alt", "Left Meta",
        "Right Control", "Right Shift", "Right Alt", "Right Meta",
       
    };
    Serial.printf("idx=%d, Pressed keys: ", ctl->index());
    for (int key = Keyboard_A; key <= Keyboard_UpArrow; key++) {
        if (ctl->isKeyPressed(static_cast<KeyboardKey>(key))) {
            const char* keyName = key_names[key-4];
            Serial.printf("%s,", keyName);
       }
    }
    for (int key = Keyboard_LeftControl; key <= Keyboard_RightMeta; key++) {
        if (ctl->isKeyPressed(static_cast<KeyboardKey>(key))) {
            const char* keyName = modifier_names[key-0xe0];
            Serial.printf("%s,", keyName);
        }
    }
    Console.printf("\n");
}

void dumpBalanceBoard(ControllerPtr ctl) {
    Serial.printf("idx=%d,  TL=%u, TR=%u, BL=%u, BR=%u, temperature=%d\n",
                   ctl->index(),        // Controller Index
                   ctl->topLeft(),      // top-left scale
                   ctl->topRight(),     // top-right scale
                   ctl->bottomLeft(),   // bottom-left scale
                   ctl->bottomRight(),  // bottom-right scale
                   ctl->temperature()   // temperature: used to adjust the scale value's precision
    );
}

void processGamepad(ControllerPtr ctl) {
   
   if (ctl->buttons() ==  0x0080) { // Botão para frente
        // Ambos os motores em alta velocidade para frente
        ledcWrite(pwmChannelLeftFwd, veloc1);
        ledcWrite(pwmChannelLeftRev, 0);
        ledcWrite(pwmChannelRightFwd, veloc1);
        ledcWrite(pwmChannelRightRev, 0);
    } 
    else if (ctl->buttons() == 0x0040) { // Botão para trás
        // Ambos os motores em alta velocidade para trás
        ledcWrite(pwmChannelLeftFwd, 0);
        ledcWrite(pwmChannelLeftRev, veloc1);
        ledcWrite(pwmChannelRightFwd, 0);
        ledcWrite(pwmChannelRightRev, veloc1);
    } 
    else if (ctl->buttons() == 0x0004) { // Curva à esquerda
        // Motor esquerdo em velocidade reduzida; motor direito em alta velocidade
        ledcWrite(pwmChannelLeftFwd, 0);
        ledcWrite(pwmChannelLeftRev, veloc1);
        ledcWrite(pwmChannelRightFwd, veloc1);
        ledcWrite(pwmChannelRightRev, 0);
    } 
    else if (ctl->buttons() == 0x0002) { // Curva à direita
        // Motor direito em velocidade reduzida; motor esquerdo em alta velocidade
        ledcWrite(pwmChannelLeftFwd, veloc1);
        ledcWrite(pwmChannelLeftRev, 0);
        ledcWrite(pwmChannelRightFwd, 0);
        ledcWrite(pwmChannelRightRev, veloc1);
    } 
    else {
        // Nenhum botão pressionado: motores desligados
        ledcWrite(pwmChannelLeftFwd, 0);
        ledcWrite(pwmChannelLeftRev, 0);
        ledcWrite(pwmChannelRightFwd, 0);
        ledcWrite(pwmChannelRightRev, 0);
    }
    dumpGamepad(ctl); // Mantém a exibição dos dados no monitor serial
}

 


void processMouse(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->scrollWheel() > 0) {
        // Do Something
    } else if (ctl->scrollWheel() < 0) {
        // Do something else
    }

    // See "dumpMouse" for possible things to query.
    dumpMouse(ctl);
}

void processKeyboard(ControllerPtr ctl) {
    if (!ctl->isAnyKeyPressed())
        return;

    // This is just an example.
    if (ctl->isKeyPressed(Keyboard_A)) {
        // Do Something
        Serial.println("Key 'A' pressed");
    }

    // Don't do "else" here.
    // Multiple keys can be pressed at the same time.
    if (ctl->isKeyPressed(Keyboard_LeftShift)) {
        // Do something else
        Serial.println("Key 'LEFT SHIFT' pressed");
    }

    // Don't do "else" here.
    // Multiple keys can be pressed at the same time.
    if (ctl->isKeyPressed(Keyboard_LeftArrow)) {
        // Do something else
        Serial.println("Key 'Left Arrow' pressed");
    }

    // See "dumpKeyboard" for possible things to query.
    dumpKeyboard(ctl);
}

void processBalanceBoard(ControllerPtr ctl) {
    // This is just an example.
    if (ctl->topLeft() > 10000) {
        // Do Something
    }
    dumpBalanceBoard(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else if (myController->isMouse()) {
                processMouse(myController);
            } else if (myController->isKeyboard()) {
                processKeyboard(myController);
            } else if (myController->isBalanceBoard()) {
                processBalanceBoard(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

void setup() {

    // declaração dos pinos
     // Configuração dos canais PWM
    ledcSetup(pwmChannelLeftFwd, freq, resolution);
    ledcSetup(pwmChannelLeftRev, freq, resolution);
    ledcSetup(pwmChannelRightFwd, freq, resolution);
    ledcSetup(pwmChannelRightRev, freq, resolution);

    // Associando canais PWM aos pinos
    ledcAttachPin(IN1, pwmChannelLeftFwd);
    ledcAttachPin(IN2, pwmChannelLeftRev);
    ledcAttachPin(IN3, pwmChannelRightFwd);
    ledcAttachPin(IN4, pwmChannelRightRev);

    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    // Setup the Bluepad32 callbacks
    BP32.setup(&onConnectedController, &onDisconnectedController);
    BP32.forgetBluetoothKeys();
    BP32.enableVirtualDevice(false);
}

void loop() {

    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();
    delay(150);
}