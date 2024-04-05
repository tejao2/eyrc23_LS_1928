#define curr_out_left A6
//#define buzzer 7
//#define r_led A0
#define curr_out_right A7
int check1;
int check2;
int strt_time = 0;
// void damaged(int strt_time)
// {
//   analogWrite(r_led, 255);
//   int dur = millis() - strt_time;
//   if(dur <= 1000 || (dur > 2000 && dur <= 3000))
//    digitalWrite(buzzer, LOW);
//   else if(dur > 1000 && dur <= 2000)
//    digitalWrite(buzzer, HIGH);
//   else if(dur > 3000)
//   {
//     analogWrite(r_led, 0);
//     digitalWrite(buzzer, HIGH);
//   }
// }

// void safe(int det_timer)
// {
//   analogWrite(g_led, 255);
//   int dur = millis() - strt_time;
//   if(dur <= 1000 || (dur > 2000 && dur <= 3000))
//    digitalWrite(buzzer, LOW);
//   else if(dur > 1000 && dur <= 2000)
//    digitalWrite(buzzer, HIGH);
//   else if(dur > 3000)
//   {
//     analogWrite(g_led, 0);
//     digitalWrite(buzzer, HIGH);
//   }
  
//}
void setup() {
  Serial.begin(9600);

  pinMode(curr_out_left,INPUT);
  pinMode(curr_out_right,INPUT);
  //pinMode(r_led, OUTPUT);
  //pinMode(g_led, OUTPUT);
  //pinMode(buzzer,OUTPUT);
 //analogWrite(g_led, 255);
 //analogWrite(r_led, 255);
 //digitalWrite(buzzer, LOW);
}

void loop() {
  check1 = analogRead(curr_out_left);
  check2 = analogRead(curr_out_right);
  Serial.println(check1);
  //Serial.println(check2);
  
  // if(check > 505)
  //  damaged(millis());
  // else if(check < 495)
  //  safe(millis());
  

}
