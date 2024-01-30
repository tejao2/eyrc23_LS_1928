#include "Wire.h"
#include <MPU6050_light.h>
#include <SPI.h>
#include <RF24.h>

#define enA 6  
#define in1 A2
#define in2 A3
#define enB 5
#define in3 9
#define in4 4
#define curr_out A7
#define buzzer 7
#define r_led A0
#define g_led A1

MPU6050 mpu(Wire);
RF24 radio(10, 8);
int check;
unsigned long timer = 0;
unsigned long s_timer = 0;
const byte add_1 = 5;
const byte add_2 = 10;
char msg_1 ;
float u = 0;
float roll_ini = 0;
float yaw_ini = 0;
float roll_pos_err = 0;
float yaw_pos_err = 0;
float roll_vel_err = 0;
float yaw_vel_err = 0;
float roll_itgl_err = 0;
float yaw_itgl_err = 0;
int det_strt_time = 0;

void dc_motor_init(){
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
}

void bo_motor_init(){
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

void det_init()
{
  pinMode(curr_out,INPUT);
  pinMode(r_led, output);
  pinMode(g_led, output);
  pinMode(buzzer,OUTPUT);
}


void motor_control(int pwm) {
 
 //Serial.println(pwm);
  if (pwm < 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    pwm = -pwm;
  } 
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  if(pwm>0)
   analogWrite(enA, pwm+60);
}

void forward(){
  analogWrite(enB, 65);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  
}

void backward(){
  analogWrite(enB, 65);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);  
}

void damaged(int strt_time)
{
  analogWrite(r_led, 255);
  int dur = millis() - strt_time;
  if(dur <= 1000 || (dur > 2000 && dur <= 3000))
   digitalWrite(buzzer, LOW);
  else if(dur > 1000 && dur <= 2000)
   digitalWrite(buzzer, HIGH);
  else if(dur > 3000)
  {
    analogWrite(r_led, 0);
    digitalWrite(buzzer, HIGH);
  }
}

void safe(int det_timer)
{
  analogWrite(g_led, 255);
  int dur = millis() - strt_time;
  if(dur <= 1000 || (dur > 2000 && dur <= 3000))
   digitalWrite(buzzer, LOW);
  else if(dur > 1000 && dur <= 2000)
   digitalWrite(buzzer, HIGH);
  else if(dur > 3000)
  {
    analogWrite(g_led, 0);
    digitalWrite(buzzer, HIGH);
  }
  
}

void detection()
{
  check = analogRead(curr_out);
  Serial.println(check);
  if(check > 505)
   damaged(det_strtr_time);
  else if(check < 495)
   safe(det_strt_time);
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

  int result = radio.begin();

  if (result) {
    Serial.println("NRF module Working");
  } 
  else {
     Serial.println("NRF module ERROR ");
  }

  radio.openWritingPipe(add_2);
  radio.openReadingPipe(1, add_1);
 // Set data rate to RF24_250KBPS, RF24_1MBPS, or RF24_2MBPS
  radio.setDataRate(RF24_2MBPS); // Adjust the data rate as needed
  radio.setPALevel(RF24_PA_MIN);
  
  det_init();
  dc_motor_init();
  bo_motor_init();

}

void loop() {
  mpu.update();
  
  //if((millis()- timer)>0){ // print data every 10ms
  //Serial.print("X : ");
  roll_pos_err = 0 - mpu.getAngleX();
  //Serial.print(roll_pos_err);
 // Serial.print("\tX_vel : ");
   roll_vel_err = 0 - mpu.getGyroX();
  // Serial.print(roll_vel_err);
   //Serial.print("\tZ : ");
  yaw_pos_err = yaw_ini - mpu.getAngleZ();
  //Serial.println(yaw_pos_err);
  
  
  //yaw_itgl_err = yaw_itgl_err + yaw_pos_err;
  //u = 2.8 * roll_pos_err - 0.3 * yaw_pos_err - 0.0009 * yaw_itgl_err  ;// + 0.002* roll_vel_err + 0.008 * roll_itgl_err;//;
  //u = 2.8 * roll_pos_err - 0.32 * yaw_pos_err - 0.003 * yaw_itgl_err  ;// till turn control
  u = 3.5 * roll_pos_err + 0.002 * roll_vel_err - 0.4 * yaw_pos_err;
  //u = 2.9 * roll_pos_err - 0.3 * yaw_pos_err; //till turning
  //u = 2.9 * roll_pos_err - 0.2 * yaw_pos_err - 0.008 * yaw_itgl_err  ;
  //u = 3.5 * roll_pos_err - 0.35 * yaw_pos_err ;//+ 0.00003 * roll_vel_err
  //Serial.println (-u);
  motor_control(-u);

  
 radio.startListening();
  if (radio.available()) {
    radio.read(&msg_1, sizeof(msg_1));
    //Serial.println(msg_1);
    if(msg_1 == 'D')
   {
     det_strt_time = millis();
     det_end_time = det_strt_time + 3000;
   }
    else if(msg_1 == 'N')
     analogWrite(enB, 0);
    else if(msg_1 == 'L')
     yaw_ini = yaw_ini + 1.5 ;
    else if(msg_1 == 'R')
      yaw_ini = yaw_ini - 1.5;
    else if (msg_1 == 'F')
      forward();
    else if (msg_1 == 'B')
     backward();
 
  }
 if(det_end_time - millis() <= 3000)
 {
   detection(det_strt_time);
 }

}