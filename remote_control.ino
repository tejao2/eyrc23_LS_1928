#include <SPI.h>
#include <RF24.h>

// NRF24L01 module
RF24 radio(10, 9);

const byte add_1 = 5;
const byte add_2 = 10;

// Analog pins for joystick
const int VRx = A1; // analog pin for x-axis 
const int VRy = A2; // analog pin for y-axis
const int SW  = 7;  // Digital pin for the button
char msg_1 = 'N'; // Message to be sent
int x_value, y_value, button_state;
float  yaw_ini = 0; // Initial yaw value

/*
Function name: Setup function to initialize
Logic: This function initializes the serial communication and NRF24L01 module. 
*/
void setup() {
 Serial.begin(9600);
  int result = radio.begin();
  if (result) {
    Serial.println("NRF module Working");
  } 
  else {
    Serial.println("NRF module ERROR ");
  }
 radio.openWritingPipe(add_1);
  radio.openReadingPipe(1, add_2);
  radio.setDataRate(RF24_2MBPS);
  radio.setPALevel(RF24_PA_MIN);
}

/*
Function Name: Main loop to read joystick values and send commands using NRF24L01.
Logic: This function reads values from the joystick, determines the command based on the joystick state, and sends the command using the NRF24L01 module.
*/
void loop() {
  x_value = analogRead(VRx);
  y_value = analogRead(VRy);
  button_state = digitalRead(SW);
  radio.stopListening();

  // Determine the command based on joystick state
  if(button_state)
  {
    msg_1 = 'D';
  }
  else if(x_value > 1000){
   msg_1 = 'B';
  }
  else if(x_value < 100) {
   msg_1 = 'F';
  }
  else if(y_value > 1000){
   msg_1 = 'L';
   yaw_ini = yaw_ini + 1.5;
  }
  else if(y_value < 100) {
   msg_1 = 'R';
   yaw_ini = yaw_ini - 1.5;
  }
  else {
   msg_1 = 'N';
  }
 radio.write(&msg_1, sizeof(msg_1)); // Send the command using NRF24L01
 //Serial.println(yaw_ini);
 Serial.println(msg_1); // Print yaw value to serial monitor
 delay(10); // Delay to control the rate of sending commands
}
