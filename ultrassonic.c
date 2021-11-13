#include <NewPing.h>

#define TRIGGER_PIN  12
#define ECHO_PIN     11  
#define MAX_DISTANCE 200

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (sonar.ping_cm() < 50) {
    Serial.println("A distancia do sonar é menor do que 50 cm" );
    delay(1000);
  }

  else{
    Serial.println("A distancia do sonar é maior do que 50 cm");
    delay(1000);
  }
}
