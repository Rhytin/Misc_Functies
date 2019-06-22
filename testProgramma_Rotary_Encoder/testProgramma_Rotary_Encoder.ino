// De conclusie hieruit is dat afhankelijk van de richting 1 van de 2 outputs eerst een nul wordt, de andere volgt, en ze gaan daarna in diezelfde volgerde terug naar 1
// Bij het draaien naar recht veranderd eerst de DT pin (in testopstelling pin3), bij draaien naar links dus eerst de CLK pin (in testopstelling pin2)

byte waardeOud = 0x00000000;
byte waardeNieuw = 0x00000000;
byte temp = 0x00000000;
bool flag = false;
bool geteldCW = false;
bool geteldCCW = false;
int rotatie = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(52, INPUT);
  pinMode(53, INPUT);
  
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(30, OUTPUT);
  pinMode(45, OUTPUT);
  pinMode(44, OUTPUT);

  
  digitalWrite(6, HIGH);
  digitalWrite(8, LOW);
  analogWrite(10, 200);
  digitalWrite(7, HIGH);
  digitalWrite(30, HIGH);
}

void loop() {

  temp = ((digitalRead(52) << 1) | digitalRead(53));
  waardeNieuw = (waardeOud << 2) | temp;
  waardeOud = temp;

  if (waardeNieuw == 0b00001110 && flag == false){
      geteldCW = true;
  }
  if ((waardeNieuw == 0b00000001 || waardeNieuw == 0b00000111) && geteldCW){
    geteldCW = false;
    rotatie++;
    Serial.println(rotatie);
  }

  
  if (waardeNieuw == 0b00000100 && flag){
      geteldCCW = true;
  }
  if ((waardeNieuw == 0b00001011 || waardeNieuw == 0b00001101) && geteldCCW){
    geteldCCW = false;
    rotatie--;
    Serial.println(rotatie);
  }


  if (rotatie > 700 && (flag == false)){
    digitalWrite(6, LOW);
    digitalWrite(7, LOW);
    digitalWrite(8, HIGH);
    analogWrite(10, 200);
    delay(5000);
    digitalWrite(6, HIGH);
    flag = true;
    
  }
  if (rotatie == 0 && flag){
    digitalWrite(6, LOW);
    Serial.println("Einde");
    while(1){};
  }
}
