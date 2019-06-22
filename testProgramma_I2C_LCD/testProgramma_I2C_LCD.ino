#include <LiquidCrystal_I2C.h>
#include <Wire.h>

LiquidCrystal_I2C lcd(0x27,16,2); 

void setup()
{
  lcd.init();
  lcd.init();                      // initialize the lcd 
  // Print a message to the LCD.
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Bewegen X as");
  lcd.setCursor(0,1);
  lcd.print("Pos: ");
}


void loop()
{
  for (byte i=0; i<10; i++){
    lcd.setCursor(4,1);
    lcd.print(i);    
    delay(1000);
  }
}
