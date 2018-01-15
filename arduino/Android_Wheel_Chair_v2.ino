#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>

ESP8266WebServer server(80);
// define pin
//#define DEVELOP
#define MOTOR_ENABLE_PIN 10
#define MOTOR_LEFT_STEP_PIN 5   //d1
#define MOTOR_LEFT_DIR_PIN 4    //d2
#define MOTOR_RIGHT_STEP_PIN 2  //d4
#define MOTOR_RIGHT_DIR_PIN 0   //d3
#define MOTOR_LEFT_FOWARD 0
#define MOTOR_LEFT_BACKWARD 1
#define MOTOR_RIGHT_FOWARD 1
#define MOTOR_RIGHT_BACKWARD 0 


#define INITIAL_DELAY  1000

#define RAMP_RATE     2
#define NO_RAMP_CYCLE 2

#define MAX_ACCELERATION  4
#define ACCELERATION      2
#define INITIAL_PERIOD  1000
#define THRESHOLD_GAP       15
#define MINIMUM_ANGLE     1
#define MIN_SPEED         1
#define MOTOR_MAX_PERIOD  12500

#define ENABLE LOW
#define DISABLE HIGH

#define STOP_PERIOD (MOTOR_MAX_PERIOD * 100)

#define _1u 80
#define IS_MOTOR_ACTIVE(x) ((x).reloadPeriod != -1)
#define STOP_MOTOR()   do{                            \
                        leftMotor->reloadPeriod = -1; \
                        rightMotor->reloadPeriod = -1;\
                        } while(0)

const char *ssid = "test123";
const char *password = "testing12345";
volatile unsigned long next;
volatile int previoustime = 0;
volatile int motorDelay = 0;
volatile int isTimerOn = 0;

volatile float mAcceleration;
/*#define updateIndex(x)    do {                              \
                    if((x)->updated == 1)                   \
                     {                                      \
                       (x)->index  = ((x)->index +1)&1;     \
                       (x)->updated =0;                     \
                     }                                      \
  } while(0)  */

// void handleUturn(MotorInfo *leftMotor , MotorInfo *rightMotor);
typedef struct MotorInfo MotorInfo;
struct MotorInfo {
  unsigned long stepPeriod;
  unsigned long reloadPeriod;
  unsigned long expDelay;
  unsigned long prevStepPeriod;
  float acceleration;
  int motorControlPin;
  int dirPin;
  int dir;
  int curDir;
  int steps;
  unsigned long prevTime;
};

MotorInfo leftMotorInfo = {

};

MotorInfo rightMotorInfo = {

};

typedef struct AngleSpeed AngleSpeed;
struct AngleSpeed {
  float Speed;
  float Angle;
  float Acceleration;
  float previousAngle;
  float previousSpeed;
};

AngleSpeed instruction = {
  .Speed = 0.0, .Angle = 90.00, .Acceleration=0, .previousAngle = 270, .previousSpeed = 0.0
};

void setup() {
  Serial.begin(115200);
  pinMode(MOTOR_LEFT_STEP_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_DIR_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_STEP_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR_PIN, OUTPUT);
  pinMode(MOTOR_ENABLE_PIN, OUTPUT);
  rightMotorInfo.stepPeriod = STOP_PERIOD;
  rightMotorInfo.prevStepPeriod = STOP_PERIOD;
  rightMotorInfo.expDelay == 72000000 / (mAcceleration * rightMotorInfo.prevStepPeriod);
  rightMotorInfo.reloadPeriod = -1;
  rightMotorInfo.motorControlPin = MOTOR_RIGHT_STEP_PIN;
  rightMotorInfo.dirPin = MOTOR_RIGHT_DIR_PIN;
  rightMotorInfo.dir = MOTOR_RIGHT_FOWARD;
  rightMotorInfo.steps = 0;
  rightMotorInfo.prevTime = 0;
  leftMotorInfo.stepPeriod = STOP_PERIOD;
  leftMotorInfo.prevStepPeriod = STOP_PERIOD;
  leftMotorInfo.expDelay == 72000000 / (mAcceleration * leftMotorInfo.prevStepPeriod);
  leftMotorInfo.reloadPeriod = -1;
  leftMotorInfo.motorControlPin = MOTOR_LEFT_STEP_PIN;
  leftMotorInfo.dirPin = MOTOR_LEFT_DIR_PIN;
  leftMotorInfo.dir = MOTOR_LEFT_FOWARD;
  leftMotorInfo.steps = 0;
  leftMotorInfo.prevTime = 0;
  mAcceleration = 1;
  WiFi.softAP(ssid, password, 1, 0 );
  handling(&instruction, &leftMotorInfo, &rightMotorInfo);
  handleForceStop();
  handleUturn(&leftMotorInfo , &rightMotorInfo);
  // handlingDebug(&leftMotorInfo , &rightMotorInfo);
  server.begin();
  IPAddress myIP = WiFi.softAPIP();
  disableMotor();
  noInterrupts();
  timer0_isr_init();

  interrupts();
  //  Serial.println(myIP);
}

