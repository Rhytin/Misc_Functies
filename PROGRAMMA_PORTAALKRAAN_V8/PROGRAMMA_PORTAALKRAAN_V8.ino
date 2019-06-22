#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2); 

#define stroomInterruptPin  2
#define kalibreerPin        3
#define dirPin1             7
#define dirPin2             8
#define motorEPin           6
#define relaisXPin          30
#define relaisYPin          32
#define relaisZPin          31
#define relaisMPin          33
#define motorSterktePin     10
#define joystickKnop        40
#define joystickX           A8
#define joystickY           A9
#define joystickDelay       350
// geen line 21 foei rick foei
#define knopDelay           10
#define microSwitchX        22
#define microSwitchY        23
#define microSwitchZ        24

#define maxUserPosX         5
#define maxUserPosY         9
#define afwijking           150
#define FALSE               0
#define TRUE                !FALSE
#define ticksPerSprongX     10
#define ticksPerSprongY     15
#define ticksPerSprongZ     1

#define dempingsStappen     5
#define motorSterkte        180
#define motorDemping        15
#define drempel             20
#define drempelSprong       4

#define kalibreerXSterkte   180
#define kalibreerYSterkte   200
#define kalibreerZSterkte   200
#define kalibreerMSterkte   150

volatile byte timerFlag = 0;
volatile byte staat = 0;
volatile bool elektromagneet = false;
volatile int hPosX = 0;
volatile int hPosY = 0;
volatile int hPosZ = 0;
volatile int bPosX = 0;
volatile int bPosY = 0;
volatile int bPosZ = 1295;
volatile int ePosX = 0;
volatile int ePosY = 0;
volatile int ePosZ = 0;
volatile int encoderXPlusPin = 52;  //  CLK
volatile int encoderXMinPin = 53;   //  DT
volatile int encoderYPlusPin = 51;  //  CLK
volatile int encoderYMinPin = 50;   //  DT
volatile int encoderZPlusPin = 48;  //  CLK
volatile int encoderZMinPin = 49;   //  DT
bool begonnen = false;
bool begonnen2 = false;
volatile bool kalibreerInterrupt = false;

void leesJoystick(int*, int*);
bool leesKnop(byte);
void motorFunctie(bool, byte, bool, byte);
// void noodStop();


void setup(){
  Serial.begin(9600);
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();

  //attachInterrupt(digitalPinToInterrupt(stroomInterruptPin), noodStop, FALLING);
  attachInterrupt(digitalPinToInterrupt(kalibreerPin), kalibreer, RISING);
  
  TCCR0A = 0;// set entire TCCR0A register to 0
  TCCR0B = 0;// same for TCCR0B
  TCNT0  = 0;//initialize counter value to 0
  // set compare match register for 1khz increments
  OCR0A = 249;// = (16*10^6) / (1000*64) - 1 (must be <256)
  // turn on CTC mode
  TCCR0A |= (1 << WGM01);
  // Set CS01 and CS00 bits for 64 prescaler
  TCCR0B |= (1 << CS01) | (1 << CS00);   
  // enable timer compare interrupt
  TIMSK0 |= (1 << OCIE0A);

  sei();

  pinMode(stroomInterruptPin, INPUT);
  pinMode(kalibreerPin, INPUT);
  pinMode(dirPin1, OUTPUT);
  pinMode(dirPin2, OUTPUT);
  pinMode(motorEPin, OUTPUT);
  pinMode(relaisXPin, OUTPUT);
  pinMode(relaisYPin, OUTPUT);
  pinMode(relaisZPin, OUTPUT);
  pinMode(relaisMPin, OUTPUT);
  pinMode(motorSterktePin, OUTPUT);
  pinMode(joystickKnop, INPUT);
  pinMode(joystickX, INPUT);
  pinMode(joystickY, INPUT);
  pinMode(microSwitchX, INPUT);
  pinMode(microSwitchY, INPUT);
  pinMode(microSwitchZ, INPUT);
  pinMode(encoderXPlusPin, INPUT);
  pinMode(encoderXMinPin, INPUT);
  pinMode(encoderYPlusPin, INPUT);
  pinMode(encoderYMinPin, INPUT);
  pinMode(encoderZPlusPin, INPUT);
  pinMode(encoderZMinPin, INPUT);
}

