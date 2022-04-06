#define DEBUG

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
  enum class SerialStates {INIT, WAITING_REQ, READ_REQ, WIRTE_REQ, WAITING_RESPONSE, PROCESS_RESPONSE};
  static SerialStates serialState =  SerialStates::INIT;

  switch (serialState)
  {
    case SerialStates::INIT:
      {
        Serial.begin(115200);
        serialState = SerialStates::WAITING_REQUESTS;
        break;
      }
    case SerialStates::WAITING_REQ:
      {
        if (Serial.available() > 0)
        {
          String dato = Serial.readStringUntil('\n');
          if (dato == "0x2A")
          {
            Serial.write("0x3E")

          }else 
          {
            Serial.write("0xB0")
          }


        }
      }
      break;  // Aqui tambien podriamos borrar el estado de Read ya que elo de Waiting tambien funcionaria como read, no?
    case SerialStates::READ_REQ:
      {


      }
      break;
    case SerialStates::WRITE_REQ:
      {


      }
      break;
    case SerialStates::WAITING_RESPONSE:
      {


      }
      break;
    case SerialStates::PROCESS_RESPONSE:
      {


      }
      break;
  }
default:
  break;
}
}

void taskBeat() {
  enum class BeatStates {INIT, BEATING};
  static BeatStates beatlState =  BeatStates::INIT;
  static uint32_t previousMillis = 0;
  const uint32_t interval = 500;
  static bool ledState = false;

  switch (beatlState) {
    case BeatStates::INIT: {
        digitalWrite(LED1, ledState);
        pinMode(LED1, OUTPUT);
        beatlState = BeatStates::BEATING;
        break;
      }
    case BeatStates::BEATING: {
        if ( (millis() - previousMillis) >= interval) {
          previousMillis = millis();

          if (ledState == false) {
            ledState = true;
          } else {
            ledState = false;
          }
          digitalWrite(LED1, ledState);
        }

        break;
      }
    default:
      break;
  }
}
