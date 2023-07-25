

#include <ECE3.h> // Used for encoder functionality

#define LED_BUILTIN 75

uint16_t sensorValues[8];
// Pin Assignments
const int left_nslp_pin = 31;
const int left_dir_pin  = 29;
const int left_pwm_pin  = 40;
const int right_pwm_pin=39;
const int right_dir_pin=30;
const int right_nslp_pin=11;

//parameters

const int maximum_speed=50;
const int base_speed=30;

//global
int leftSpd = base_speed;
int rightSpd = base_speed;

//char new_duty_cycle[4];
//constants
const int weights[8]={-15,-14,-12,-8,8,12,14,15};
const int minimum[8]={782,666,689,686,693,643,666,712};
const float nomal[8]={1.718,1.834,1.811,1.814,1.806,1.857,1.834,1.788};
// Prototypes
int get_parameter();
void changespeed(int sensor_fus);
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
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
}

void loop() {
    int sensor_fus=get_parameter();
    changespeed(sensor_fus);
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
  for (int i=0;i<8;++i){
    sum+=int((sensorValues[i]-minimum[i])/nomal[i])*weights[i];
//     Serial.print(sum);
//     Serial.print('\t');
  }
  sum=sum/8;
  Serial.println(); 
  Serial.print(sum);
  Serial.println(); 
  return sum;
}

void changespeed(int sensorFuse){
  if (sensorFuse>1500){
    leftSpd=base_speed-15;
    rightSpd =base_speed+15;
  }
  else if (sensorFuse>1000){
    leftSpd=base_speed-10;
    rightSpd =base_speed+10;
  }
  else if (sensorFuse>0){
    leftSpd=base_speed-5;
    rightSpd =base_speed+5;
  }
  else if(sensorFuse<-1000){
    leftSpd=base_speed+10;
    rightSpd =base_speed-10;
  }
  else{
    leftSpd=base_speed+5;
    rightSpd =base_speed-5;
  }
  analogWrite(left_pwm_pin,leftSpd);
  analogWrite(right_pwm_pin,rightSpd);
  
}