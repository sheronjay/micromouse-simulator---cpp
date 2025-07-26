#pragma once

#include <Arduino.h>
// #include "config.h"

//add to config.h
#define LEDC_CH_MOTOR_L 0
#define LEDC_CH_MOTOR_R 1
#define LEDC_MOTOR_L 20
#define LEDC_MOTOR_R 21
#define MOTOR_CONTROL_L1 22
#define MOTOR_CONTROL_L2 23
#define MOTOR_CONTROL_R1 24
#define MOTOR_CONTROL_R2 25




#define MOTOR_CTRL_FREQUENCY  250000 //pwm frequency
#define MOTOR_CTRL_BIT_NUM    10 //pwm resolution 0-1023
#define MOTOR_DUTY_MAX        1023 // Maximum duty cycle value
#define MOTOR_DUTY_SAT        1023 //saturation value (maximum duty allowed) - control max speed

class Motor{
    public:
        Motor(){
            ledcSetup(LEDC_CH_MOTOR_L, MOTOR_CTRL_FREQUENCY, MOTOR_CTRL_BIT_NUM);
            ledcSetup(LEDC_CH_MOTOR_R, MOTOR_CTRL_FREQUENCY, MOTOR_CTRL_BIT_NUM);
            ledcAttachPin(LEDC_MOTOR_L, LEDC_CH_MOTOR_L);
            ledcAttachPin(LEDC_MOTOR_R, LEDC_CH_MOTOR_R);
            free();
        }
        void left(int duty){
            if (emergency) return;

            duty = constrain(duty, -MOTOR_DUTY_SAT, MOTOR_DUTY_SAT);

            if (duty > 0) {
                digitalWrite(MOTOR_CONTROL_L1, HIGH);
                digitalWrite(MOTOR_CONTROL_L2, LOW);
                ledcWrite(LEDC_CH_MOTOR_L, duty);
            } else if (duty < 0) {
                digitalWrite(MOTOR_CONTROL_L1, LOW);
                digitalWrite(MOTOR_CONTROL_L2, HIGH);
                ledcWrite(LEDC_CH_MOTOR_L, -duty);
            } else {
                digitalWrite(MOTOR_CONTROL_L1, LOW);
                digitalWrite(MOTOR_CONTROL_L2, LOW);
                ledcWrite(LEDC_CH_MOTOR_L, 0);  // Brake
            }
        }
        void right(int duty){
            if (emergency) return;

            duty = constrain(duty, -MOTOR_DUTY_SAT, MOTOR_DUTY_SAT);

            if (duty > 0) {
                digitalWrite(MOTOR_CONTROL_R1, HIGH);
                digitalWrite(MOTOR_CONTROL_R2, LOW);
                ledcWrite(LEDC_CH_MOTOR_R, duty);
            } else if (duty < 0) {
                digitalWrite(MOTOR_CONTROL_R1, LOW);
                digitalWrite(MOTOR_CONTROL_R2, HIGH);
                ledcWrite(LEDC_CH_MOTOR_R, -duty);
            } else {
                digitalWrite(MOTOR_CONTROL_R1, LOW);
                digitalWrite(MOTOR_CONTROL_R2, LOW);
                ledcWrite(LEDC_CH_MOTOR_R, 0);  // Brake
            }
        }
        void drive(int16_t valueL, int16_t valueR){
            left(valueL);
            right(valueR);
        }
        void free(){
            digitalWrite(MOTOR_CONTROL_R1, LOW);
            digitalWrite(MOTOR_CONTROL_R2, LOW);
            ledcWrite(LEDC_CH_MOTOR_R, 0); 
            digitalWrite(MOTOR_CONTROL_L1, LOW);
            digitalWrite(MOTOR_CONTROL_L2, LOW);
            ledcWrite(LEDC_CH_MOTOR_L, 0);
        }
        void emergency_stop(){
            emergency = true;
            free();
        }
        void emergency_release(){
            emergency = false;
            free();
        }
        bool isEmergency(){
            return emergency;
        }
    private:
        bool emergency;
};