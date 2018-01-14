#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "spiSlave_test.h"
#include "Timer.h"

ESP8266WebServer server(80);
// define pin
#define MOTOR_LEFT_STEP_PIN   4
#define MOTOR_LEFT_DIR_PIN    5
#define MOTOR_RIGHT_STEP_PIN  0
#define MOTOR_RIGHT_DIR_PIN   2
#define MOTOR_LEFT_FOWARD     1
#define MOTOR_LEFT_BACKWARD   0
#define MOTOR_RIGHT_FOWARD    0
#define MOTOR_RIGHT_BACKWARD  1


#define updateIndex(x)    do {                              \
                    if((x)->updated == 1)                   \
                     {                                      \
                       (x)->index  = ((x)->index +1)&1;     \
                       (x)->updated =0;                     \
                     }                                      \
} while(0)                                                  \

typedef struct MotorInfo MotorInfo;
struct  MotorInfo {
  unsigned long stepPeriod[2];
  int motorControlPin;
  int pinState;
  int dirPin;
  int dir;
  int steps;
  unsigned long prevTime;
  int updated;
  int index;
};

MotorInfo leftMotorInfo = {

};

MotorInfo rightMotorInfo = {

};

typedef struct AngleSpeed AngleSpeed;
struct  AngleSpeed {
  float Speed[2];
  float Angle[2];
  float previousAngle[2];
  float previousSpeed[2];
  int index;
  int updated;
};

AngleSpeed instruction ={

};
Timer t;
void setup(){
  Serial.begin(9600);
  Serial.print("start!");
  Serial.print("start!");
  t.every(1000,printSomething1);
  t.every(1000, printSomething2);
}
void printSomething1(){
  Serial.println("abcd");
 
}
void printSomething2(){
  Serial.println("ssss");
}

void loop()
{
    t.update();
    Serial.println("hey there");
}



void xsetup() {
  Serial.begin(9600);
  pinMode(MOTOR_LEFT_STEP_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_DIR_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_STEP_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR_PIN, OUTPUT);
  rightMotorInfo.stepPeriod[0] = -1;
  rightMotorInfo.motorControlPin = MOTOR_RIGHT_STEP_PIN;
  rightMotorInfo.pinState = LOW;
  rightMotorInfo.dirPin = MOTOR_RIGHT_DIR_PIN;
  rightMotorInfo.dir = MOTOR_RIGHT_FOWARD;
  rightMotorInfo.steps = 0;
  rightMotorInfo.prevTime = 0;
  rightMotorInfo.updated =0;
  rightMotorInfo.index =0;
  leftMotorInfo.stepPeriod[0] = -1;
  leftMotorInfo.motorControlPin = MOTOR_LEFT_STEP_PIN;
  leftMotorInfo.pinState = LOW;
  leftMotorInfo.dirPin = MOTOR_LEFT_DIR_PIN;
  leftMotorInfo.dir = MOTOR_LEFT_FOWARD;
  leftMotorInfo.steps = 0;
  leftMotorInfo.prevTime = 0;
  leftMotorInfo.updated =0;
  leftMotorInfo.index =0;

  instruction.Speed[0] = 0.0;
  instruction.Angle[0] = 90.00;
  instruction.previousAngle[0] = 270;
  instruction.previousSpeed[0] = 0.0 ;
  instruction.index = 0;
  instruction.updated = 0;

  handling(&instruction );
//  handleUturn(&leftMotorInfo , &rightMotorInfo);
  server.begin();
  //IPAddress myIP=WiFi.softAPIP();
//  Serial.println(myIP);

  spiSetup();
}

int TimerExpired(unsigned long duration ,unsigned long previous )
{
   unsigned long current = micros();
    if(duration != -1 && (current - previous >= duration))
    {
      return 1;
    }
    return 0;
}

void motorStep(MotorInfo *motorInfo)
{
  //unsigned long stepPeriod = getStepPeriod(motorInfo);
  updateIndex(motorInfo);

  if(TimerExpired(motorInfo->stepPeriod[motorInfo->index], motorInfo->prevTime))
  {
     digitalWrite(motorInfo->dirPin , motorInfo->dir);
     digitalWrite(motorInfo->motorControlPin,HIGH);
     digitalWrite(motorInfo->motorControlPin ,LOW);
     motorInfo->prevTime = micros();
     motorInfo-> steps ++ ;
  }
}

