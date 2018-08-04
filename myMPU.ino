#include <Wire.h>

int gyro_x, gyro_y, gyro_z;
long acc_x, acc_y, acc_z, acc_total_vector;
int temperature;
int gyro_x_cal, gyro_y_cal, gyro_z_cal;
float angle_pitch, angle_roll, angle_yaw;
float angle_pitch_acc, angle_roll_acc;
bool set_gyro_angles = false;
float angle_pitch_output, angle_roll_output, angle_yaw_output;
int angle_pitch_buffer, angle_roll_buffer;
double timer;

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(115200);
  angle_pitch = 0;
  angle_roll = 0;
  angle_yaw = 0;
  angle_pitch_output = 0;
  angle_roll_output = 0;
  angle_yaw_output = 0;
  
  //Activate the MPU-6050
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x6B);                                                    //Send the requested starting register
  Wire.write(0x00);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the accelerometer (+/-8g)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1C);                                                    //Send the requested starting register
  Wire.write(0x10);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  //Configure the gyro (500dps full scale)
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x1B);                                                    //Send the requested starting register
  Wire.write(0x08);                                                    //Set the requested starting register
  Wire.endTransmission();                                              //End the transmission
  delay(3000);
  /*for(int i = 0; i < 2000; i++) {
     read_mpu_6050_data();
     gyro_x_cal += gyro_x;
     gyro_y_cal += gyro_y;
     gyro_z_cal += gyro_z;
     delay(3); 
  }*/
  
  gyro_x_cal /= 2000;
  gyro_y_cal /= 2000;
  gyro_z_cal /= 2000;
  timer = micros();
}

void loop() {
  // put your main code here, to run repeatedly:
 
  read_mpu_6050_data();                                     
  /*Serial.print(gyro_x);
  Serial.print("\t");  
  Serial.print(gyro_y);
  Serial.print("\t");
  Serial.print(gyro_z);
  Serial.print("\t");
  Serial.print(acc_x);
  Serial.print("\t");
  Serial.print(acc_y);
  Serial.print("\t");
  Serial.print(acc_z);
  Serial.print("\t");
  Serial.print(temperature);
  Serial.print("\t");
  Serial.println(""); */
  
  /*gyro_x -= 135.8;
  gyro_y -= -246.9;
  gyro_z -= -229.5;
  */
  //angle_pitch += (gyro_x/65.5)*0.004;
  //angle_roll += (gyro_y/65.5)*0.004;
  
  angle_pitch += gyro_x * 0.0000611;
  angle_roll += gyro_y * 0.0000611;
  angle_yaw += gyro_z * 0.0000611;
  
  angle_pitch += angle_roll * sin(gyro_z * 0.000061*3.142/180);
  angle_roll -= angle_pitch * sin(gyro_z * 0.000061*3.142/180);
  
  acc_total_vector = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));
  angle_pitch_acc = asin((float)acc_y/acc_total_vector)/(3.142/180);
  angle_roll_acc = asin((float)acc_x/acc_total_vector)/(-3.142/180); 
  
  angle_pitch_acc -= 0.0;
  angle_roll_acc -= 0.0;
  
  if(set_gyro_angles) {
    angle_pitch = angle_pitch * 0.9996 + angle_pitch_acc * 0.0004;
    angle_roll = angle_roll * 0.9996 + angle_roll_acc * 0.0004;
  } else {
    angle_pitch = angle_pitch_acc;
    angle_roll = angle_roll_acc;
    set_gyro_angles = true; 
  }
  
  angle_pitch_output = angle_pitch_output * 0.9 + angle_pitch * 0.1;
  angle_roll_output = angle_roll_output * 0.9 + angle_roll * 0.1;
  angle_yaw_output = angle_yaw_output * 0.9 + angle_yaw * 0.1;
  //angle_yaw_output *= -1;
  
  Serial.println(angle_roll_acc);
  /*
  Serial.print("Pitch: ");
  Serial.print(angle_pitch_output);
  Serial.print("  ");
  Serial.print("Roll: ");
  Serial.print(angle_roll_output);
  Serial.print("  Yaw: ");
  Serial.print(angle_yaw_output);
  Serial.println("");*/
  /*
  angle_pitch_buffer = angle_pitch_output * 10;
  if(angle_pitch_buffer < 0)Serial.print("-");                          //Print - if value is negative
  else Serial.print("+");                                               //Print + if value is negative
  
  Serial.print(abs(angle_pitch_buffer)/1000);    //Print first number
  Serial.print((abs(angle_pitch_buffer)/100)%10);//Print second number
  Serial.print((abs(angle_pitch_buffer)/10)%10); //Print third number
  Serial.print(".");                             //Print decimal point
  Serial.print(abs(angle_pitch_buffer)%10);      //Print decimal number
  
  angle_roll_buffer = angle_roll_output * 10;
  Serial.print("  ");
  if(angle_roll_buffer < 0)Serial.print("-");                           //Print - if value is negative
  else Serial.print("+");                                               //Print + if value is negative
  
  Serial.print(abs(angle_roll_buffer)/1000);    //Print first number
  Serial.print((abs(angle_roll_buffer)/100)%10);//Print second number
  Serial.print((abs(angle_roll_buffer)/10)%10); //Print third number
  Serial.print(".");                            //Print decimal point
  Serial.print(abs(angle_roll_buffer)%10);      //Print decimal number
  Serial.println("");
  //Serial.print("Gyro Cal X: ");
  //Serial.print(gyro_x_cal);
  */
  while(micros() - timer < 4000);
  timer = micros();
}

void read_mpu_6050_data(){                                             //Subroutine for reading the raw gyro and accelerometer data
  Wire.beginTransmission(0x68);                                        //Start communicating with the MPU-6050
  Wire.write(0x3B);                                                    //Send the requested starting register
  Wire.endTransmission();                                              //End the transmission
  Wire.requestFrom(0x68,14);                                           //Request 14 bytes from the MPU-6050
  while(Wire.available() < 14);                                        //Wait until all the bytes are received
  acc_x = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_x variable
  acc_y = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_y variable
  acc_z = Wire.read()<<8|Wire.read();                                  //Add the low and high byte to the acc_z variable
  temperature = Wire.read()<<8|Wire.read();                            //Add the low and high byte to the temperature variable
  gyro_x = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_x variable
  gyro_y = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_y variable
  gyro_z = Wire.read()<<8|Wire.read();                                 //Add the low and high byte to the gyro_z variable

}

