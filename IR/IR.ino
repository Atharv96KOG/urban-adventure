int IR = 10; 
int buzzer = 9;
int dis = 0;
int led =8;

void setup() {
  pinMode(IR, INPUT); 
  pinMode(buzzer, OUTPUT); 
  pinMode(led, OUTPUT); 
  Serial.begin(9600); 
}

void loop() {
  dis = digitalRead(IR); 
  if (dis == 0) {
    Serial.println("object is detected....");
    digitalWrite(buzzer, HIGH); 
    digitalWrite(led, HIGH);
    delay(1000);
  } else { 
    Serial.println("object isn't detected....");
    digitalWrite(buzzer, LOW); 
     digitalWrite(led, LOW);
  }
  delay(1000); 
}

