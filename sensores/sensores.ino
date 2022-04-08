#define DEBUG
//Configuracion para la Pantalla de Nico
#ifdef DEBUG
#include "SSD1306Wire.h"
static SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
#define LOG(msg) display.clear();display.drawString(display.width()/2, display.height()/2, msg); display.display()
#define INITLOG() display.init(); display.setContrast(255); display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH); display.setFont(ArialMT_Plain_16); display.clear()
#else
#define LOG(msg)
#define INITLOG()
#endif

#define LED1 25

void taskSerial();
void taskBeat();

bool ledState = false;

void setup() {
  INITLOG();     //Pantalla
  taskSerial();  //Serial

}

void loop() {
  taskSerial();
}

void taskSerial()
{
  enum class SerialStates {INIT, WAITING_REQ, WRITE_REQ, READ_RESPONSE, CORRECT_RESPONSE, INCORRECT_RESPONSE};
  static SerialStates serialState =  SerialStates::INIT;
  static uint8_t bufferRx[17] = {0};
  static uint8_t dataCounter = 0;
  static uint8_t sendPackages = 0; //incrementarlo cada vez que se envie un paquete
  static uint8_t packagesArr[17] = {0}; //paquete de numeros que vamos a enviar


  switch (serialState)
  {
    case SerialStates::INIT:
      {
        Serial.begin(115200);
        //Aquí podría meter la inicialización de la pantalla

        serialState = SerialStates::WAITING_REQ;
        break;
      }
    case SerialStates::WAITING_REQ:
      {
        if (Serial.available())
        {
          Serial.print("Holii");
          if (Serial.read() == 0x2A)
          {
            Serial.print("2a Recibido");
            Serial.println();
            serialState = SerialStates::WRITE_REQ;
          }
          //          else
          //          {
          //            serialState = SerialStates::INIT;
          //          }
        }
        break;
      }
    case SerialStates::WRITE_REQ:
      {
        //Armamos el paquete:
        //        float fnum01 = 39.36;
        //        float fnum02 = 15.27;
        //        float fnum03 = 13.21;
        uint32_t inum32 = 42;
        float fArr[3] = {39.36, 15.27, 13.21};

        //Llenamos el paquete:
        //        memcpy(packagesArr, (uint8_t *)&fnum01, 4);
        //        memcpy(packagesArr + 4, (uint8_t *)&fnum02, 4);
        //        memcpy(packagesArr + 8, (uint8_t *)&fnum03, 4);
        //        memcpy(packagesArr + 12, (uint8_t *)&inum32, 4);

        //Mostramos en pantalla el paquete armado:
        for (int  j = 0; j < 3; j++) {
          //los envio por consola para verificar
          Serial.println(fArr[j]);
        }
        Serial.println (inum32);
        //para el Checksum:
        uint8_t calcChecksum = 0;
        //dataCounter = 0;

        //Llenamos el Buffer del Checksum con los numeros del paquete
        //        bufferRx[0] = {( (uint8_t *)&packagesArr[0], 4)};
        //        bufferRx[4] = {( (uint8_t *)&packagesArr[1], 4)};
        //        bufferRx[8] = {( (uint8_t *)&packagesArr[2], 4)};
        bufferRx[0] = {( (uint8_t *)&fArr[0], 4)};
        bufferRx[4] = {( (uint8_t *)&fArr[1], 4)};
        bufferRx[8] = {( (uint8_t *)&fArr[2], 4)};
        bufferRx[12] = inum32;
        Serial.println();
        //El Calculamos el Checksum:
        for (uint8_t i = 1; i <= dataCounter - 1; i++) {
          calcChecksum = calcChecksum ^ bufferRx[i - 1];
        }
        Serial.write(calcChecksum);
        Serial.print("Checksum Done");
        serialState = SerialStates::READ_RESPONSE;
        break;
      }
    case SerialStates::READ_RESPONSE:
      {
        if (sendPackages ==  3)
        {
          Serial.print("Limite De Packetes Superado");
          serialState = SerialStates::INCORRECT_RESPONSE;
        }
        if (Serial.available())
        {
          if (Serial.read() == 0xE3)
          {
            Serial.print("E3 Recibido");
            Serial.println();
            serialState = SerialStates::CORRECT_RESPONSE;              //Si llega el E3 manda a CORRECT RESPONSE
          }
          else if (Serial.read() == 0xB0)
          {
            Serial.print("B0 Recibido");
            Serial.println(sendPackages);
            sendPackages++;
            serialState = SerialStates::WRITE_REQ;
          }
        }

        break;
      }
    case SerialStates::CORRECT_RESPONSE:
      {

        static uint32_t CorrectCounter = 0;
        //static uint32_t previousCounter = 0;
        const uint32_t CorrectMaxTime = 3000;

        CorrectCounter = millis();

        //Avisamos por pantalla que todo está correcto:
        display.clear();
        //display.drawString(9, 0, "CORRECT!");
        display.drawString(0, 5, "CORRECT!");
        display.display();

        if ( CorrectCounter >= CorrectMaxTime)
        {
          display.clear();
          Serial.print("Good ending.");
          serialState = SerialStates::INIT;
        }

        break;
      }
    case SerialStates::INCORRECT_RESPONSE:
      {

        const uint32_t interval = 500; //Ya está a 1Hz
        static uint32_t previousMillis = 0;
        static uint32_t LedtaskCounter = 0;
        static uint8_t ledState = LOW;
        uint8_t ledTimer = 3;

        uint32_t currentMillis = millis();
        digitalWrite(LED1, ledState);
        pinMode(LED1, OUTPUT);
        LedtaskCounter = millis();

        //Aviso por pantalla:
        display.clear();
        //display.drawString(9, 0, "INCORRECT!");
        display.drawString(0, 5, "INCORRECT!");
        display.display();

        //Prendo y apagó el Led:
        if ( (currentMillis - previousMillis) >= interval)
        {
          previousMillis = millis();

          if (ledState == LOW)
          {
            ledState = HIGH; //revisar si necesita high y low
          } else {
            ledState = LOW;
            ledTimer --; //Actualizamos el timer cada Segundo
          }

          digitalWrite(LED1, ledState);
          //Cuando hayan pasado los 3 seg
          if (ledTimer == 0) {
            Serial.print("Bad ending.");
            Serial.print("Led OFF");
            serialState = SerialStates::INIT;
          }
        }


        break;
      }

    default:
      break;
  }

}
