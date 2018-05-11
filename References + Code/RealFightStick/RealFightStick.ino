
#include "UnoJoy.h"

int TrianglePin = 44;
int SquarePin = 46;
int CrossPin = 52;
int CirclePin = 50;

int r2Pin = 48;
int r1Pin = 42;
int l1Pin = 40;
int l2Pin = 38;


int LeftPin = 51;
int UpPin = 47;
int RightPin = 53;
int DownPin = 49;

int StartPin =  36;
int SelectPin = 34;

void setup(){
  setupPins();
  setupUnoJoy();
}

void loop(){ 
// Always be getting fresh data
  dataForController_t controllerData = getControllerData();
  setControllerData(controllerData);
}

void setupPins(void){
  // Set all the digital pins as inputs
  // with the pull-up enabled, except for the 
  // two serial line pins
  for (int i = 34; i <= 54; i++){
    pinMode(i, INPUT);
    digitalWrite(i, HIGH);
  }
//  pinMode(A1, INPUT);
//  digitalWrite(A1, HIGH);
//  pinMode(A2, INPUT);
//  digitalWrite(A2, HIGH);
//   pinMode(A3, INPUT);
//  digitalWrite(A3, HIGH);
}

dataForController_t getControllerData(void){
  
  // Set up a place for our controller data
  //  Use the getBlankDataForController() function, since
  //  just declaring a fresh dataForController_t tends
  //  to get you one filled with junk from other, random
  //  values that were in those memory locations before
  dataForController_t controllerData = getBlankDataForController();
  // Since our buttons are all held high and
  //  pulled low when pressed, we use the "!"
  //  operator to invert the readings from the pins
  controllerData.triangleOn = !digitalRead(TrianglePin);
  controllerData.circleOn = !digitalRead(CirclePin);
  controllerData.squareOn = !digitalRead(SquarePin);
  controllerData.crossOn = !digitalRead(CrossPin);
  
  controllerData.r1On = !digitalRead(r1Pin);
  controllerData.r2On = !digitalRead(r2Pin);
  controllerData.l1On = !digitalRead(l1Pin);
  controllerData.l2On = !digitalRead(l2Pin);
  
  
  
  controllerData.dpadUpOn = !digitalRead(UpPin);
  controllerData.dpadDownOn = !digitalRead(DownPin);
  controllerData.dpadLeftOn = !digitalRead(LeftPin);
  controllerData.dpadRightOn = !digitalRead(RightPin);
  
  controllerData.startOn = !digitalRead(StartPin);
  controllerData.selectOn = !digitalRead(SelectPin);
  
  // And return the data!
  return controllerData;
}
