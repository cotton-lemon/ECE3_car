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
const int base_speed=30;
// const int change_speed_constant=5;
const int Kp=1000;//P will be devided by Kp
const float Kd=1;
//global
int leftSpd = base_speed;
int rightSpd = base_speed;
int preError=0;
unsigned long preTime=0;
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
  
  delay(50000); //Wait 5 seconds before starting 
  preError=get_parameter();
  preTime=millis();
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}

void loop() {
  bump_sw_0_reading = digitalRead(bump_sw_0_pin);
  if((not bump_sw_0_reading)||(outCount>5)){//exit
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
  int sensorSum=0;
  char checkWhite=0;
  for (int i=0;i<8;++i){
    if (sensorValues[i]<850){
      checkWhite+=1;
    }
    sensorSum+=sensorValues[i];
    sum+=int((sensorValues[i]-minimum[i])/nomal[i])*weights[i];
//     Serial.print(sum);
//     Serial.print('\t');
  }
  if (sensorSum>16000){//2000*8
    return preError;
  }
  if (checkWhite>7){
    outCount+=1;
  }
  else{
    outCount=0;
  }
  sum=sum/8;
  Serial.println(); 
  Serial.print(sum);
  Serial.println(); 
  return sum;
}

void changeSpeed(int error){
  int change=error/Kp;//for p
  unsigned long newtime=millis();
  unsigned int deltaTime=newtime-preTime;
  unsigned int deltaError=error-preError;
  preTime=newtime;
  preError=error;
  change=int((error/Kp)+(deltaError/deltaTime)*Kd);
  leftSpd=base_speed-change;
  rightSpd=base_speed+change;
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}