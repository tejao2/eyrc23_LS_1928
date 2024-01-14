#include "Wire.h"
#include <MPU6050_light.h>

#define enA 6  
#define in1 A2
#define in2 A3
MPU6050 mpu(Wire);
unsigned long timer = 0;
float u = 0;
float roll_ini = 0;
float yaw_ini = 0;
float roll_pos_err = 0;
float yaw_pos_err = 0;
float roll_vel_err = 0;
float yaw_vel_err = 0;
float roll_itgl_err = 0;
float yaw_itgl_err = 0;

void dc_motor_init(){
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
}

void motor_control(int pwm) {
  
  if (pwm < 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    pwm = -pwm;
  } 
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  if(pwm > 255)
   pwm = 255;
  if (pwm < -255)
   pwm = -255;
  analogWrite(enA, pwm);
}

void setup() {
  Serial.begin(9600);
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  dc_motor_init();
roll_ini = mpu.getAngleX();
yaw_ini = mpu.getAngleZ();
}

void loop() {
  mpu.update();
  
  if((millis()-timer)>10){ // print data every 10ms
  Serial.print("X : ");
  roll_pos_err = roll_ini - mpu.getAngleX()
  Serial.print(roll_pos_err);
  //Serial.print("\tY : ");
  //Serial.print(mpu.getAngleY());
  Serial.print("\tZ : ");
  yaw_pos_err = yaw_ini - mpu.getAngleZ();
  Serial.println(yaw_pos_err);
  timer = millis();  
  }
 u = 1 * roll_pos_err + 1 * yaw_pos_err;
 motor_control(fabs(u));

}