void handleForceStop(){
  server.on("/forcestop", [ = ](){
    rightMotorInfo.reloadPeriod = MOTOR_MAX_PERIOD;
    leftMotorInfo.reloadPeriod = MOTOR_MAX_PERIOD;
    rightMotorInfo.stepPeriod = MOTOR_MAX_PERIOD;
    leftMotorInfo.stepPeriod = MOTOR_MAX_PERIOD;
    rightMotorInfo.prevStepPeriod = MOTOR_MAX_PERIOD;
    leftMotorInfo.prevStepPeriod = MOTOR_MAX_PERIOD;
    leftMotorInfo.expDelay = 72000000 / (mAcceleration * leftMotorInfo.prevStepPeriod);
    rightMotorInfo.expDelay = 72000000 / (mAcceleration * rightMotorInfo.prevStepPeriod);
    timer0_detachInterrupt();
    isTimerOn = 0;
  });
}

int TimerExpired(unsigned long duration, unsigned long previous) {
  unsigned long current = micros();
  if (duration != -1 && (current - previous >= duration)) {
    return 1;
  }
  return 0;
}

void motorStep(MotorInfo *motorInfo) {
  next = next + 400000;
  timer0_write(next);
  // unsigned long stepPeriod = getStepPeriod(motorInfo);
  // updateIndex(motorInfo);

  if (TimerExpired(motorInfo->stepPeriod, motorInfo->prevTime)) {
    digitalWrite(motorInfo->dirPin, motorInfo->dir);
    digitalWrite(motorInfo->motorControlPin, HIGH);
    digitalWrite(motorInfo->motorControlPin, LOW);
    Serial.println(motorInfo->stepPeriod);
    motorInfo->prevTime = micros();
    motorInfo->steps++;
  }
}

void pulsePin(int pin) {
  digitalWrite(pin, HIGH);
  digitalWrite(pin, LOW);
}
 
void pulseMotorOnTimeout(MotorInfo *motor) {
  if (motor->stepPeriod < 2000 && motor->reloadPeriod != 0) {
    if (motor == &leftMotorInfo) {
      pulsePin(MOTOR_LEFT_STEP_PIN);
      digitalWrite(MOTOR_LEFT_DIR_PIN , leftMotorInfo.curDir);
    } else {
      pulsePin(MOTOR_RIGHT_STEP_PIN);
      digitalWrite(MOTOR_RIGHT_DIR_PIN , rightMotorInfo.curDir);
    }

    if (motor->prevStepPeriod > STOP_PERIOD) {
      motor->curDir = motor->dir;
    }
    if (motor->curDir != motor->dir) {
      //ramp to zero
      motor->expDelay -= RAMP_RATE;
      motor->prevStepPeriod = 72000000 / (motor->acceleration * motor->expDelay);
      motor->stepPeriod += motor->prevStepPeriod;
    } else {
      if(motor->prevStepPeriod >= STOP_PERIOD && motor->reloadPeriod >= STOP_PERIOD){
        disableMotor();
        timer0_detachInterrupt();
        isTimerOn = 0;
        return;
      }
      if (motor->prevStepPeriod > (motor->reloadPeriod + THRESHOLD_GAP)) {
        motor->expDelay += RAMP_RATE;
        motor->prevStepPeriod = 72000000
            / (motor->acceleration * motor->expDelay);
        motor->stepPeriod += motor->prevStepPeriod;
      } else if (motor->prevStepPeriod
          < (motor->reloadPeriod - THRESHOLD_GAP)) {
        motor->expDelay -= RAMP_RATE;
        motor->prevStepPeriod = 72000000
            / (motor->acceleration * motor->expDelay);
        motor->stepPeriod += motor->prevStepPeriod;
      } else {
        motor->stepPeriod += motor->reloadPeriod;
        motor->prevStepPeriod = motor->stepPeriod;
        //calculate expDelay so that the when next speed request is fasterm expDelay can be used straight away
      }
    }
  }
}

