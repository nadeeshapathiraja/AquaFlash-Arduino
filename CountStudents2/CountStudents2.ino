#include <Servo.h>
#include <ESP8266WiFi.h>
#include <FirebaseArduino.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

//water sensor useage
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)


//Set these to run example
#define FIREBASE_AUTH "4iCMXd8iz4UaUDbd0HLT2APwtN80oOtC3baiU2L3"
#define FIREBASE_HOST "poolmanager-3956f.firebaseio.com"
#define WIFI_SSID "Nadeesha"
#define WIFI_PASSWORD "123456123"

//set frontend pins for stcount
int trigpin1 = D0;
int echopin1 = D1;

//set backend pins for stcount
int trigpin2 = D2;
int echopin2 = D3;

//water level
#define trig D6
#define echo D7

long duration1, cm1 , duration2, cm2;
int freeslots, maxslots = 3;

Servo servo1, servo2;

//default student count=0
int count = 0;


void setup() {
  //for detect person

  Serial.begin(9600);
  pinMode(trigpin1, OUTPUT);
  pinMode(echopin1, INPUT);
  pinMode(trigpin2, OUTPUT);
  pinMode(echopin2, INPUT);



  //for servor moter enterence
  //enterence gate
  servo1.attach(D4);
  servo1.write(0);


  //exit gate
  servo2.attach(D5);
  servo2.write(0);


  digitalWrite(trigpin1, LOW);
  delayMicroseconds(5);

  digitalWrite(trigpin2, LOW);
  delayMicroseconds(5);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("WiFi Connected!!!");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);


}

void motoron() {
  digitalWrite(D8, LOW);
}

void motoroff () {
  digitalWrite(D8, HIGH);
}

void loop() {

  if (Firebase.failed())
  {
    Serial.print("setting number failed:");
    Serial.println(Firebase.error());
    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
    delay(2000);
    return;
  }

  delay(500);

  digitalWrite(trigpin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin1, LOW);

  duration1 = pulseIn(echopin1, HIGH);

  cm1 = (duration1 / 2) / 29.1;

  Serial.print(duration1);
  Serial.print(",");

  Serial.print(cm1);
  Serial.print("cm-1 ");
  Serial.println();



  if (count < maxslots) {

    if (cm1 <= 50) {
      Serial.println("Sensor Detect the person in Front");
      delay(50);          // wait for sensors to stabilize
      servo1.write(90);
      delay(3000);//servo moter aka pallehata ena aka ta kalaya(Uda redila tina)
      if (cm1 <= 50) {
        count += 1;
        Serial.print(count);
        Serial.print("Students In this Time ");
        Serial.println();

        freeslots = maxslots - count;
        Serial.print(freeslots);
        Serial.print(" Students can Enterd the Swimming Pool this Time ");
        Serial.println();

      }

      servo1.write(0);
      delay(2000);

    }
  }



  delay(500);

  digitalWrite(trigpin2, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin2, LOW);

  duration2 = pulseIn(echopin2, HIGH);

  cm2 = (duration2 / 2) / 29.1;

  Serial.print(duration2);
  Serial.print(",");

  Serial.print(cm2);
  Serial.print("cm-2 ");
  Serial.println();


  //Studentsla akkenk ho ita wada pool ake innawanm witari meka weda karanne
  if (count > 0) {
    if (cm2 <= 50) {
      Serial.println("Sensor Detect the person in Back");


      delay(50);          // wait for sensors to stabilize
      servo2.write(90);
      delay(3000);//servo moter aka pallehata ena aka ta kalaya(Uda redila tina)
      if (cm2 <= 50) {
        count -= 1;
        Serial.print(count);
        Serial.print("Students In this Time ");
        Serial.println();

        freeslots = maxslots - count;
        Serial.print(freeslots);
        Serial.print(" Students can Enterd the Swimming Pool this Time ");
        Serial.println();



      }

      servo2.write(0);
      delay(2000);

    }

  }
  if (count >= maxslots) {
    Serial.print("The Swimming Pool is full In this Time ");
    Serial.println();
  }

  Firebase.setFloat("studentCount/count/maxCount", maxslots);
  Firebase.setFloat("studentCount/count/freeSlots", freeslots);
  Firebase.setFloat("studentCount/count/count", count);


 


  

}
