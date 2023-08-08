//8/8
//all to the loop
//recovery
//
//

#include <ECE3.h>

#define LED_BUILTIN 75

uint16_t sensorValues[8];
// Pin Assignments
const int left_nslp_pin = 31;
const int left_dir_pin  = 29;
const int left_pwm_pin  = 40;
const int right_pwm_pin=39;
const int right_dir_pin=30;
const int right_nslp_pin=11;

// const int LED_RF        = 41;//?
const int user_sw_2_pin=74;
const int bump_sw_0_pin = 24;
//parameters

// const int maximum_speed=50;
const int base_speed=50;
// const int change_speed_constant=5;
const int Kp=2200/base_speed;//P will be devided by Kp
const float Kd=base_speed/450;//0.5*2
//global
int leftSpd = base_speed;
int rightSpd = base_speed;
int preError[4]={0,0,0,0};
int preDelta[4]={0,0,0,0};
char outCount=0;//for stop
//for bumps;
bool bump_sw_0_reading = digitalRead(bump_sw_0_pin);

//constants
const int weights[8]={-15,-14,-12,-8,8,12,14,15};
const int minimum[8]={782,666,689,686,693,643,666,712};
const float nomal[8]={1.718,1.834,1.811,1.814,1.806,1.857,1.834,1.788};
// Prototypes
// int get_parameter();
// void changeSpeed(int sensor_fus);

///////////////////////////////////
void setup() {
  //pin setting
  pinMode(left_nslp_pin,OUTPUT);
  pinMode(left_dir_pin,OUTPUT);
  pinMode(left_pwm_pin,OUTPUT);

  pinMode(right_nslp_pin,OUTPUT);
  pinMode(right_dir_pin,OUTPUT);
  pinMode(right_pwm_pin,OUTPUT);

  //initial
  digitalWrite(left_dir_pin,LOW);
  digitalWrite(left_nslp_pin,HIGH);
  digitalWrite(right_dir_pin,LOW);
  digitalWrite(right_nslp_pin,HIGH);
  ECE3_Init(); // for sensor

  // Serial.begin(9600); // data rate for serial data transmission
  
  delay(2000); //Wait 2 seconds before starting 
  // int tmp=get_parameter();
  // for(int i=0;i<4;++i){
  //   preError[i]=tmp;
  // }
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}

void loop() {
  if((outCount>10)){//exit
    analogWrite(left_pwm_pin,50);
    analogWrite(right_pwm_pin,50);
    delay(100);
    analogWrite(left_pwm_pin,40);
    analogWrite(right_pwm_pin,40);
    delay(100);
    analogWrite(left_pwm_pin,30);
    analogWrite(right_pwm_pin,30);
    delay(100);
    analogWrite(left_pwm_pin,20);
    analogWrite(right_pwm_pin,20);
    delay(100);
    analogWrite(left_pwm_pin,0);
    analogWrite(right_pwm_pin,0);
    exit(0);//or just use the flag
  }
  ECE3_read_IR(sensorValues);
  int error=0;
  // int sensorerror=0;
  char checkWhite=0;
  bool iswhite[8]={0,};
  //check w b w
  for (int i=0;i<8;++i){//check white
    if (sensorValues[i]<1000){
      checkWhite+=1;//for stop
      iswhite[i]=1;
    }
  }
  bool firstwhites[5]={0,};
  char whitenum=0;
  if (iswhite[0]==1){
    firstwhites[0]=0;
    whitenum+=1;
  }
  for (int i=1;i<8;++i){
    if ((iswhite[i]==1)&&(iswhite[i-1]==0)){
      firstwhites[whitenum]=i;
      whitenum+=1;
    }
  }
  char lastwhite=10;

  if (whitenum==2){
    for (int i=7;i>0;i-=1){
      if (iswhite[i]==1){
        lastwhite=i;
        break;
      }
    }
    for(int i=0;i<firstwhites[0];++i){
      sensorValues[i]=minimum[i];
    }
    for(int i=lastwhite;i<8;++i){
      sensorValues[i]=minimum[i];
    }
  }


  

  for(int i=0;i<8;++i){
    error+=int((sensorValues[i]-minimum[i])/nomal[i])*weights[i];
  }
  if (checkWhite>7){//out of track or end
    outCount+=1;
    return;
  }
  outCount=0;
  error=error/8;

  //change speed
  int change=error/Kp;//for p

  int deltaError=error-preError[3];
  for(int i=1;i<4;++i){
    preError[i]=preError[i-1];
    preDelta[i]=preDelta[i-1];
  }
  preError[0]=error;
  preDelta[0]=deltaError;
  float weighted=deltaError*0.35+preError[1]*0.25+preError[2]*0.2+preError[3]*0.2;
  int Ichange=constrain(weighted*Kd,-base_speed*0.8,base_speed*0.8);
  change=constrain(error/Kp+Ichange,-base_speed,base_speed);
  leftSpd=base_speed-change;
  rightSpd=base_speed+change;
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}