#include "stepperMotor.h"
#include <stdint.h>
#include <stdio.h>

void rampMotorExp(int delayValue, int initialValue){
  int i;
  int initialDelay = 72000000 / (ACCELERATION * initialValue);
  int period = initialValue;
  while(period > delayValue){
	  initialDelay += RAMP_RATE;
	  period = 72000000 / (ACCELERATION * initialDelay);	
	  printf("%d, %d\n", period, (int)(period * 9));
  }
}

int main(){
	//printf("increase motor speed from period 10000 to period 750\n");
	rampMotorExp(750, 10000);
}