void loop() {
  byte gebruiksArray[3] = {0,0,0}; 


    if (staat == 0){
      elektromagneet = EEPROM.read(0);

    if (elektromagneet){
      digitalWrite(relaisMPin, LOW);
    }
    else{
      digitalWrite(relaisMPin, HIGH);
    }

    staat = 10;
    }
    
    /*
    if (staat == 0){ //IDLE
      // GEGEVENS CONTROLEREN
      lcd.clear();
      lcd.print("Data uitlezen");
      elektromagneet = (EEPROM.read(0) >> 7);
      staat = (EEPROM.read(0) << 1);
      staat = (staat >> 1);
      hPosX = ((EEPROM.read(1) << 8) + EEPROM.read(2));
      hPosY = ((EEPROM.read(3) << 8) + EEPROM.read(4));
      hPosZ = ((EEPROM.read(5) << 8) + EEPROM.read(6));
      bPosX = ((EEPROM.read(7) << 8) + EEPROM.read(8));
      bPosY = ((EEPROM.read(9) << 8) + EEPROM.read(10));
      ePosX = ((EEPROM.read(11) << 8) + EEPROM.read(12));
      ePosY = ((EEPROM.read(13) << 8) + EEPROM.read(14));
      
      lcd.setCursor(0,0);
      lcd.print("Data uitgelezen");
      lcd.setCursor(0,1);
      lcd.print("Staat: ");
      lcd.setCursor(7,1);
      lcd.print(staat);
      Serial.print("Staat 0, data uitlezen gelukt, staat is: ");
      Serial.println(staat);
      static int teller = 0;
      while(teller < 2000){
        teller += timerFlag;
        timerFlag = 0;
      }
      teller = 0;
    }
    */
      
    if (staat == 1){  //GEBRUIKERSINPUT
      //1.1 TONPOSITIE OPVRAGEN + PRINTEN OP LCD
      //1.2 TONPOSITIE OPSLAAN ALS KNOP IS INGEDRUKT
      //1.3 EINDPOSITIE OPVRAGEN + PRINTEN OP LCD
      //1.4 EINDPOSITIE OPSLAAN ALS KNOP IS INGEDRUKT
      if (!begonnen2){
        timerFlag = 0;
        begonnen2 = true;
        lcd.clear();
        /*
        Serial.println("Eerste keer staat 1 in gegaan.");
        Serial.print("Huidige positie X is: ");
        Serial.println(hPosX);
        Serial.print("Huidige positie Y is: ");
        Serial.println(hPosY);
        
        Serial.print("Tonpositie X is: ");
        Serial.println(bPosX);
        Serial.print("Tonpositie Y is: ");
        Serial.println(bPosY);
        
        Serial.print("Eindpositie X is: ");
        Serial.println(ePosX);
        Serial.print("Eindpositie Y is: ");
        Serial.println(ePosY);
        */
      }

      
      static int switchVar = 1;
      static bool uitgeweest = false;
      int xas = 512;
      int yas = 512;
      static int posx=0;
      static int posy=0;
      static int posxTeller = 0;
      static int posyTeller = 0;
      static int knopTeller = 0;
      if (timerFlag !=0){
        if (posxTeller != 0){
          posxTeller += timerFlag;
        }
        if (posyTeller != 0){
          posyTeller += timerFlag;
        }
        if (knopTeller != 0){
          knopTeller += timerFlag;
        }
        timerFlag = 0;
      }
      if (posxTeller > joystickDelay){
        posxTeller = 0;
      }
      if (posyTeller > joystickDelay){
        posyTeller = 0;
      }
      if (knopTeller > knopDelay){
        knopTeller = 0;
      }
      
      lcd.setCursor(0,1);
      lcd.print("X=");
      if (posx < 10){
        lcd.print(" ");
      }
      lcd.print(posx);
      lcd.setCursor(6,1);
      lcd.print("Y=");
      if (posy < 10){
        lcd.print(" ");
      }
      lcd.print(posy);
         
      leesJoystick(&xas, &yas);
      if (xas > (512+afwijking) && posxTeller == 0){
        posxTeller = 1;
        posx++;
      }
      if (xas < (512-afwijking) && posxTeller == 0){
        posxTeller = 1;
        posx--;
      }
      if (yas > (512+afwijking) && posyTeller == 0){
        posyTeller = 1;
        posy--;
      }
      if (yas < (512-afwijking) && posyTeller == 0){
        posyTeller = 1;
        posy++;
      }
      if (posx < 1){
        posx=1;
      }
      if (posx > maxUserPosX){
        posx=maxUserPosX;
      }
      if (posy < 1){
        posy=1;
      }
      if (posy > maxUserPosY){
        posy=maxUserPosY;
      }
      switch(switchVar){
        case 1:
          if (!begonnen){
            lcd.clear();
            begonnen = true;
          }
          lcd.setCursor(0,0);
          lcd.print("Positie ton?");

          if (leesKnop(joystickKnop) == true){
            //einde state
            switchVar = 2;
            lcd.clear();
            bPosX = ((posx-1)*ticksPerSprongX) + 2;
            bPosY = ((posy-1)*ticksPerSprongY) + 4;
            posx=0;
            posy=0;
            posxTeller = 0;
            posyTeller = 0;
            knopTeller = 1;
            begonnen = false;
          }
          break;
          
          case 2:
          if (!begonnen){
            lcd.clear();
            begonnen = true;
          }
          lcd.setCursor(0,0);
          lcd.print("Eindpositie?");

          if ((!leesKnop(joystickKnop)) && knopTeller == 0){
            uitgeweest = true;
          }
          
          if (leesKnop(joystickKnop) && knopTeller == 0 && uitgeweest == true){
            // einde state
            switchVar = 1;
            lcd.clear();
            ePosX = ((posx-1)*ticksPerSprongX) + 2;
            ePosY = ((posy-1)*ticksPerSprongY) + 4;
            posx=0;
            posy=0;
            posxTeller = 0;
            posyTeller = 0;
            uitgeweest = false;
            begonnen = false;
            begonnen2 = false;
            staat = 2;
            timerFlag = 0;
            Serial.println("Einde staat 1");
            Serial.print("Tonpositie X is: ");
            Serial.println(bPosX);
            Serial.print("Tonpositie Y is: ");
            Serial.println(bPosY);
            
            Serial.print("Eindpositie X is: ");
            Serial.println(ePosX);
            Serial.print("Eindpositie Y is: ");
            Serial.println(ePosY);
                
          }
          break;
      }
    }
    else if (staat >= 2 && staat <=9){  //X-AS NAAR TONPOSITIE
      
      gebruiksArray[0] = 0;
      gebruiksArray[1] = 1;
 
    //case 3 : //Y-AS NAAR TONPOSITIE
      if (staat == 3){
        gebruiksArray[0] = 1;
        gebruiksArray[1] = 1; 
      }
    //case 4 : //Z-AS NAAR TONPOSITIE
      if (staat == 4){
        gebruiksArray[0] = 2;
        gebruiksArray[1] = 1; 
      }
    //case 5 ://Z-AS NAAR NUL
      if (staat == 5){
        gebruiksArray[0] = 2;
        gebruiksArray[1] = 2;
      }
    //case 6 ://X-AS NAAR EINDPOSITIE
      if (staat == 6){
        gebruiksArray[0] = 0;
        gebruiksArray[1] = 2;
      }
    //case 7 ://Y-AS NAAR EINDPOSITIE
      if (staat == 7){
        gebruiksArray[0] = 1;
        gebruiksArray[1] = 2;
      }
    //case 8 ://Z-AS NAAR EINDPOSITIE
      if (staat == 8){
        gebruiksArray[0] = 2;
        gebruiksArray[1] = 1;
      }
    //case 9 :// Z-AS NAAR NUL
      if (staat == 9){
        gebruiksArray[0] = 2;
        gebruiksArray[1] = 2;
      }
      //2.1 MOTOR X AANZETTEN (IN DE JUISTE RICHTING)
      //2.2 ALS ER POSITIEVERANDERINNG WORDT GEMETEN --> NIEUWE POSITIE OPSLAAN
      //2.3 ALS HUIDIGE POSITIE == TONPOSITIE --> MOTOR X UITZETTEN
      
      if (begonnen2 == false){
        lcd.print("Staat: ");
        lcd.print(staat);
        begonnen2 = true;
        Serial.println("Enter staat 2");
        Serial.println(bPosX);
        Serial.println(bPosY);
        Serial.println(ePosX);
        Serial.println(ePosY);

        lcd.setCursor(0,1);
        if (gebruiksArray[0] == 0){
          lcd.print("X = ");
          gebruiksArray[2] = ticksPerSprongX;
        }
        else if (gebruiksArray[0] == 1){
          lcd.print("Y = ");
          gebruiksArray[2] = ticksPerSprongY;
        }
        else if (gebruiksArray[0] == 2){
          lcd.print("Z = ");
          gebruiksArray[2] = ticksPerSprongZ;
        }
      }

      //bovenste is de juiste, 2de is voor testen met 1 encoder
      int*XYZArray[15] = {&hPosX, &bPosX, &ePosX, &encoderXPlusPin, &encoderXMinPin, &hPosY, &bPosY, &ePosY, &encoderYPlusPin, &encoderYMinPin,&hPosZ, &bPosZ, &ePosZ, &encoderZPlusPin, &encoderZMinPin};
      //int*XYZArray[15] = {&hPosX, &bPosX, &ePosX, &encoderXPlusPin, &encoderXMinPin, &hPosY, &bPosY, &ePosY, &encoderXPlusPin, &encoderXMinPin,&hPosZ, &bPosZ, &ePosZ, &encoderXPlusPin, &encoderXMinPin};
      
      int a = 5*(gebruiksArray[0]);
      int b = gebruiksArray[1];
      int tempPos=0;

      int tempAfstand = sqrt(pow((*XYZArray[(a+b)] - *XYZArray[a]), 2));

      if (staat == 5 || staat == 9){
        motorFunctie(true, 2, false, 250);
        while(!leesKnop(microSwitchZ)){
          
        }
        motorFunctie(false, 2, false, 0);
        hPosZ = 0;
      }
      
      if((*XYZArray[a]) < *XYZArray[(a+b)]){
        tempAfstand = *XYZArray[(a+b)] - *XYZArray[a];
        Serial.println(tempAfstand);
        for(byte i=0; i<dempingsStappen; i++){
          Serial.println("For loop door");
          while(((tempAfstand <= (drempel-i*drempelSprong) && tempAfstand > (drempel-(i+1)*drempelSprong)) || tempAfstand > drempel) && kalibreerInterrupt == false){
            if (!begonnen){
              motorFunctie(true, gebruiksArray[0], true, motorSterkte-(i*motorDemping));
              begonnen = true;
            }
            tempPos = *XYZArray[a];
            byte waardeOud = 0;
            byte waardeNieuw = 0;
            bool geteldCW = false;
            while (tempPos == *XYZArray[a]){
              byte temp = ((digitalRead(*XYZArray[a+3]) << 1) | digitalRead(*XYZArray[a+4]));
              waardeNieuw = (waardeOud << 2) | temp;
              waardeOud = temp;
            
              if (waardeNieuw == 0b00001110){
                geteldCW = true;
              }
              if ((waardeNieuw == 0b00000001 || waardeNieuw == 0b00000111) && geteldCW){
                geteldCW = false;
                *XYZArray[a] = *XYZArray[a]+1;
              }
            }

            int tempPos = ((*XYZArray[a])/ (gebruiksArray[2]));
            if (staat != 4 && staat != 8){
              lcd.setCursor(4,1);
              lcd.print(tempPos);
              lcd.print(" ");
            }
            tempAfstand = *XYZArray[a+b] - *XYZArray[a];
          }

          if(kalibreerInterrupt){
            break;
          }
          
          begonnen = false;
        }
      }
      else if(*XYZArray[a] >  *XYZArray[(a+b)]){
        tempAfstand = *XYZArray[a] - *XYZArray[(a+b)];
        for(byte i=0; i<dempingsStappen; i++){
          int tempPos=0;
          while(((tempAfstand <= (drempel-i*drempelSprong) && tempAfstand > (drempel-(i+1)*drempelSprong)) || tempAfstand > drempel)  && kalibreerInterrupt == false){
            if (!begonnen){
              motorFunctie(true, gebruiksArray[0], false, motorSterkte-(i*motorDemping));
              begonnen = true;
            }
            tempPos = *XYZArray[a];
            byte waardeOud = 0;
            byte waardeNieuw = 0;
            bool geteldCCW = false;
            while (tempPos == *XYZArray[a]){
              byte temp = ((digitalRead(*XYZArray[a+3]) << 1) | digitalRead(*XYZArray[a+4]));
              waardeNieuw = (waardeOud << 2) | temp;
              waardeOud = temp;
              
              if (waardeNieuw == 0b00000100){
                  geteldCCW = true;
              }
              if (waardeNieuw == 0b00001011 || waardeNieuw == 0b00001101 && geteldCCW){
                geteldCCW = false;
                *XYZArray[a] = *XYZArray[a] - 1;
              }
            }
            
            int tempPos = ((*XYZArray[a])/ gebruiksArray[2]);
            lcd.setCursor(4,1);
            lcd.print(tempPos);
            lcd.print(" ");
            
            tempAfstand = *XYZArray[a] - *XYZArray[a+b];
          }

          if(kalibreerInterrupt){
            break;
          }
          
          begonnen = false;
        }
      }
      if (tempAfstand == 0){
        Serial.print("Einde staat ");
        Serial.println(staat);
        // einde staat
        motorFunctie(false, gebruiksArray[0], true, 0);
        begonnen = false;
        begonnen2 = false;
        timerFlag = 0;
        if (staat == 4){
          digitalWrite(relaisMPin, LOW);
          elektromagneet = true;
        }
        if (staat == 8){
          digitalWrite(relaisMPin, HIGH);
          elektromagneet = false;
        }
        lcd.clear();
        if (staat == 9){
          staat = 1;
        }
        else {
          staat++;
        }
        //while(true){};
      }
    }
      
    else if (staat == 10){//KALIBREREN
      if (!begonnen){
        Serial.println("Staat 10 in gegaan, kalibreren...");
        lcd.clear();
        lcd.print("Kalibreren...");
        begonnen = true;
      }
            
      kalibreerInterrupt = false;

      Serial.println("begonnen");
      motorFunctie(true, 0, false, kalibreerXSterkte);
      delay(5000);
      lcd.setCursor(0,1);
      lcd.print("X-as");
      while(!leesKnop(microSwitchX)){
      }
      motorFunctie(false, 0, false, 0);

      motorFunctie(true, 1, false, kalibreerYSterkte);
      lcd.setCursor(0,1);
      lcd.print("Y-as");
      while(!leesKnop(microSwitchY)){
      }
      motorFunctie(false, 1, false, 0);

      motorFunctie(true, 2, false, kalibreerZSterkte);
      lcd.setCursor(0,1);
      lcd.print("Z-as");
      while(!leesKnop(microSwitchZ)){
      }
      motorFunctie(false, 2, false, 0);

      if (elektromagneet){
        staat = 8;
      }

      hPosX = 0;
      hPosY = 0;
      hPosZ = 0;
      staat = 1;
      begonnen = false;
    }
    
    //default:
      // statements
      //Serial.println("Default HAHAHAHAH");
      //break;
  }