void Uturn(MotorInfo *leftInfo , MotorInfo *rightInfo)
{
    int index = getStepPeriod(leftInfo);
    int index2 = getStepPeriod(rightInfo);
    leftInfo->dir = MOTOR_LEFT_FOWARD;
    leftInfo->stepPeriod[index] = 400;
    rightInfo->dir = MOTOR_RIGHT_BACKWARD;
    rightInfo  ->stepPeriod[index2] = 400;
  while( (leftInfo->steps < 4000) && (rightInfo-> steps <4000) )
  {
      motorStep(leftInfo);
      motorStep(rightInfo);
  }
}

int getStepPeriod(MotorInfo *info)
{
  updateIndex(info);
  return info->stepPeriod[info->index];
}

void ForceStop(MotorInfo *leftMotor , MotorInfo *rightMotor)
{
  //updateIndex(leftMotor);
  //updateIndex(rightMotor);
  leftMotor-> stepPeriod[leftMotor->index] = -1;
  rightMotor ->stepPeriod[rightMotor->index] = -1;
}

void Calculation(AngleSpeed *MainInfo , MotorInfo *leftMotor , MotorInfo *rightMotor){

  //updateIndex(leftMotor);
 // updateIndex(rightMotor);
  updateIndex(MainInfo);
 //  Serial.print("Angle at calculation =");
//   Serial.println(MainInfo->Angle[MainInfo->index]);

   if(MainInfo->Speed[MainInfo->index] == 0)
    {
    ForceStop(leftMotor ,rightMotor);
    return;
    }

   else if(MainInfo->previousSpeed[MainInfo->index] != MainInfo->Speed[MainInfo->index] || MainInfo->previousAngle[MainInfo->index] != MainInfo->Angle[MainInfo->index])
    {
      MainInfo->previousSpeed[MainInfo->index] = MainInfo->Speed[MainInfo->index];
      MainInfo->previousAngle[MainInfo->index] = MainInfo->Angle[MainInfo->index];
      int max_period;
      float tempAngle;

      if(leftMotor->updated == 0 && rightMotor->updated == 0)
       {

      if( MainInfo->Angle[MainInfo->index] >= 180)
       {
         tempAngle = 360 - MainInfo->Angle[MainInfo->index];
         leftMotor->dir = MOTOR_LEFT_BACKWARD;
         rightMotor->dir = MOTOR_RIGHT_BACKWARD;
     //    Serial.println("backward");
       }
       else{
        tempAngle = MainInfo->Angle[MainInfo->index];
        leftMotor->dir = MOTOR_LEFT_FOWARD;
        rightMotor->dir = MOTOR_RIGHT_FOWARD;
    //    Serial.println("foward");
        }


      max_period = (8000 / MainInfo->Speed[MainInfo->index]);
      leftMotor->stepPeriod[(leftMotor->index +1)&1] = ( max_period * ( tempAngle/180) ) + 200 ;
      rightMotor->stepPeriod[(rightMotor->index+1)&1] = ( max_period - ( max_period * ( tempAngle/180) ) ) + 200 ;
      leftMotor->updated = 1;
      rightMotor->updated = 1;
      }

    }
}

void handling(AngleSpeed *info){
  server.on("/body", [=](){
     if (server.hasArg("plain")== false){ //Check if body received

      server.send(200, "text/plain", "Body not received");
      return;
    }
    String message = "Body received:\n";
       message += server.arg("plain");
       message += "\n";

    server.send(200, "text/plain", message);
    Serial.println(message);

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));

    if(!root.success()){
//      Serial.println("parseObject() failed");
      return;
    }
      if(info->updated == 0)
      {
        info->Speed[(info->index+1)&1] = root["offset"];
        info->Angle[(info->index+1)&1] = root["degrees"];
        info->updated = 1;
      }
  });
}

void handleUturn(MotorInfo *leftMotor , MotorInfo *rightMotor){
   server.on("/uturn", [=](){
     leftMotor->steps = 0;
     rightMotor->steps = 0;
     Uturn(leftMotor ,rightMotor);
  });
  return;
}

// the loop function runs over and over again forever
void xloop() {

    motorStep(&leftMotorInfo);
    yield();
    motorStep(&rightMotorInfo);
    yield();
    server.handleClient();
    yield();
    Calculation(&instruction , &leftMotorInfo ,&rightMotorInfo);
    yield();
    spiLoop();
}