void initMotor(){
   rightMotorInfo.stepPeriod = 30000;
   rightMotorInfo.prevStepPeriod = 30000;
   rightMotorInfo.expDelay == 72000000 / (mAcceleration * rightMotorInfo.prevStepPeriod);
   rightMotorInfo.reloadPeriod = -1;
  rightMotorInfo.motorControlPin = MOTOR_RIGHT_STEP_PIN;
  rightMotorInfo.dirPin = MOTOR_RIGHT_DIR_PIN;
  rightMotorInfo.dir = MOTOR_RIGHT_FOWARD;
  rightMotorInfo.steps = 0;
  rightMotorInfo.prevTime = 0;
  leftMotorInfo.stepPeriod = 30000;
  leftMotorInfo.prevStepPeriod = 30000;
  leftMotorInfo.expDelay == 72000000 / (mAcceleration * leftMotorInfo.prevStepPeriod);
  leftMotorInfo.reloadPeriod = -1;
  leftMotorInfo.motorControlPin = MOTOR_LEFT_STEP_PIN;
  leftMotorInfo.dirPin = MOTOR_LEFT_DIR_PIN;
  leftMotorInfo.dir = MOTOR_LEFT_FOWARD;
  leftMotorInfo.steps = 0;
  leftMotorInfo.prevTime = 0;
}


void leftMotorStep_test() {
  int numOfActive = 0;
  unsigned long timeout;
  pulseMotorOnTimeout(&leftMotorInfo);
  pulseMotorOnTimeout(&rightMotorInfo);
  timeout = leftMotorInfo.stepPeriod;
  timeout = timeout <= rightMotorInfo.stepPeriod
            ? timeout
            : rightMotorInfo.stepPeriod;
  //  printf("timeout = %d\n", timeout);
  if (IS_MOTOR_ACTIVE(leftMotorInfo)) {
    leftMotorInfo.stepPeriod -= timeout;
    numOfActive++;
  }
  if (IS_MOTOR_ACTIVE(rightMotorInfo)) {
    rightMotorInfo.stepPeriod -= timeout;
    numOfActive++;
  }
  if (numOfActive > 0)
    timer0_write(ESP.getCycleCount() + timeout);
  else {
    printf("detach called");
    timer0_detachInterrupt();
    isTimerOn = 0;
  }

  /*next=ESP.getCycleCount()+79000;
    timer0_write(next);
    pulsePin(MOTOR_RIGHT_STEP_PIN);*/
}

void Uturn(MotorInfo *leftInfo, MotorInfo *rightInfo) {
  leftInfo->dir = MOTOR_LEFT_FOWARD;
  leftInfo->stepPeriod = 400;
  rightInfo->dir = MOTOR_RIGHT_BACKWARD;
  rightInfo->stepPeriod = 400;
  while ((leftInfo->steps <= 4000) && (rightInfo->steps <= 4000)) {
    motorStep(leftInfo);
    motorStep(rightInfo);
  }
}

int getStepPeriod(MotorInfo *info) {
  // updateIndex(info);
  return info->stepPeriod;
}

void ForceStop(MotorInfo *leftMotor, MotorInfo *rightMotor) {
  // leftMotor-> stepPeriod[0] = -1;
  // rightMotor ->stepPeriod[0] = -1;
  disableMotor();
  // Serial.println("Stopped");
}

void enableMotor() {
  digitalWrite(MOTOR_ENABLE_PIN, ENABLE);
  // Serial.println("Enable motor");
}

void disableMotor() {
  digitalWrite(rightMotorInfo.motorControlPin, LOW);
  digitalWrite(leftMotorInfo.motorControlPin, LOW);
  digitalWrite(MOTOR_ENABLE_PIN, DISABLE);
  // Serial.println("Disable motor");
}

float getSlowRatio(float angle)
{
    float value = angle/90.00;
    float result = 1.00/value;
    Serial.print("result:");
    Serial.println(result);
    return result;
}

int getQuadrant(int angle){
    return (angle/90) + 1;
}

  

