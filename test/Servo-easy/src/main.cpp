#include <Arduino.h>
#include <Servo.h>
#include <string.h>

#define SERVO_PIN 2

Servo servo;

void setup() {
    servo.attach(SERVO_PIN);
    servo.write(0);
    Serial.begin(9600);
}

void loop() {
    if(Serial.available()) {
        int angle = Serial.readStringUntil('\n').toInt();
        Serial.println(angle);
        servo.write(angle);
    }
}