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

// MPU6050 object
MPU6050 mpu(Wire);
RF24 radio(10, 8);

// Timer for balancing sensor data and motor actuation
unsigned long timer = 0;
unsigned long s_timer = 0;
const byte add_1 = 5;
const byte add_2 = 10;
char msg_1 ;

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
Function Name:BO_motor_initialization
Logic:This function initializes the pins for the BO motor and sets their initial state.
Example Call:BO_motor_init()
*/
void bo_motor_init(){
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
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
  analogWrite(enA, pwm+60);
}

/*
Function Name:Forward control 
Logic:This function initializes the pins for Forward control and sets their initial state.
Example Call:Forward()
*/
void forward(){
  analogWrite(enB, 92);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);  
}

/*
Function Name:Backward control 
Logic:This function initializes the pins for Backward control and sets their initial state.
Example Call:Backward()
*/
void backward(){
  analogWrite(enB, 92);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);  
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
  while(status!=0){ } // stop everything if could not connect to MPU6050
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  mpu.calcOffsets(); // Gyro and accelerometer offsets calibration
  Serial.println("Done!\n");
  int result = radio.begin(); // Initializing Wireless communication
  if (result) {
    Serial.println("NRF module Working");
  } 
  else {
     Serial.println("NRF module ERROR ");
  }
  radio.openWritingPipe(add_2);
  radio.openReadingPipe(1, add_1);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MIN);
  dc_motor_init(); // Initialize DC motor
  bo_motor_init(); // Initialize BO motor
}

/*
Function Name: Main loop to read MPU6050 data and control the DC motor.
Logic: This function 1. Reads data from the MPU6050 sensor, calculates errors, and controls the DC motor based on the feedback.
                     2. Get the signal from Joystick for forward/backward/left/right- motion control  
*/
void loop() {
  mpu.update();
  radio.startListening();
  if((millis()- timer)>20){ // print data every 10ms
  Serial.print("X : ");
  roll_pos_err = 0 - mpu.getAngleX();
  Serial.print(roll_pos_err);
   Serial.print("\tZ : ");
   yaw_pos_err = yaw_ini - mpu.getAngleZ();
  Serial.println(yaw_pos_err);
  motor_control(-u); //Calling the Motor controlling Function
  timer = millis();  
  }
 if (radio.available()) {
    radio.read(&msg_1, sizeof(msg_1));
    Serial.println(msg_1);
    if(msg_1 == 'N')
    {
      analogWrite(enB, 0);
    }
    else if(msg_1 == 'L')
     yaw_ini = yaw_ini + 1 ;// left turn 
    else if(msg_1 == 'R')
      yaw_ini = yaw_ini - 1;
    else if (msg_1 == 'F')// right turn
    {
      forward(); //Calling the Forward moving Function
    } 
    else if (msg_1 == 'B')
     backward(); //Calling the Backward moving Function
  }
}