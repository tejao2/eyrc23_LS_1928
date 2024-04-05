#include "Wire.h"
#include <MPU6050_light.h>
#define enA 6  
#define in1 A2
#define in2 A3

// MPU6050 object
MPU6050 mpu(Wire);

// Timer for balancing sensor data and motor actuation
unsigned long timer = 0;

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

/*
Function Name:dc_motor_initialization
Logic:This function initializes the pins for the DC motor and sets their initial state.
Example Call:dc_motor_init()
*/
void dc_motor_init(){
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);  
}

/*
Function Name:Moto Controlling
Input:The PWM value controlling the motor speed and direction.
Logic:This function adjusts the direction and speed of the DC motor based on the PWM value.
Example: motor_control(85)
*/
void motor_control(int pwm) {
 Serial.println(pwm);
  if (pwm < 0) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
    pwm = -pwm;
  } 
  else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  analogWrite(enA, pwm+62);
}

/*
Function Name: Setup function to initialize serial communication and MPU6050.
Logic: This function initializes the serial communication and MPU6050 sensor.
*/
void setup() {
  Serial.begin(9600);
  Wire.begin();
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // // Stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // Gyro and accelerometer offsets calibration
  Serial.println("Done!\n");
  dc_motor_init(); // Initialize DC motor
}


/*
Function Name: Main loop to read MPU6050 data and control the DC motor.
Logic: This function reads data from the MPU6050 sensor, calculates errors, and controls the DC motor based on the feedback.
*/
void loop() {
  mpu.update();
  if((millis()-timer)>20){ // print data every 10ms
  Serial.print("X : ");
  roll_pos_err = 0 - mpu.getAngleX(); //Error value of Roll postion
  Serial.print(roll_pos_err);
  Serial.print("\tX_vel : ");
  roll_vel_err = 0 - mpu.getGyroX(); //Error value of Roll velocity
  Serial.print(roll_vel_err);
  Serial.print("\tZ : ");
  yaw_pos_err = 0 - mpu.getAngleZ(); //Error value of Yaw postion
  Serial.println(yaw_pos_err);
  roll_itgl_err += roll_pos_err;
  u = 2.9 * roll_pos_err - 0.3 * yaw_pos_err ; //PWM value
  motor_control(-u); //Calling the Motor controlling Function
  timer = millis();  
  }
}