void Calculation(AngleSpeed *MainInfo, MotorInfo *leftMotor,
                 MotorInfo *rightMotor) {

  int maxPeriod;
  float acute;
  float ratio;
  int quadrant = getQuadrant(MainInfo->Angle);
  if( MainInfo->Speed == 0){
    leftMotor->reloadPeriod = STOP_PERIOD;
    rightMotor->reloadPeriod = STOP_PERIOD;
    return;
  }
  else{
    maxPeriod = MOTOR_MAX_PERIOD / (MainInfo->Speed/ 100.00);
  }
  switch (quadrant) {
    case 1:
      acute = 90.00 - (float)(MainInfo->Angle);
      acute = acute == 0 ? 1 : acute;
      ratio = getSlowRatio(acute);
      Serial.print("ratio :");
      Serial.println(ratio);
      // left wheel will be fast, right wheel will be slow
      leftMotor->reloadPeriod = maxPeriod;
      rightMotor->reloadPeriod = ratio * maxPeriod;
      leftMotor->acceleration = mAcceleration;
      rightMotor->acceleration = mAcceleration / ratio;
      leftMotor->dir = MOTOR_LEFT_FOWARD;
      rightMotor->dir = MOTOR_RIGHT_FOWARD;
      break;
    case 2:
      acute = 90 - (180 - MainInfo->Angle);
      acute = acute == 0 ? 1 : acute;
      ratio = getSlowRatio(acute);
      leftMotor->reloadPeriod = maxPeriod;
      rightMotor->reloadPeriod = ratio * maxPeriod;
      leftMotor->acceleration = mAcceleration;
      rightMotor->acceleration = mAcceleration / ratio;
      leftMotor->dir = MOTOR_LEFT_BACKWARD;
      rightMotor->dir = MOTOR_RIGHT_BACKWARD;
      break;
    case 3:
      acute = 90 - (MainInfo->Angle - 180);
      acute = acute == 0 ? 1 : acute;
      ratio = getSlowRatio(acute);
      rightMotor->reloadPeriod = maxPeriod;
      leftMotor->reloadPeriod = ratio * maxPeriod;
      leftMotor->acceleration = mAcceleration / ratio;
      rightMotor->acceleration = mAcceleration;
      leftMotor->dir = MOTOR_LEFT_BACKWARD;
      rightMotor->dir = MOTOR_RIGHT_BACKWARD;
      break;
    case 4:
      acute = 90 - (360 - MainInfo->Angle);
      acute = acute == 0 ? 1 : acute;
      ratio = getSlowRatio(acute);
      rightMotor->reloadPeriod = maxPeriod;
      leftMotor->reloadPeriod = ratio * maxPeriod;
      leftMotor->acceleration = mAcceleration / ratio;
      rightMotor->acceleration = mAcceleration;
      leftMotor->dir = MOTOR_LEFT_FOWARD;
      rightMotor->dir = MOTOR_RIGHT_FOWARD;
      break;
  }
  // expDelay needs to be reload here for some reason that needs investigate
  leftMotor->expDelay = 72000000 / (leftMotor->acceleration * leftMotor->prevStepPeriod);
  rightMotor->expDelay = 72000000 / (rightMotor->acceleration * rightMotor->prevStepPeriod);
}

