#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <Servo.h>
#include <Keypad.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET D5
Adafruit_SSD1306 display(OLED_RESET);

boolean takeinput = false;
// start of keypad
String user_otp;
String otp;
int i;
long y; 
const byte n_rows = 3;
const byte n_cols = 3;
 
char keys[n_rows][n_cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'}
};
 
byte colPins[n_rows] = {D4, D3, D0};
byte rowPins[n_cols] = {D7, D6, D5};
 
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);
// end of keypad
//start of servo
Servo myservo;

char auth[] = "377c95c233d54af4bdb467fb5e7b1854";


String to_num,dest,OTP,temp;
int a=0;
int dir;


WidgetTerminal terminal(V4);


ESP8266WiFiMulti WiFiMulti;
HTTPClient http;


void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();
  Serial.println();
  Blynk.begin(auth, "Y 5", "still learning");
  myservo.attach(D8);
  Serial.println("Connecting...");
  WiFiMulti.addAP("Y 5", "still learning");
  Serial.println(a);
  terminal.clear();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  drawStr(10,30, "Wecome  !!");
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  delay(2000);
  display.clearDisplay();
  drawStr(10,30, "Initializing....");
  display.display();
  delay(3000);
  display.clearDisplay();
  drawStr(10,30, "connected.");
  display.display();


  
  
}
void loop() {
  // wait for WiFi connection
  if(a==0)
  Serial.println(a);
  Blynk.run();
  
  if ((WiFiMulti.run() == WL_CONNECTED) && (to_num.length()==10) && (a==1) && takeinput==false){ 
  OTP = otpgen();
  Serial.println(OTP);
  call_to_send_sms();
  
  Serial.println("waiting for input");
  
  }
  
  if(takeinput){
       char  key = myKeypad.getKey();
    if(key!=NULL){
      user_otp=user_otp + key;
      display.clearDisplay();
       drawStr(5,10,"Please enter:");
       drawStr(20,30,user_otp);
       display.display();
      Serial.println(user_otp);
    
  }
}

if(user_otp==OTP && user_otp.length()==6){
  
  Serial.println("accept");
  user_otp="";
  display.clearDisplay();
  drawStr(10,30,"Opening door..");
  display.display();
  rotate_180();
  delay(10000);
  display.clearDisplay();
  drawStr(10,30,"Closing door..");
  display.display();
  rotate_0();
  takeinput=false;
  display.clearDisplay();
  drawStr(10,30, "connected.");
  display.display();
  
    }
    else if (user_otp!=OTP && user_otp.length()==6){
      Serial.println("incorrect try agai");
      Serial.println("This is ur second attemp, remaining attempt 2/3");
      user_otp="";
    }




    
}

void call_to_send_sms(){
  {
    dest="https://maker.ifttt.com/trigger/sms/with/key/bbUmzTcEhsHrJ0ghM4ESjvof0yWSwcxF-9zFy7atA3x/?value1=+91"+to_num+"&value3="+OTP;
    Serial.println(dest);
    http.begin(dest,"AA:75:CB:41:2E:D5:F9:97:FF:5D:A0:8B:7D:AC:12:21:08:4B:00:8C"); //HTTP
    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        display.clearDisplay();
        terminal.print("OTP sent !!");
        drawStr(10,30,"OTP Sent!!");
        display.display();
        delay(3000);
        display.clearDisplay();
        drawStr(5,10,"Please enter:");
        display.display();
        takeinput=true;
        terminal.flush();

        to_num="";
      }
    } else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
    http.end();
    
  }
}

BLYNK_WRITE(V4)
{
  temp=param.asStr();
  if(temp.length()==10){
  to_num = param.asStr(); // assigning incoming value from pin V1to a variable
   Serial.println(to_num);
  }
  else{
    terminal.print("Please enter a valid input");
  }
  terminal.flush();
}

BLYNK_WRITE(V1)
{
  a= param.asInt(); // assigning incoming value from pin V1 to a variable

  // process received value
}

BLYNK_WRITE(V5){
  dir=param.asInt();
  Serial.print(dir);
  if(dir==1){
 rotate_180();
  }
  else if(dir==0){
  rotate_0();
  }
}

String otpgen(){

  String temp;
  for(i=1;i<=6;i++){
    temp=temp+String(random(1,10));
    Serial.print(otp);
  }
    return temp;
}

void rotate_180(){
  for(i=0;i<180;i++)
    {
      delay(15);
      myservo.write(i);
    }
}

void rotate_0(){
  //  myservo.write(0);
  for(i=180;i>0;i--)
    {
      delay(15);
      myservo.write(i);
    }
}

void drawStr(uint8_t x, uint8_t y, String str){
  display.setCursor(x, y);  /* Set x,y coordinates */
  display.println(str);
}
