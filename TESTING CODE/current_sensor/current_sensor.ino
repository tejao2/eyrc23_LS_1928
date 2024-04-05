#define curr_out A7
#define buzzer 7
#define r_led A0
#define g_led A1
int check;
int strt_time = 0;
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
void setup() {
  Serial.begin(9600);
  pinMode(curr_out,INPUT);
  // pinMode(r_led, output);
  // pinMode(g_led, output);
  pinMode(buzzer,OUTPUT);
}

void loop() {
  check = analogRead(curr_out);
  Serial.println(check);  
  // int dur = millis() - strt_time;
  // if(dur <= 1000 || (dur > 2000 && dur <= 3000))
  //  digitalWrite(buzzer, LOW);
  // else if(dur > 1000 && dur <= 2000)
  //  digitalWrite(buzzer, HIGH);
  // else if(dur > 3000)
  // {
  //   //analogWrite(g_led, 0);
  //   digitalWrite(buzzer, HIGH);
  // }
  digitalWrite(buzzer, HIGH);
  // if(check > 505)
  //  damaged(millis());
  // else if(check < 495)
  //  safe(millis());
  

}