void handling(AngleSpeed *info, MotorInfo *leftMotor, MotorInfo *rightMotor) {
  server.on("/body", [ = ]() {
    if (server.hasArg("plain") == false) { // Check if body received

      server.send(200, "text/plain", "Body not received");
      return;
    }
    String message = "Body received:\n";
    message += server.arg("plain");
    message += "\n";

    server.send(200, "text/plain", message);
    Serial.println("handling!");

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject &root = jsonBuffer.parseObject(server.arg("plain"));

    if (!root.success()) {
      //      Serial.println("parseObject() failed");
      return;
    }
    if ( root.containsKey("offset") && root.containsKey("degrees")) {
      info->Speed = root["offset"];
      info->Angle = root["degrees"];
      info->Acceleration= root["acceleration"];
    }
    mAcceleration = (info->Acceleration/100.00) * MAX_ACCELERATION;
    
    Serial.print("offset = ");
    Serial.println(info->Speed);
    Serial.print("angle = ");
    Serial.println(info->Angle);
    Serial.print("acceleration = ");
    Serial.println(mAcceleration);
    Calculation(info, leftMotor, rightMotor);
    Serial.print("left reload = ");
    Serial.println(leftMotor->reloadPeriod);
    Serial.print("right reload = ");
    Serial.println(rightMotor->reloadPeriod);
    if (!isTimerOn && info->Speed > 0) {
      leftMotorInfo.stepPeriod = leftMotorInfo.prevStepPeriod;
      rightMotorInfo.stepPeriod = rightMotorInfo.prevStepPeriod;
      rightMotorInfo.expDelay = 72000000 / (mAcceleration * rightMotorInfo.prevStepPeriod);
      leftMotorInfo.expDelay = 72000000 / (mAcceleration * leftMotorInfo.prevStepPeriod);
      enableMotor();
      leftMotorInfo.curDir = leftMotorInfo.dir;
      rightMotorInfo.curDir = rightMotorInfo.dir;
      timer0_attachInterrupt(leftMotorStep_test);
      Serial.println("timer on!");
      isTimerOn = 1;
      noInterrupts();
      next = ESP.getCycleCount() + 2000;
      timer0_write(next);
      interrupts();
    }

#ifdef DEVELOP
    if (root.containsKey("whichmotor") && root.containsKey("delay") &&
        root.containsKey("direction")) {
      unsigned long stepPeriod;
      int direc;
      int whichMotor;
      whichMotor = root["whichmotor"];
      MotorInfo *Stepinfo;
      Serial.println("Entered the wifi function");
      Stepinfo = whichMotor == 1 ? leftMotor : rightMotor;
      stepPeriod = root["delay"];
      // stepPeriod *= _1u;
      direc = root["direction"];
      enableMotor();
      if (stepPeriod < 2000) {
        stepPeriod = 2000;
      }
      Stepinfo->reloadPeriod = stepPeriod;
      Stepinfo->dir = direc;
      Serial.print("right reload period ");
      Serial.println(rightMotor->reloadPeriod);
      Serial.print("left reload period ");
      Serial.println(leftMotor->reloadPeriod);
      Serial.print("left step period ");
      Serial.println(leftMotor->stepPeriod[0]);
      Serial.print("right step period ");
      Serial.println(rightMotor->stepPeriod[0]);

      if (!isTimerOn) {
        timer0_attachInterrupt(leftMotorStep_test);
        Serial.println("timer on!");
        isTimerOn = 1;
        noInterrupts();
        rightMotor->stepPeriod = 0;
        leftMotor->stepPeriod = 0;
        next = ESP.getCycleCount() + 2000;
        timer0_write(next);
        interrupts();
      }
    }
#endif
  });
}

void handleUturn(MotorInfo *leftMotor, MotorInfo *rightMotor) {
  server.on("/uturn", [ = ]() {
    Uturn(leftMotor, rightMotor);
  });
  return;
}

/*void handlingDebug(MotorInfo *leftMotor, MotorInfo *rightMotor){
  server.on("/debug", [=](){
     if (server.hasArg("plain")== false){ //Check if body received

      server.send(200, "text/plain", "Body not received");
      return;
    }
    String message = "Body received:\n";
       message += server.arg("plain");
       message += "\n";

    server.send(200, "text/plain", message);
  //   Serial.println(message);

    StaticJsonBuffer<200> jsonBuffer;

    JsonObject& root = jsonBuffer.parseObject(server.arg("plain"));

    if(!root.success()){
  //      Serial.println("parseObject() failed");
        return;
       }
      int whichMotor;
      whichMotor = root["whichmotor"];
      MotorInfo *info;

        info = whichMotor == 1? leftMotor:rightMotor;
        info->stepPeriod[0] = root["delay"];
        info->dir = root["direction"];
  });
  }*/

// the loop function runs over and over again forever
void loop() {
  // motorStep(&leftMotorInfo);
  yield();
  // motorStep(&rightMotorInfo);
  yield();
  server.handleClient();
  yield();
  // Calculation(&instruction , &leftMotorInfo ,&rightMotorInfo);
  yield();

  /*delay(3000);
  as.Speed = 100;
  as.Angle = 90;
  Calculation(&as, &leftMotorInfo, &rightMotorInfo);
  Serial.print("leftMotor 2: ");
  Serial.println(leftMotorInfo.reloadPeriod);
  Serial.print("rightMotor 2: ");
  Serial.println(rightMotorInfo.reloadPeriod);
  delay(3000);
  as.Speed = 100;
  as.Angle = 0;
  Calculation(&as, &leftMotorInfo, &rightMotorInfo);
  Serial.print("leftMotor 3: ");
  Serial.println(leftMotorInfo.reloadPeriod);
  Serial.print("rightMotor 3: ");
  Serial.println(rightMotorInfo.reloadPeriod);
  delay(3000);*/
}
