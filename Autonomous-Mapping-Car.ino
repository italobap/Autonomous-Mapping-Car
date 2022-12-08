//Projeto italo e angeli
#include <NewPing.h>        //Ultrasonic sensor function library.

//Roxo, cinza, branco e preto
//our L298N control pins 
const int LeftMotorForward = 18;
const int LeftMotorBackward = 19;
const int RightMotorForward = 4;
const int RightMotorBackward = 5;
const int PWMMotorA = 12;
const int PWMMotorB = 13;

const byte enconderA = 26;
const byte enconderB = 27;

unsigned int counter_A = 0;
unsigned int counter_B = 0;

// Float for number of slots in encoder disk
float diskslots = 20.00;  

// Constant for wheel diameter
const float wheeldiameter = 66.00; 

int ultimaMudanca1 = 0;
int ultimaMudanca2 = 0;

// Motor 1 pulse count ISR
void ISR_count1()  {
  if(millis() - ultimaMudanca1 >= 5){
  counter_A++;  // increment Motor 1 counter value
  ultimaMudanca1 = millis();
  }
} 
 
// Motor 2 pulse count ISR
void ISR_count2()  {
    if(millis() - ultimaMudanca2 >= 5){
    counter_B++;  // increment Motor 2 counter value
    ultimaMudanca2 = millis();
  }
} 

// Function to convert from centimeters to steps
int CMtoSteps(float cm) {
 
  int result;  // Final calculation result
  float circumference = (wheeldiameter * 3.14) / 10; // Calculate wheel circumference in cm
  float cm_step = circumference / diskslots;  // CM per Step
  
  float f_result = cm / cm_step;  // Calculate result as a float
  result = (int) f_result; // Convert to an integer (note this is NOT rounded)
  
  return result;  
 
}

hw_timer_t *timer = NULL;

// TimerOne ISR
void IRAM_ATTR onTimer() {
  timerDetachInterrupt(timer);
  //Timer1.detachInterrupt();  // Stop the timer
  Serial.print("Motor Speed 1: "); 
  float rotation1 = (counter_A / diskslots) * 60.00;  // calculate RPM for Motor 1
  Serial.print(rotation1);  
  Serial.print(" RPM - "); 
  counter_A = 0;  //  reset counter to zero
  Serial.print("Motor Speed 2: "); 
  float rotation2 = (counter_B / diskslots) * 60.00;  // calculate RPM for Motor 2
  Serial.print(rotation2);  
  Serial.println(" RPM"); 
  counter_B = 0;  //  reset counter to zero
  //Timer1.attachInterrupt( onTimer );  // Enable the timer
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmEnable(timer);
}

//sensor pins
#define trig_pin 17 //analog input 1 - A1
#define echo_pin 16 //analog input 2 - A2

#define maximum_distance 200
boolean goesForward = false;
int distance = 100;

NewPing sonar(trig_pin, echo_pin, maximum_distance); //sensor function

// Setting PWM properties
const int freq = 30000;
const int pwmChannelA = 0;
const int pwmChannelB = 1;
const int resolution = 8;
int dutyCycle = 200;

void setup(){
  Serial.begin(115200);
  pinMode(RightMotorForward, OUTPUT);
  pinMode(LeftMotorForward, OUTPUT);
  pinMode(LeftMotorBackward, OUTPUT);
  pinMode(RightMotorBackward, OUTPUT);
  pinMode(PWMMotorA,OUTPUT);
  pinMode(PWMMotorB,OUTPUT);
  
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true);
  //Timer1.initialize(1000000); // set timer for 1sec
  attachInterrupt(digitalPinToInterrupt (enconderA), ISR_count1, RISING);  // Increase counter 1 when speed sensor pin goes High
  attachInterrupt(digitalPinToInterrupt (enconderB), ISR_count2, RISING);  // Increase counter 2 when speed sensor pin goes High
  //Timer1.attachInterrupt( ISR_timerone ); // Enable the timer
  timerAlarmEnable(timer);

  ledcSetup(pwmChannelA, freq, resolution);
  ledcAttachPin(PWMMotorA, pwmChannelA);
  ledcSetup(pwmChannelB, freq, resolution);
  ledcAttachPin(PWMMotorB, pwmChannelB);
}

int readPing(){
  delay(70);
  int cm = sonar.ping_cm();
  if (cm==0){
    cm=200;
  }
  return cm;
}

