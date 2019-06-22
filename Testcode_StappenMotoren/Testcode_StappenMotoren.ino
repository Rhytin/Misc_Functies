/* 
   De MS1 MS2 en MS3 pins zijn om hlave stappen etc in te stellen, die gebruiken we dus niet want 1 stap is goed genoeg
   Er moet een condensator van 47 microFarad tussen de Vmot(VMotor, motorvoeding), en de bijhorende GND
   De 1A en 1B moeten op dezelfde coil zitten
   Met de dir kies je de richting, en door de step pin even hoog te zetten wordt er één stap gezet
   Reset en sleep moeten aan elkaar doorverbonden zijn
   De enable pin moet aan begin high gemaakt worden, zodat de stappenmotor geen rare dingen gaat doen
   Rechtsom draaien op de stappenmotor drivers is het limiet van stroom hoger leggen, linksom lager
*/


#define stapPin 9
#define richtingPin 10 
#define enablePin 8
 
void setup() {
  pinMode(stapPin, OUTPUT); 
  pinMode(richtingPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, HIGH);
  delay(2000);
  digitalWrite(enablePin, LOW);

  
}

void loop() {

  delay(1000);
  
  digitalWrite(richtingPin,LOW);
  for(int x = 0; x < 200; x++) {    // Meestal 200 stappen voor 1 volledige rotatie 
    digitalWrite(stapPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stapPin,LOW); 
    delayMicroseconds(4500); 
  }
  
  delay(1000);
  
  digitalWrite(richtingPin,HIGH);
  for(int x = 0; x < 401; x++) {    // 2 complete rotaties
    digitalWrite(stapPin,HIGH);
    delayMicroseconds(500);
    digitalWrite(stapPin,LOW);
    delayMicroseconds(4500);
  }
}
