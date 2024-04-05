#include <SPI.h>
#include <RF24.h>

RF24 radio(10, 8);

const byte add_1 = 5;
const byte add_2 = 10;

char msg_1[50] = "";

void setup() {
  Serial.begin(9600);

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

}

void loop() {
  radio.startListening();

  // Wait for a short time to check for incoming data
  delay(10);

  if (radio.available()) {
    radio.read(&msg_1, sizeof(msg_1));
    Serial.println(msg_1);
  }

  // No need for a delay after reading

  radio.stopListening();
  const char msg_2[] = "msg_from_2nd : 'Lunar_scout_2'";
  radio.write(&msg_2, sizeof(msg_2));

  // Uncomment the lines below if you want to print both received and sent messages
  // Serial.println(msg_1);
  // Serial.println(msg_2);

  // Add a small delay if needed
  delay(10);
}