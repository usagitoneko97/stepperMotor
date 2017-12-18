#ifndef _STEPPERMOTOR_H
#define _STEPPERMOTOR_H

#define INITIAL_DELAY	1000
#define RAMP_RATE		4
#define NO_RAMP_CYCLE	2

#define ACCELERATION	3
#define INITIAL_PERIOD	1000
void rampMotorExp(int delayValue, int initialValue);
#endif // _STEPPERMOTOR_H
