#include <SPI.h>
#include <RF24.h>

RF24 radio(10, 9);

const byte add_1 = 5;
const byte add_2 = 10;

char msg_2[50] = "";

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
  // Set data rate to RF24_250KBPS, RF24_1MBPS, or RF24_2MBPS
  radio.setDataRate(RF24_2MBPS); // Adjust the data rate as needed

  radio.setPALevel(RF24_PA_MIN);
  
}

void loop() {
  const char msg_1[] = "msg_from_1st : 'Lunar_scout_1'";

  radio.stopListening();
  radio.write(&msg_1, sizeof(msg_1));

  delay(10); // Introduce a small delay after write

  radio.startListening();
  delay(100); // Give some time for the module to switch to listening mode
  if (radio.available()) {
    radio.read(&msg_2, sizeof(msg_2));
    Serial.println(msg_1);
    delay(1000);
    Serial.println(msg_2);
  }
}