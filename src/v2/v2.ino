//simple PD version
//stop when all white
//ignore phantom


//to do recovery algorithm


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
const int Kp=44;//P will be devided by Kp
const int Kd=9;
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
int get_parameter();
void changeSpeed(int sensor_fus);

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

  Serial.begin(9600); // data rate for serial data transmission
  
  delay(2000); //Wait 2 seconds before starting 
  int tmp=get_parameter();
  for(int i=0;i<4;++i){
    preError[i]=tmp;
  }
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}

void loop() {
  // bump_sw_0_reading = digitalRead(bump_sw_0_pin);
  // if((not bump_sw_0_reading)||(outCount>5)){//exit
  //   analogWrite(left_pwm_pin,0);
  //   analogWrite(right_pwm_pin,0);
  //   exit(0);//or just use the flag
  // }
  if((outCount>5)){//exit
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
  int sensor_fus=get_parameter();
  changeSpeed(sensor_fus);
}


int get_parameter(){
  ECE3_read_IR(sensorValues);
  int sum=0;
//  Serial.print("raw values\n");
//  for (unsigned char i = 0; i < 8; i++)
//  {
//    Serial.print(sensorValues[i]);
//    Serial.print('\t'); // tab to format the raw data into columns in the Serial monitor
//  }
//  Serial.println(); 
//  Serial.print("sums\n");
  // int sensorSum=0;
  char checkWhite=0;
  for (int i=0;i<8;++i){
    if (sensorValues[i]<850){
      checkWhite+=1;
    }
    // sensorSum+=sensorValues[i];
    sum+=int((sensorValues[i]-minimum[i])/nomal[i])*weights[i];
//     Serial.print(sum);
//     Serial.print('\t');
  }
  // if (sensorSum>16000){//2000*8
  //   return preError;
  // }
  if (checkWhite>7){
    outCount+=1;
  }
  else{
    outCount=0;
  }
  sum=sum/8;
  // Serial.println(); 
  // Serial.print(sum);
  // Serial.println(); 
  return sum;
}

void changeSpeed(int error){
  int change=error/Kp;//for p

  int deltaError=error-preError[3];
  for(int i=1;i<4;++i){
    preError[i]=preError[i-1];
    preDelta[i]=preDelta[i-1];
  }
  preError[0]=error;
  //0.5 0.25 0.125 0.125
  int Ichange=constrain(deltaError/Kd,-base_speed*0.8,base_speed*0.8);
  // preDelta[0]=Ichange;
  
  // change=int((error/Kp)+(deltaError)*Kd);
  
  // change=constrain(error/Kp+Ichange,-base_speed,base_speed);
  change=constrain(error/Kp,-base_speed,base_speed);
  leftSpd=base_speed-change;
  rightSpd=base_speed+change;
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}