void motorFunctie(bool aanUit, byte motor, bool richting, byte sterkte){
  byte relaisArray[3] = {relaisXPin, relaisYPin, relaisZPin};

  if (motor == 2){
    sterkte = kalibreerZSterkte;
  }

  if (richting){
    analogWrite(dirPin1, sterkte);
    analogWrite(dirPin2, 0);
  }
  else{
    analogWrite(dirPin1, 0);
    analogWrite(dirPin2, sterkte);
  }

  if (aanUit){
    digitalWrite(relaisArray[motor], HIGH);
  }
  else{
    digitalWrite(relaisArray[motor], LOW);
  }
}

/*
void noodStop(){
  digitalWrite(motorEPin, LOW);
  lcd.clear();
  lcd.print("NOODSTOP");
  if (elektromagneet){
    EEPROM.write(0, (staat & 0b10000000));
  }
  else{
    EEPROM.write(0, staat);
  }
  if (hPosX > 255){
    EEPROM.write(1, hPosX >> 8);
  }
  EEPROM.write(2, hPosX);
  
  if (hPosY > 255){
    EEPROM.write(3, hPosY >> 8);
  }
  EEPROM.write(4, hPosY);
  
  if (hPosZ > 255){
    EEPROM.write(5, hPosZ >> 8);
  }
  EEPROM.write(6, hPosZ);


  if (bPosX > 255){
    EEPROM.write(7, bPosX >> 8);
  }
  EEPROM.write(8, bPosX);
  
  if (hPosY > 255){
    EEPROM.write(9, bPosY >> 8);
  }
  EEPROM.write(10, bPosY);
  
  if (hPosZ > 255){
    EEPROM.write(11, bPosZ >> 8);
  }
  EEPROM.write(12, bPosZ);


  if (ePosX > 255){
    EEPROM.write(13, hPosX >> 8);
  }
  EEPROM.write(14, ePosX);
  
  if (hPosY > 255){
    EEPROM.write(15, ePosY >> 8);
  }
  EEPROM.write(16, ePosY);
  
  if (hPosZ > 255){
    EEPROM.write(17, ePosZ >> 8);
  }
  EEPROM.write(18, ePosZ);

  lcd.setCursor(0,1);
  lcd.print("Data opgeslagen");
}
*/

void leesJoystick(int *xas, int *yas){
  *xas = analogRead(joystickX);
  *yas = analogRead(joystickY);
}

bool leesKnop(byte knopPin) {
  for(byte i=0; i<10; i++){
    if (digitalRead(knopPin) != 0){
      return(true); 
    }
  }
  return(false);
}

void kalibreer(){
  kalibreerInterrupt = true;
  staat = 10;
}

ISR(TIMER0_COMPA_vect){
  timerFlag++;
}
