#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define CE 5
#define CSN 4

// variables for communications
byte msg[10];
RF24 radio(CE, CSN);
const byte pipe[6] = "00001";

int x_axis1 = 0;
int y_axis1 = 0;
byte button1 = 0;
int x_axis2 = 0;
int y_axis2 = 0;
byte button2 = 0;

unsigned long timer = 0;

void setup() {
        // begin communication
        Serial.begin(9600);
        radio.begin();
        radio.openReadingPipe(0, pipe);
        radio.startListening();
}

void loop() {
        timer = micros();
        if(radio.available()) {
                radio.read(msg, 10);

                x_axis1 = (msg[1] << 8)|msg[0];
                y_axis1 = (msg[3] << 8)|msg[2];
                button1 = msg[4];
                x_axis2 = (msg[6] << 8)|msg[5];
                y_axis2 = (msg[8] << 8)|msg[7];
                button2 = msg[9];

                Serial.print("X1: ");
                Serial.println(x_axis1);
                Serial.print("Y1: ");
                Serial.println(y_axis1);
                Serial.print("Button1: ");
                Serial.println(button1);
                Serial.print("X2: ");
                Serial.println(x_axis2);
                Serial.print("Y2: ");
                Serial.println(y_axis2);
                Serial.print("Button2: ");
                Serial.println(button2);
        }
        while(timer + 4000 > micros());
}