void moveStop(){
  
  digitalWrite(RightMotorForward, LOW);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  digitalWrite(LeftMotorBackward, LOW);
}
/*
void moveForward(){

  if(!goesForward){

    goesForward=true;
    
    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    ledcWrite(pwmChannelB, dutyCycle);
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    ledcWrite(pwmChannelA, dutyCycle); 
  }
}

void moveBackward(){

  goesForward=false;

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
  
}

void turnRight(){

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
  
  delay(500);
  
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
  
}

void turnLeft(){

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);

  delay(500);
  
  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorBackward, LOW);
}
*/

// Function to Move Forward
void MoveForward(int steps, int mspeed) 
{
   counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero

    digitalWrite(LeftMotorForward, HIGH);
    digitalWrite(RightMotorForward, HIGH);
    //ledcWrite(pwmChannelB, dutyCycle);
    digitalWrite(LeftMotorBackward, LOW);
    digitalWrite(RightMotorBackward, LOW);
    //ledcWrite(pwmChannelA, dutyCycle); 
   
   // Go forward until step value is reached
   while (steps > counter_A && steps > counter_B) {
   
    if (steps > counter_A) {
    ledcWrite(pwmChannelA, mspeed); 
    } else {
    ledcWrite(pwmChannelA, 0); 
    }
    if (steps > counter_B) {
    ledcWrite(pwmChannelB, mspeed);
    } else {
    ledcWrite(pwmChannelB, 0); 
    }
   }
   
  // Stop when done
  ledcWrite(pwmChannelA, 0); 
  ledcWrite(pwmChannelB, 0); 
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
 
}

// Function to Move in Reverse
void MoveReverse(int steps, int mspeed) 
{
   counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero

  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorForward, LOW);
   
   // Go in reverse until step value is reached
   while (steps > counter_A && steps > counter_B) {
   
    if (steps > counter_A) {
    ledcWrite(pwmChannelA, mspeed); 
    } else {
    ledcWrite(pwmChannelA, 0); 
    }
    if (steps > counter_B) {
    ledcWrite(pwmChannelB, mspeed);
    } else {
    ledcWrite(pwmChannelB, 0); 
    }
   }
   
  // Stop when done
  ledcWrite(pwmChannelA, 0); 
  ledcWrite(pwmChannelB, 0); 
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
 
}
 
// Function to Spin Right
void SpinRight(int steps, int mspeed) 
{
   counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero

  digitalWrite(LeftMotorForward, HIGH);
  digitalWrite(RightMotorBackward, HIGH);
  
  digitalWrite(LeftMotorBackward, LOW);
  digitalWrite(RightMotorForward, LOW);
  
   // Go until step value is reached
   while (steps > counter_A && steps > counter_B) {
   
    if (steps > counter_A) {
    ledcWrite(pwmChannelA, mspeed); 
    } else {
    ledcWrite(pwmChannelA, 0); 
    }
    if (steps > counter_B) {
    ledcWrite(pwmChannelB, mspeed);
    } else {
    ledcWrite(pwmChannelB, 0); 
    }
   }
   
  // Stop when done
  ledcWrite(pwmChannelA, 0); 
  ledcWrite(pwmChannelB, 0); 
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
 
}
 
// Function to Spin Left
void SpinLeft(int steps, int mspeed) 
{
   counter_A = 0;  //  reset counter A to zero
   counter_B = 0;  //  reset counter B to zero
   
  digitalWrite(LeftMotorBackward, HIGH);
  digitalWrite(RightMotorForward, HIGH);
  
  digitalWrite(LeftMotorForward, LOW);
  digitalWrite(RightMotorBackward, LOW);
   
   // Go until step value is reached
   while (steps > counter_A && steps > counter_B) {
   
    if (steps > counter_A) {
    ledcWrite(pwmChannelA, mspeed); 
    } else {
    ledcWrite(pwmChannelA, 0); 
    }
    if (steps > counter_B) {
    ledcWrite(pwmChannelB, mspeed);
    } else {
    ledcWrite(pwmChannelB, 0); 
    }
   }
   
  // Stop when done
  ledcWrite(pwmChannelA, 0); 
  ledcWrite(pwmChannelB, 0); 
  counter_A = 0;  //  reset counter A to zero
  counter_B = 0;  //  reset counter B to zero 
 
}

void loop(){


  distance = readPing();

  if (distance <= 20){
    moveStop();
    delay(500);
    MoveReverse(10, 200);
    delay(500);
    moveStop();
    delay(500);
    SpinRight(20, 200);  // Spin right 20 steps at 255 speed
    delay(500);  // Wait one second
   MoveForward(CMtoSteps(50), 220);  // Forward 10 steps at 150 speed

  }
  else{
   MoveForward(CMtoSteps(70), 230);
  }
  distance = readPing();
    Serial.print(distance);
}
