#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"

#define JOYSTICKX1 A0
#define JOYSTICKY1 A1
#define BUTTON1 4
#define JOYSTICKX2 A2
#define JOYSTICKY2 A3
#define BUTTON2 5
#define LED 3
#define BATTERY A4
#define CE 7
#define CSN 6

// variables for timing and power
unsigned long timer = 0;
double battery_level = 0.0;

// variables for joysticks
double x_axis1 = 0;
double y_axis1 = 0;
double x_axis2 = 0;
double y_axis2 = 0;
double counter = 10;

// variables for communications
int message_data = 0;
byte msg[10];
RF24 radio(CE, CSN);
const byte pipe[6] = "00001";

void setup() {
  pinMode(BUTTON1, INPUT);
  pinMode(BUTTON2, INPUT);
  pinMode(LED, OUTPUT);

  for(int i = 0; i < 10; i++) {
    msg[i] = 0;
  }

  // begin communication
  radio.begin();
  radio.openWritingPipe(pipe);
  radio.stopListening();

  // read initial battery voltage
  for (int i = 0; i < counter; i++) {
    x_axis1 += analogRead(JOYSTICKX1);
    y_axis1 += analogRead(JOYSTICKY1);
    x_axis2 += analogRead(JOYSTICKX2);
    y_axis2 += analogRead(JOYSTICKY2);
    battery_level += analogRead(BATTERY);
  }
  x_axis1 /= counter;
  y_axis1 /= counter;
  x_axis2 /= counter;
  y_axis2 /= counter;
  battery_level /= counter;
  // 12.96 Volts = 1024 analog value
  battery_level = (battery_level / 1024) * 12.96;

  // check voltage is greater than 8 Volts
    while(battery_level < 8.0) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }


  // if battery is greater than 8.0 Volts flash LED 3 times only
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
  }

  // Uncomment when debugging
  //Serial.begin(9600);
}

void loop() {
  // Store start time
  // Code will execute every 4000 microseconds = 250 Hz
  timer = micros();
  x_axis1 = 0;
  y_axis1 = 0;
  x_axis2 = 0;
  y_axis2 = 0;
  battery_level = 0.0;

  // Create a sum of analog readings for each direction + battery
  for (int i = 0; i < counter; i++) {
    x_axis1 += analogRead(JOYSTICKX1);
    y_axis1 += analogRead(JOYSTICKY1);
    x_axis2 += analogRead(JOYSTICKX2);
    y_axis2 += analogRead(JOYSTICKY2);
    battery_level += analogRead(BATTERY);
  }

  // Divide sum to create average analog readings
  x_axis1 /= counter;
  y_axis1 /= counter;
  x_axis2 /= counter;
  y_axis2 /= counter;
  battery_level /= counter;

  // check voltage is greater than 8 Volts
  battery_level = (battery_level / 1024) * 12.96;
  while(battery_level < 8.0) {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);
    delay(100);
  }

  // Move x_axis1 into a int and divide into two bytes for TX
  message_data = (int) x_axis1;
  msg[0] = (byte)(message_data & 0xFF);
  msg[1] = (byte)(message_data >> 8);
  message_data = 0;

  // Move y_axis1 into a int and divide into two bytes for TX
  message_data = (int) y_axis1;
  msg[2] = (byte)(message_data & 0xFF);
  msg[3] = (byte)(message_data >> 8);
  message_data = 0;

  // Read BUTTON1 into message for TX
  msg[4] = digitalRead(BUTTON1);

  // Move x_axis2 into a int and divide into two bytes for TX
  message_data = (int) x_axis2;
  msg[5] = (byte)(message_data & 0xFF);
  msg[6] = (byte)(message_data >> 8);
  message_data = 0;

  // Move y_axis2 into a int and divide into two bytes for TX
  message_data = (int) y_axis2;
  msg[7] = (byte)(message_data & 0xFF);
  msg[8] = (byte)(message_data >> 8);
  message_data = 0;

  // Read BUTTON2 into message for TX
  msg[9] = digitalRead(BUTTON2);

  /* // Uncomment for debugging
  for (int i = 0; i < 10; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.println(msg[i]);
  }
  */
  
  // TX msg to quadcopter
  radio.write(msg, 10);

  // Wait for code to execute at 250 Hz = 4000 microseconds
  while (timer + 4000 > micros()); // wait here for 4000 microseconds
  //Serial.println(micros());
}

// Converts 16 bit number to 2 8 bit numbers for transmitting
void convert_tx(int data, byte num1, byte num2) {
  msg[num1] = (byte)(data & 0xFF);
  msg[num2] = (byte)(data >> 8); 
}

// Converts 10 bit analog input (double after averaging) to a 1000-1500-2000 microsecond pulse
int convert_analog_to_pulse(double analog) {
  int pulse = 0;

  // Set bounds for analog
  analog = (int)analog;
  if(analog < 0) analog = 0;
  if(analog > 1023) analog = 1023;

  pulse = map(analog, 0, 1023, 1000, 2000);
  
  return pulse;
}

