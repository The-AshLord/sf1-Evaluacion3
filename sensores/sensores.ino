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

void setup() {
  INITLOG();     //Pantalla
  taskSerial();  //Serial
  taskBeat();    //Led
}

void loop() {
  taskSerial();
  taskBeat();
}

void taskSerial()
{
  enum class SerialStates {INIT, WAITING_REQ, WRITE_REQ, WAITING_RESPONSE, CORRECT_RESPONSE};
  static SerialStates serialState =  SerialStates::INIT;
  static uint8_t bufferRx[17] = {0};
  static uint8_t dataCounter = 0;
  //static uint32_t timerOld;
  static uint8_t bufferTx[17];
  static uint8_t sendPackages = 0; //incrementarlo cada vez que se envie un paquete

  switch (serialState)
  {
    case SerialStates::INIT:
      {
        Serial.begin(115200);
        serialState = SerialStates::WAITING_REQ;
        break;
      }
    case SerialStates::WAITING_REQ:
      {
        if (Serial.available() > 0)
        {
          String dato = Serial.readStringUntil('\n');
          if (dato == "0x2A")
          {
            Serial.write("0x3E");
            dataCounter = 0;
            //timerOld = millis();
            serialState = SerialStates::WRITE_REQ;
          }
          else
          {
            Serial.write("0xB0");
          }


        }
      }
      break;
    case SerialStates::WRITE_REQ: //aqui incluimos el checksum
      {
        /*if ( se recibio el 0xB0 ) {
          //Bad route
          //state = StateTaskCom::WAIT_INIT;
          }*/
        //Hay que revisar que neceistamos de todo esto
        if (Serial.available()) {
          uint8_t dataRx = Serial.read();
          if (dataCounter >= 20) {
            Serial.write(0x3F);
            dataCounter = 0;
            //timerOld = millis();
            serialState = SerialStates::WRITE_REQ;
          } else
          { bufferRx[dataCounter] = dataRx;
            dataCounter++;

            // is the packet completed?
            if (bufferRx[0] == dataCounter - 1) {

              // Check received data
              uint8_t calcChecksum = 0;
              for (uint8_t i = 1; i <= dataCounter - 1; i++) {
                calcChecksum = calcChecksum ^ bufferRx[i - 1];
              }
              if (calcChecksum == bufferRx[dataCounter - 1]) {
                bufferTx[0] = dataCounter - 3; //Length
                calcChecksum = bufferTx[0];

                // Calculate Tx checksum
                for (uint8_t i = 4; i <= dataCounter - 1; i++) {
                  bufferTx[i - 3] = bufferRx[i - 1];
                  calcChecksum = calcChecksum ^ bufferRx[i - 1];
                }

                bufferTx[dataCounter - 3] = calcChecksum;     //CUANDO ESTA BUENO EL CHECK SUM
                Serial.write(0x4A);                           //AQUI SE ENVIA EL PAQUETE
                Serial.write(bufferTx, dataCounter - 2);
                //timerOld = millis();
                serialState = SerialStates::CORRECT_RESPONSE;
              } else {                                        //CUANDO ESTA BUENO EL CHECK SUM
                Serial.write(0x3F);                           //AQUI SE ENVIA EL PAQUETE
                dataCounter = 0;
                //timerOld = millis();
                serialState = SerialStates::WRITE_REQ;


                // DESPUES DE LOS 3 INTENTOS SE INVOCA  taskBeat();
              }
            }

          }


        }
        break;
      case SerialStates::WAITING_RESPONSE:
        {


        }
        break;
      case SerialStates::CORRECT_RESPONSE:
        {

          static uint32_t CorrectCounter = 0;
          const uint32_t CorrectMaxTime = 3000;

          CorrectCounter = millis();

          display.clear();
          display.drawString(9, 0, "CORRECT!");
          display.display();

          if ( (millis() - CorrectCounter) >= CorrectMaxTime)
          {
            display.clear();
            serialState = SerialStates::INIT;
          }
          break;
        }
      default:
        break;
      }
  }
}
void taskBeat()
{
  enum class BeatStates {INIT, BEATING};
  static BeatStates beatlState =  BeatStates::INIT;
  static uint32_t previousMillis = 0;
  const uint32_t interval = 500; //Ya está a 1Hz
  static bool ledState = false;

  const uint32_t LedTaskTime = 3000;
  static uint32_t LedtaskCounter = 0;

  switch (beatlState)
  {
    case BeatStates::INIT:
      {
        digitalWrite(LED1, ledState);
        pinMode(LED1, OUTPUT);
        beatlState = BeatStates::BEATING;
        break;
      }
    case BeatStates::BEATING:
      {
        LedtaskCounter = millis();

        if ( (millis() - previousMillis) >= interval)
        {
          previousMillis = millis();

          if (ledState == false)
          {
            ledState = true;
          } else {
            ledState = false;
          }
          digitalWrite(LED1, ledState);
        }

        if ( (millis() - LedtaskCounter) >= LedTaskTime)
        {
          ledState = false;
          //serialState = SerialStates::INIT;
        }

        break;
      }
    default:
      break;
  }
}
