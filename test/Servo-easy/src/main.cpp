#include <Arduino.h>

void setup() {

    // servo.write(0);
    Serial.begin(9600);

    pinMode(6, OUTPUT);
}

void loop() {
    if(Serial.available()) {
        int angle = Serial.readStringUntil('\n').toInt();
        Serial.println(angle);
        analogWrite(6, angle);
    }
}