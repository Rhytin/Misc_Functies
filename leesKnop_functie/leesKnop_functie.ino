bool leesKnop(byte);

void setup(){
  Serial.begin(9600);
}

void loop(){
  delay(1000);
  Serial.println(leesKnop(A0));
  Serial.println(leesKnop(A1));
  Serial.println(leesKnop(A2));
  Serial.println("");
}

bool leesKnop(byte knopPin) {
  for(byte i=0; i<8; i++){
    if (digitalRead(knopPin) != 0){
      return(false); 
    }
  }
  return(true);
}
