
#include "SSD1306Wire.h"

#define BOMB_OUT 25 // rojo
#define LED_COUNT 26 // blanco
#define UP_BTN 13  //rojo
#define DOWN_BTN 32 //cafe
#define ARM_BTN 33 //naranja
int UPbuttonState = 0;
int lastUPbuttonState = 0;
int DOWNbuttonState = 0;
int lastDOWNbuttonState = 0;
int ARMbuttonState = 0;
int lastARMbuttonState = 0;
bool bPress = false;
bool entro = false;
char u  = UP_BTN;
char d  = DOWN_BTN;
char a  = ARM_BTN;
static char contrasena[7] = {'u', 'u', 'd', 'd', 'u', 'd', 'a'};
static char ingreso[7];
int contador = 0;

bool digitocorrecto(char *contrasenacorrecta, char *contrasensingresasa)
{
  int digito = 0;

  for (int i = 0; i < 7; i++) {
    if (contrasenacorrecta[i] == contrasensingresasa[i])
    {
      digito = digito + 1;
    }
  }
  if (digito == 7)
  {
    return true;
  }
  else
  {
    return false;
  }
}

// Selecciona uno según tu display.
//SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_128_32);
SSD1306Wire display(0x3c, SDA, SCL, GEOMETRY_64_48);

void setup() {
  digitalWrite(BOMB_OUT, LOW);
  digitalWrite(LED_COUNT, LOW);
  pinMode(BOMB_OUT, OUTPUT);
  pinMode(LED_COUNT, OUTPUT);
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(ARM_BTN, INPUT_PULLUP);
  display.init();
  display.setContrast(255);
  display.clear();
  Serial.begin(115200);


}

void loop() {
  static uint8_t counter = 0;
  static uint32_t oldTime = 0;
  uint32_t newTime;
  static int x = 10;
  static int y = 20;
  UPbuttonState = digitalRead(UP_BTN);
  DOWNbuttonState = digitalRead(DOWN_BTN);
  ARMbuttonState = digitalRead(ARM_BTN);

  enum class BombStates {Init, Configuracion, Activa, Desactivada, BOOM};
  static BombStates bombState =  BombStates::Init;

  switch (bombState) {

    case BombStates::Init: {
        counter = 20;
        contador = 0;
        display.init();
        display.setContrast(255);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        digitalWrite(BOMB_OUT, LOW);
        display.clear();
        display.drawString(x, y, String(counter));
        display.display();
        bombState = BombStates::Configuracion;

        break;
      }

    case BombStates::Configuracion: {
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        digitalWrite(BOMB_OUT, LOW);
        display.clear();
        display.drawString(x, y, String(counter));
        display.display();
        if (UPbuttonState != lastUPbuttonState) {
          if (UPbuttonState == HIGH) {
            bPress = true;
            counter++;
            if (counter > 60) {
              counter = 60;
              delay(50);
            }
          }
          lastUPbuttonState = UPbuttonState;
        }

        else if (DOWNbuttonState != lastDOWNbuttonState) {
          if (DOWNbuttonState == HIGH) {
            bPress = true;
            counter--;
            if (counter < 10) {
              counter = 10;
              delay(50);
            }
          }
          lastDOWNbuttonState = DOWNbuttonState;
        }

        else if (ARMbuttonState != lastARMbuttonState) {
          if (ARMbuttonState == HIGH) {
            bPress = true;
            contador = 0;
            bombState = BombStates::Activa;
          }
        }
        lastARMbuttonState = ARMbuttonState;

        digitalWrite(LED_COUNT, HIGH);
        //counter = 20;
        break;

      }

    case BombStates::BOOM: {
        digitalWrite(LED_COUNT, LOW);
        digitalWrite(BOMB_OUT, HIGH);
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);
        display.drawString(x, y, String("BOOM!!"));
        display.display();
        delay (3000);
        bombState = BombStates::Init;

        break;
      }

    case BombStates::Activa: {
        entro = false;
        const uint32_t TiempoLED_COUNT = 500;
        static uint32_t previousTMinus = 0;
        static uint8_t LED_countState = LOW;
        uint32_t currentTMinus = millis();

        if (currentTMinus - previousTMinus >= TiempoLED_COUNT) {
          previousTMinus = currentTMinus;
          if (LED_countState == LOW) {
            LED_countState = HIGH;
          }
          else {
            LED_countState = LOW;
            counter--;
            display.clear();
            display.setTextAlignment(TEXT_ALIGN_LEFT);
            display.setFont(ArialMT_Plain_16);
            display.drawString(x, y, String(counter));
            display.display();
          }
        }

        UPbuttonState = digitalRead(UP_BTN);
        DOWNbuttonState = digitalRead(DOWN_BTN);
        ARMbuttonState = digitalRead(ARM_BTN);
        digitalWrite(LED_COUNT, LED_countState);
        if (UPbuttonState != lastUPbuttonState) {
          if (UPbuttonState == HIGH) {
            ingreso[contador] = 'u';
            Serial.println("up" + contador);
            contador++;
          }
        }

        else if (DOWNbuttonState != lastDOWNbuttonState) {
          if (DOWNbuttonState == HIGH) {
            ingreso[contador] = 'd';
            Serial.println("down" + contador);
            contador++;
          }
        }

        else if (ARMbuttonState != lastARMbuttonState) {
          if (ARMbuttonState == HIGH) {
            ingreso[contador] = 'a';
            Serial.println("arm" + contador);
            contador++;
          }
        }
        lastUPbuttonState = UPbuttonState;
        lastARMbuttonState = ARMbuttonState;
        lastDOWNbuttonState = DOWNbuttonState;
        if (contador == 7) {
          entro = true;
          Serial.print(contador);
          if (digitocorrecto(contrasena, ingreso)) {
            counter = 20;
            Serial.println("correcta");
            bombState = BombStates::Configuracion;

          }

          contador = 0;
        }


        if (counter == 0) {
          Serial.println("entro caso");
          entro = true;
          bombState = BombStates::BOOM;

        }
        if (entro == true) {
          Serial.println("salio caso");
          break;
        }
      }
  }
}
