#include "Wire.h"
#include <MPU6050_light.h>
#include <SPI.h>
#include <RF24.h>
#include <Encoder.h>

// Motor Pins
#define enA 6
#define in1 A2
#define in2 A3
#define enB 5
#define in3 9
#define in4 4

// Detection Pins
#define curr_out_1 A7
#define curr_out_2 A6
#define buzzer 7
#define r_led A0
#define g_led A1

// Encoder for DC Motor
Encoder DC_Encoder(2, 3);

// MPU6050 and NRF24 Objects
MPU6050 mpu(Wire);
RF24 radio(10, 8);

// Variables for current detection
int check1;
int check2;

// Timing variables
unsigned long timer = 0;
unsigned long s_timer = 0;

// NRF24 Addresses
const byte add_1 = 5;
const byte add_2 = 10;

// Character for NRF24 communication
char msg_1;

// Control variables
float u = 0;
float roll_ini = 0;
float yaw_ini = 0;
float roll_pos_err = 0;
float yaw_pos_err = 0;
float roll_vel_err = 0;
float yaw_vel_err = 0;
float roll_itgl_err = 0;
float yaw_itgl_err = 0;
float det_strt_time = 0;
float det_end_time = 0;
float t_prev = 0;
float t = 0;
float roll_pos_err_prev = 0;
float yaw_pos_err_prev = 0;

// Function to initialize DC Motor
/* 
 * Function Name: dc_motor_init
 * Logic: Initializes the pins for DC motor control.
 * Example Call:  dc_motor_init();
 */
void dc_motor_init() {
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
}

// Function to initialize BO Motor
/* 
 * Function Name: bo_motor_init
 * Logic:         Initializes the pins for BO motor control.
 * Example Call:  bo_motor_init();
 */
void bo_motor_init() {
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}

// Function to initialize detection components
/* 
 * Function Name: det_init
 * Logic:         Initializes pins for current detection, LEDs, and buzzer.
 * Example Call:  det_init();
 */
void det_init() {
  pinMode(curr_out_1, INPUT);
  pinMode(curr_out_2, INPUT);
  pinMode(r_led, OUTPUT);
  pinMode(g_led, OUTPUT);
  pinMode(buzzer, OUTPUT);
}

// Function to control DC Motor based on PWM
/* 
 * Function Name: motor_control
 * Input:         int pwm - PWM value for motor control
 * Logic:         Controls the DC motor direction and speed based on the PWM value.
 * Example Call:  motor_control(200);
 */
void motor_control(int pwm) {
  if (pwm < 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    pwm = -pwm;
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  if (pwm > 255)
    analogWrite(enA, 255);
  if (pwm > 0)
    analogWrite(enA, pwm + 59);
}

// Function to move the robot forward
/* 
 * Function Name: forward
 * Logic:         Moves the robot forward.
 * Example Call:  forward();
 */
void forward() {
  analogWrite(enB, 76);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

// Function to move the robot backward
/* 
 * Function Name: backward
 * Logic:         Moves the robot backward.
 * Example Call:  backward();
 */
void backward() {
  analogWrite(enB, 76);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

// Function to handle damaged state
/* 
 * Function Name: damaged
 * Logic:         Detects the damaged CS which is North pole
 * Example Call:  damaged();
 */
void damaged() {
  analogWrite(r_led, 255);
  float dur = millis() - det_strt_time;
  if (dur <= 1000 || (dur > 2000 && dur <= 3000))
    digitalWrite(buzzer, HIGH);
  else if (dur > 1000 && dur <= 2000)
    digitalWrite(buzzer, LOW);
  else if (dur > 3000) {
    analogWrite(r_led, 0);
    digitalWrite(buzzer, LOW);
  }
}

// Function to handle safe state
/* 
 * Function Name: safe
 * Logic:         Detects the safe CS which is South pole
 * Example Call:  safe();
 */
void safe() {
  analogWrite(g_led, 255);
  float dur = millis() - det_strt_time;
  if (dur <= 1000 || (dur > 2000 && dur <= 3000))
    digitalWrite(buzzer, HIGH);
  else if (dur > 1000 && dur <= 2000)
    digitalWrite(buzzer, LOW);
  else if (dur > 3000) {
    analogWrite(g_led, 0);
    digitalWrite(buzzer, LOW);
  }
}

// Function to perform detection based on current readings
/* 
 * Function Name: detection
 * Logic:         Checks the current readings for damaged CS or Safe CS and calls the function accordingly.
 * Example Call:  detection();
 */
void detection() {
  check1 = analogRead(curr_out_1);
  check2 = analogRead(curr_out_2);

  if (check1 > 550 || check2 > 550)
    damaged();
  else if (check1 < 500 || check2 < 500)
    safe();
}

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // Initialize MPU6050
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);

  // Wait until MPU6050 is connected
  while (status != 0) { }

  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);

  // Calculate MPU6050 offsets
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");

  // Initialize NRF24 module
  int result = radio.begin();

  if (result) {
    Serial.println("NRF module Working");
  } else {
    Serial.println("NRF module ERROR ");
  }

  // Set NRF24 addresses
  radio.openWritingPipe(add_2);
  radio.openReadingPipe(1, add_1);

  // Set NRF24 configuration
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MIN);

  // Initialize detection components
  det_init();

  // Initialize DC and BO motors
  dc_motor_init();
  bo_motor_init();

  // Turn off LEDs
  analogWrite(g_led, 0);
  analogWrite(r_led, 0);

  // Initialize initial yaw value from encoder
  yaw_ini = DC_Encoder.read();
}

void loop() {
  // Update MPU6050 data
  mpu.update();

  // Calculate roll and yaw errors
  roll_pos_err = 0 - mpu.getAngleX();
  yaw_pos_err = yaw_ini - DC_Encoder.read();

  // Timing calculation
  t = millis();
  
  yaw_vel_err = (yaw_pos_err - yaw_pos_err_prev) / (t - t_prev);
  t_prev = t;
  yaw_pos_err_prev = yaw_pos_err;
  

  // Integral error calculation
  roll_itgl_err = roll_itgl_err + roll_pos_err;
  yaw_itgl_err = yaw_itgl_err + yaw_pos_err;

  // Control law calculation
  u = 27 * roll_pos_err + 0.1 * roll_itgl_err - 0.39 * yaw_pos_err - 0.07 * yaw_vel_err - 0.0003 * yaw_itgl_err;

  // Motor control based on calculated control signal
  motor_control(-u);

  // Start NRF24 listening
  radio.startListening();
     if (radio.available()) {
    radio.read(&msg_1, sizeof(msg_1));
    //Serial.println(msg_1);
    if(msg_1 == 'D')
   {
     det_strt_time = millis();
     det_end_time = det_strt_time + 4000;
   }
    else if(msg_1 == 'N')
     {
      analogWrite(enB, 0);
      digitalWrite(buzzer, LOW);
     }
    else if(msg_1 == 'L')
     yaw_ini = yaw_ini + 5;
    else if(msg_1 == 'R')
      yaw_ini = yaw_ini - 5;
    else if (msg_1 == 'F')
      forward();
    else if (msg_1 == 'B')
     backward();
    else if (msg_1 == 'H')
     digitalWrite(buzzer, HIGH);
 
  }
 if(det_end_time - millis() <= 4000)
 {
   detection();
 }
 
}





